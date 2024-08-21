GCC = gcc
CFLAGS = -g -Wall -Wshadow

compfs: main.o testScanner.o scanner.o parser.o static.o
	$(GCC) $(CFLAGS) main.o testScanner.o scanner.o parser.o static.o -o compfs -lm

main.o: main.c token.h
	$(GCC) $(CFLAGS) -c main.c

testScanner.o: testScanner.c testScanner.h
	$(GCC) $(CFLAGS) -c testScanner.c

scanner.o: scanner.c scanner.h
	$(GCC) $(CFLAGS) -c scanner.c

parser.o: parser.c parser.h
	$(GCC) $(CFLAGS) -c parser.c

static.o: static.c static.h
	$(GCC) $(CFLAGS) -c static.c

clean: 
	-rm *.o *.asm $(objects) compfs
