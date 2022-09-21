#ifndef WZNMICKEY_STRING
#define WZNMICKEY_STRING
#include <stdbool.h>

typedef struct String {
  char *start;
  int len;
  int size;
  int sizeIndex;

  int used; // free will free all the malloc, which will make splice illegal.
            // Only can be free when it is 1(itself only) and mallocStart==this
            // (start point).
  struct String
      *mallocStart; // free will free all the malloc, which will make splice
                    // illegal. Use this to find the start point of malloc.
} String;
const int STRING_LEN[9] = {8, 16, 32, 64, 128, 256, 512, 1024, 2048};
int getNextLen(int x);
String *initString(char input[]);
String *deleteString(String *val);
int changeString(String *st, char aim, char newOne);
String *spiltString(String *st, char aim);
#endif
