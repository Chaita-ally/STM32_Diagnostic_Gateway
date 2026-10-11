
#ifndef DIAG_CONSOLE_H
#define DIAG_CONSOLE_H

#include <stdint.h>

#define DIAG_CMD_BUFFER_SIZE 64U

void DiagConsole_Init(void);
void DiagConsole_ProcessByte(uint8_t byte);

#endif
