#include <spi.h>

#include <clock.h>

#define SPI_FMT_CLEAR 0xFFF0FFF0ul
#define SPI_FMT_FLASH_DIR 8u
#define SPI_FMT_ENDIAN_SHIFT 1u
#define SPI_FMT_BPF_SHIFT 16u
#define SPI_FMT_POL_SHIFT 1u

#define SPI_SCK_MODE_CLEAR 0xFFFFFFFCul

void spi_init(spi_t spi, spi_config_t* cfg) {
  uint32_t baud = get_cpu_freq() / cfg->baud - 1;

  mmio(spi, SPI_REG_FMT) &= SPI_FMT_CLEAR;  // clear fmt
  mmio(spi, SPI_REG_FMT) |=
      cfg->protocol |
      (spi == QSPI0 ? SPI_FMT_FLASH_DIR : 0u) |  // set to 1 for flash
      (uint32_t)(cfg->endian << SPI_FMT_ENDIAN_SHIFT) |
      (0x8u << SPI_FMT_BPF_SHIFT);  // 8 bits per frame

  mmio(spi, SPI_REG_SCK_MODE) &= SPI_SCK_MODE_CLEAR;  // clear sck mode
  mmio(spi, SPI_REG_SCK_MODE) |=
      (uint32_t)(cfg->phase | (cfg->polarity << SPI_FMT_POL_SHIFT));
}

void spi_putc(spi_t spi, char c) { spi_putb(spi, (uint8_t)c); }

void spi_putb(spi_t spi, uint8_t b) {}

void spi_write(spi_t spi, uint8_t* b, int count) {}
