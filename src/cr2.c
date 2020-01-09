#include <cr2.h>

#include <clint.h>
#include <clock.h>
#include <gpio.h>
#include <platform.h>
#include <string.h>

#define cr2_enable_interrupts() __asm__ volatile("csrsi mstatus, 8")
#define cr2_disable_interrupts() __asm__ volatile("csrci mstatus, 8")
#define cr2_enable_timer_interrupts() \
  __asm__ volatile("csrs mie, %0" ::"r"(0x80))
#define cr2_disable_timer_interrupts() \
  __asm__ volatile("csrc mie, %0" ::"r"(0x80))

extern void cr2_start_first_task(void);
extern void* cr2_thread_init_stack(cr2_stack_type_t* stack_ptr,
                                   cr2_thread_handler_t function_addr);

void cr2_sys_interrupt_handler(uintptr_t mcause);
void cr2_sys_exception_handler(uintptr_t mcause);
static void cr2_set_timer(void);
static void cr2_tick(void);
static void cr2_idle_thread_task(void);

static cr2_thread_t cr2_idle_thread;
cr2_thread_t* volatile cr2_current_thread;
static cr2_thread_t* cr2_threads[CR2_MAX_THREADS + 1] = {0};
static uint32_t cr2_thread_ready;

extern const cr2_stack_type_t __cr2_irq_stack_top[];
const cr2_stack_type_t* cr2_isr_stack_top =
    (const cr2_stack_type_t*)__cr2_irq_stack_top;

static unsigned int cr2_critical_nesting = 0xCAFEBABE;

void cr2_init(void) {
  cr2_idle_thread.priority = CR2_MAX_THREADS;
  cr2_current_thread = &cr2_idle_thread;
  memset(cr2_idle_thread.stack, 0,
         CR2_THREAD_STACK_SIZE * sizeof(cr2_stack_type_t));
  cr2_stack_type_t* stack_ptr = &(cr2_idle_thread.stack[CR2_THREAD_STACK_SIZE]);
  cr2_idle_thread.stack_ptr =
      cr2_thread_init_stack(stack_ptr, cr2_idle_thread_task);
  cr2_threads[CR2_MAX_THREADS] = &cr2_idle_thread;
}

void cr2_start(void) {
  cr2_critical_nesting = 0;
  cr2_schedule();
  cr2_set_timer();
  cr2_enable_timer_interrupts();
  cr2_enable_interrupts();
  cr2_start_first_task();
}

void cr2_schedule(void) {
  if (cr2_thread_ready == 0) {
    cr2_current_thread = &cr2_idle_thread;
  } else {
    unsigned int count = 0;
    uint32_t working_set = cr2_thread_ready;
    while (!(working_set & 1u)) {
      count++;
      working_set >>= 1;
    }
    cr2_current_thread = cr2_threads[count];
  }
}

void cr2_thread_init(cr2_thread_t* t, unsigned int priority,
                     cr2_thread_handler_t th) {
  if (priority >= CR2_MAX_THREADS) {
    // TODO: error
    return;
  }
  if (cr2_threads[priority] != 0) {
    // TODO: error
    return;
  }
  memset(t->stack, 0, CR2_THREAD_STACK_SIZE * sizeof(cr2_stack_type_t));
  cr2_stack_type_t* stack_ptr = &(t->stack[CR2_THREAD_STACK_SIZE]);
  t->stack_ptr = cr2_thread_init_stack(stack_ptr, th);
  t->priority = priority;
  cr2_thread_ready |= (1u << priority);
  cr2_threads[priority] = t;
}

void cr2_enter_critical_section(void) {
  cr2_disable_interrupts();
  cr2_critical_nesting++;
}

void cr2_exit_critical_section(void) {
  cr2_critical_nesting--;
  if (cr2_critical_nesting == 0) {
    __asm__ volatile("fence");
    __asm__ volatile("fence.i");
    cr2_enable_interrupts();
  }
}

void cr2_delay(uint32_t timeout) {
  cr2_enter_critical_section();
  cr2_current_thread->timeout = timeout;
  cr2_thread_ready &= ~(1u << cr2_current_thread->priority);
  cr2_exit_critical_section();
  cr2_yield();
}

void cr2_sys_interrupt_handler(uintptr_t mcause) {
  if (((mcause & MCAUSE_CAUSE) == IRQ_M_TIMER)) {
    cr2_tick();
    cr2_set_timer();
    cr2_schedule();
  } else {
    for (;;) {
      for (int i = 0; i < 100000; ++i)
        ;
      gpio_reg(GPIO_REG_OUTPUT_VAL) ^= BLUE_LED;
    }
  }
}

void cr2_sys_exception_handler(uintptr_t mcause) {
  if (mcause == ERQ_M_ECALL) {
    cr2_schedule();
  } else {
    for (;;) {
      for (int i = 0; i < 100000; ++i)
        ;
      gpio_reg(GPIO_REG_OUTPUT_VAL) ^= RED_LED;
    }
  }
}

void cr2_set_timer(void) {
  static unsigned int tick_min, tick_max, tick_mod, tick_count;
  if (tick_min == 0) {
    unsigned int rtc_freq = get_timer_freq();
    tick_min = rtc_freq / CR2_TICK_RATE_HZ;
    tick_max = tick_min + 1;
    tick_mod = CR2_TICK_RATE_HZ - (rtc_freq % CR2_TICK_RATE_HZ);
  }
  uint64_t next = get_timer_value();
  if (tick_count >= tick_mod) {
    next += tick_max;
  } else {
    next += tick_min;
  }
  tick_count++;
  if (tick_count == CR2_TICK_RATE_HZ) {
    tick_count = 0;
  }
  clint_reg(CLINT_REG_MTIMECMP) = (uint32_t)(next);
  clint_reg(CLINT_REG_MTIMECMP + 4) = (uint32_t)(next >> 32);
}

void cr2_tick(void) {
  for (unsigned int i = 0; i < CR2_MAX_THREADS; i++) {
    if (cr2_threads[i] != 0 && cr2_threads[i]->timeout != 0) {
      --(cr2_threads[i]->timeout);
      if (cr2_threads[i]->timeout == 0) {
        cr2_thread_ready |= (1u << i);
      }
    }
  }
}

void cr2_idle_thread_task(void) {
  for (;;) {
    __asm__ volatile("wfi");
  }
}