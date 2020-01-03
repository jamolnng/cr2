#include "clock.h"

#include "memory_map.h"
#include "prci.h"

void clock_init() {
  // enable hfrosc
  uint32_t mmval = mmio_read_u32(PRCI_CTRL_ADDR, PRCI_REG_HFROSC);
  mmio_write_u32(PRCI_CTRL_ADDR, PRCI_REG_HFROSC, mmval | 0x40000000);

  // configure pll
  mmval = mmio_read_u32(PRCI_CTRL_ADDR, PRCI_REG_PLL_CFG);
  mmio_write_u32(PRCI_CTRL_ADDR, PRCI_REG_PLL_CFG, mmval | 0x00060000);

  // switch to pll
  mmval = mmio_read_u32(PRCI_CTRL_ADDR, PRCI_REG_PLL_CFG);
  mmio_write_u32(PRCI_CTRL_ADDR, PRCI_REG_PLL_CFG, mmval | 0x00010000);

  // disable hfrosc
  mmval = mmio_read_u32(PRCI_CTRL_ADDR, PRCI_REG_HFROSC);
  mmio_write_u32(PRCI_CTRL_ADDR, PRCI_REG_HFROSC, mmval & ~0x40000000);
}

uint32_t clock_freq() { return 16000000; }