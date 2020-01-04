#ifndef __CR2_CLOCK_H__
#define __CR2_CLOCK_H__

#include <stddef.h>
#include <stdint.h>

#include <platform.h>

void clock_init();
unsigned long get_timer_freq();
unsigned long mtime_lo(void);
uint32_t mtime_hi(void);
uint64_t get_timer_value();
unsigned long __attribute__((noinline)) measure_cpu_freq(size_t n);
unsigned long get_cpu_freq();

#endif