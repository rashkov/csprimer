#include <stdio.h>
#include <math.h>
#include <stdint.h>
#include <assert.h>
#include <stdlib.h>

// could go as low as 0xFFF0... but we can't fit a char into 4 bytes anyway
uint64_t NAN_MASK = 0xFFFF000000000000;

void encode(char * str, double * nan){
  uint64_t * nan_u = (uint64_t *) nan;
  uint64_t * str_u = (uint64_t *) str;
  *nan_u = *nan_u & NAN_MASK; // clear low order bits
  *nan_u = *nan_u | *str_u;
}

void decode(double * nan){
  uint64_t decoded = *((uint64_t *) nan) & 0x0000FFFFFFFFFFFF; // grab low-order bits
  printf("decoded: %s", &decoded);
}

int main(int argc, char ** argv){
  if (argc < 2){
    printf("Usage ./sneaky <string>");
    exit(1);
  }

  char * str = argv[1];
  printf("Input string: %s\n", str);
  uint64_t nan = NAN_MASK + 0x1;
  union {
      double f;
      uint64_t u;
  } u2f = { .u = nan };

  assert(isnan(u2f.f));

  // encode
  encode(str, &u2f.f);

  assert(isnan(u2f.f));

  // decode
  decode(&u2f.f);
}
