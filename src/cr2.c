#include <cr2.h>

#include <clint.h>
#include <clock.h>
#include <gpio.h>
#include <platform.h>
#include <string.h>

#define CR2_MAX_THREADS 3

cr2_thread_t* volatile cr2_current_thread;
static cr2_thread_t cr2_idle_thread;
static cr2_thread_t* volatile cr2_threads[CR2_MAX_THREADS] = {0};

void cr2_init(void) {
  register uintptr_t trap_vec = (uintptr_t)&cr2_trap_vec_entry;
  if ((trap_vec & 0xFFFFFFFCu) != trap_vec) {
    // trap vector not aligned to 4 byte boundary
    return;
  }
  __asm__("csrw mtvec, %0" ::"r"(trap_vec));
  memset(*((cr2_thread_t**)&cr2_threads), 0,
         CR2_MAX_THREADS * sizeof(cr2_thread_t*));
  cr2_current_thread = &cr2_idle_thread;
  cr2_threads[CR2_MAX_THREADS - 1] = &cr2_idle_thread;
}

void cr2_start(void) {
  cr2_set_timer();
  __asm__("csrs mie, %0" ::"r"(0x80));  // enable timer interrupts
  __asm__("csrsi mstatus, 0x8");        // enable global interrupts
}

void cr2_thread_init(cr2_thread_t* t, cr2_thread_handler_t th, uint32_t* stack,
                     uint32_t stack_size) {
  static int cr2_next_thread = 0;
  if (cr2_next_thread == CR2_MAX_THREADS - 1) {
    return;
  }
  uint32_t* sp = &stack[stack_size];
  memset(stack, 0, stack_size * sizeof(uint32_t));
  --sp;
  *sp = *(uint32_t*)&th;
  sp -= 31;
  t->sp = (void*)sp;
  cr2_threads[cr2_next_thread++] = t;
}

/*__attribute__((noinline)) __attribute__((section(".itim")))*/ void
cr2_sys_tick_handler(uintptr_t mcause) {
  static int test = 0;
  if ((mcause & MCAUSE_INT) && ((mcause & MCAUSE_CAUSE) == IRQ_M_TIMER)) {
    while (cr2_threads[test] == 0) {
      test++;
    }
    if (test == CR2_MAX_THREADS) {
      test = 0;
    }
    cr2_current_thread = cr2_threads[test];
    test++;
    cr2_set_timer();
  }
}

/*__attribute__((section(".itim")))*/ void cr2_set_timer(void) {
  // static int timer_counter;

  uint64_t next = get_timer_value();
  // if (timer_counter == 50) {
  //   timer_counter = 0;
  // }
  // // 50Hz (655 * 32 + 656 * 18) = 32768
  // if (timer_counter > 31) {
  //   next += 656;
  // } else {
  //   next += 655;
  // }
  // next += 6550;
  next += 655;
  clint_reg(CLINT_REG_MTIMECMP) = (uint32_t)(next);
  clint_reg(CLINT_REG_MTIMECMP + 4) = (uint32_t)(next >> 32);
  // timer_counter++;
}