#include "clock.h"

#include "memory_map.h"
#include "prci.h"

unsigned long get_timer_freq() { return 32768; }

unsigned long mtime_lo(void) {
  return *(volatile unsigned long *)(CLINT_CTRL_ADDR + CLINT_MTIME);
}

uint32_t mtime_hi(void) {
  return *(volatile uint32_t *)(CLINT_CTRL_ADDR + CLINT_MTIME + 4);
}

void clock_init() {
  // enable hfrosc
  prci_reg(PRCI_REG_HFROSC) |= 0x40000000;

  // configure pll
  prci_reg(PRCI_REG_PLL_CFG) |= 0x00060000;

  // switch to pll
  prci_reg(PRCI_REG_PLL_CFG) |= 0x00010000;

  // disable hfrosc
  prci_reg(PRCI_REG_HFROSC) &= ~0x40000000;
}

unsigned long __attribute__((noinline)) measure_cpu_freq(size_t n) {
  unsigned long start_mtime, delta_mtime;
  unsigned long mtime_freq = get_timer_freq();

  // Don't start measuruing until we see an mtime tick
  unsigned long tmp = mtime_lo();
  do {
    start_mtime = mtime_lo();
  } while (start_mtime == tmp);

  unsigned long start_mcycle = read_csr(mcycle);

  do {
    delta_mtime = mtime_lo() - start_mtime;
  } while (delta_mtime < n);

  unsigned long delta_mcycle = read_csr(mcycle) - start_mcycle;

  return (delta_mcycle / delta_mtime) * mtime_freq +
         ((delta_mcycle % delta_mtime) * mtime_freq) / delta_mtime;
}

unsigned long get_cpu_freq() { return measure_cpu_freq(10000); }