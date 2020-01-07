#ifndef __CR2_CR2_H__
#define __CR2_CR2_H__

#include <stdint.h>

#define CR2_MAX_THREADS 8
#define CR2_STACK_SIZE 128

// Thread Control Block
typedef struct {
  void* stack_ptr __attribute__((aligned(4)));
  uint32_t stack[CR2_STACK_SIZE] __attribute__((aligned(4)));
} cr2_thread_t;

typedef void (*cr2_thread_handler_t)();

void cr2_init(void);
void cr2_start(void);
void cr2_schedule(void);
void cr2_thread_init(cr2_thread_t* t, cr2_thread_handler_t th);

#endif