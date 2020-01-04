#ifndef __CR2_PRCI_H__
#define __CR2_PRCI_H__

#include <memory_map.h>

#define PRCI_REG_HFROSC 0x00
#define PRCI_REG_HFXOSC 0x04
#define PRCI_REG_PLL_CFG 0x08
#define PRCI_REG_PLL_DIV 0x0C
#define PRCI_REG_PROC_MON_CFG 0xF0

#define prci_reg(reg) mmio(PRCI_CTRL_ADDR, reg)

#endif