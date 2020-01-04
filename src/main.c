#include <clint.h>
#include <clock.h>
#include <gpio.h>
#include <memory_map.h>
#include <platform.h>
#include <stdlib.h>
#include <string.h>
#include <uart.h>

extern void trap_entry(void);

void set_timer(void);
void timer_isr(void);
int main();
uintptr_t handle_trap(uintptr_t mcause, uintptr_t epc);

void set_timer(void) {
  // 50Hz (655 * 32 + 656 * 18) = 32768
  uint64_t next = get_timer_value() + 655;
  clint_reg(CLINT_REG_MTIMECMP) = (uint32_t)(next);
  clint_reg(CLINT_REG_MTIMECMP + 4) = (uint32_t)(next >> 32);
}

void timer_isr(void) {
  gpio_reg(GPIO_REG_OUTPUT_VAL) ^= RED_LED;
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
  ultoa(get_cpu_freq(), &str[15], 63, 10);
  uart_puts(UART0, str, strlen(str));

  clear_csr(mstatus, MSTATUS_MIE);
  clear_csr(mie, MIP_MTIP);

  set_timer();

  /* Enable timer and all interrupts */
  set_csr(mtvec, &trap_entry);
  set_csr(mie, MIP_MTIP);
  set_csr(mstatus, MSTATUS_MIE);

  int i;
  for (;;) {
    for (i = 0; i < 100000; ++i)
      ;
  }

  return 0;
}