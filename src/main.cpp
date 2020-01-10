#include <clock.h>
#include <gpio.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <cr2.h>

void set_timer(void);
void blink1(void);
void blink2(void);
void blink3(void);
int main();

void blink1(void) {
  for (;;) {
    cr2_delay(2000);
    cr2_enter_critical_section();
    unsigned long f = measure_cpu_freq(100);
    cr2_exit_critical_section();
    printf("Running at %lu Hz\n", f);
    gpio_reg(GPIO_REG_OUTPUT_VAL) ^= RED_LED;
  }
}

void blink2(void) {
  for (;;) {
    cr2_delay(1000);
    gpio_reg(GPIO_REG_OUTPUT_VAL) ^= GREEN_LED;
  }
}

void blink3(void) {
  for (;;) {
    cr2_delay(500);
    gpio_reg(GPIO_REG_OUTPUT_VAL) ^= BLUE_LED;
  }
}

cr2_thread_t thread1;
cr2_thread_t thread2;
cr2_thread_t thread3;
cr2_stack_type_t stack1[128];
cr2_stack_type_t stack2[128];
cr2_stack_type_t stack3[128];

int main() {
  printf("Running at %lu Hz\n", measure_cpu_freq(10000));
  puts("Press ENTER to start cr2...");
  getchar();

  CR2 cr2;
  cr2.add_thread(0, blink1, sizeof(stack1));
  cr2.add_thread(1, blink2, sizeof(stack2));
  cr2.add_thread(2, blink3, sizeof(stack3));
  cr2.start();

  // thread1.stack = stack1;
  // thread2.stack = stack2;
  // thread3.stack = stack3;
  // thread1.stack_size = 128;
  // thread2.stack_size = 128;
  // thread3.stack_size = 128;
  //
  // cr2_init();
  // cr2_thread_init(&thread1, 0, blink1);
  // cr2_thread_init(&thread2, 1, blink2);
  // cr2_thread_init(&thread3, 2, blink3);
  // cr2_start();

  // should not get here

  return 0;
}
