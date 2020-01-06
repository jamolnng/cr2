#include "clock.h"

#include <clint.h>
#include <memory_map.h>
#include <prci.h>

unsigned long get_timer_freq() { return 32768; }

unsigned long mtime_lo(void) { return clint_reg(CLINT_REG_MTIME); }

uint32_t mtime_hi(void) { return clint_reg(CLINT_REG_MTIME + 4); }

uint64_t get_timer_value() {
  uint32_t hi = mtime_hi();
  uint32_t lo = mtime_lo();
  return ((uint64_t)hi << 32) | lo;
}

void clock_init() {
  // enable hfrosc
  prci_reg(PRCI_REG_HFROSC) |= 0x40000000ul;

  // configure pll
  prci_reg(PRCI_REG_PLL_CFG) |= 0x00060000ul;

  // switch to pll
  prci_reg(PRCI_REG_PLL_CFG) |= 0x00010000ul;

  // disable hfrosc
  prci_reg(PRCI_REG_HFROSC) &= ~0x40000000ul;
  get_cpu_freq();  // warm up clock
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

unsigned long get_cpu_freq() { return measure_cpu_freq(10); }