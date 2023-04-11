#include <stdio.h>
#include <math.h>
#include <stdint.h>
#include <assert.h>
#include <stdlib.h>

// could go as low as 0xFFF0... but we can't fit a char into half a byte anyway
uint64_t NAN_MASK = 0xFFFF000000000000;
uint64_t INV_NAN_MASK = ~0xFFFF000000000000;

void encode(char * str, double * nan){
  uint64_t * nan_u = (uint64_t *) nan;
  uint64_t * str_u = (uint64_t *) str;
  *nan_u = *nan_u & NAN_MASK; // clear low order bits
  *nan_u = *nan_u | *str_u;
}

void decode(double * nan){
  uint64_t decoded = *((uint64_t *) nan) & INV_NAN_MASK; // grab low-order bits
  // fun fact.. we didn't explicitly null terminate this string,
  // but it works out because the last byte is zeroed out during masking & un-masking
  printf("outpt: %s", &decoded);
}

int main(int argc, char ** argv){
  if (argc < 2){
    printf("Usage ./sneaky <string>");
    exit(1);
  }

  char * str = argv[1];

  printf("input: %s\n", str);

  uint64_t nan = NAN_MASK;
  double * nan_f = (double *) &nan; // make nan_f a pointer to avoid conversion during cast

  assert(isnan(*nan_f));

  // encode
  encode(str, nan_f);

  assert(isnan(*nan_f));

  // decode
  decode(nan_f);
}
