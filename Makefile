SOURCES = gitwho.c
BIN = gitwho
OBJS = gitwho.o
CFLAGS = -O2

all: ${BIN}

${OBJS}: ${SOURCES}
	gcc -c ${SOURCES} ${CFLAGS}

${BIN}: ${OBJS}
	gcc -o ${BIN} ${OBJS} ${CFLAGS}

clean:
	rm -f *.o
	rm -f ${BIN}
