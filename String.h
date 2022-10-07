#ifndef WZNMICKEY_STRING
#define WZNMICKEY_STRING
#include <stdbool.h>

typedef struct String {
  char *start;
  int len;
  int used;  // free will free all the malloc, which will make splice illegal.
             // Only can be free when it is 1(itself only) and mallocStart==this
             // (start point).
  struct String
      *mallocStart;  // free will free all the malloc, which will make splice
                     // illegal. Use this to find the start point of malloc.
  int *rawStatus;    // flag whether this char is raw (in '' or "")
  // 0 -> not in
  // 1 -> is in
  // 2 -> is itself
} String;
typedef struct twoString {
  String *st1;
  String *st2;
} twoString;
String *initString(char input[]);
String *deleteString(String *val);
int changeString(String *st, char aim, char newOne);
String *spiltString(String *st, char aim);
int findString(String *st, char aim);
String *copyString(String *st);
char *getCharArray(String *st);
String *spiltStringByIndex(String *st, int find);
int findStringEscape(String *st, char aim, int offset);
String *spiltStringByIndexLevel2(String *st, int find);
String *copyFromIndex(String *st, int placeLeft, int placeRight);
twoString getString(String *st, int placeLeft, int placeRight);
void deleteChar2Array(char **array);
bool allSpace(char *input);
#endif
