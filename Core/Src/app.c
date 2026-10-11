
#include "app.h"
#include "uart_driver.h"
#include "stm32f4xx_nucleo.h"

#include <stdint.h>

#define LED_TOGGLE_PERIOD_MS  500U
#define UART_RX_TIMEOUT_MS     10U

static uint32_t last_led_tick = 0U;

void App_Init(void)
{
    last_led_tick = HAL_GetTick();

    UART_Driver_SendString(
        "\r\n"
        "================================\r\n"
        " STM32 Diagnostic Gateway\r\n"
        " Board    : NUCLEO-F446RE\r\n"
        " Firmware : v0.2.0\r\n"
        " Status   : BOOT_OK\r\n"
        "================================\r\n"
        "\r\n"
        "UART Echo Test Ready\r\n"
        "Type characters to test.\r\n"
    );
}

void App_Run(void)
{
    uint8_t rx_byte = 0U;

    /* Periodic LED heartbeat */
    uint32_t current_tick = HAL_GetTick();

    if ((uint32_t)(current_tick - last_led_tick)
        >= LED_TOGGLE_PERIOD_MS)
    {
        BSP_LED_Toggle(LED2);
        last_led_tick = current_tick;
    }

    /* Receive one character from the PC */
    HAL_StatusTypeDef status =
        UART_Driver_ReceiveByte(
            &rx_byte,
            UART_RX_TIMEOUT_MS
        );

    if (status == HAL_OK)
    {
        /* Echo received character */
        UART_Driver_Send(&rx_byte, 1U);
    }
}
