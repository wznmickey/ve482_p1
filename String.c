
#include "String.h"
#include <stdlib.h>
#include <string.h>
// since the string is not very long. Use int rather than size_t.
String *initString(char input[]) {
  String *val = malloc(sizeof(String));
  val->sizeIndex = getNextLen((int)strlen(input));
  val->size = STRING_LEN[val->sizeIndex];
  val->start = malloc(sizeof(char) * (size_t) val->size);
  val->len = (int)strlen(input);
  strcpy(val->start, input);
  val->mallocStart = val;
  val->used = 1; // itself
  return val;
}
String *deleteString(String *val) {
  if ((val->used == 1) &&
      (val->mallocStart ==
       val)) // it is the first string and the full block is useless
  {
    free(val->start);
    free(val);
  } else {
    if (val->mallocStart->used ==
        1) { // it is not the first string but the full block is useless. Can
             // not hebing with the above because we need make sure
             // val!=val->mallocStart. Otherwise it is free twice.
      free(val->mallocStart->start);
      free(val->mallocStart);
      free(val);
    } else {
      if (val->mallocStart ==
          val) { // it is the first string but the full block is useful

        val->used--;
      } else { // it is not the first string and the full block is useful

        val->mallocStart->used--;
      }
    }
  }
  return NULL;
}
int getNextLen(int x) {
  if (x <= 3) {
    return 0;
  }
  if (x <= 7) {
    return 1;
  }
  if (x <= 15) {
    return 2;
  }
  if (x <= 31) {
    return 3;
  }
  if (x <= 63) {
    return 4;
  }
  if (x <= 127) {
    return 5;
  }
  if (x <= 255) {
    return 6;
  }
  if (x <= 511) {
    return 7;
  }
  return 8;
}
int findString(String *st, char aim) {
  for (int i = 0; i < st->len; i++) {
    if (st->start[i] == aim) {
      return i;
    }
  }
  return -1; // Not found
}

int changeString(String *st, char aim, char newOne) {
  int x = findString(st, aim);
  if (x == -1) {
    return -1; // failed
  }
  st->start[x] = newOne;
  return x;
}

String *spiltString(String *st, char aim) {
  int find = changeString(st, aim, '\0');
  if (find == -1) {
    return NULL; // failed
  }
  String *val = malloc(sizeof(String));

  val->mallocStart = st->mallocStart;
  val->start = st->start + find + 1;
  val->len = (int)strlen(val->start);
  val->size = st->size; // It is meaningless. Just to init. The value will not
                        // be updated.
  val->sizeIndex = st->sizeIndex; // It is meaningless. Just to init. The value
                                  // will not be updated.
  val->used =
      1; // It is meaningless. Just to init. The value will not be updated.
  st->len = (int)strlen(st->start);
  st->mallocStart->used++;
  return val;
}