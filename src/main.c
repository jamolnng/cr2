#include <clint.h>
#include <clock.h>
#include <gpio.h>
#include <memory_map.h>
#include <platform.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <uart.h>

#include <cr2.h>

void set_timer(void);
void blink1(void);
void blink2(void);
void blink3(void);
int main();

void blink1(void) {
  for (;;) {
    cr2_delay(200);
    gpio_reg(GPIO_REG_OUTPUT_VAL) ^= RED_LED;
  }
}
void blink2(void) {
  for (;;) {
    cr2_delay(100);
    gpio_reg(GPIO_REG_OUTPUT_VAL) ^= GREEN_LED;
  }
}

void blink3(void) {
  for (;;) {
    cr2_delay(50);
    gpio_reg(GPIO_REG_OUTPUT_VAL) ^= BLUE_LED;
  }
}

cr2_thread_t thread1;
cr2_thread_t thread2;
cr2_thread_t thread3;

int main() {
  clock_init_hfxosc();
  uart_init(UART0, 115200);
  // max f=37
  // clock_init_hfpll(1, 37, 1);
  gpio_reg(GPIO_REG_OUTPUT_VAL) |= RED_LED | GREEN_LED | BLUE_LED;
  gpio_reg(GPIO_REG_OUTPUT_EN) |= RED_LED | GREEN_LED | BLUE_LED;

  printf("Running at %lu Hz\n", measure_cpu_freq(10000));
  puts("Press ENTER to start cr2...");
  getchar();

  cr2_init();
  cr2_thread_init(&thread1, 0, blink1);
  cr2_thread_init(&thread2, 1, blink2);
  cr2_thread_init(&thread3, 2, blink3);
  cr2_start();

  // should not get here

  return 0;
}
