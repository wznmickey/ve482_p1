#include "string.h"
#include <stdlib.h>

// since the string is not very long. Use int rather than size_t.
string *initString(char input[]) {
  string *val = malloc(sizeof(string));
  val->sizeIndex = getNextLen(strlen(input));
  val->size = STRING_LEN[val->sizeIndex];
  val->start = malloc(sizeof(char) * val->size);
  val->len = strlen(input);
  strcpy(val->start, input);
  val->mallocStart = val;
  val->used = 1; // itself
  return val;
}
string *deleteString(string *val) {

  if ((val->used == 1) &&
      (val->mallocStart ==
       val)) // it is the first string and the full block is useless
  {
    free(val->start);
    free(val);
  } else {
    if (val->mallocStart->used ==
        1) // it is not the first string but the full block is useless. Can
           // not hebing with the above because we need make sure
           // val!=val->mallocStart. Otherwise it is free twice.
    {
      free(val->mallocStart->start);
      free(val->mallocStart);
      free(val);
    } else {
      if (val->mallocStart ==
          val) // it is the first string but the full block is useful
      {
        val->used--;
      } else // it is not the first string and the full block is useful
      {
        val->mallocStart->used--;
      }
    }
  }
  return NULL;
}
int getNextLen(int x) {
  if (x <= 3)
    return 0;
  if (x <= 7)
    return 1;
  if (x <= 15)
    return 2;
  if (x <= 31)
    return 3;
  if (x <= 63)
    return 4;
  if (x <= 127)
    return 5;
  if (x <= 255)
    return 6;
  if (x <= 511)
    return 7;
  return 8;
}
int findString(string *st, char aim) {
  for (int i = 0; i < st->len; i++) {
    if (st->start[i] == aim) {
      return i;
    }
  }
  return -1; // Not found
}

int changeString(string *st, char aim, char newOne) {
  int x = findString(st, aim);
  if (x == -1)
    return -1; // failed
  st->start[x] = newOne;
  return x;
}

string *spiltString(string *st, char aim) {
  int find = changeString(st, aim, '\0');
  if (find == -1)
    return NULL; // failed
  string *val = malloc(sizeof(string));

  val->mallocStart = st->mallocStart;
  val->start = st->start + find + 1;
  val->len = strlen(val->start);
  val->size = st->size; // It is meaningless. Just to init. The value will not
                        // be updated.
  val->sizeIndex = st->sizeIndex; // It is meaningless. Just to init. The value
                                  // will not be updated.
  val->used =
      1; // It is meaningless. Just to init. The value will not be updated.
  st->len = strlen(st->start);
  st->mallocStart->used++;
  return val;
}