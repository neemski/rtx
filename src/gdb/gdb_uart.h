#ifndef _GDB_UART_H_
#define _GDB_UART_H_

int gdb_uart_init();

VOID gdb_uart_out_char(CHAR c);

CHAR gdb_uart_in_char();


#endif
