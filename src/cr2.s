#ifndef __CR2_CTX_SWITCH_ASM__
#define __CR2_CTX_SWITCH_ASM__

.macro epc_save
  .endm

  .section .text.entry
  .align 2
  .weak cr2_trap_vec_entry
cr2_trap_vec_entry:

  csrr a0, mcause
  call cr2_sys_tick_handler
  li t0, 0x1880
  csrs mstatus, t0
  
	mret

#endif
