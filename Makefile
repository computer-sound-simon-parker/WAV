CC = gcc
LIBS = -lportaudio -lm

all: wav

wav: wav.c
	$(CC) -o wav wav.c $(LIBS)

clean:
	rm -f wav *.o *.wav
