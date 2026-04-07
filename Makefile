CC = gcc
CFLAGS = -Wall -Wextra

all: wav

wav: wav.c
	$(CC) $(CFLAGS) -o wav wav.c 

clean:
	rm -f wav *.o *.wav
