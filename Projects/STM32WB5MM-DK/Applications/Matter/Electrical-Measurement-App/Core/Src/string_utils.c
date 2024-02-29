#include "string_utils.h"

uint32_t strtoul_len(const char* str, size_t len)
{
    if (str == NULL)
    {
        return 0;
    }

    uint32_t ret = 0;
    for(size_t i = 0; i < len; ++i)
    {
        if ((str[i] >= '0')  && (str[i] <= '9'))
        {
            ret = ret * 10 + (str[i] - '0');
        }
        else { return 0; }
    }
    return ret;
}

size_t strcntchar(char* start, char* stop, const char nibble)
{
    if (start == NULL || stop == NULL)
    {
        return 0;
    }

    size_t cnt = 0;
    
    for (char* ptr = start; ptr < stop; ptr++)
    {
        if (*ptr == nibble)
        {
            cnt++;
        }
    }

    return cnt;
}

void strtrim(char *str, char c)
{
    size_t index = 0;
    size_t shift_count = 0;
    size_t length = strlen(str);

    // Trim start
    while(index < length)
    {
        if (str[index] == c)
        {
            shift_count++;
        }
        else
        {
            break;
        }
        index++;
    }

    index = 0;
    while(index <= length - shift_count + 1)
    {
        str[index] = str[index + shift_count];
        index++;
    }

    // Trim end
    length -= shift_count;
    index = length;
    while(index)
    {
        if (str[index - 1] == c)
        {
            str[index - 1] = 0;
        }
        else
        {
            return;
        }
        index--;
    }
}
