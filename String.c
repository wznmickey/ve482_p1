
#include "String.h"
#include <alloca.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
// since the string is not very long. Use int rather than size_t.
String *initString(char input[]) {
  String *val = malloc(sizeof(String));
  val->start = malloc(sizeof(char) * strlen(input) + 5);  // use +5 as buffer
  val->len = (int)strlen(input);
  strcpy(val->start, input);
  val->mallocStart = val;
  val->used = 1;  // itself
  val->rawStatus = malloc(sizeof(int) * strlen(input) + 5);

  for (int i = 0; i < (int)strlen(input); i++) {
    (val->rawStatus)[i] = 0;
  }
  (val->rawStatus)[(int)strlen(input)] = '\0'; // for copy
  // reserve for future develop
  // int isTwo = false; // is in ""
  // int isOne = false; // is in ''
  // for (int i = 0; i < strlen(input); i++) {
  //   if ((val->start)[i] == '"') {
  //     isRaw = !isRaw;
  //     (val->rawStatus)[i] = 2;
  //     continue;
  //   }
  //   if ((val->start)[i] == '\'') {
  //     isRaw = !isRaw;
  //     (val->rawStatus)[i] = 2;
  //     continue;
  //   }
  //   if (isRaw) {
  //     (val->rawStatus)[i] = 1;
  //     continue;
  //   }
  //   if (!isRaw) {
  //     (val->rawStatus)[i] = 0;
  //     continue;
  //   }
  // }
  return val;
}
String *deleteString(String *val) {
  if (val == NULL) {
    return NULL;
  }
  if ((val->used == 1) &&
      (val->mallocStart ==
       val))  // it is the first string and the full block is useless
  {
    free(val->start);
    free(val->rawStatus);
    free(val);
  } else {
    if (val->mallocStart->used ==
        1) {  // it is not the first string but the full block is useless. Can
              // not hebing with the above because we need make sure
              // val!=val->mallocStart. Otherwise it is free twice.
      free(val->mallocStart->start);
      free(val->mallocStart->rawStatus);
      free(val->mallocStart);
      free(val);
    } else {
      if (val->mallocStart ==
          val) {  // it is the first string but the full block is useful

        val->used--;
      } else {  // it is not the first string and the full block is useful

        val->mallocStart->used--;
        free(val);
      }
    }
  }
  return NULL;
}
int findString(String *st, char aim) {
  for (int i = 0; i < st->len; i++) {
    if (st->start[i] == aim) {
      if (st->rawStatus[i] == 0) {
        return i;
      }
    }
  }
  return -1;  // Not found
}

int changeString(String *st, char aim, char newOne) {
  int x = findString(st, aim);
  if (x == -1) {
    return -1;  // failed
  }
  st->start[x] = newOne;
  if (newOne == '\0') 
  {
    st->rawStatus[x] = '\0';
  }
  return x;
}

String *spiltString(String *st, char aim) {
  int find = changeString(st, aim, '\0');
  if (find == -1) {
    return NULL;  // failed
  }
  String *val = malloc(sizeof(String));
  
  val->mallocStart = st->mallocStart;
  val->start = st->start + find + 1;
  val->len = (int)strlen(val->start);
  val->used =
      1;  // It is meaningless. Just to init. The value will not be updated.
      val->rawStatus =  st->rawStatus + find + 1;
  st->len = (int)strlen(st->start);
  st->mallocStart->used++;
  return val;
}
String *copyString(String *st) {
  String *temp = malloc(sizeof(String));
  temp->len = st->len;
  temp->used = 1;
  temp->start =
      malloc(sizeof(char) * (size_t)(st->len + 1));  // 1 more for '\0'
  temp->mallocStart = temp;
  strcpy(temp->start, st->start);
  temp->rawStatus = malloc(sizeof(int) * (size_t)(st->len + 1));  // 1 more for '\0'
  strcpy((char *)temp->rawStatus, (char *)st->rawStatus);
  return temp;
}
char *getCharArray(String *st) { return st->start; }
