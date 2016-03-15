default: all

CFLAGS := -I./include -g --std=gnu99
CC := gcc

BINARIES := flock-bots-multi-control-server
all : $(BINARIES)

LIBS := -lach 

flock-bots-multi-control-server: src/flock-bots-multi-control-server.o
	gcc -o $@ $< $(LIBS)

%.o: %.c
	$(CC) $(CFLAGS) -o $@ -c $<

clean:
	rm -f $(BINARIES) src/*.o
