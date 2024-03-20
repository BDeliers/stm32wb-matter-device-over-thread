#include "app_linky.h"
#include "app_uart.h"
#include "string_utils.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <time.h>

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

static AppLinky::DataObjectBase linky_data[AppLinky::Field::Field_COUNT] = {
    AppLinky::DataObject<sizeof(uint64_t)>(),           // ADSC
    AppLinky::DataObject<sizeof(uint8_t)>(),            // VTIC
    AppLinky::DataObject<sizeof(struct tm), false>(),   // DATE
    AppLinky::DataObject<17, true>(),                   // NGTF
    AppLinky::DataObject<17, true>(),                   // LTARF
    AppLinky::DataObject<sizeof(uint32_t)>(),           // EAST
    AppLinky::DataObject<sizeof(uint32_t)>(),           // EASF01
    AppLinky::DataObject<sizeof(uint32_t)>(),           // EASF02
    AppLinky::DataObject<sizeof(uint32_t)>(),           // EASF03
    AppLinky::DataObject<sizeof(uint32_t)>(),           // EASF04
    AppLinky::DataObject<sizeof(uint32_t)>(),           // EASF05
    AppLinky::DataObject<sizeof(uint32_t)>(),           // EASF06
    AppLinky::DataObject<sizeof(uint32_t)>(),           // EASF07
    AppLinky::DataObject<sizeof(uint32_t)>(),           // EASF08
    AppLinky::DataObject<sizeof(uint32_t)>(),           // EASF09
    AppLinky::DataObject<sizeof(uint32_t)>(),           // EASF10
    AppLinky::DataObject<sizeof(uint32_t)>(),           // EASD01
    AppLinky::DataObject<sizeof(uint32_t)>(),           // EASD02
    AppLinky::DataObject<sizeof(uint32_t)>(),           // EASD03
    AppLinky::DataObject<sizeof(uint32_t)>(),           // EASD04
    AppLinky::DataObject<sizeof(uint32_t)>(),           // EAIT
    AppLinky::DataObject<sizeof(uint32_t)>(),           // ERQ1
    AppLinky::DataObject<sizeof(uint32_t)>(),           // ERQ2
    AppLinky::DataObject<sizeof(uint32_t)>(),           // ERQ3
    AppLinky::DataObject<sizeof(uint32_t)>(),           // ERQ4
    AppLinky::DataObject<sizeof(uint16_t)>(),           // IRMS1
    AppLinky::DataObject<sizeof(uint16_t)>(),           // IRMS2
    AppLinky::DataObject<sizeof(uint16_t)>(),           // IRMS3
    AppLinky::DataObject<sizeof(uint16_t)>(),           // URMS1
    AppLinky::DataObject<sizeof(uint16_t)>(),           // URMS2
    AppLinky::DataObject<sizeof(uint16_t)>(),           // URMS3
    AppLinky::DataObject<sizeof(uint16_t)>(),           // PREF
    AppLinky::DataObject<sizeof(uint16_t)>(),           // PCOUP
    AppLinky::DataObject<sizeof(uint32_t)>(),           // SINSTS
    AppLinky::DataObject<sizeof(uint32_t)>(),           // SINSTS1
    AppLinky::DataObject<sizeof(uint32_t)>(),           // SINSTS2
    AppLinky::DataObject<sizeof(uint32_t)>(),           // SINSTS3
    AppLinky::DataObject<sizeof(uint32_t)>(),           // SMAXSN
    AppLinky::DataObject<sizeof(uint32_t)>(),           // SMAXSN1
    AppLinky::DataObject<sizeof(uint32_t)>(),           // SMAXSN2
    AppLinky::DataObject<sizeof(uint32_t)>(),           // SMAXSN3
    AppLinky::DataObject<sizeof(uint32_t)>(),           // SMAXSN_M1
    AppLinky::DataObject<sizeof(uint32_t)>(),           // SMAXSN1_M1
    AppLinky::DataObject<sizeof(uint32_t)>(),           // SMAXSN2_M1
    AppLinky::DataObject<sizeof(uint32_t)>(),           // SMAXSN3_M1
    AppLinky::DataObject<sizeof(uint32_t)>(),           // SINSCTI
    AppLinky::DataObject<sizeof(uint32_t)>(),           // SMAXIN
    AppLinky::DataObject<sizeof(uint32_t)>(),           // SMAXIN_M1
    AppLinky::DataObject<sizeof(uint32_t)>(),           // CCASN
    AppLinky::DataObject<sizeof(uint32_t)>(),           // CCASN_M1
    AppLinky::DataObject<sizeof(uint32_t)>(),           // CCAIN
    AppLinky::DataObject<sizeof(uint32_t)>(),           // CCAIN_M1
    AppLinky::DataObject<sizeof(uint16_t)>(),           // UMOY1
    AppLinky::DataObject<sizeof(uint16_t)>(),           // UMOY2
    AppLinky::DataObject<sizeof(uint16_t)>(),           // UMOY3
    AppLinky::DataObject<sizeof(uint32_t)>(),           // STGE
    AppLinky::DataObject<sizeof(uint8_t)>(),            // DPM1
    AppLinky::DataObject<sizeof(uint8_t)>(),            // FPM1
    AppLinky::DataObject<sizeof(uint8_t)>(),            // DPM2
    AppLinky::DataObject<sizeof(uint8_t)>(),            // FPM2
    AppLinky::DataObject<sizeof(uint8_t)>(),            // DPM3
    AppLinky::DataObject<sizeof(uint8_t)>(),            // FPM3
    AppLinky::DataObject<33, true>(),                   // MSG1
    AppLinky::DataObject<17, true>(),                   // MSG2
    AppLinky::DataObject<sizeof(uint64_t)>(),           // PRM
    AppLinky::DataObject<sizeof(uint16_t)>(),           // RELAIS
    AppLinky::DataObject<sizeof(uint8_t)>(),            // NTARF
    AppLinky::DataObject<sizeof(uint8_t)>(),            // NJOURF
    AppLinky::DataObject<sizeof(uint8_t)>(),            // NJOURF_P1
    AppLinky::DataObject<99, true>(),                   // PJOURF_P1
    AppLinky::DataObject<99, true>(),                   // PPOINTE
};

static uint8_t MAX_LABEL_LEN = 12;

bool AppLinky::Init(osMessageQId feedback_queue)
{
    // Prepare and start a thread to handle the business logic
    const osThreadAttr_t attr = { 
                        .name = "Thread Linky", 
                        .stack_size = 2048, 
                        .priority = osPriorityBelowNormal,
                        };

    osThreadNew(AppLinky::Handler, this, &attr);

    queue_data_update = feedback_queue;

    AppUart_InitExternal(UartRxCallback);

    timer_rx = xTimerCreate("Delay_LinkyRx",  // Just a text name, not used by the RTOS kernel
            10000U,                                     // == default timer period (mS)
            pdTRUE,                                    // Timer reload
            0,                                          // init timer
            DelayReceiveTimerHandler                    // timer callback handler
            );

    xTimerStart(timer_rx, 0);

    return true;
}

void AppLinky::Handler(void* arg)
{
    AppLinky* this_ptr = static_cast<AppLinky*>(arg);

    char *ptr_line_start, *ptr_line_end, *ptr_frame_end;

    this_ptr->StartReceiving();

    for (;;)
    {
        if (this_ptr->frame_received)
        {
            this_ptr->frame_received = false;

            ptr_line_start = reinterpret_cast<char*>(this_ptr->buffer_incoming_bytes);
            ptr_frame_end  = reinterpret_cast<char*>(this_ptr->buffer_incoming_bytes + this_ptr->bytes_count);

            for (char* ptr = ptr_line_start; ptr < ptr_frame_end; ptr++)
            {
                *ptr &= 0x7F;
            }

            while ((*ptr_line_start != ASCII_STX) && (ptr_line_start < ptr_frame_end))
            {
                ptr_line_start++;
            }

            // Iterate on each byte
            for (;;)
            {
                // Find the line end
                ptr_line_end = ptr_line_start;
                while ((*ptr_line_end != ASCII_CR)
                        && (ptr_line_end != (ptr_frame_end - 2)))
                {
                    ptr_line_end++;
                }

                if (ptr_line_end >= ptr_frame_end)
                {
                    APP_DBG("[AppLinky] Malformed frame, aborting");
                    break;
                }

                // Process the line
                if (ptr_line_end - ptr_line_start > 3)
                {
                    if (!this_ptr->ProcessLine(ptr_line_start, ptr_line_end))
                    {
                        APP_DBG("[AppLinky] Failed to process a line");
                    }
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

        // Don't block !
        osDelay(1);
    }
}

void AppLinky::DelayReceiveTimerHandler(TimerHandle_t xTimer) {
    GetInstance().StartReceiving();
}

bool AppLinky::StartReceiving(void)
{
    // Reset all the business logic
    memset(buffer_incoming_bytes, 0, sizeof(buffer_incoming_bytes));
    frame_received   = false;

    // Start the RX driver
    AppUart_ReceiveFrameExternal(buffer_incoming_bytes, sizeof(buffer_incoming_bytes));

    return true;
}

AppLinky::DataObjectBase& AppLinky::GetField(Field field)
{
    return linky_data[field];
}

void AppLinky::UartRxCallback(uint16_t size)
{
    AppLinky& self = AppLinky::GetInstance();

    self.frame_received = true;
    self.bytes_count    = size;
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

            // Write to the queue that the field has was updated
            Field f = Field::DATE;
            osMessageQueuePut(queue_data_update, &f, 0, 0);
        }
        *(checksum_start-1) = 0;

        switch (field)
        {
        case NGTF:
        case LTARF:
        case MSG1:
        case MSG2:
        case PJOURF_P1:
        case PPOINTE:
        {
            UpdateField(field, data_start);
            break;
        }
        case STGE:
        {
            uint32_t tmp = strtoul(data_start, NULL, 16);
            UpdateField(field, &tmp);
            break;
        }
        case ADSC:
        case PRM:
        {
            uint64_t tmp = strtoull(data_start, NULL, 10);
            UpdateField(field, &tmp);
            break;
        }
        // Discarded
        case DATE:
            break;

        default:
        {
            uint32_t tmp = strtoul(data_start, NULL, 10);
            UpdateField(field, &tmp);
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

    struct tm struct_time = {
        .tm_sec  = static_cast<int>(strtoul_len(data + 11, 2)),
        .tm_min  = static_cast<int>(strtoul_len(data + 9, 2)),
        .tm_hour = static_cast<int>(strtoul_len(data + 7, 2)),
        .tm_mday = static_cast<int>(strtoul_len(data + 5, 2)),
        .tm_mon  = static_cast<int>(strtoul_len(data + 3, 2)),
        .tm_year = static_cast<int>(strtoul_len(data + 1, 2) - 1900),
    };

    UpdateField(Field::DATE, &struct_time);

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

bool AppLinky::UpdateField(Field field, void* val_ptr)
{
    if ((field < Field::Field_COUNT) && (val_ptr != nullptr))
    {
        DataObjectBase& obj = linky_data[field];

        if (!obj.Equals(val_ptr)
            && obj.SetData(val_ptr))
        {
            osMessageQueuePut(queue_data_update, &field, 0, 0);
        }
        return true;
    }

    return false;
}

bool AppLinky::DataObjectBase::SetData(void* data_ptr)
{
    if (data_ptr == nullptr) { return false; }

    if (payload_is_string)
    {
        strcpy(reinterpret_cast<char*>(payload_ptr), static_cast<const char*>(data_ptr));
        strtrim(reinterpret_cast<char*>(payload_ptr), ASCII_SPACE);
    }
    else
    {
        memcpy(payload_ptr, data_ptr, payload_size);
    }

    return true;
}

bool AppLinky::DataObjectBase::Equals(void* src)
{
    if (src == nullptr) { return false; }

    if (payload_is_string)
    {
        return (strcmp(reinterpret_cast<char*>(payload_ptr), static_cast<const char*>(src)) == 0);
    }
    else
    {
        return (memcmp(payload_ptr, src, payload_size) == 0);
    }

    return true;
}
