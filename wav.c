//Simon Parker
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <stdint.h>
#include <portaudio.h>

#define PATH_1 "./sine.wav\0"
#define PATH_2 "./clipped.wav\0"
#define DURATION 1
#define SAMPLE_RATE 48000
#define FREQUENCY 440
#define NUM_CHANNELS 1
#define SAMPLE_BITS 16
#define AMPLITUDE 0.25
#define MAX_INT16 32767


//writes num_bytes of an int to a file pointed to by fp, in little endian
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

//writes a sine wave to a file pointed to by fp, each sample is little endian
void write_sine(FILE *fp){
  int16_t sample;
  for (int i = 0; i < DURATION*SAMPLE_RATE; i++){
    sample = (int16_t) (sin(2.0*M_PI*i*FREQUENCY/SAMPLE_RATE)*MAX_INT16*AMPLITUDE);
    write_LE(fp, sample, 2);
  }
}

//writes a clipped sine wave to fp
void write_clipped_sine(FILE *fp){
  int16_t sample;
  for (int i = 0; i < DURATION*SAMPLE_RATE; i++){
    sample = (int16_t) (sin(2.0*M_PI*i*FREQUENCY/SAMPLE_RATE)*MAX_INT16*2.0*AMPLITUDE);
    sample = sample > 8192 ? 8192 : (sample < -8192 ? -8192 : sample);
    write_LE(fp, sample, 2);
  }
}

//used to keep track of progress of audio playback
typedef struct {
    int cur_sample;
    int frames_remaining;
} SampleTracker;


//fills a buffer with samples and sends that to the audio stream
int audio_callback(const void *input, void *output,
                          unsigned long frame_count,
                          const PaStreamCallbackTimeInfo *time_info,
                          PaStreamCallbackFlags flags,
                          void *user_data) {
    SampleTracker *data = (SampleTracker *) user_data;
    int16_t *out = (int16_t *) output;
    int16_t sample;

    for (unsigned long i = 0; i < frame_count; i++) {
        if (data->frames_remaining <= 0) {
            out[i] = 0.0; //fill out the frame buffer with 0s. can't send partially filled buffers :(
            continue;
        }
        sample = (int16_t) (sin(2.0*M_PI*data->cur_sample*FREQUENCY/SAMPLE_RATE)*MAX_INT16*2.0*AMPLITUDE);
        sample = sample > 8192 ? 8192 : (sample < -8192 ? -8192 : sample);
        //i don't like that i have to recalculate the sample. But, I can't write to the wav file in this function, that's too slow

        out[i] = sample;

        data->cur_sample += 1;

        data->frames_remaining--;
    }

    return data->frames_remaining <= 0 ? paComplete : paContinue;
}

  
int main(){
  // File Creation
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

  // Audio Playback
  SampleTracker data = {
      .cur_sample = 0,
      .frames_remaining = SAMPLE_RATE * DURATION
  };

  Pa_Initialize();

  PaStream *stream;
  Pa_OpenDefaultStream(&stream,
      0,                  // no input channels
      1,                  // mono
      paInt16,            // 16 bit int samples
      SAMPLE_RATE,
      256,                // arbitrary?
      audio_callback,
      &data);

  Pa_StartStream(stream);

  while (Pa_IsStreamActive(stream) == 1) //busy waiting, bad practice but whatever
      Pa_Sleep(100);

  Pa_StopStream(stream);
  Pa_CloseStream(stream);
  Pa_Terminate();

  return 0;
}
