#ifndef WZNMICKEY_STRING
#define WZNMICKEY_STRING
#include <stdbool.h>
typedef struct string {
  char *start;
  int len;
  int size;
  int sizeIndex;

  int used; // free will free all the malloc, which will make splice illegal.
            // Only can be free when it is 1(itself only) and mallocStart==this
            // (start point).
  struct string
      *mallocStart; // free will free all the malloc, which will make splice
                    // illegal. Use this to find the start point of malloc.
} string;
const int STRING_LEN[9] = {8, 16, 32, 64, 128, 256, 512, 1024, 2048};
int getNextLen(int x);
string *initString(char input[]);
string *deleteString(string *val);
int changeString(string *st, char aim, char newOne);
string *spiltString(string *st, char aim);
#endif
