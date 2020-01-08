#ifndef __CR2_CR2_H__
#define __CR2_CR2_H__

#include <stdint.h>

#define CR2_MAX_THREADS 32
#define CR2_THREAD_STACK_SIZE 128
#define CR2_IRQ_STACK_SIZE 128

#define cr2_yield() __asm__ volatile("ecall")

typedef uint32_t cr2_stack_type_t;

// Thread Control Block
typedef struct {
  void* stack_ptr __attribute__((aligned(4)));
  cr2_stack_type_t stack[CR2_THREAD_STACK_SIZE] __attribute__((aligned(4)));
} cr2_thread_t;

typedef void (*cr2_thread_handler_t)();

void cr2_init(void);
void cr2_start(void);
void cr2_schedule(void);
void cr2_thread_init(cr2_thread_t* t, cr2_thread_handler_t th);
void cr2_enter_critical_section(void);
void cr2_exit_critical_section(void);

#endif