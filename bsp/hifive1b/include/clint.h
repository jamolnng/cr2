#ifndef __CR2_CLINT_H__
#define __CR2_CLINT_H__

#include <memory_map.h>

#define CLINT_REG_MTIME 0xBFF8
#define CLINT_REG_MTIMECMP 0x4000

#define clint_reg(reg) mmio(CLINT_CTRL_ADDR, reg)

#endif