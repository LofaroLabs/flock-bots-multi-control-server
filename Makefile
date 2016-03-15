default: all

CFLAGS := -I./include -g --std=gnu99
CC := gcc

BINARIES := flock-bots-multi-control-server flock-bots-sender
all : $(BINARIES)

LIBS := -lach 

flock-bots-sender: src/flock-bots-sender.o
	gcc -o $@ $< $(LIBS)

flock-bots-multi-control-server: src/flock-bots-multi-control-server.o
	gcc -o $@ $< $(LIBS)

%.o: %.c
	$(CC) $(CFLAGS) -o $@ -c $<

clean:
	rm -f $(BINARIES) src/*.o
