#ifndef __SPI_H__
#define __SPI_H__
#include <stdint.h>

enum spi_type {
#ifdef HAS_LIBMPSSE
	SPI_LIBMPSSE,
#endif
	SPI_SPIDEV,
};

typedef struct {
	enum spi_type type;
	intptr_t dev;
} spi_t;

spi_t *dev_open(char *addr);
void dev_xfer(spi_t *dev, uint8_t *data, unsigned len);
#endif
