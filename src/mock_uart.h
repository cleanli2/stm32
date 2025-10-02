#ifndef _MOCK_UART_H
#define _MOCK_UART_H

void mock_uart_init();
void mock_uart_tx(char da);
int mock_uart_rx(char*cp, int maxn, int timeout_ms);
#endif
