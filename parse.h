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
char **getArgFromCommand(command *output);
void parse(String *input, command *output);
#endif
