#include "clock.h"

#include <clint.h>
#include <memory_map.h>
#include <prci.h>
#include <spi.h>

#define ROSC_DIV(x) ((x)&0x2Fu)
#define ROSC_TRIM(x) (((x)&0x1Fu) << 16)
#define ROSC_EN 0x40000000u
#define ROSC_RDY 0x80000000u

#define PLL_R 0x00000007UL
#define PLL_F 0x000003F0UL
#define PLL_Q 0x00000C00UL
#define PLL_R_SHIFT(r) ((r << 0) & PLL_R)
#define PLL_F_SHIFT(f) ((f << 4) & PLL_F)
#define PLL_Q_SHIFT(q) ((q << 10) & PLL_Q)
#define PLL_SEL 0x10000u
#define PLL_HFXOSC_SEL 0x20000u
#define PLL_BYPASS 0x40000u
#define PLL_LOCK 0x80000000u

unsigned long get_timer_freq() { return 32768; }

unsigned long mtime_lo(void) { return clint_reg(CLINT_REG_MTIME); }

uint32_t mtime_hi(void) { return clint_reg(CLINT_REG_MTIME + 4); }

uint64_t get_timer_value() {
  uint32_t hi = mtime_hi();
  uint32_t lo = mtime_lo();
  return ((uint64_t)hi << 32) | lo;
}

void clock_init_hfrosc(unsigned int div, unsigned int trim) {
  // configure ring oscillator to default setting
  prci_reg(PRCI_REG_HFROSC) |= (ROSC_DIV(div) | ROSC_TRIM(trim) | ROSC_EN);
  // wait for ring oscillator to be ready
  while ((prci_reg(PRCI_REG_HFROSC) & ROSC_RDY) == 0)
    ;
  // select ring oscillator
  prci_reg(PRCI_REG_PLL_CFG) &= ~PLL_SEL;
  // warm up clock
  measure_cpu_freq(100);
}

void clock_init_hfxosc() {
  if (prci_reg(PRCI_REG_PLL_CFG) & PLL_SEL) {
    clock_init_hfrosc(4, 16);
  }
  // "overclock" the QSPI a bit since it
  // should be able to run at a max of 100MHz
  // and this tops it out at 8MHz
  qspi0_reg(SPI_REG_SCK_DIV) = 0;
  // set pll divide by one
  prci_reg(PRCI_REG_PLL_DIV) = 0x100u;
  // select pll select, reference, and bypass
  prci_reg(PRCI_REG_PLL_CFG) = (PLL_SEL | PLL_BYPASS | PLL_HFXOSC_SEL);
  // disable hfrosc
  prci_reg(PRCI_REG_HFROSC) &= ~ROSC_EN;
  // warm up clock
  measure_cpu_freq(100);
}

void clock_init_hfpll(unsigned int r, unsigned int f, unsigned int q) {
  if (prci_reg(PRCI_REG_PLL_CFG) & PLL_SEL) {
    clock_init_hfrosc(4, 16);
  }
  // need to set QSPI clock divider temporarily
  qspi0_reg(SPI_REG_SCK_DIV) = 4;
  // configure pll
  prci_reg(PRCI_REG_PLL_CFG) =
      (PLL_BYPASS | PLL_R_SHIFT(r) | PLL_F_SHIFT(f) | PLL_Q_SHIFT(q));

  // switch to pll
  prci_reg(PRCI_REG_PLL_CFG) &= ~PLL_BYPASS;
  // wait 100us
  uint32_t now = mtime_lo();
  while (mtime_lo() - now < 4)
    ;
  // Now it is safe to check for PLL Lock
  while ((prci_reg(PRCI_REG_PLL_CFG) & PLL_LOCK) == 0)
    ;

  prci_reg(PRCI_REG_PLL_CFG) |= PLL_SEL;
  // warm up clock
  measure_cpu_freq(100);

  // calculate new QSPI divisor for max performance
  unsigned long fin = get_cpu_freq();
  unsigned int new_div = 0;
  while (fin / (2 * (new_div + 1)) > 50000000u) {
    new_div++;
  }
  qspi0_reg(SPI_REG_SCK_DIV) = new_div;
}

unsigned long __attribute__((noinline)) measure_cpu_freq(size_t n) {
  unsigned long start_mtime, delta_mtime;
  unsigned long mtime_freq = get_timer_freq();

  // Don't start measuruing until we see an mtime tick
  unsigned long tmp = mtime_lo();
  do {
    start_mtime = mtime_lo();
  } while (start_mtime == tmp);

  unsigned long start_mcycle;
  __asm__ volatile("csrr %0, mcycle" : "=r"(start_mcycle));

  do {
    delta_mtime = mtime_lo() - start_mtime;
  } while (delta_mtime < n);

  __asm__ volatile("csrr %0, mcycle" : "=r"(tmp));

  unsigned long delta_mcycle = tmp - start_mcycle;
  unsigned long ret = (delta_mcycle / delta_mtime) * mtime_freq +
                      ((delta_mcycle % delta_mtime) * mtime_freq) / delta_mtime;
  // __asm__("fence.i");
  // __asm__("fence");
  return ret;
}

unsigned long get_cpu_freq() { return measure_cpu_freq(50); }