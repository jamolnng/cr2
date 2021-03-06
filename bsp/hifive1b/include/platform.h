#ifndef __CR2_PLATFORM_H__
#define __CR2_PLATFORM_H__

#define GREEN_LED 0x00080000ul
#define BLUE_LED 0x00200000ul
#define RED_LED 0x00400000ul

#define GLOBAL_INT_EN 0x00000008ul
#define TMR_INT_EN 0

#define IRQ_M_TIMER 7
#define ERQ_M_ECALL 11
#define MIP_MTIP (1 << IRQ_M_TIMER)

#define MCAUSE_INT 0x80000000
#define MCAUSE_CAUSE 0x7FFFFFFF

#define MSTATUS_MIE 0x00000008

#endif