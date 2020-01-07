#include <clint.h>
#include <clock.h>
#include <gpio.h>
#include <memory_map.h>
#include <platform.h>
#include <stdlib.h>
#include <string.h>
#include <uart.h>

#include <cr2.h>

void set_timer(void);
void blink1(void);
void blink2(void);
int main();

__attribute__((aligned(2))) void blink1(void) {
  for (;;) {
    for (int i = 0; i < 200000; ++i)
      ;
    gpio_reg(GPIO_REG_OUTPUT_VAL) ^= RED_LED;
  }
}
__attribute__((aligned(2))) void blink2(void) {
  for (;;) {
    for (int i = 0; i < 100000; ++i)
      ;
    gpio_reg(GPIO_REG_OUTPUT_VAL) ^= GREEN_LED;
  }
}

#define STACK_SIZE 96

uint32_t stack1[STACK_SIZE] __attribute__((aligned(8)));
cr2_thread_t thread1;

uint32_t stack2[STACK_SIZE] __attribute__((aligned(8)));
cr2_thread_t thread2;

uintptr_t *currentTCB;

int main() {
  gpio_reg(GPIO_REG_OUTPUT_VAL) |= RED_LED | GREEN_LED | BLUE_LED;
  gpio_reg(GPIO_REG_OUTPUT_EN) |= RED_LED | GREEN_LED | BLUE_LED;
  gpio_reg(GPIO_REG_OUTPUT_VAL) ^= BLUE_LED;

  char str[64] = "Running at Hz: ";
  ultoa(get_cpu_freq(), &str[15], 63, 10);
  uart_puts(UART0, str, strlen(str));

  cr2_init();
  cr2_thread_init(&thread1, blink1, stack1, STACK_SIZE);
  cr2_thread_init(&thread2, blink2, stack2, STACK_SIZE);

  cr2_start();

  for (;;) {
    for (int i = 0; i < 400000; ++i)
      ;
    gpio_reg(GPIO_REG_OUTPUT_VAL) ^= BLUE_LED;
  }

  return 0;
}
