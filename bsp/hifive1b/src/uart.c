#include "uart.h"

#include "clock.h"
#include "gpio.h"

#include <stdio.h>

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

void uart_puts(uart_t instance, const char* str, unsigned int count) {
  for (unsigned int i = 0; i < count; ++i) {
    uart_putc(instance, str[i]);
  }
}

int uart_getc(uart_t instance) {
  uint32_t mmval = mmio(instance, UART_REG_RX_DATA);
  if (mmval & UART_RX_DATA_EMPTY) {
    return EOF;  // EOF
  } else {
    return mmval & 0x0FF;
  }
}

int uart_getchar(uart_t instance) {
  int c;
  while ((c = uart_getc(instance)) == EOF)
    ;
  return c;
}

char* uart_gets(uart_t instance, char* str, unsigned int count) {
  int c;
  unsigned int cur = 0;
  // EOF check
  for (;;) {
    if (cur == count) {
      break;
    }
    c = uart_getc(instance);
    if (c >= 0) {
      if (c == '\n' || c == '\r') {
        break;
      }
      if (c == 0x08) {
        if (cur > 0) {
          --cur;
        }
      } else {
        str[cur++] = (char)c;
      }
    }
  }
  str[cur++] = '\n';
  str[cur] = '\0';
  return str;
}