#include <stdio.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include <wchar.h>
#include <locale.h>
#include <stdint.h>


char * INPUT = "./cases";

char UTF_LEN_2_MASK = 0xC0; // 110x xxxx
char UTF_LEN_3_MASK = 0xE0; // 1110 xxxx
char UTF_LEN_4_MASK = 0xF0; // 1111 0xxx

int read_one(int fd, void * buf, char * c){
  if(read(fd, buf, 1) != 1){
    if(c != NULL){
      printf("%c", *c);
    }
    exit(0);
  }
  return 1;
}

void read_through_newline(int fd, char * buf){
  while(read_one(fd, buf, NULL) && *buf != '\n'){};
}

int main(int argc, char **argv){
  int fd;
  char c;
  uint8_t n;

  fd = open(INPUT, O_RDONLY);

  read_one(fd, &n, NULL);

  for(; n >= 0; n--){
    // read in a character
    read_one(fd, &c, NULL);
    if (c == '\n') {
      read_one(fd, &n, &c);
    }
    if ((c & UTF_LEN_4_MASK) == UTF_LEN_4_MASK && n < 3){
      read_through_newline(fd, &c);
      read_one(fd, &n, &c);
    }
    if ((c & UTF_LEN_3_MASK) == UTF_LEN_3_MASK && n < 2){
      read_through_newline(fd, &c);
      read_one(fd, &n, &c);
    }
    if ((c & UTF_LEN_2_MASK) == UTF_LEN_2_MASK && n < 1){
      read_through_newline(fd, &c);
      read_one(fd, &n, &c);
    }
    if (n == 0){
      printf("%c", c);
      read_through_newline(fd, &c);
      read_one(fd, &n, &c);
    }
    printf("%c", c);
  }
}
