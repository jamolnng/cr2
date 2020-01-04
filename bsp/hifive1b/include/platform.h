#ifndef __CR2_PLATFORM_H__
#define __CR2_PLATFORM_H__

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

#define IRQ_M_TIMER 7
#define MIP_MTIP (1 << IRQ_M_TIMER)

#define MCAUSE_INT 0x80000000
#define MCAUSE_CAUSE 0x7FFFFFFF

#define MSTATUS_MIE 0x00000008

#endif