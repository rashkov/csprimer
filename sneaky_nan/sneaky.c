#include <stdio.h>
#include <math.h>
#include <stdint.h>
#include <assert.h>
#include <stdlib.h>

uint32_t NAN_MASK = 0xFF800000;

void encode(char * str, float * nan){
  uint32_t * nan_u = (uint32_t *) nan;
  *nan_u = *nan_u & NAN_MASK; // clear low order bits
  *nan_u = *nan_u ^ *((uint32_t *) str);
}

void decode(float * nan){
  uint32_t decoded = *((uint32_t *) nan) & 0x000FFFFF; // grab low-order bits
  printf("decoded: %s", &decoded);
}

int main(int argc, char ** argv){
  if (argc < 2){
    printf("Usage ./sneaky <string>");
    exit(1);
  }
  char * str = argv[1];
  printf("Input string: %s\n", str);
  uint32_t nan = NAN_MASK + 0x1;
  union {
      float f;
      uint32_t u;
  } u2f = { .u = nan };

  assert(isnan(u2f.f));

  // encode
  encode(str, &u2f.f);

  assert(isnan(u2f.f));

  // decode
  decode(&u2f.f);
}
