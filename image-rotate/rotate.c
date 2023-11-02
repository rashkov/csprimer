#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdint.h>
#include <inttypes.h>
#include <arpa/inet.h>

const char * FILENAME = "teapot.bmp";

struct bmp_header {
  // two bytes magic identifier 0x4d42
  uint16_t magic;
  uint32_t size;
  uint16_t reserved1;
  uint16_t reserved2;
  uint32_t offset;
} __attribute__((packed)); // prevent padding this struct to 16 bytes, which
                           // breaks things bigly

int main(){
  struct bmp_header header;
  uint32_t dib_size;
  int32_t width;
  int32_t height;

  printf("Rotating %s\n", FILENAME);

  int fd = open(FILENAME, O_RDONLY);
  printf("%d\n", sizeof(struct bmp_header));
  read(fd, &header, sizeof(struct bmp_header));

  // first byte 42, second byte 4d
  //printf("%02x %02x\n", buf[0], buf[1]);
  if(header.magic != 0x4d42){
    perror("Not a valid BMP file");
    exit(-1);
  }

  printf("Filesize %" PRIu32 "\n", header.size);
  printf("Offset %" PRIx32 "\n", header.offset);

  read(fd, &dib_size, sizeof(dib_size));
  printf("DIB size (124 means BITMAPV5Header) %" PRIu32 "\n", dib_size);

  read(fd, &width, sizeof(width));
  printf("Width %" PRId32 "\n", width);

  read(fd, &height, sizeof(height));
  printf("Height %" PRId32 "\n", height);

  return 0;
}
