#include <clock.h>
#include <uart.h>

extern int main();
void _start(void);

__attribute__((section(".init"))) void _start(void) {
  clock_init();
  uart_init(UART0, 115200);
  main();
}