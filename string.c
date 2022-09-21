#include "string.h"
#include <stdlib.h>
#include <string.h>

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
  } else if (val->mallocStart->used ==
             1) // it is not the first string but the full block is useless. Can
                // not hebing with the above because we need make sure
                // val!=val->mallocStart. Otherwise it is free twice.
  {
    free(val->mallocStart->start);
    free(val->mallocStart);
    free(val);
  } else if (val->mallocStart ==
             val) // it is the first string but the full block is useful
  {
    val->used--;
  } else // it is not the first string and the full block is useful
  {
    val->mallocStart->used--;
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