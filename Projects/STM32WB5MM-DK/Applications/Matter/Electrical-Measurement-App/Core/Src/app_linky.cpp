#include "app_linky.h"
#include "app_uart.h"
#include "string_utils.h"

#include "cmsis_os.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "app_conf.h"
#include "dbg_trace.h"
#include "stm_logging.h"

/*
    Data blocks are received as following:
    
    STX
        LF <label> (HTAB <date>) HTAB <data> HTAB <checksum> CR
        [...]
    ETX

    Checksum is computed from the label to the HTAB before checksum

*/

// This has to stick to the enum
static const char* LABELS[AppLinky::Field::Field_COUNT] = {
    "ADSC", "VTIC", "DATE", "NGTF", "LTARF", "EAST", "EASF01", "EASF02", "EASF03", "EASF04", "EASF05", "EASF06", "EASF07", "EASF08", "EASF09", "EASF10",
    "EASD01", "EASD02", "EASD03", "EASD04", "EAIT", "ERQ1", "ERQ2", "ERQ3", "ERQ4", "IRMS1", "IRMS2", "IRMS3", "URMS1", "URMS2", "URMS3", "PREF", "PCOUP",
    "SINSTS", "SINSTS1", "SINSTS2", "SINSTS3", "SMAXSN", "SMAXSN1", "SMAXSN2", "SMAXSN3", "SMAXSN-1", "SMAXSN1-1", "SMAXSN2-1", "SMAXSN3-1", "SINSCTI",
    "SMAXIN", "SMAXIN-1", "CCASN", "CCASN-1", "CCAIN", "CCAIN-1", "UMOY1", "UMOY2", "UMOY3", "STGE", "DPM1", "FPM1", "DPM1", "FPM2", "DPM3", "FPM3",
    "MSG1", "MSG2", "PRM", "RELAIS", "NTARF", "NJOURF", "NJOURF+1", "PJOURF+1", "PPOINTE",
};
static uint8_t MAX_LABEL_LEN = 12;

bool AppLinky::Init(void)
{
    // Prepare and start a thread to handle the business logic
    const osThreadAttr_t attr = { 
                        .name = "Thread Linky", 
                        .stack_size = 2048, 
                        .priority = osPriorityBelowNormal,
                        };

    osThreadNew(AppLinky::Handler, this, &attr);

    AppUart_InitExternal(UartRxCallback);
    
    memset(linky_data,      0, sizeof(linky_data));
    memset(linky_address,   0, sizeof(linky_address));
    memset(linky_calname,   0, sizeof(linky_calname));
    memset(linky_curprice,  0, sizeof(linky_curprice));
    memset(linky_prm,       0, sizeof(linky_prm));

    return true;
}

void AppLinky::Handler(void* arg)
{
    AppLinky* this_ptr = static_cast<AppLinky*>(arg);

    char *ptr_line_start, *ptr_line_end;
    uint16_t bytes_count;

    this_ptr->StartReceiving();

    for (;;)
    {
        if (this_ptr->frame_received)
        {
            // Stop the RX driver
            AppUart_EnableRxExternal(false);

            ptr_line_start = reinterpret_cast<char*>(this_ptr->buffer_incoming_bytes);

            // If a frame starts properly as it should, put the pointer to the first line starter
            if (*ptr_line_start == ASCII_STX)
            {
                bytes_count = this_ptr->buffer_incoming_bytes - this_ptr->ptr_incoming_bytes;
                ptr_line_start++;

                // Iterate on each byte
                for (;;)
                {
                    // Find the line end
                    ptr_line_end = ptr_line_start;
                    while ((*ptr_line_end != ASCII_CR)
                            && (ptr_line_end != reinterpret_cast<char*>(this_ptr->ptr_incoming_bytes)))
                    {
                        ptr_line_end++;
                    };

                    if (ptr_line_end == reinterpret_cast<char*>(this_ptr->ptr_incoming_bytes))
                    {
                        APP_DBG("[AppLinky] Malformed frame, aborting");
                        break;
                    }

                    // Process the line
                    if (!this_ptr->ProcessLine(ptr_line_start, ptr_line_end))
                    {
                        APP_DBG("[AppLinky] Failed to process a line");
                    }

                    // Move to the next line
                    ptr_line_start = ptr_line_end+1;

                    // End of frame
                    if (*ptr_line_start == ASCII_ETX)
                    {
                        break;
                    }
                }
            }


            // Restart RX procedure
            this_ptr->StartReceiving();
        }

        // Don't block !
        osDelay(1);
    }
}

bool AppLinky::StartReceiving(void)
{
    // Reset all the business logic
    memset(buffer_incoming_bytes, 0, sizeof(buffer_incoming_bytes));
    ptr_incoming_bytes   = buffer_incoming_bytes;
    ready_to_receive = true;
    frame_started    = false;
    frame_received   = false;

    // Start the RX driver
    AppUart_EnableRxExternal(true);

    return true;
}

void AppLinky::UartRxCallback(uint8_t byte)
{
    AppLinky& self = AppLinky::GetInstance();

    // If we're safe to write in the buffer
    if (self.ready_to_receive)
    {
        // Wait for start of frame
        if (byte == ASCII_STX)
        {
            self.frame_started = true;
        }
        // If the frame has started
        if (self.frame_started)
        {
            // End of frame, lock the buffer and process it
            if (byte == ASCII_ETX)
            {
                self.ready_to_receive = false;
                self.frame_received   = true;
            }

            // Store the byte in the buffer
            *(self.ptr_incoming_bytes++) = byte;
        }
    }
}

bool AppLinky::CheckCrc(const char* data, char checksum, size_t size)
{
    if (data == NULL)
    {
        return false;
    }

    uint32_t sum = 0;

    for (size_t i = 0; i < size; i++)
    {
        sum += data[i];
    }

    sum &= 0x3F;
    sum += 0x20;

    return (static_cast<char>(sum & 0xFF) == checksum);
}

bool AppLinky::ProcessLine(char* start, char* end)
{
    // If the date is here, we'll count 3 tabs
    bool has_date = strcntchar(start, end, ASCII_HTAB) == 3;

    if ((*start == ASCII_LF) && (*end == ASCII_CR))
    {
        if (!CheckCrc(start+1, *(end-1), (end-start) - 2))
        {
            APP_DBG("[AppLinky] Bad CRC");
            return false;
        }

        // Retrieve label as a string and convert it to an enum Field
        char* label_start    = start + 1;
        char* data_start     = static_cast<char*>(memchr(label_start, ASCII_HTAB, end-label_start)) + 1;
        char* checksum_start = static_cast<char*>(memchr(data_start, ASCII_HTAB, end-data_start)) + 1;
    
        // Make string terminations
        *(data_start-1) = 0;
        *(checksum_start-1) = 0;
        Field field = StringToField(label_start);

        if (has_date)
        {
            // Save current date if needed
            if (field == Field::DATE)
            {
                if (!ParseAndSaveDate(data_start))
                {
                    APP_DBG("[AppLinky] Date parsing error");
                    return false;
                }
            }
            data_start  = checksum_start;
            checksum_start = static_cast<char*>(memchr(data_start, ASCII_HTAB, end-data_start)) + 1;
        }
        *(checksum_start-1) = 0;


        switch (field)
        {
        case ADSC:
        {
            strcpy(linky_address, data_start);
            strtrim(linky_address, ASCII_SPACE);
            break;
        }
        case NGTF:
        {
            strcpy(linky_calname, data_start);
            strtrim(linky_calname, ASCII_SPACE);
            break;
        }
        case LTARF:
        {
            strcpy(linky_curprice, data_start);
            strtrim(linky_curprice, ASCII_SPACE);
            break;
        }
        case STGE:
        {
            linky_data[Field::STGE] = strtoul(data_start, NULL, 16);
            break;
        }
        case PRM:
        {
            strcpy(linky_prm, data_start);
            strtrim(linky_prm, ASCII_SPACE);
            break;
        }
        // Discarded
        case DATE:
        case DPM1:
        case FPM1:
        case DPM2:
        case FPM2:
        case DPM3:
        case FPM3:
        case MSG1:
        case MSG2:
        case PJOURF_P1:
        case PPOINTE:
            break;

        default:
        {
            linky_data[field] = strtoul(data_start, NULL, 10);
            break;
        }
        }

        return true;
    }

    APP_DBG("[AppLinky] Line endings issue");
    return false;
}

bool AppLinky::ParseAndSaveDate(const char* data)
{
    if (data == NULL)
    {
        return false;
    }

    switch(data[0])
    {
    case 'P':
        season = Season::SPRING;
        break;
    case 'E':
        season = Season::SUMMER;
        break;
    case 'A':
        season = Season::AUTUMN;
        break;
    default:
        season = Season::WINTER;
        break;
    }

    year   = strtoul_len(data + 1, 2);
    month  = strtoul_len(data + 3, 2);
    day    = strtoul_len(data + 5, 2);
    hour   = strtoul_len(data + 7, 2);
    minute = strtoul_len(data + 9, 2);
    second = strtoul_len(data + 11, 2);

    return true;
}

AppLinky::Field AppLinky::StringToField(const char* str)
{
    for (uint8_t i = 0; i < Field::Field_COUNT; i++)
    {
        if (strcmp(str, LABELS[i]) == 0)
        {
            return static_cast<Field>(i);
        }
    }

    return Field_COUNT;
}
