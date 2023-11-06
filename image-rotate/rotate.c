#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdint.h>
#include <inttypes.h>
#include <arpa/inet.h>
#include <string.h>

const char * FILENAME = "teapot.bmp";
const char * OUT_FILENAME= "out.bmp";

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
  uint16_t bitcount;
  int32_t width;
  int32_t height;
  unsigned char * pixels = NULL;
  unsigned char * out_pixels= NULL;
  int fd, fd2;
  unsigned char * outbuf;

  printf("Rotating %s\n", FILENAME);

  fd = open(FILENAME, O_RDONLY);

  // Prior to _attribute__((packed)) this was reporting 16 instead of 14!
  // which broke everything that used sizeof(struct bmp_header)
  // printf("%d\n", sizeof(struct bmp_header));

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

  read(fd, &height, sizeof(height)); // 420
  printf("Height %" PRId32 "\n", height); // 420

  // skip planes uint_16t
  lseek(fd, sizeof(uint16_t), SEEK_CUR);

  read(fd, &bitcount, sizeof(bitcount));
  printf("Bitcount (bits per pixel) %" PRIu16 "\n", bitcount);

  lseek(fd, header.offset, SEEK_SET);
  int bytes_per_px = bitcount/8;
  int bytes_per_row = width * bytes_per_px;
  int pxbufsz = bytes_per_row * height;
  pixels = malloc(pxbufsz);
  out_pixels = malloc(pxbufsz);
  read(fd, pixels, pxbufsz);

  // rotate pixels
  unsigned char tmp[bytes_per_px];
  int r_new, c_new;
  ssize_t offset, new_offset;
  for(int r=0; r < height; r++){
    for(int c=0; c < width; c++){
      // pixels are stored "upside down"
      // ie. top of image is last row of pixel data
      // so if we want 90deg right turn of the final picture,
      // we must actually do a 90deg left turn of pixeld ata

      // 90 deg left turn
      // last row becomes last column
      c_new = r;
      // first column becomes last row
      r_new = height - 1 - c;

      // 90 deg right turn
      // zeroth row maps to last column
      // c_new = width - 1 - r;
      // zeroth col maps to zeroth row
      // r_new = c;

      offset = r * bytes_per_row + c * bytes_per_px;
      new_offset = r_new * bytes_per_row + c_new * bytes_per_px;

      memcpy(&(out_pixels[new_offset]), &(pixels[offset]), bytes_per_px);
    }
  }

  int n;
  // write everything back
  lseek(fd, 0, SEEK_SET);
  outbuf = malloc(header.size);
  n = read(fd, outbuf, header.offset);
  if(n != header.offset){
    perror("Short read");
    exit(-1);
  }
  memcpy(outbuf + header.offset, out_pixels, pxbufsz);
  lseek(fd, header.offset + pxbufsz, SEEK_SET);

  int total = header.offset + pxbufsz;
  int remaining = header.size - total;
  n = read(fd, outbuf + total, remaining);
  if(n != remaining){
    perror("Short read");
    exit(-1);
  }

  // write
  fd2 = open(OUT_FILENAME, O_CREAT | O_WRONLY, 0666);
  write(fd2, outbuf, header.size);

  return 0;
}
