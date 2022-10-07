
#include "String.h"
#include <alloca.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "parse.h"
// since the string is not very long. Use int rather than size_t.
String *initString(char input[]) {
  String *val = malloc(sizeof(String));
  val->start = malloc(sizeof(char) * strlen(input) + 5);  // use +5 as buffer
  val->len = (int)strlen(input);
  strcpy(val->start, input);
  val->mallocStart = val;
  val->used = 1;  // itself
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
    free(val);
  } else {
    if (val->mallocStart->used ==
        1) {  // it is not the first string but the full block is useless. Can
              // not hebing with the above because we need make sure
              // val!=val->mallocStart. Otherwise it is free twice.
      free(val->mallocStart->start);
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
      return i;
    }
  }
  return -1;  // Not found
}
int findStringEscape(String *st, char aim, int offset) {
  int index = offset;
  for (; index < st->len; index++) {
    if ((st->start)[index] != ' ') {
      break;
    }
  }
  for (int i = index; i < st->len; i++) {
    if ((st->start[i] == aim) || (st->start[i] == '>') ||
        (st->start[i] == '<')) {
      return i;
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
  st->len = (int)strlen(st->start);
  st->mallocStart->used++;
  return val;
}
String *spiltStringByIndex(String *st, int find) {
  String *val = malloc(sizeof(String));
  val->mallocStart = st->mallocStart;
  val->start = st->start + find + 1;
  val->len = (int)strlen(val->start);
  val->used =
      1;  // It is meaningless. Just to init. The value will not be updated.
  String *ans = copyString(val);
  free(val);
  st->start[find + 1] = '\0';
  st->len = (int)strlen(st->start);
  return ans;
}

String *spiltStringByIndexLevel2(String *st, int find) {
  String *val = malloc(sizeof(String));
  val->mallocStart = st->mallocStart;
  val->start = st->start + find + 1;
  val->len = (int)strlen(val->start);
  val->used =
      1;  // It is meaningless. Just to init. The value will not be updated.
  String *ans = copyString(val);
  free(val);
  st->start[find + 1] = '\0';
  st->len = (int)strlen(st->start);
  return ans;
}

// include left, not include right
String *copyFromIndex(String *st, int placeLeft, int placeRight) {
  String *temp = malloc(sizeof(String));
  temp->len = placeRight - placeLeft;
  temp->start = malloc(sizeof(char) * (size_t)((temp->len) + 1));
  temp->mallocStart = temp;
  temp->used = 1;
  for (int i = 0; i < temp->len; i++) {
    (temp->start)[i] = (st->start)[i + placeLeft];
  }
  (temp->start)[temp->len] = '\0';
  return temp;
}

String *copyString(String *st) {
  String *temp = malloc(sizeof(String));
  temp->len = st->len;
  temp->used = 1;
  temp->start =
      malloc(sizeof(char) * (size_t)(st->len + 1));  // 1 more for '\0'
  temp->mallocStart = temp;
  strcpy(temp->start, st->start);
  return temp;
}
char *getCharArray(String *st) { return st->start; }
twoString getString(String *st, int placeLeft, int placeRight) {
  int getLen = placeRight - placeLeft;
  int getRemain = st->len - getLen;
  String *remain = malloc(sizeof(String));
  String *get = malloc(sizeof(String));
  remain->start = malloc(sizeof(char) * (size_t)(getRemain + 1));
  get->start = malloc(sizeof(char) * (size_t)(getLen + 1));
  remain->mallocStart = remain;
  get->mallocStart = get;
  remain->used = 1;
  get->used = 1;
  twoString ans;
  ans.st1 = remain;
  ans.st2 = get;
  int toGetI = 0;
  int remainI = 0;
  for (int i = 0; i < st->len; i++) {
    if ((i >= placeLeft) && (i < placeRight)) {
      (get->start)[toGetI] = (st->start)[i];
      toGetI++;
    } else {
      (remain->start)[remainI] = (st->start)[i];
      remainI++;
    }
  }
  (get->start)[toGetI] = '\0';
  (remain->start)[remainI] = '\0';
  get->len = (int)strlen(get->start);
  remain->len = (int)strlen(remain->start);
  deleteString(st);
  return ans;
}
void deleteChar2Array(char **array) {
  int index = 0;
  while (true) {
    if (array[index] == NULL) {
      break;
    }
    free(array[index]);
    index++;
  }
  free(array);
  return;
}
bool allSpace(char *input) {
  for (int i = 0; i < (int)strlen(input); i++) {
    if ((input[i] != ' ') && (input[i] != '\n')) {
      return false;
    }
  }
  return true;
}
