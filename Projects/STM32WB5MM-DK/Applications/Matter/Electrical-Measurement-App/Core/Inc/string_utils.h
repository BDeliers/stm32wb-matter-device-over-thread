#ifndef STRING_UTILS_H
#define STRING_UTILS_H

#include <string.h>
#include <stdint.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

static const char ASCII_STX   = 0x02;
static const char ASCII_ETX   = 0x03;
static const char ASCII_HTAB  = 0x09;
static const char ASCII_CR    = 0x0D;
static const char ASCII_LF    = 0x0A;
static const char ASCII_SPACE = 0x20;

uint32_t strtoul_len(const char* str, size_t len);
size_t strcntchar(char* start, char* stop, const char nibble);
void strtrim(char *str, char c);

#ifdef __cplusplus
}
#endif

#endif // STRING_UTILS_H
