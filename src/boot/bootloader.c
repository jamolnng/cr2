#include <clock.h>
#include <uart.h>

extern int main();
void _start(void);

#define MSTATUS_FLAG 0x8
#define MIE_FLAG 0xAAA

__attribute__((section(".init"))) void _start(void) {
  // disable global interrupts
  __asm__("csrci mstatus, %0" ::"i"(MSTATUS_FLAG));
  // disable all interrupts
  __asm__("csrc mie, %0" ::"r"(MIE_FLAG));
  // init 16MHz clock
  clock_init();
  // enable UART0, may remove later
  uart_init(UART0, 115200);
  main();
}