#ifndef __CS2_UART_H__
#define __CS2_UART_H__

#include <stdbool.h>

#include "memory_map.h"

/* UART Registers */
#define UART_REG_TX_DATA 0x00
#define UART_REG_RX_DATA 0x04
#define UART_REG_TX_CTRL 0x08
#define UART_REG_RX_CTRL 0x0c
#define UART_REG_INT_EN 0x10
#define UART_REG_INT_PENDING 0x14
#define UART_REG_DIV 0x18

typedef enum uart_t { UART0 = UART0_CTRL_ADDR, UART1 = UART1_CTRL_ADDR } uart_t;
void uart_init(uart_t instance, uint32_t baud);
void uart_putc(uart_t instance, char c);
void uart_puts(uart_t instance, const char* str, int len);
int uart_getc(uart_t instance);
char* uart_gets(uart_t instance, char* str, int count);

#endif