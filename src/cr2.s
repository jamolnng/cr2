#ifndef __CR2_CTX_SWITCH_ASM__
#define __CR2_CTX_SWITCH_ASM__

.set CR2_REGISTER_WIDTH, 4
.set CR2_STACK_STORE_SIZE, 30

.section .text.cr2_trap_vec_entry
.align 2
.weak cr2_trap_vec_entry
cr2_trap_vec_entry:
  .global cr2_trap_vec_entry
  .global cr2_current_thread

  # make room in current thread stack
  # RISC-V 32 words are 4 bytes wide
  # hold all of registers except sp and gp
  addi sp, sp, -(CR2_REGISTER_WIDTH * CR2_STACK_STORE_SIZE)

  # store registers
  sw   x1, 0 * 4(sp)
  # stack pointer already being saved to cr2_current_thread
  # sw x2 , 1  * 4(sp)  # stack pointer
  # global pointer does not change
  # sw x3 , 2  * 4(sp)  # global pointer
  sw   x4, 1 * 4(sp)
  sw   x5, 2 * 4(sp)
  sw   x6, 3 * 4(sp)
  sw   x7, 4 * 4(sp)
  sw   x8, 5 * 4(sp)
  sw   x9, 6 * 4(sp)
  sw   x10, 7 * 4(sp)
  sw   x11, 8 * 4(sp)
  sw   x12, 9 * 4(sp)
  sw   x13, 10 * 4(sp)
  sw   x14, 11 * 4(sp)
  sw   x15, 12 * 4(sp)
  sw   x16, 13 * 4(sp)
  sw   x17, 14 * 4(sp)
  sw   x18, 15 * 4(sp)
  sw   x19, 16 * 4(sp)
  sw   x20, 17 * 4(sp)
  sw   x21, 18 * 4(sp)
  sw   x22, 19 * 4(sp)
  sw   x23, 20 * 4(sp)
  sw   x24, 21 * 4(sp)
  sw   x25, 22 * 4(sp)
  sw   x26, 23 * 4(sp)
  sw   x27, 24 * 4(sp)
  sw   x28, 25 * 4(sp)
  sw   x29, 26 * 4(sp)
  sw   x30, 27 * 4(sp)
  sw   x31, 28 * 4(sp)

  # store stack pointer in current thread
  lw   t0, cr2_current_thread
  sw   sp, 0x0(t0)

  # save epc (our thread's pc)
  csrr t0, mepc
  sw   t0, 29 * 4(sp)

  csrr a0, mcause
  call cr2_sys_tick_handler
  # enable interrupts and return in machine mode
  li   t0, 0x1880
  csrs mstatus, t0

  # load stack pointer from current thread
  lw   t0, cr2_current_thread
  lw   sp, 0x0(t0)

  # load mepc return address
  lw   t0, 29 * 4(sp)
  csrw mepc, t0

  # load registers
  lw   x1, 0 * 4(sp)
  # stack pointer already being loaded from cr2_current_thread
  # lw x2 , 1  * 4(sp)  # stack pointer
  # global pointer does not change
  # lw x3 , 2  * 4(sp)  # global pointer
  lw   x4, 1 * 4(sp)
  lw   x5, 2 * 4(sp)
  lw   x6, 3 * 4(sp)
  lw   x7, 4 * 4(sp)
  lw   x8, 5 * 4(sp)
  lw   x9, 6 * 4(sp)
  lw   x10, 7 * 4(sp)
  lw   x11, 8 * 4(sp)
  lw   x12, 9 * 4(sp)
  lw   x13, 10 * 4(sp)
  lw   x14, 11 * 4(sp)
  lw   x15, 12 * 4(sp)
  lw   x16, 13 * 4(sp)
  lw   x17, 14 * 4(sp)
  lw   x18, 15 * 4(sp)
  lw   x19, 16 * 4(sp)
  lw   x20, 17 * 4(sp)
  lw   x21, 18 * 4(sp)
  lw   x22, 19 * 4(sp)
  lw   x23, 20 * 4(sp)
  lw   x24, 21 * 4(sp)
  lw   x25, 22 * 4(sp)
  lw   x26, 23 * 4(sp)
  lw   x27, 24 * 4(sp)
  lw   x28, 25 * 4(sp)
  lw   x29, 26 * 4(sp)
  lw   x30, 27 * 4(sp)
  lw   x31, 28 * 4(sp)

  # reset the stack
  addi sp, sp, (CR2_REGISTER_WIDTH * CR2_STACK_STORE_SIZE)
  
	mret

#endif
