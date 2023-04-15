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

int read_one(int fd, void * buf){
  if(read(fd, buf, 1) != 1) exit(0);
  return 1;
}

void read_through_newline(int fd, char * buf){
  while(read_one(fd, buf) && *buf != '\n'){};
}

int main(int argc, char **argv){
  int fd;
  char c;
  uint8_t n;

  fd = open(INPUT, O_RDONLY);
  read_one(fd, &n);

  for(; n >= 0; n--){
    // read in a character
    read_one(fd, &c);
    if (c == '\n') {
      read_one(fd, &n);
    }
    if ((c & UTF_LEN_4_MASK) == UTF_LEN_4_MASK && n < 3){
      read_through_newline(fd, &c);
      read_one(fd, &n);
    }
    if ((c & UTF_LEN_3_MASK) == UTF_LEN_3_MASK && n < 2){
      read_through_newline(fd, &c);
      read_one(fd, &n);
    }
    if ((c & UTF_LEN_2_MASK) == UTF_LEN_2_MASK && n < 1){
      read_through_newline(fd, &c);
      read_one(fd, &n);
    }
    if (n == 0){
      printf("%c", c);
      read_through_newline(fd, &c);
      read_one(fd, &n);
    }
    // if newline:
    //   print it
    //   read(n)
    //   continue
    // if unicode_4
    //   if n < 3
    //     read through newline
    //     continue
    //   else
    //     print it
    // if unicode_3
    //   if n < 2
    //     read through newline
    //     continue
    //   else
    //     print it
    // if unicode_2
    //   if n < 1
    //     read through newline
    //     continue
    //   else
    //     print it
    // if n == 0 // time to truncate
    //   read through newline
    //   continue
    // else
    //   print it

    printf("%c", c);
  }
}
