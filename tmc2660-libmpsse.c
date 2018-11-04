#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <inttypes.h>
#include <mpsse.h>
#include <endian.h>
#include <unistd.h>

static struct mpsse_context *ctx = NULL;

#define STEP_PIN GPIOL0
#define DIR_PIN GPIOL1

static uint8_t *xfer(uint8_t data[3])
{
	Start(ctx);
	printf("Writing: %x %x %x\n", data[0], data[1], data[2]);
	uint8_t *rv = Transfer(ctx, data, 3);
	if (rv) {
		printf("Read: %x %x %x, %d\n", rv[0], rv[1], rv[2], rv[1] >> 6 | rv[0] << 2);
		printf("Motor stall: %d\n", rv[2] & (1 << 4));
		printf("Overtemperature shutdown: %d\n", rv[2] & (1 << 5));
		printf("Overtemperature warning: %d\n", rv[2] & (1 << 6));
		printf("Short to ground A: %d\n", rv[2] & (1 << 7));
		printf("Short to ground B: %d\n", rv[1] & (1 << 0));
		printf("Open load A: %d\n", rv[1] & (1 << 1));
		printf("Open load B: %d\n", rv[1] & (1 << 2));
		printf("Standstill: %d\n", rv[1] & (1 << 3));
	}
	Stop(ctx);
	return rv;
}

int main(int argc, char *argv[])
{
	(void)argc; (void)argv;
	int retval = EXIT_FAILURE;

	if ((ctx = MPSSE(SPI3, 100000, MSB)) != NULL && ctx->open) {
		uint8_t chopconf[] = { 0xf9, 0x01, 0xb4, };
		xfer(chopconf);
		uint8_t sgcsconf[] = { 0xfd, 1, 0x04, };
		xfer(sgcsconf);
		uint8_t drvconf[] = { 0xfe, 0x00, 0x50, };
		xfer(drvconf);
		uint8_t drvctrl[] = { 0xf0, 0x03, 0x04, };
		xfer(drvctrl);
		uint8_t smarten[] = { 0xfa, 0x82, 0x02, };
		xfer(smarten);
		while (true) {
			for (int i = 0; i < 100; ++i) {
				PinLow(ctx, STEP_PIN);
				usleep(1000);
				PinHigh(ctx, STEP_PIN);
				usleep(1000);
			}
			xfer(drvctrl);
		}
		retval = EXIT_SUCCESS;
	} else {
		fprintf(stderr, "Failed to initialize MPSSE: %s\n", ErrorString(ctx));
	}

	Close(ctx);

	return retval;
}
