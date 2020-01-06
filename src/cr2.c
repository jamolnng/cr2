#include <cr2.h>

#include <clint.h>
#include <clock.h>
#include <gpio.h>
#include <platform.h>
#include <string.h>

void cr2_init(void) {
  register uintptr_t trap_vec = (uintptr_t)&cr2_trap_vec_entry;
  if ((trap_vec & 0xFFFFFFFCu) != trap_vec) {
    // trap vector not aligned to 4 byte boundary
    return;
  }
  __asm__("csrw mtvec, %0" ::"r"(trap_vec));
}

void cr2_start(void) {
  __asm__("csrs mie, %0" ::"r"(0x80));  // enable timer interrupts
  __asm__("csrsi mstatus, 0x8");        // enable global interrupts
  cr2_set_timer();
}

void cr2_thread_init(cr2_thread_t* t, cr2_thread_handler_t th, uint32_t* stack,
                     uint32_t stack_size) {
  uint32_t* sp = &stack[stack_size];
  memset(stack, 0, stack_size * sizeof(uint32_t));
  --sp;
  *sp = (uint32_t)th;
  sp -= 31;
  t->sp = (void*)sp;
}

void blink0(void);
void blink0(void) { gpio_reg(GPIO_REG_OUTPUT_VAL) ^= RED_LED; }

void cr2_sys_tick_handler(uintptr_t mcause) {
  if ((mcause & MCAUSE_INT) && ((mcause & MCAUSE_CAUSE) == IRQ_M_TIMER)) {
    blink0();
    cr2_set_timer();
  }
}

void cr2_set_timer(void) {
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
  next += 32768;
  clint_reg(CLINT_REG_MTIMECMP) = (uint32_t)(next);
  clint_reg(CLINT_REG_MTIMECMP + 4) = (uint32_t)(next >> 32);
  // timer_counter++;
}