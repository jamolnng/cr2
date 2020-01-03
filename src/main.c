#include <stdlib.h>
#include <string.h>
#include <uart.h>

int main() {
  char str[21];
  itoa(115200, str, 20, 10);
  uart_puts(UART0, str, strlen(str));
  uart_putc(UART0, '\n');
  uart_putc(UART0, '\r');
  itoa(-9600, str, 20, 10);
  uart_puts(UART0, str, strlen(str));
  uart_putc(UART0, '\n');
  uart_putc(UART0, '\r');
  for (;;) {
    uart_puts(UART0, "> ", 2);
    uart_gets(UART0, str, 20);
    uart_putc(UART0, '\n');
    uart_putc(UART0, '\r');
    int l = strlen(str);
    if (l > 0) {
      uart_puts(UART0, str, l);
      uart_putc(UART0, '\n');
      uart_putc(UART0, '\r');
    }
  }
  return 0;
}