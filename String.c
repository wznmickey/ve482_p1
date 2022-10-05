
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
  val->rawStatus = malloc(sizeof(int) * strlen(input) + 5);

  for (int i = 0; i < (int)strlen(input); i++) {
    (val->rawStatus)[i] = -1;
  }
  (val->rawStatus)[(int)strlen(input)] = '\0';  // for copy
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

  // printf("used:  %d \n",val->used);

  // printf("real st: %s\n",val->start);
  fflush(stdout);
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
  // printf("length : %d\n", st->len);
  // fflush(NULL);
  for (int i = 0; i < st->len; i++) {
    if (st->start[i] == aim) {
      // printf("here \n");
      // fflush(NULL);
      // if (st->rawStatus[i] == -1) {
      return i;
      // }
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
      // printf("here \n");
      // fflush(NULL);
      // if (st->rawStatus[i] == -1) {
      return i;
      // }
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
  if (newOne == '\0') {
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
  val->rawStatus = st->rawStatus + find + 1;
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
  val->rawStatus = st->rawStatus + find + 1;

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
  val->rawStatus = st->rawStatus + find + 1;
  // printf("str:%s \n",val->start);

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
  temp->rawStatus = malloc(sizeof(int) * (size_t)(temp->len + 1));
  for (int i = 0; i < temp->len; i++) {
    (temp->start)[i] = (st->start)[i + placeLeft];
    (temp->rawStatus)[i] = (st->rawStatus)[i + placeLeft];
  }
  (temp->start)[temp->len] = '\0';
  (temp->rawStatus)[temp->len] = '\0';
  // printf("%d,%d,copy place %s \n",placeLeft,placeRight, (st->start));
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
  temp->rawStatus =
      malloc(sizeof(int) * (size_t)(st->len + 1));  // 1 more for '\0'
  for (int i = 0; i < temp->len; i++) {
    temp->rawStatus[i] = st->rawStatus[i];
  }
  return temp;
}
char *getCharArray(String *st) { return st->start; }
// char *getCharArray(String **st) {
//   String *temp = malloc(sizeof(String));

//   temp->mallocStart = temp;
//   temp->used = 1;
//   temp->start = malloc(sizeof(char) * (size_t)((*st)->len + 1));

//   int index = 0;

//   for (int i = 0; i <= (*st)->len; i++) {
//     if ((((*st)->start)[i]) != DROP) {
//       (temp->start)[index] = ((*st)->start)[i];
//       switch ((temp->start)[index]) {
//         case LEFTARROW: {
//           (temp->start)[index] = '<';
//           index++;
//           continue;
//         }
//         case RIGHTARROW: {
//           (temp->start)[index] = '>';
//           index++;

//           continue;
//         }
//         case SPACE: {
//           (temp->start)[index] = ' ';
//           index++;

//           continue;
//         }
//         case PIPE: {
//           (temp->start)[index] = '|';
//           index++;

//           continue;
//         }
//         case SINGLE: {
//           (temp->start)[index] = '\'';
//           index++;

//           continue;
//         }
//         case DOUBLE: {
//           (temp->start)[index] = '"';
//           index++;

//           continue;
//         }
//         default: {
//           index++;

//           continue;
//         }
//       }
//     }
//   }

//   temp->len = (int)strlen(temp->start);
//   // deleteString(*st);
//   *st = temp;
//   // temp->rawStatus =

//   return temp->start;
// }
twoString getString(String *st, int placeLeft, int placeRight) {
  int getLen = placeRight - placeLeft;
  int getRemain = st->len - getLen;
  // printf("%d %d \n", getLen, getRemain);
  fflush(NULL);
  String *remain = malloc(sizeof(String));
  String *get = malloc(sizeof(String));

  remain->start = malloc(sizeof(char) * (size_t)(getRemain + 1));
  get->start = malloc(sizeof(char) * (size_t)(getLen + 1));

  remain->rawStatus = malloc(sizeof(int) * (size_t)(getRemain + 1));
  get->rawStatus = malloc(sizeof(int) * (size_t)(getLen + 1));

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
    // printf("%d %d \n", toGetI, remainI);
    if ((i >= placeLeft) && (i < placeRight)) {
      (get->start)[toGetI] = (st->start)[i];
      (get->rawStatus)[toGetI] = (st->rawStatus)[i];
      toGetI++;
    } else {
      (remain->start)[remainI] = (st->start)[i];
      (remain->rawStatus)[remainI] = (st->rawStatus)[i];
      remainI++;
    }
  }
  (get->start)[toGetI] = '\0';
  (remain->start)[remainI] = '\0';
  // printf("    %d %d \n", toGetI, remainI);

  get->len = (int)strlen(get->start);
  // printf("    %d %d \n", toGetI, remainI);

  remain->len = (int)strlen(remain->start);
  // printf("    %d %d \n", toGetI, remainI);

  deleteString(st);
  return ans;
}
void deleteChar2Array(char **array) {
  int index = 0;
  while (true) {
    if (array[index] == NULL) break;
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
