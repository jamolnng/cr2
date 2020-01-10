#ifndef __HIFIVE1B_CRT0_S__
#define __HIFIVE1B_CRT0_S__

.global __bsp_specific_init__
.section .init
__bsp_specific_init__:
  # init itim
  # first by clearning old itim by storing a
  # zero to the first byte after the itim region
  # la    t0, __itim_memory_end
  # sw    x0, 0(t0)
  # fence.i
  la    t0, __itim_source_start
  la    t1, __itim_target_start
  la    t2, __itim_target_end
  beq   t0, t1, itim_init_end
  bge   t1, t2, itim_init_end
itim_init_loop:
  lw    a0, 0(t0)
  addi  t0, t0, 4
  sw    a0, 0(t1)
  addi  t1, t1, 4
  blt   t1, t2, itim_init_loop
itim_init_end:
  fence.i
  ret

#endif