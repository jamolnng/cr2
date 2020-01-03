#include "uart.h"

#include "clock.h"
#include "gpio.h"

void uart_init(uart_t instance, uint32_t baud) {
  uint32_t mask = (instance == 0) ? GPIO_IOF0_UART0 : GPIO_IOF0_UART1;
  baud = clock_freq() / baud - 1;

  uint32_t mmval = mmio_read_u32(instance, GPIO_REG_IOF_SEL);
  mmio_write_u32(instance, GPIO_REG_IOF_SEL, mmval & ~mask);
  mmval = mmio_read_u32(instance, GPIO_REG_IOF_EN);
  mmio_write_u32(instance, GPIO_REG_IOF_EN, mmval | mask);

  mmio_write_u32(instance, UART_REG_DIV, baud);
  mmval = mmio_read_u32(instance, UART_REG_TX_CTRL);

  // enable RX and TX
  mmio_write_u32(instance, UART_REG_TX_CTRL, mmval | 1);
  mmval = mmio_read_u32(instance, UART_REG_RX_CTRL);
  mmio_write_u32(instance, UART_REG_RX_CTRL, mmval | 1);
}

void uart_putc(uart_t instance, char c) {
  while (mmio_read_u32(instance, UART_REG_TX_DATA) & 0x80000000)
    ;
  mmio_write_u8(instance, UART_REG_TX_DATA, c);
}

void uart_puts(uart_t instance, const char* str, int len) {
  for (int i = 0; i < len; ++i) {
    uart_putc(instance, str[i]);
  }
}

int uart_getc(uart_t instance) {
  uint32_t mmval = mmio_read_u32(instance, UART_REG_RX_DATA);
  if (mmval & 0x80000000) {
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
        str[cur] = c;
        ++cur;
      }
    }
  }
  str[cur] = '\0';
  return str;
}