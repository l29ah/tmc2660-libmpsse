CFLAGS += -D_DEFAULT_SOURCE -Wall -Wextra -std=c11
CFLAGS += -Og -ggdb3
LDLIBS += -lmpsse

PROJECT=tmc2660-libmpsse

all: ${PROJECT}

${PROJECT}: ${PROJECT}.o

.PHONY:	clean

clean:
	rm -rf *.o
	rm -rf ${PROJECT}

