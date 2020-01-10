#include <clock.h>
#include <gpio.h>
#include <uart.h>

void bsp_init(void);
void bsp_init(void) {
  // clock_init_hfxosc();
  // max f=38, min f=22
  clock_init_hfpll(1, 22, 1);
  uart_init(UART0, 115200);
  gpio_reg(GPIO_REG_OUTPUT_VAL) |= RED_LED | GREEN_LED | BLUE_LED;
  gpio_reg(GPIO_REG_OUTPUT_EN) |= RED_LED | GREEN_LED | BLUE_LED;
}