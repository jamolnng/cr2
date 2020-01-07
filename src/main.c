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

void blink1(void) {
  for (;;) {
    for (int i = 0; i < 200000; ++i)
      ;
    gpio_reg(GPIO_REG_OUTPUT_VAL) ^= RED_LED;
  }
}
void blink2(void) {
  for (;;) {
    for (int i = 0; i < 100000; ++i)
      ;
    gpio_reg(GPIO_REG_OUTPUT_VAL) ^= GREEN_LED;
  }
}

cr2_thread_t thread1;
cr2_thread_t thread2;

int main() {
  clock_init();
  uart_init(UART0, 115200);
  gpio_reg(GPIO_REG_OUTPUT_VAL) |= RED_LED | GREEN_LED | BLUE_LED;
  gpio_reg(GPIO_REG_OUTPUT_EN) |= RED_LED | GREEN_LED | BLUE_LED;

  char str[64] = "Running at ";
  ultoa(get_cpu_freq(), &str[strlen(str)], 63, 10);
  uart_puts(UART0, str, strlen(str));
  uart_puts(UART0, " Hz\n", 5);

  cr2_init();
  cr2_thread_init(&thread1, blink1);
  cr2_thread_init(&thread2, blink2);

  cr2_start();

  for (;;) {
    uart_gets(UART0, str, 5);
    if (strcmp(str, "on") == 0) {
      gpio_reg(GPIO_REG_OUTPUT_VAL) &= ~BLUE_LED;
    }
    if (strcmp(str, "off") == 0) {
      gpio_reg(GPIO_REG_OUTPUT_VAL) |= BLUE_LED;
    }
  }

  return 0;
}
