#ifndef __CS2_MEMORY_MAP_H__
#define __CS2_MEMORY_MAP_H__

#include <stdint.h>

/* On-Chip Non-Volatile Memeory */
#define DEBUG_ADDR 0x00000000ul
#define ERR_DEVICE_ADDR 0x00002000ul
#define MASK_ROM_ADDR 0x00010000ul
#define OTP_ADDR 0x00020000ul

/* On-Chip Peripherals */
#define CLINT_CTRL_ADDR 0x02000000ul
#define ITIM_CTRL_ADDR 0x08000000ul
#define PLIC_CTRL_ADDR 0x0C000000ul
#define AON_CTRL_ADDR 0x10000000ul
#define PRCI_CTRL_ADDR 0x10008000ul
#define OTP_CTRL_ADDR 0x10010000ul
#define GPIO_CTRL_ADDR 0x10012000ul
#define UART0_CTRL_ADDR 0x10013000ul
#define QSPI0_CTRL_ADDR 0x10014000ul
#define PWM0_CTRL_ADDR 0x10015000ul
#define I2C0_CTRL_ADDR 0x10016000ul
#define UART1_CTRL_ADDR 0x10023000ul
#define SPI1_CTRL_ADDR 0x10024000ul
#define PWM1_CTRL_ADDR 0x10025000ul
#define SPI2_CTRL_ADDR 0x10034000ul
#define PWM2_CTRL_ADDR 0x10035000ul

/* Off-Chip Non-Volatile Memory */
#define FLASH_ADDR 0x20000000ul

/* On-Chip Volatile Memory */
#define DTIM_ADDR 0x80000000ul

static inline uint32_t mmio_read_u32(unsigned long reg, unsigned int offset) {
  return (*(volatile uint32_t *)((reg) + (offset)));
}

/* This function will write a byte to an MMIO register */
static inline void mmio_write_u8(unsigned long reg, unsigned int offset,
                                 uint8_t val) {
  (*(volatile uint32_t *)((reg) + (offset))) = val;
}

/*This function will write a 32-bit value to an MMIO register */
static inline void mmio_write_u32(unsigned long reg, unsigned int offset,
                                  uint32_t val) {
  (*(volatile uint32_t *)((reg) + (offset))) = val;
}

#endif