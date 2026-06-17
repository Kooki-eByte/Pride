#include <stdio.h>
#include <stddef.h>
#include "rex_defines.h"

#ifndef PRIDEDEF
#define PRIDEDEF static inline
#endif

// Helper macros
#define PRIDE_SWAP(T, a, b) do { T t = a; a = b; b = t; } while (0)
#define PRIDE_SIGN(T, x) ((T)((x) > 0) - (T)((x) < 0))
#define PRIDE_ABS(T, x) (PRIDE_SIGN(T, x)*(x))

// Main context for Pride to work with.
typedef struct PrideContext PrideContext;
struct PrideContext {
  u32 *pixels;
  size_t width;
  size_t height;
  size_t stride; 
};



int main(void) {
  printf("Hello World\n");
  return 0;
}