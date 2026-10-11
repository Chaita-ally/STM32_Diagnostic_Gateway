
#include "diag_console.h"
#include "uart_driver.h"

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

            UART_Driver_SendString("Received: ");
            UART_Driver_SendString(cmd_buffer);
            UART_Driver_SendString("\r\n");
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
