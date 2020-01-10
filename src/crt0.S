.global _enter
.section .init.enter
_enter:
  # clear registers
  mv     x1, x0
  mv     x2, x0
  mv     x3, x0
  mv     x4, x0
  mv     x5, x0
  mv     x6, x0
  mv     x7, x0
  mv     x8, x0
  mv     x9, x0
  mv    x10, x0
  mv    x11, x0
  mv    x12, x0
  mv    x13, x0
  mv    x14, x0
  mv    x15, x0
  mv    x16, x0
  mv    x17, x0
  mv    x18, x0
  mv    x19, x0
  mv    x20, x0
  mv    x21, x0
  mv    x22, x0
  mv    x23, x0
  mv    x24, x0
  mv    x25, x0
  mv    x26, x0
  mv    x27, x0
  mv    x28, x0
  mv    x29, x0
  mv    x30, x0
  mv    x31, x0
  .cfi_startproc
  .cfi_undefined ra
  # disable interrupts
  csrci mstatus, 8
  lui   a5, 0x1
  addi  a5, a5, -1366 # 0xaaa
  csrc  mie, a5
.option push
.option norelax
  la    gp, __global_pointer$
.option pop
  la    t0, early_unhandled_exception
  csrw  mtvec, t0
  .weak __stack_pointer
  la    sp, __stack_pointer
  call  _start
  .cfi_endproc

.section .init
.align 2
early_unhandled_exception:
  .cfi_startproc
  csrr  t0, mcause
  csrr  t0, mepc
  csrr  t0, mtval
  j     early_unhandled_exception
  .cfi_endproc

_start:
  .global _start
  .cfi_startproc
  # init data
  la    t0, __data_source_start
  la    t1, __data_target_start
  la    t2, __data_target_end
  beq   t0, t1, data_init_end
  bge   t1, t2, data_init_end
data_init_loop:
  lw    a0, 0(t0)
  addi  t0, t0, 4
  sw    a0, 0(t1)
  addi  t1, t1, 4
  blt   t1, t2, data_init_loop
data_init_end:
  .global __bsp_specific_init__
  call __bsp_specific_init__
  # init bss
  la    x26, __bss_start
  la    x27, __bss_end
  bge   x26, x27, bss_init_end
bss_init_loop:
  sw    x0, 0(x26)
  addi  x26, x26, 4
  ble   x26, x27, bss_init_loop
bss_init_end:

  # ctors and dtors
  la a0, __libc_fini_array
  call atexit
  call __libc_init_array

main_entry:
  # ensure all data and instructions are done
  fence
  fence.i
  # set argc = argv = 0
  mv    x0, a0
  mv    x0, a1
  call  main
  call  exit
  .cfi_endproc
