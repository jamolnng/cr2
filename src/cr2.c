#include <cr2.h>

#include <clint.h>
#include <clock.h>
#include <gpio.h>
#include <platform.h>
#include <string.h>

extern void cr2_start_first_task(void);
void cr2_sys_interrupt_handler(uintptr_t mcause);
void cr2_sys_exception_handler(uintptr_t mcause);
void cr2_set_timer(void);

static cr2_thread_t cr2_idle_thread;

cr2_thread_t* volatile cr2_current_thread;
static unsigned int cr2_current_thread_id;

static cr2_thread_t* cr2_threads[CR2_MAX_THREADS + 1];
static unsigned int cr2_thread_count;

const unsigned int cr2_timer_tick_increment = 655;

extern const uint32_t __cr2_irq_stack_top[];
const uint32_t* crq_isr_stack_top = (const uint32_t*)__cr2_irq_stack_top;

void cr2_start(void) {
  cr2_threads[CR2_MAX_THREADS] = &cr2_idle_thread;
  if (cr2_thread_count == 0) {
    cr2_current_thread = &cr2_idle_thread;
    cr2_current_thread_id = CR2_MAX_THREADS;
  } else {
    cr2_current_thread = cr2_threads[0];
    cr2_current_thread_id = 0;
  }
  cr2_set_timer();
  __asm__("csrs mie, %0" ::"r"(0x80));  // enable timer interrupts

  cr2_start_first_task();
}

void cr2_thread_init(cr2_thread_t* t, cr2_thread_handler_t th) {
  if (cr2_thread_count == CR2_MAX_THREADS - 1) {
    // TODO: error
    return;
  }
  uint32_t* stack_ptr = &(t->stack[CR2_THREAD_STACK_SIZE]);
  memset(t->stack, 0, CR2_THREAD_STACK_SIZE * sizeof(uint32_t));
  union {
    cr2_thread_handler_t func;
    uint32_t ptr;
  } helper = {.func = th};
  *(--stack_ptr) = helper.ptr;
  *(--stack_ptr) |= 0x1888;
  stack_ptr -= 28;
  t->stack_ptr = (void*)stack_ptr;
  cr2_threads[cr2_thread_count++] = t;
}

void cr2_sys_interrupt_handler(uintptr_t mcause) {
  if (((mcause & MCAUSE_CAUSE) == IRQ_M_TIMER)) {
    cr2_set_timer();
    if (cr2_thread_count == 0) {
      return;
    }
    cr2_current_thread_id++;
    if (cr2_current_thread_id == cr2_thread_count) {
      cr2_current_thread_id = 0;
    }
    cr2_current_thread = cr2_threads[cr2_current_thread_id];
  }
}

void cr2_sys_exception_handler(uintptr_t mcause) {}

void cr2_set_timer(void) {
  uint64_t next = get_timer_value();
  next += cr2_timer_tick_increment;
  clint_reg(CLINT_REG_MTIMECMP) = (uint32_t)(next);
  clint_reg(CLINT_REG_MTIMECMP + 4) = (uint32_t)(next >> 32);
}