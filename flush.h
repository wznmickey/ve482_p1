#ifndef FLUSH_WZNMICKEY
#define FLUSH_WZNMICKEY
#include <stdio.h>
#define printf(...)      \
  do {                   \
    printf(__VA_ARGS__); \
    fflush(NULL);        \
  } while (0);
#define fgets(...)      \
  do {                  \
    fflush(NULL);       \
    fgets(__VA_ARGS__); \
    fflush(NULL);       \
  } while (0);
#endif
