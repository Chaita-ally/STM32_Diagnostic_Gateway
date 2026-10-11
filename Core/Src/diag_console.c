
#include "diag_console.h"
#include "uart_driver.h"
#include "led_service.h"
#include <stdio.h>
#include <string.h>

static char cmd_buffer[DIAG_CMD_BUFFER_SIZE];

static uint16_t cmd_index = 0U;
static uint8_t cmd_overflow = 0U;
static uint8_t previous_was_cr = 0U;

static void DiagConsole_PrintPrompt(void)
{
    UART_Driver_SendString("gateway> ");
}

void DiagConsole_Init(void)
{
    memset(cmd_buffer, 0, sizeof(cmd_buffer));

    cmd_index = 0U;
    cmd_overflow = 0U;
    previous_was_cr = 0U;

    UART_Driver_SendString(
        "\r\nDiagnostic Console Ready\r\n"
        "Enter a command and press Enter.\r\n\r\n"
    );

    DiagConsole_PrintPrompt();
}


static void DiagConsole_ExecuteCommand(const char *cmd)
{
    if (strcmp(cmd, "help") == 0)
    {
        UART_Driver_SendString(
            "Available commands:\r\n"
            " help\r\n"
            " version\r\n"
            " uptime\r\n"
            " status\r\n"
            " led on\r\n"
            " led off\r\n"
            " led toggle\r\n"
            " led auto\r\n"
        );
    }
    else if (strcmp(cmd, "version") == 0)
    {
        UART_Driver_SendString(
            "Firmware: v0.3.0-dev\r\n"
            "MCU: STM32F446RE\r\n"
            "Core: ARM Cortex-M4F\r\n"
        );
    }
    else if (strcmp(cmd, "uptime") == 0)
    {
        char response[64];

        unsigned long uptime_seconds =
            (unsigned long)(HAL_GetTick() / 1000U);

        snprintf(
            response,
            sizeof(response),
            "Uptime: %lu seconds\r\n",
            uptime_seconds
        );

        UART_Driver_SendString(response);
    }
    else if (strcmp(cmd, "status") == 0)
    {
        const char *led_mode = "UNKNOWN";

        switch (LedService_GetMode())
        {
            case LED_MODE_AUTO:
                led_mode = "AUTO";
                break;
            case LED_MODE_ON:
                led_mode = "ON";
                break;
            case LED_MODE_OFF:
                led_mode = "OFF";
                break;
            case LED_MODE_MANUAL:
                led_mode = "MANUAL";
                break;
            default:
                break;
        }

        UART_Driver_SendString(
            "System: RUNNING\r\n"
            "UART: ACTIVE\r\n"
            "LED mode: "
        );
        UART_Driver_SendString(led_mode);
        UART_Driver_SendString("\r\n");
    }
    else if (strcmp(cmd, "led on") == 0)
    {
        LedService_SetMode(LED_MODE_ON);
        UART_Driver_SendString("LED turned ON\r\n");
    }
    else if (strcmp(cmd, "led off") == 0)
    {
        LedService_SetMode(LED_MODE_OFF);
        UART_Driver_SendString("LED turned OFF\r\n");
    }
    else if (strcmp(cmd, "led toggle") == 0)
    {
        LedService_Toggle();
        UART_Driver_SendString("LED toggled\r\n");
    }
    else if (strcmp(cmd, "led auto") == 0)
    {
        LedService_SetMode(LED_MODE_AUTO);
        UART_Driver_SendString(
            "LED heartbeat enabled\r\n"
        );
    }
    else
    {
        UART_Driver_SendString(
            "ERROR: Unknown command\r\n"
        );
    }
}


void DiagConsole_ProcessByte(uint8_t byte)
{
    /* Handle CR, LF, or CRLF line endings */
    if ((byte == '\r') || (byte == '\n'))
    {
        /* Ignore LF immediately following CR */
        if ((byte == '\n') && (previous_was_cr != 0U))
        {
            previous_was_cr = 0U;
            return;
        }

        previous_was_cr = (byte == '\r') ? 1U : 0U;

        UART_Driver_SendString("\r\n");

        if (cmd_overflow != 0U)
        {
            UART_Driver_SendString(
                "ERROR: Command too long\r\n"
            );
        }
        else if (cmd_index > 0U)
        {
            cmd_buffer[cmd_index] = '\0';

            DiagConsole_ExecuteCommand(cmd_buffer);
        }

        /* Reset the command buffer */
        cmd_index = 0U;
        cmd_overflow = 0U;

        DiagConsole_PrintPrompt();
        return;
    }

    previous_was_cr = 0U;

    /* Handle Backspace and Delete */
    if ((byte == '\b') || (byte == 0x7FU))
    {
        if ((cmd_index > 0U) &&
            (cmd_overflow == 0U))
        {
            cmd_index--;
            cmd_buffer[cmd_index] = '\0';

            UART_Driver_SendString("\b \b");
        }
        return;
    }

    /* Ignore non-printable characters */
    if ((byte < 0x20U) || (byte > 0x7EU))
    {
        return;
    }

    /* Reject remaining input after overflow */
    if (cmd_overflow != 0U)
    {
        return;
    }

    /* Store character, reserving null terminator */
    if (cmd_index < (DIAG_CMD_BUFFER_SIZE - 1U))
    {
        cmd_buffer[cmd_index++] = (char)byte;

        /* Echo typed character */
        UART_Driver_Send(&byte, 1U);
    }
    else
    {
        cmd_overflow = 1U;
    }
}
