
#include "app.h"
#include "uart_driver.h"
#include "diag_console.h"
#include "led_service.h"

#include <stdint.h>

#define UART_RX_TIMEOUT_MS 10U

void App_Init(void)
{
    /* BSP_LED_Init(LED2) has already run in main.c */
    LedService_Init();

    UART_Driver_SendString(
        "\r\n"
        "================================\r\n"
        " STM32 Diagnostic Gateway\r\n"
        " Board    : NUCLEO-F446RE\r\n"
        " Firmware : v0.3.0-dev\r\n"
        " Status   : BOOT_OK\r\n"
        "================================\r\n"
    );

    DiagConsole_Init();
}

void App_Run(void)
{
    uint8_t rx_byte = 0U;

    /* Nonblocking LED heartbeat scheduler */
    LedService_Process();

    /* UART receive: temporarily polling with timeout */
    HAL_StatusTypeDef status =
        UART_Driver_ReceiveByte(
            &rx_byte,
            UART_RX_TIMEOUT_MS
        );

    if (status == HAL_OK)
    {
        DiagConsole_ProcessByte(rx_byte);
    }
}
