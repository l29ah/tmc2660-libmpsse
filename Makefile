CFLAGS += -D_DEFAULT_SOURCE -Wall -Wextra -std=c11
CFLAGS += -Og -ggdb3

ifdef HAS_LIBMPSSE
CFLAGS += -DHAS_LIBMPSSE
LDLIBS += -lmpsse
endif

PROJECT=tmc2660-libmpsse

all: ${PROJECT}

${PROJECT}: ${PROJECT}.o spi.o

.PHONY:	clean

clean:
	rm -rf *.o
	rm -rf ${PROJECT}

