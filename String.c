
#include "String.h"
#include <alloca.h>
#include <stdlib.h>
#include <string.h>
// since the string is not very long. Use int rather than size_t.
String *initString(char input[]) {
  String *val = malloc(sizeof(String));
  val->start = malloc(sizeof(char) * strlen(input) + 5); // use +5 as buffer
  val->len = (int)strlen(input);
  strcpy(val->start, input);
  val->mallocStart = val;
  val->used = 1; // itself
  return val;
}
String *deleteString(String *val) {
  if (val == NULL) {
    return NULL;
  }
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
  val->used =
      1; // It is meaningless. Just to init. The value will not be updated.
  st->len = (int)strlen(st->start);
  st->mallocStart->used++;
  return val;
}
String *copyString(String *st) {
  String *temp = malloc(sizeof(String));
  temp->len = st->len;
  temp->used = st->used;
  temp->start = malloc(sizeof(char) * (st->len));
  temp->mallocStart = temp;
  strcpy(temp->start, st->start);
  return temp;
}
char *getCharArray(String *st) { return st->start; }