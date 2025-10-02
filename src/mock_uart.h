#ifndef _MOCK_UART_H
#define _MOCK_UART_H

void mock_uart_init();
void mock_uart_tx(char da);
int mock_uart_rx(char*cp, int maxn, unsigned int timeout_ms);
#define MOCK_UART_BAUTRATE_115200 400000u
#define MOCK_UART_BAUTRATE_9600 9000u
#define MOCK_UART_BAUTRATE MOCK_UART_BAUTRATE_9600
#endif
