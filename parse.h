#ifndef WZNMICKEY_PARSE
#define WZNMICKEY_PARSE
#include "String.h"
typedef struct ArgList {
  String **argc;
  int argv;
  int argvIndex; // index in ARRAY_LEN
} ArgList;

typedef struct command {
  String *mainCommand;
  ArgList *args;
  String *stdin;
  String *stdout;
  int argv;
  struct command *before;
  struct command *after;
} command;

const int ARRAY_LEN[10] = {8, 16, 32, 64, 128, 256, 512, 1024, 2048, 4096};

#endif