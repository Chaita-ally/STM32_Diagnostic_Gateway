
#include "uart_driver.h"
#include "usart.h"

#include <string.h>

HAL_StatusTypeDef UART_Driver_Send(
    const uint8_t *data,
    uint16_t length
)
{
    if ((data == NULL) || (length == 0U))
    {
        return HAL_ERROR;
    }

    return HAL_UART_Transmit(
        &huart2,
        (uint8_t *)data,
        length,
        1000U
    );
}

HAL_StatusTypeDef UART_Driver_ReceiveByte(
    uint8_t *data,
    uint32_t timeout_ms
)
{
    if (data == NULL)
    {
        return HAL_ERROR;
    }

    return HAL_UART_Receive(
        &huart2,
        data,
        1U,
        timeout_ms
    );
}

HAL_StatusTypeDef UART_Driver_SendString(
    const char *str
)
{
    if (str == NULL)
    {
        return HAL_ERROR;
    }

    size_t length = strlen(str);

    if (length == 0U)
    {
        return HAL_OK;
    }

    if (length > UINT16_MAX)
    {
        return HAL_ERROR;
    }

    return UART_Driver_Send(
        (const uint8_t *)str,
        (uint16_t)length
    );
}
