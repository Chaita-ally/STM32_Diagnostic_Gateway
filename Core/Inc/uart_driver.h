
#ifndef UART_DRIVER_H
#define UART_DRIVER_H

#include <stdint.h>
#include <stddef.h>
#include "stm32f4xx_hal.h"

/* Send bytes over the diagnostic UART */
HAL_StatusTypeDef UART_Driver_Send(
    const uint8_t *data,
    uint16_t length
);

/* Receive one byte with a timeout */
HAL_StatusTypeDef UART_Driver_ReceiveByte(
    uint8_t *data,
    uint32_t timeout_ms
);

/* Send a null-terminated string */
HAL_StatusTypeDef UART_Driver_SendString(
    const char *str
);

#endif
