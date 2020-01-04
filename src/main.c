#include <clock.h>
#include <gpio.h>
#include <memory_map.h>
#include <platform.h>
#include <stdlib.h>
#include <string.h>
#include <uart.h>

extern void trap_entry(void);

void set_timer() {
  volatile uint64_t* mtime = (uint64_t*)(CLINT_CTRL_ADDR + CLINT_MTIME);
  volatile uint64_t* mtimecmp = (uint64_t*)(CLINT_CTRL_ADDR + CLINT_MTIMECMP);
  uint64_t now = *mtime;
  uint64_t then = now + 32768;
  *mtimecmp = then;
}

void timer_isr() {
  gpio_reg(GPIO_REG_OUTPUT_VAL) ^= GREEN_LED;
  set_timer();
}

uintptr_t handle_trap(uintptr_t mcause, uintptr_t epc) {
  if ((mcause & MCAUSE_INT) && ((mcause & MCAUSE_CAUSE) == IRQ_M_TIMER)) {
    timer_isr();
  }
  return epc;
}

int main() {
  gpio_reg(GPIO_REG_OUTPUT_VAL) |= RED_LED | GREEN_LED;
  gpio_reg(GPIO_REG_OUTPUT_EN) |= RED_LED | GREEN_LED;

  char str[64] = "Running at Hz: ";
  itoa(get_cpu_freq(), &str[15], 63, 10);
  uart_puts(UART0, str, strlen(str));

  clear_csr(mstatus, MSTATUS_MIE);
  clear_csr(mie, MIP_MTIP);

  set_timer();

  /* Enable timer and all interrupts */
  set_csr(mie, MIP_MTIP);
  set_csr(mstatus, MSTATUS_MIE);
  set_csr(mtvec, &trap_entry);

  for (;;) {
  }

  return 0;
}