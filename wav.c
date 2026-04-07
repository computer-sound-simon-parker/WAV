//Simon Parker
#include "stdio.h"
#include "stdlib.h"

char *path1 = "./sine.wav\0";
char *path2 = "./clipped.wav\0";

//writes count bytes from buff to the end of the file pointed to by f, done in Little Endian
//not doing checks to make sure you don't read from beyond buff, so be good. count <= buff size
void write_LE_u32(FILE *f, uint32_t *buff, int count){
  char temp[count];
  for (int i = 0; i < count; i++){
    for (int j = 0; j < count; j++){
      temp[j] = (buff[i] >> 8*j) & 0xff;
    }
    fwrite(temp, count, 1, f);
  }
}

int main(){
  printf("Hello World\n");  
  char buff[5] = "RIFF";
  FILE *fp = fopen("./test.wav", "wb");
  fwrite(buff, 1, 4, fp);
  uint32_t buf32[4] = {0, 1, 2, 3};
  write_LE_u32(fp, buf32, 4);
  fclose(fp);
}
