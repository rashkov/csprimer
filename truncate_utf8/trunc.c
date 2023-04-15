#include <stdio.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include <wchar.h>
#include <locale.h>
#include <stdint.h>


int main(int argc, char **argv){

  int fd = open("./cases", O_RDONLY);
  char * buf = calloc(sizeof(char), 1);

  char c;
  uint8_t n;
  read(fd, &n, 1);
  for(; n >= 0; n--){
    //printf("%u\n", n);
    // read in a chracter
    if(read(fd, &c, 1) != 1) exit(-1);
    printf("%c", c);
    // check if we hit a newline
    if(c == '\n'){
      // read in the next number
      if(read(fd, &n, 1) != 1) exit(-1);
      //printf("%u\n", n);
      continue;
    } else if(n == 0 && 0x80 & c){
      // it's time to cut off but we're in a unicode sequence
      printf("unicode");
    } else if(n == 0) {
      // read through newline
      while(read(fd, &c, 1) == 1 && c != '\n');
      // TODO: error above? what if we get a short read?
      printf("\n");
      // read in a new n
      if(read(fd, &n, 1) != 1) exit(-1);
      continue;
    }
  }
  //printf("%d", n);
  //while(read(fd, buf, 1) == 1){
  //  printf("%s", buf);
  //}
}
