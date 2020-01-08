#ifndef __CR2_CTX_SWITCH_ASM__
#define __CR2_CTX_SWITCH_ASM__

/*
stack pointer already being saved to cr2_current_thread
global pointer does not change
thread pointer does not change
cr2 context frame:
29(sp) mepc/pc
       mstatus
       x31
       x30
       x29
       x28
       x27
       x26
       x25
       x24
       x23
       x22
       x21
       x20
       x19
       x18
       x17
       x16
       x15
       x14
       x13
       x12
       x11
       x10
       x9
       x8
       x7
       x6
       x5
0(sp)  x1
*/

.set CR2_WORD_SIZE, 4
.set CR2_CTX_SIZE, 30 * CR2_WORD_SIZE

.global cr2_isr_stack_top
.global cr2_current_thread
.global cr2_trap_vec_entry
.global cr2_start_first_task

.align 2
.func
cr2_trap_vec_entry:
  # make room in current thread stack
  # RISC-V 32 words are 4 bytes wide
  # hold all of registers except sp, gp, and tp
  addi sp,  sp, -CR2_CTX_SIZE

  # store registers
  sw   x1,   0 * CR2_WORD_SIZE(sp)  # ra
  sw   x5,   1 * CR2_WORD_SIZE(sp)  # t0
  sw   x6,   2 * CR2_WORD_SIZE(sp)  # t1
  sw   x7,   3 * CR2_WORD_SIZE(sp)  # t2
  sw   x8,   4 * CR2_WORD_SIZE(sp)  # s0/fp
  sw   x9,   5 * CR2_WORD_SIZE(sp)  # s1
  sw   x10,  6 * CR2_WORD_SIZE(sp)  # a0
  sw   x11,  7 * CR2_WORD_SIZE(sp)  # a1
  sw   x12,  8 * CR2_WORD_SIZE(sp)  # a2
  sw   x13,  9 * CR2_WORD_SIZE(sp)  # a3
  sw   x14, 10 * CR2_WORD_SIZE(sp)  # a4
  sw   x15, 11 * CR2_WORD_SIZE(sp)  # a5
  sw   x16, 12 * CR2_WORD_SIZE(sp)  # a6
  sw   x17, 13 * CR2_WORD_SIZE(sp)  # a7
  sw   x18, 14 * CR2_WORD_SIZE(sp)  # s2
  sw   x19, 15 * CR2_WORD_SIZE(sp)  # s3
  sw   x20, 16 * CR2_WORD_SIZE(sp)  # s4
  sw   x21, 17 * CR2_WORD_SIZE(sp)  # s5
  sw   x22, 18 * CR2_WORD_SIZE(sp)  # s6
  sw   x23, 19 * CR2_WORD_SIZE(sp)  # s7
  sw   x24, 20 * CR2_WORD_SIZE(sp)  # s8
  sw   x25, 21 * CR2_WORD_SIZE(sp)  # s9
  sw   x26, 22 * CR2_WORD_SIZE(sp)  # s10
  sw   x27, 23 * CR2_WORD_SIZE(sp)  # s11
  sw   x28, 24 * CR2_WORD_SIZE(sp)  # t3
  sw   x29, 25 * CR2_WORD_SIZE(sp)  # t4
  sw   x30, 26 * CR2_WORD_SIZE(sp)  # t5
  sw   x31, 27 * CR2_WORD_SIZE(sp)  # t6
  csrr a0,  mstatus
  sw   a0,  28 * CR2_WORD_SIZE(sp)  # mstatus
  # store stack pointer in current thread
  lw   t0,  cr2_current_thread
  sw   sp,  0(t0)

  csrr a0,  mcause
  csrr a1,  mepc

  # test for synchronous interrupt
  srli a2,  a0, 31
  beqz a2,  cr2_synchronous_exception
  # handle interrupt
cr2_asynchronous_interrupt:
  sw   a1,  29 * CR2_WORD_SIZE(sp)
  lw   sp,  cr2_isr_stack_top
  call cr2_sys_interrupt_handler
  j    cr2_restore_ctx

  # handly synchronous exception
cr2_synchronous_exception:
  addi a1,  a1, CR2_WORD_SIZE
  sw   a1,  29 * CR2_WORD_SIZE(sp)
  lw   sp,  cr2_isr_stack_top
  call cr2_sys_exception_handler

cr2_restore_ctx:
  # load stack pointer from current thread
  lw   t0,  cr2_current_thread
  lw   sp,  0(t0)

  # load mepc return address
  lw   t0,  29 * CR2_WORD_SIZE(sp)
  csrw mepc, t0

  # load mstatus
  lw   t0,  28 * CR2_WORD_SIZE(sp)
  csrw mstatus, t0

  # load registers
  lw   x1,   0 * CR2_WORD_SIZE(sp)  # ra
  lw   x5,   1 * CR2_WORD_SIZE(sp)  # t0
  lw   x6,   2 * CR2_WORD_SIZE(sp)  # t1
  lw   x7,   3 * CR2_WORD_SIZE(sp)  # t2
  lw   x8,   4 * CR2_WORD_SIZE(sp)  # s0/fp
  lw   x9,   5 * CR2_WORD_SIZE(sp)  # s1
  lw   x10,  6 * CR2_WORD_SIZE(sp)  # a0
  lw   x11,  7 * CR2_WORD_SIZE(sp)  # a1
  lw   x12,  8 * CR2_WORD_SIZE(sp)  # a2
  lw   x13,  9 * CR2_WORD_SIZE(sp)  # a3
  lw   x14, 10 * CR2_WORD_SIZE(sp)  # a4
  lw   x15, 11 * CR2_WORD_SIZE(sp)  # a5
  lw   x16, 12 * CR2_WORD_SIZE(sp)  # a6
  lw   x17, 13 * CR2_WORD_SIZE(sp)  # a7
  lw   x18, 14 * CR2_WORD_SIZE(sp)  # s2
  lw   x19, 15 * CR2_WORD_SIZE(sp)  # s3
  lw   x20, 16 * CR2_WORD_SIZE(sp)  # s4
  lw   x21, 17 * CR2_WORD_SIZE(sp)  # s5
  lw   x22, 18 * CR2_WORD_SIZE(sp)  # s6
  lw   x23, 19 * CR2_WORD_SIZE(sp)  # s7
  lw   x24, 20 * CR2_WORD_SIZE(sp)  # s8
  lw   x25, 21 * CR2_WORD_SIZE(sp)  # s9
  lw   x26, 22 * CR2_WORD_SIZE(sp)  # s10
  lw   x27, 23 * CR2_WORD_SIZE(sp)  # s11
  lw   x28, 24 * CR2_WORD_SIZE(sp)  # t3
  lw   x29, 25 * CR2_WORD_SIZE(sp)  # t4
  lw   x30, 26 * CR2_WORD_SIZE(sp)  # t5
  lw   x31, 27 * CR2_WORD_SIZE(sp)  # t6

  # reset the stack
  addi sp,  sp, CR2_CTX_SIZE
  
  mret
  .endfunc

.func
cr2_start_first_task:
  .global cr2_start_first_task
  la   t0, cr2_trap_vec_entry
  csrw mtvec, t0

  lw   sp,  cr2_current_thread
  lw   sp,  0(sp)

  lw   x1,  29 * CR2_WORD_SIZE(sp)  # ra

  # load mstatus
  lw   t0,  28 * CR2_WORD_SIZE(sp)  # mstatus
  csrw mstatus, t0

  # load registers
  lw   x5,   1 * CR2_WORD_SIZE(sp)  # t0
  lw   x6,   2 * CR2_WORD_SIZE(sp)  # t1
  lw   x7,   3 * CR2_WORD_SIZE(sp)  # t2
  lw   x8,   4 * CR2_WORD_SIZE(sp)  # s0/fp
  lw   x9,   5 * CR2_WORD_SIZE(sp)  # s1
  lw   x10,  6 * CR2_WORD_SIZE(sp)  # a0
  lw   x11,  7 * CR2_WORD_SIZE(sp)  # a1
  lw   x12,  8 * CR2_WORD_SIZE(sp)  # a2
  lw   x13,  9 * CR2_WORD_SIZE(sp)  # a3
  lw   x14, 10 * CR2_WORD_SIZE(sp)  # a4
  lw   x15, 11 * CR2_WORD_SIZE(sp)  # a5
  lw   x16, 12 * CR2_WORD_SIZE(sp)  # a6
  lw   x17, 13 * CR2_WORD_SIZE(sp)  # a7
  lw   x18, 14 * CR2_WORD_SIZE(sp)  # s2
  lw   x19, 15 * CR2_WORD_SIZE(sp)  # s3
  lw   x20, 16 * CR2_WORD_SIZE(sp)  # s4
  lw   x21, 17 * CR2_WORD_SIZE(sp)  # s5
  lw   x22, 18 * CR2_WORD_SIZE(sp)  # s6
  lw   x23, 19 * CR2_WORD_SIZE(sp)  # s7
  lw   x24, 20 * CR2_WORD_SIZE(sp)  # s8
  lw   x25, 21 * CR2_WORD_SIZE(sp)  # s9
  lw   x26, 22 * CR2_WORD_SIZE(sp)  # s10
  lw   x27, 23 * CR2_WORD_SIZE(sp)  # s11
  lw   x28, 24 * CR2_WORD_SIZE(sp)  # t3
  lw   x29, 25 * CR2_WORD_SIZE(sp)  # t4
  lw   x30, 26 * CR2_WORD_SIZE(sp)  # t5
  lw   x31, 27 * CR2_WORD_SIZE(sp)  # t6

  # reset the stack
  addi sp, sp, CR2_CTX_SIZE

  ret
  .endfunc

#endif
