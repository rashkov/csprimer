#include <stdio.h>
#include <math.h>
#include <stdint.h>

void encode(char * str){

}

void decode(float nan){

}

int main(int argc, char * argv){
  // float is 4 bytes
  //
  //float nan = 0xFF800000;
  //float nan = 0xFFFFFFFFFFFFFFFE;
  //float nan = 0xFF900000;
  //double nan = 0b0111111111111000000000000000000000000000000000000000;
  //printf("%d\n", isnan(sqrt(-1.0)));
  //printf("%a\n", sqrt(-1.0));
  //printf("%p\n", sqrt(-1.0));
  //printf("%x\n", sqrt(-1.0));

  // Apparently this is not a valid way to assign hex to float!
  float nan = 0x7FC00000;
  printf("%d\n", isnan(nan)); // false
  printf("%a\n", nan); // 0x1.ffp+30

  float nan2 = 0b01000000000000000000000000000000;
  printf("%d\n", isnan(nan2)); // false
  printf("%a\n", nan2); // 0x1p+30

  // Use this instead
  union {
      float f;
      uint32_t u;
  } u2f = { .u = nan };
  printf("%d\n", isnan(u2f.f)); // true
  printf("%f\n", u2f.f);
}


// floats are 4 bytes or 32 bits
// printf("%d\n", sizeof(float));
// IEEE 754:
//   NaN: set exponent bits to all 1
//     fraction bits to any non-zero values
//     fraction bits used to encode message:
//       signaling NaN or quiet NaN
//       signaling NaN = invalid operation, such as divide by zero
//                       or sqrt(-1)
//       quiet NaN = used to represent undefined or unrepresentable values,
//                   such as the result of an operation involving infinity or a NaN itself.
//       the most significant bit of the fraction field
//       is used to distinguish between signaling NaNs and quiet NaNs.
//  We can use the fraction bits ot encode an arbitrary message
//  The format of a 32-bit float:
//    1 sign bit, 8 exponent bits, 23 mantissa bits = 32 bits total
//  Does the sign bit have significance? No it can be set to anything
//
//  okay so conver this to hex, and then how to check if it's a NaN in C?
//  Each hex bit takes on 16 possible values 0 through 15, or 2^4, so four bits
//  Let's set sign bit to 1 for simplicity
//  0xFF takes care of sign bit and 7 out of 8 exponent bits
//  The fellowing hex digit digit must be greater than 0b1000 = 8 dec = 0x8
//  So anything greater than 0xFF800000 is definitely a NaN
//  let's try this in C and check if it's a NaN
