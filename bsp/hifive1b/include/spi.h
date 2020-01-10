#ifndef __CR2_SPI_H__
#define __CR2_SPI_H__
#ifdef __cplusplus
extern "C" {
#endif

#include <memory_map.h>

#define SPI_REG_SCK_DIV 0x00
#define SPI_REG_SCK_MODE 0x04
#define SPI_REG_CS_ID 0x10
#define SPI_REG_CS_DEF 0x14
#define SPI_REG_CS_MODE 0x18

#define SPI_REG_DELAY_CTRL0 0x28
#define SPI_REG_DELAY_CTRL1 0x2C

#define SPI_REG_FMT 0x40

#define SPI_REG_TX_DATA 0x48
#define SPI_REG_RX_DATA 0x4C
#define SPI_REG_TX_MARK 0x50
#define SPI_REG_RX_MARK 0x54

#define SPI_REG_FLASH_CTRL 0x60
#define SPI_REG_FLASH_FMT 0x64

#define SPI_REG_INT_EN 0x70
#define SPI_REG_INT_PENDING 0x74

typedef enum spi_t {
  QSPI0 = QSPI0_CTRL_ADDR,
  SPI1 = SPI1_CTRL_ADDR,
  SPI2 = SPI2_CTRL_ADDR
} spi_t;

#define qspi0_reg(reg) mmio(QSPI0_CTRL_ADDR, reg)
#define spi1_reg(reg) mmio(SPI1_CTRL_ADDR, reg)
#define spi2_reg(reg) mmio(SPI2_CTRL_ADDR, reg)

#define SPI_TX_DATA_FULL 0x80000000ul
#define SPI_RX_DATA_EMPTY 0x80000000ul

#define SPI_POLARITY_LOW 0x0
#define SPI_POLARITY_HIGH 0x1

#define SPI_SCK_PHASE_LEADING 0x0
#define SPI_SCK_PHASE_TRAILING 0x1

#define SPI_CS_MODE_AUTO 0x0
#define SPI_CS_MODE_HOLD 0x2
#define SPI_CS_MODE_OFF 0x3

#define SPI_PROTOCOL_SINGLE 0x0
#define SPI_PROTOCOL_DUAL 0x1
#define SPI_PROTOCOL_QUAD 0x2

#define SPI_ENDIAN_MSB 0x0
#define SPI_ENDIAN_LSB 0x1

#define SPI_DIR_RX 0x0
#define SPI_DIR_TX 0x1

typedef struct spi_config_t {
  uint8_t csid;
  unsigned int cs_polarity : 4;
  unsigned int protocol : 2;
  unsigned int polarity : 1;
  unsigned int phase : 1;
  unsigned int endian : 1;
} spi_config_t;

void spi_init(spi_t spi, uint32_t baud);
void spi_deinit(spi_t spi);
void spi_config(spi_t spi, spi_config_t* cfg);
void spi_write_byte(spi_t spi, uint8_t b);
void spi_write_bytes(spi_t spi, uint8_t* b, unsigned int count);
uint8_t spi_read_byte(spi_t spi);
void spi_read_bytes(spi_t spi, uint8_t* b, unsigned int count);
void spi_transfer(spi_t spi, uint8_t* send, uint8_t* recv, unsigned int count);

#ifdef __cplusplus
}
#endif
#endif