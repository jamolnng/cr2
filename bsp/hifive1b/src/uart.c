#include "uart.h"

#include "clock.h"
#include "gpio.h"

#define TXRX_EN 0x1u

void uart_init(uart_t instance, uint32_t baud) {
  // enable GPIO functions
  uint32_t mask = (instance == UART0) ? GPIO_IOF0_UART0 : GPIO_IOF0_UART1;
  gpio_reg(GPIO_REG_IOF_SEL) &= ~mask;  // select IOF 0
  gpio_reg(GPIO_REG_IOF_EN) |= mask;    // enable uart IOF

  // set baud divider
  baud = (get_cpu_freq() / baud) - 1;
  mmio(instance, UART_REG_DIV) = baud;

  // enable RX and TX
  mmio(instance, UART_REG_TX_CTRL) |= TXRX_EN;
  mmio(instance, UART_REG_RX_CTRL) |= TXRX_EN;
}

void uart_deinit(uart_t instance) {
  // disable RX and TX
  mmio(instance, UART_REG_TX_CTRL) &= ~TXRX_EN;
  mmio(instance, UART_REG_RX_CTRL) &= ~TXRX_EN;

  // disable GPIO functions
  uint32_t mask = (instance == UART0) ? GPIO_IOF0_UART0 : GPIO_IOF0_UART1;
  gpio_reg(GPIO_REG_IOF_EN) &= ~mask;
}

void uart_putc(uart_t instance, char c) {
  while (mmio(instance, UART_REG_TX_DATA) & UART_TX_DATA_FULL)
    ;
  mmio(instance, UART_REG_TX_DATA) = c;
}

void uart_puts(uart_t instance, const char* str, unsigned int len) {
  for (unsigned int i = 0; i < len; ++i) {
    uart_putc(instance, str[i]);
  }
}

int uart_getc(uart_t instance) {
  uint32_t mmval = mmio(instance, UART_REG_RX_DATA);
  if (mmval & UART_RX_DATA_EMPTY) {
    return -1;  // EOF
  } else {
    return mmval & 0x0FF;
  }
}

char* uart_gets(uart_t instance, char* str, int count) {
  int c;
  int cur = 0;
  // EOF check
  for (;;) {
    if (cur == count) {
      break;
    }
    c = uart_getc(instance);
    if (c >= 0) {
      if (c == '\n' || c == '\r') {
        break;
      } else if (c == 0x08) {
        --cur;
        if (cur < 0) {
          cur = 0;
        }
      } else {
        str[cur] = (char)c;
        ++cur;
      }
    }
  }
  str[cur] = '\0';
  return str;
}