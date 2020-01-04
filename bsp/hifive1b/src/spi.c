#include <spi.h>

#include <clock.h>
#include <gpio.h>

#define SPI_FMT_CLEAR 0xFFF0FFF0ul
#define SPI_FMT_FLASH_DIR 8u
#define SPI_FMT_ENDIAN_SHIFT 1u
#define SPI_FMT_BPF_SHIFT 16u
#define SPI_FMT_POL_SHIFT 1u

#define SPI_SCK_MODE_CLEAR 0xFFFFFFFCul

#define SPI_CS_DEF_CLEAR 0u

#define SPI_FLASH_CTRL_IO 0u

#define SPI_SCK_DIV_CLEAR 0x0FFFul

#define SPI_CS_MODE_CLEAR 0xFFFFFFFCul

void spi_init(spi_t spi, uint32_t baud) {
  mmio(spi, SPI_REG_FLASH_CTRL) = SPI_FLASH_CTRL_IO;
  baud = (get_cpu_freq() / (2 * baud)) - 1;

  mmio(spi, SPI_REG_SCK_DIV) &= SPI_SCK_DIV_CLEAR;
  mmio(spi, SPI_REG_SCK_DIV) |= baud;

  uint32_t mask;
  switch (spi) {
    case QSPI0:
      return;  // QSPI0 always enabled
      break;
    case SPI1:
      mask = GPIO_IOF0_SPI1;
      break;
    case SPI2:
      mask = GPIO_IOF0_SPI2;
      break;
    default:
      return;
  }
  gpio_reg(GPIO_REG_IOF_SEL) &= ~mask;  // select IOF 0
  gpio_reg(GPIO_REG_IOF_EN) |= mask;    // enable spi IOF
}

void spi_deinit(spi_t spi) {
  uint32_t mask;
  switch (spi) {
    case QSPI0:
      return;  // QSPI0 always enabled
      break;
    case SPI1:
      mask = GPIO_IOF0_SPI1;
      break;
    case SPI2:
      mask = GPIO_IOF0_SPI2;
      break;
    default:
      return;
  }
  // disable GPIO function
  gpio_reg(GPIO_REG_IOF_EN) &= ~mask;
}

void spi_config(spi_t spi, spi_config_t* cfg) {
  mmio(spi, SPI_REG_FMT) &= SPI_FMT_CLEAR;  // clear fmt
  mmio(spi, SPI_REG_FMT) |=
      cfg->protocol |
      (spi == QSPI0 ? SPI_FMT_FLASH_DIR : 0u) |  // set to 1 for flash
      (uint32_t)(cfg->endian << SPI_FMT_ENDIAN_SHIFT) |
      (0x8u << SPI_FMT_BPF_SHIFT);  // 8 bits per frame

  mmio(spi, SPI_REG_SCK_MODE) &= SPI_SCK_MODE_CLEAR;  // clear sck mode
  mmio(spi, SPI_REG_SCK_MODE) |=
      (uint32_t)(cfg->phase | (cfg->polarity << SPI_FMT_POL_SHIFT));

  mmio(spi, SPI_REG_CS_DEF) &= SPI_CS_DEF_CLEAR;  // clear csdef
  mmio(spi, SPI_REG_CS_DEF) |= (~(uint32_t)cfg->cs_polarity);

  mmio(spi, SPI_REG_CS_ID) = cfg->csid;
}

void spi_write_byte(spi_t spi, uint8_t b) {
  mmio(spi, SPI_REG_CS_MODE) &= SPI_CS_MODE_CLEAR;
  mmio(spi, SPI_REG_CS_MODE) |= SPI_CS_MODE_HOLD;
  while (mmio(spi, SPI_REG_TX_DATA) & SPI_TX_DATA_FULL)
    ;
  mmio(spi, SPI_REG_TX_DATA) = b;
}

void spi_write_bytes(spi_t spi, uint8_t* b, unsigned int count) {
  mmio(spi, SPI_REG_CS_MODE) &= SPI_CS_MODE_CLEAR;
  mmio(spi, SPI_REG_CS_MODE) |= SPI_CS_MODE_HOLD;

  unsigned int i;
  for (i = 0; i < count; ++count) {
    while (mmio(spi, SPI_REG_TX_DATA) & SPI_TX_DATA_FULL)
      ;
    mmio(spi, SPI_REG_TX_DATA) = b[i];
  }
  mmio(spi, SPI_REG_CS_MODE) &= SPI_CS_MODE_CLEAR;
}

uint8_t spi_read_byte(spi_t spi) {
  mmio(spi, SPI_REG_CS_MODE) &= SPI_CS_MODE_CLEAR;
  mmio(spi, SPI_REG_CS_MODE) |= SPI_CS_MODE_HOLD;
  uint32_t mmval = mmio(spi, SPI_REG_RX_DATA);
  while (mmval & SPI_RX_DATA_EMPTY)
    // TODO: potentially infinite loop
    ;
  return mmval & 0x0FF;
}

void spi_read_bytes(spi_t spi, uint8_t* b, unsigned int count) {
  mmio(spi, SPI_REG_CS_MODE) &= SPI_CS_MODE_CLEAR;
  mmio(spi, SPI_REG_CS_MODE) |= SPI_CS_MODE_HOLD;

  unsigned int i;
  uint32_t mmval;
  for (i = 0; i < count; ++count) {
    mmval = mmio(spi, SPI_REG_RX_DATA);
    while (mmval & SPI_RX_DATA_EMPTY)
      // TODO: potentially infinite loop
      ;
    b[i] = mmval & 0x0FF;
  }
  mmio(spi, SPI_REG_CS_MODE) &= SPI_CS_MODE_CLEAR;
}

void spi_transfer(spi_t spi, uint8_t* send, uint8_t* recv, unsigned int count) {
  mmio(spi, SPI_REG_CS_MODE) &= SPI_CS_MODE_CLEAR;
  mmio(spi, SPI_REG_CS_MODE) |= SPI_CS_MODE_HOLD;

  unsigned int i;
  uint32_t mmval;
  if (send) {
    for (i = 0; i < count; ++count) {
      while (mmio(spi, SPI_REG_TX_DATA) & SPI_TX_DATA_FULL)
        ;
      mmio(spi, SPI_REG_TX_DATA) = send[i];
    }
  }
  if (recv) {
    for (i = 0; i < count; ++count) {
      mmval = mmio(spi, SPI_REG_RX_DATA);
      while (mmval & SPI_RX_DATA_EMPTY)
        // TODO: potentially infinite loop
        ;
      recv[i] = mmval & 0x0FF;
    }
  }

  mmio(spi, SPI_REG_CS_MODE) &= SPI_CS_MODE_CLEAR;
}