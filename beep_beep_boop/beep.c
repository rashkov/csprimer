#include <stdio.h>
#include <unistd.h>
#include <termios.h>
#include <stdint.h>

int main(){
  char in;
  uint8_t count;
  struct termios term;
  tcgetattr(1, &term);
  term.c_lflag &= ~ICANON;
  term.c_lflag &= ~ECHO;
  tcsetattr(1, TCSANOW, &term);
  while(in = getchar()){
    if(in >= '0' && in <= '9'){
      for(count=0; count < in - '0'; count++){
        printf("beep\a\r\n");
        fflush(stdout);
        sleep(1);
      }
    }
  }
  return 0;
}
