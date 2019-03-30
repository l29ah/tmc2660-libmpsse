#include "spi.h"

#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <linux/types.h>
#include <linux/spi/spidev.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#ifdef HAS_LIBMPSSE
#include <mpsse.h>
#endif

spi_t *dev_open(char *addr)
{
	if (addr) {
		int fd = open(addr, O_RDWR);
		if (fd < 0) {
			perror("open");
			return NULL;
		}
		ioctl(fd, SPI_IOC_WR_MODE, SPI_MODE_3);
		spi_t *rv = malloc(sizeof(spi_t));
		rv->type = SPI_SPIDEV;
		rv->dev = (intptr_t)fd;
		return rv;
	} else {
#ifdef HAS_LIBMPSSE
		struct mpsse_context *ctx = NULL;
		if ((ctx = MPSSE(SPI3, 100000, MSB)) != NULL && ctx->open) {
			spi_t *rv = malloc(sizeof(spi_t));
			rv->type = SPI_LIBMPSSE;
			rv->dev = (intptr_t)ctx;
			return rv;
		} else {
			fprintf(stderr, "Failed to initialize MPSSE: %s\n", ErrorString(ctx));
		}
#endif
	}
	return NULL;
}

void dev_xfer(spi_t *dev, uint8_t *data, unsigned len)
{
	switch (dev->type) {
	case SPI_SPIDEV:
		;
		int fd = dev->dev;
		struct spi_ioc_transfer	xfer[2];
		unsigned char		*bp;
		int			status;

		memset(xfer, 0, sizeof xfer);

		xfer[0].tx_buf = (unsigned long)data;
		xfer[0].len = len;

		xfer[1].rx_buf = (unsigned long)data;
		xfer[1].len = len;

		status = ioctl(fd, SPI_IOC_MESSAGE(2), xfer);
		if (status < 0) {
			perror("SPI_IOC_MESSAGE");
			return;
		}

		printf("response(%2d, %2d): ", len, status);
		for (bp = data; len; len--)
			printf(" %02x", *bp++);
		printf("\n");
		break;
#ifdef HAS_LIBMPSSE
	case SPI_LIBMPSSE:
		;
		struct mpsse_context *ctx = (void *)dev->dev;
		Start(ctx);
		char *rv = Transfer(ctx, (char *)data, len);
		Stop(ctx);
		memcpy(data, rv, len);
		free(rv);
		break;
#endif
	}
}
