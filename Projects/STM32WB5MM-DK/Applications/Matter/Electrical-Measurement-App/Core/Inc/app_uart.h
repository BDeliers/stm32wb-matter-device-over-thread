#ifndef APP_UART_H
#define APP_UART_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

// Debug UART related functions
bool AppUart_InitDebug(void);
bool AppUart_TransmitDmaDebug(uint8_t* data, size_t size, void (*callback)(void));
bool AppUart_TransmitDebug(uint8_t* data, size_t size);

// External UART related functions
bool AppUart_InitExternal(void (*rx_callback)(uint16_t));
bool AppUart_ReceiveFrameExternal(uint8_t* buff, uint16_t buff_size);
void AppUart_NotifyIdleEventExternal(void);

#ifdef __cplusplus
}
#endif

#endif // APP_UART_H
