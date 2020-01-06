#ifndef __CR2_CTX_SWITCH_ASM__
#define __CR2_CTX_SWITCH_ASM__

.macro epc_save
  .endm

  .section .text.entry
  .align 2
  .weak cr2_trap_vec_entry
cr2_trap_vec_entry:
  .global cr2_current_thread

  # make room in current thread stack
  # RISC-V 32 words are 4 bytes wide
  # 32 to hold all of registers except x0
  #    and the pc
  addi sp, sp, -4 * 32

  # store registers
  sw x1 , 0  * 4(sp)
  sw x2 , 1  * 4(sp)
  sw x3 , 2  * 4(sp)
  sw x4 , 3  * 4(sp)
  sw x5 , 4  * 4(sp)
  sw x6 , 5  * 4(sp)
  sw x7 , 6  * 4(sp)
  sw x8 , 7  * 4(sp)
  sw x9 , 8  * 4(sp)
  sw x10, 9  * 4(sp)
  sw x11, 10 * 4(sp)
  sw x12, 11 * 4(sp)
  sw x13, 12 * 4(sp)
  sw x14, 13 * 4(sp)
  sw x15, 14 * 4(sp)
  sw x16, 15 * 4(sp)
  sw x17, 16 * 4(sp)
  sw x18, 17 * 4(sp)
  sw x19, 18 * 4(sp)
  sw x20, 19 * 4(sp)
  sw x21, 20 * 4(sp)
  sw x22, 21 * 4(sp)
  sw x23, 22 * 4(sp)
  sw x24, 23 * 4(sp)
  sw x25, 24 * 4(sp)
  sw x26, 25 * 4(sp)
  sw x27, 26 * 4(sp)
  sw x28, 27 * 4(sp)
  sw x29, 28 * 4(sp)
  sw x30, 29 * 4(sp)
  sw x31, 30 * 4(sp)

  # store stack pointer in current thread
  lw t0, cr2_current_thread
  sw sp, 0x0(t0)

  # save epc (our thread's pc)
  csrr t0, mepc
  sw t0, 31 * 4(sp)

  csrr a0, mcause
  call cr2_sys_tick_handler
  # enable interrupts and return in machine mode
  li t0, 0x1880
  csrs mstatus, t0

  # load stack pointer from current thread
  lw t0, cr2_current_thread
  lw sp, 0x0(t0)

  # load mepc return address
  lw t0, 31 * 4(sp)
  csrw mepc, t0

  # load registers
  lw x1 , 0  * 4(sp)
  # lw x2 , 1  * 4(sp)
  # lw x3 , 2  * 4(sp)
  lw x4 , 3  * 4(sp)
  lw x5 , 4  * 4(sp)
  lw x6 , 5  * 4(sp)
  lw x7 , 6  * 4(sp)
  lw x8 , 7  * 4(sp)
  lw x9 , 8  * 4(sp)
  lw x10, 9  * 4(sp)
  lw x11, 10 * 4(sp)
  lw x12, 11 * 4(sp)
  lw x13, 12 * 4(sp)
  lw x14, 13 * 4(sp)
  lw x15, 14 * 4(sp)
  lw x16, 15 * 4(sp)
  lw x17, 16 * 4(sp)
  lw x18, 17 * 4(sp)
  lw x19, 18 * 4(sp)
  lw x20, 19 * 4(sp)
  lw x21, 20 * 4(sp)
  lw x22, 21 * 4(sp)
  lw x23, 22 * 4(sp)
  lw x24, 23 * 4(sp)
  lw x25, 24 * 4(sp)
  lw x26, 25 * 4(sp)
  lw x27, 26 * 4(sp)
  lw x28, 27 * 4(sp)
  lw x29, 28 * 4(sp)
  lw x30, 29 * 4(sp)
  lw x31, 30 * 4(sp)

  # reset the stack
  addi sp, sp, 4 * 32
  
	mret

#endif
