#ifndef __CR2_CR2_H__
#define __CR2_CR2_H__

#include <stdint.h>

// Thread Control Block
typedef struct {
  void* sp;
} cr2_thread_t;

typedef void (*cr2_thread_handler_t)();

void cr2_init(void);

void cr2_start(void);

void cr2_thread_init(cr2_thread_t* t, cr2_thread_handler_t th, uint32_t* stack,
                     uint32_t stack_size);

extern void cr2_trap_vec_entry(void);

void cr2_sys_tick_handler(uintptr_t mcause);

void cr2_set_timer(void);

#endif