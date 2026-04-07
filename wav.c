//Simon Parker
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <stdint.h>

#define PATH_1 "./sine.wav\0"
#define PATH_2 "./clipped.wav\0"
#define DURATION 1
#define SAMPLE_RATE 48000
#define FREQUENCY 440
#define NUM_CHANNELS 1
#define SAMPLE_BITS 16
#define AMPLITUDE 0.25
#define MAX_INT16 32767


void write_LE(FILE *fp, int num, int num_bytes){
  char byte;
  for (int i = 0; i < num_bytes; i++){
    byte = (num >> 8*i) & 0xff;
    fwrite(&byte, 1, 1, fp);
  }
}

//writes a string to the file pointed to by f
void write_str(FILE *fp, char *str){
  fwrite(str, strlen(str), 1, fp);
}

void write_header(FILE *fp){
  write_str(fp, "RIFF");
  write_LE(fp, (44 + (NUM_CHANNELS*SAMPLE_RATE*DURATION*SAMPLE_BITS/8)) - 8, 4);
  write_str(fp, "WAVE");
  write_str(fp, "fmt ");
  write_LE(fp, 16, 4);
  write_LE(fp, 1, 2);
  write_LE(fp, NUM_CHANNELS, 2);
  write_LE(fp, SAMPLE_RATE, 4);
  write_LE(fp, SAMPLE_RATE*NUM_CHANNELS*SAMPLE_BITS/8, 4); //Byte rate
  write_LE(fp, NUM_CHANNELS*SAMPLE_BITS/8, 2); //Bytes per frame
  write_LE(fp, SAMPLE_BITS, 2); 
  write_str(fp, "data");
  write_LE(fp,NUM_CHANNELS*SAMPLE_RATE*DURATION*SAMPLE_BITS/8, 4); 
}

void write_sine(FILE *fp){
  int16_t sample;
  for (int i = 0; i < DURATION*SAMPLE_RATE; i++){
    sample = (int16_t) (sin(2.0*M_PI*i*FREQUENCY/SAMPLE_RATE)*MAX_INT16*AMPLITUDE);
    write_LE(fp, sample, 2);
  }
}

void write_clipped_sine(FILE *fp){
  int16_t sample;
  for (int i = 0; i < DURATION*SAMPLE_RATE; i++){
    sample = (int16_t) (sin(2.0*M_PI*i*FREQUENCY/SAMPLE_RATE)*MAX_INT16*2.0*AMPLITUDE);
    sample = sample > 8192 ? 8192 : (sample < -8192 ? -8192 : sample);
    write_LE(fp, sample, 2);
  }
}
  
int main(){
  FILE *fp = fopen(PATH_1, "wb");
  if (fp == NULL) {
    perror("fopen");
    return 1;
  }
  write_header(fp);
  write_sine(fp);
  fclose(fp);
  fp = fopen(PATH_2, "wb");
  if (fp == NULL) {
    perror("fopen");
    return 1;
  }
  write_header(fp);
  write_clipped_sine(fp);
  fclose(fp);
}
