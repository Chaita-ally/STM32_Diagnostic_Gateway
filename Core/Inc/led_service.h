
#ifndef LED_SERVICE_H
#define LED_SERVICE_H

#include <stdint.h>

typedef enum
{
    LED_MODE_AUTO = 0,
    LED_MODE_ON,
    LED_MODE_OFF,
    LED_MODE_MANUAL
} LedMode_t;

void LedService_Init(void);
void LedService_Process(void);

void LedService_SetMode(LedMode_t mode);
void LedService_Toggle(void);
LedMode_t LedService_GetMode(void);

#endif
