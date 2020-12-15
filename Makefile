CC=gcc
CFLAGS=-Og -g -march=native -std=c11 -Wall -Wno-stringop-truncation -I.
LIBS = lclib.h
OBJS = lc.o lclib.o
MAIN = lc.c

%.o: %.c $(LIBS) #libs = objects confirmed
	$(CC) -c -o $@ $(CFLAGS) $< 

lc: $(OBJS)
	$(CC) -o lc -I $(CFLAGS) $^

debug-lc: lc.c
	$(CC) -o lc-debug -Og -g $<

check:
	echo "check"
	valgrind --leak-check=full ./lc --help


clean:
	$(RM) lc
	$(RM) $(OBJS)

