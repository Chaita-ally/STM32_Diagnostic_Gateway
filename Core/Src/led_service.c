
#include "led_service.h"
#include "stm32f4xx_nucleo.h"
#include "stm32f4xx_hal.h"

#define LED_HEARTBEAT_PERIOD_MS 500U

static LedMode_t current_mode = LED_MODE_AUTO;
static uint32_t last_toggle_tick = 0U;

void LedService_Init(void)
{
    current_mode = LED_MODE_AUTO;
    last_toggle_tick = HAL_GetTick();

    BSP_LED_Off(LED2);
}

void LedService_Process(void)
{
    if (current_mode != LED_MODE_AUTO)
    {
        return;
    }

    uint32_t now = HAL_GetTick();

    if ((uint32_t)(now - last_toggle_tick)
        >= LED_HEARTBEAT_PERIOD_MS)
    {
        BSP_LED_Toggle(LED2);
        last_toggle_tick = now;
    }
}

void LedService_SetMode(LedMode_t mode)
{
    switch (mode)
    {
        case LED_MODE_AUTO:
            current_mode = LED_MODE_AUTO;
            last_toggle_tick = HAL_GetTick();
            break;

        case LED_MODE_ON:
            current_mode = LED_MODE_ON;
            BSP_LED_On(LED2);
            break;

        case LED_MODE_OFF:
            current_mode = LED_MODE_OFF;
            BSP_LED_Off(LED2);
            break;

        case LED_MODE_MANUAL:
            current_mode = LED_MODE_MANUAL;
            break;

        default:
            break;
    }
}

void LedService_Toggle(void)
{
    BSP_LED_Toggle(LED2);
    current_mode = LED_MODE_MANUAL;
}

LedMode_t LedService_GetMode(void)
{
    return current_mode;
}
