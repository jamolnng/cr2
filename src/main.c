#include <gpio.h>
#include <memory_map.h>
#include <stdlib.h>
#include <string.h>
#include <uart.h>

#define read_csr(reg)                             \
  ({                                              \
    unsigned long __tmp;                          \
    asm volatile("csrr %0, " #reg : "=r"(__tmp)); \
    __tmp;                                        \
  })

#define write_csr(reg, val)                                     \
  ({                                                            \
    if (__builtin_constant_p(val) && (unsigned long)(val) < 32) \
      asm volatile("csrw " #reg ", %0" ::"i"(val));             \
    else                                                        \
      asm volatile("csrw " #reg ", %0" ::"r"(val));             \
  })

#define swap_csr(reg, val)                                             \
  ({                                                                   \
    unsigned long __tmp;                                               \
    if (__builtin_constant_p(val) && (unsigned long)(val) < 32)        \
      asm volatile("csrrw %0, " #reg ", %1" : "=r"(__tmp) : "i"(val)); \
    else                                                               \
      asm volatile("csrrw %0, " #reg ", %1" : "=r"(__tmp) : "r"(val)); \
    __tmp;                                                             \
  })

#define set_csr(reg, bit)                                              \
  ({                                                                   \
    unsigned long __tmp;                                               \
    if (__builtin_constant_p(bit) && (unsigned long)(bit) < 32)        \
      asm volatile("csrrs %0, " #reg ", %1" : "=r"(__tmp) : "i"(bit)); \
    else                                                               \
      asm volatile("csrrs %0, " #reg ", %1" : "=r"(__tmp) : "r"(bit)); \
    __tmp;                                                             \
  })

#define clear_csr(reg, bit)                                            \
  ({                                                                   \
    unsigned long __tmp;                                               \
    if (__builtin_constant_p(bit) && (unsigned long)(bit) < 32)        \
      asm volatile("csrrc %0, " #reg ", %1" : "=r"(__tmp) : "i"(bit)); \
    else                                                               \
      asm volatile("csrrc %0, " #reg ", %1" : "=r"(__tmp) : "r"(bit)); \
    __tmp;                                                             \
  })

#define GREEN_LED 0x00080000ul
#define BLUE_LED 0x00200000ul
#define RED_LED 0x00400000ul

#define GLOBAL_INT_EN 0x00000008ul
#define TMR_INT_EN 0

#define CLINT_MTIME 0xBFF8
#define CLINT_MTIMECMP 0x4000

#define IRQ_M_TIMER 7
#define MIP_MTIP (1 << IRQ_M_TIMER)

#define MCAUSE_INT 0x80000000
#define MCAUSE_CAUSE 0x7FFFFFFF

#define MSTATUS_MIE 0x00000008

extern void trap_entry(void);

void set_timer() {
  volatile uint64_t* mtime = (uint64_t*)(CLINT_CTRL_ADDR + CLINT_MTIME);
  volatile uint64_t* mtimecmp = (uint64_t*)(CLINT_CTRL_ADDR + CLINT_MTIMECMP);
  uint64_t now = *mtime;
  uint64_t then = now + 32768;
  *mtimecmp = then;
}

void isr() {
  mmio_write_u32(
      GPIO_CTRL_ADDR, GPIO_REG_OUTPUT_VAL,
      mmio_read_u32(GPIO_CTRL_ADDR, GPIO_REG_OUTPUT_VAL) ^ GREEN_LED);

  set_timer();
}

uintptr_t handle_trap(uintptr_t mcause, uintptr_t epc) {
  if ((mcause & MCAUSE_INT) && ((mcause & MCAUSE_CAUSE) == IRQ_M_TIMER)) {
    isr();
  }
  return epc;
}

int main() {
  uint32_t mmval;
  mmval = mmio_read_u32(GPIO_CTRL_ADDR, GPIO_REG_OUTPUT_EN);
  mmio_write_u32(GPIO_CTRL_ADDR, GPIO_REG_OUTPUT_EN,
                 mmval | RED_LED | GREEN_LED);
  mmval = mmio_read_u32(GPIO_CTRL_ADDR, GPIO_REG_OUTPUT_VAL);
  mmio_write_u32(GPIO_CTRL_ADDR, GPIO_REG_OUTPUT_VAL, mmval | RED_LED);

  //
  clear_csr(mstatus, MSTATUS_MIE);
  clear_csr(mie, MIP_MTIP);

  set_timer();

  /* Enable timer and all interrupts */
  set_csr(mie, MIP_MTIP);
  set_csr(mstatus, MSTATUS_MIE);
  set_csr(mtvec, &trap_entry);
  //

  int i;
  for (;;) {
    // for (i = 0; i < 1000000; ++i)
    //   ;
    // mmval = mmio_read_u32(GPIO_CTRL_ADDR, GPIO_REG_OUTPUT_VAL);
    // mmio_write_u32(GPIO_CTRL_ADDR, GPIO_REG_OUTPUT_VAL, mmval ^ RED_LED);
  }

  return 0;
}