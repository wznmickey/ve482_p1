#ifndef WZNMICKEY_PARSE
#define WZNMICKEY_PARSE
#include "String.h"
typedef struct ArgList {
  String **argc;
  int argv;
  int argvIndex; // index in ARRAY_LEN
} ArgList;

typedef struct Command {
  String *mainCommand;
  ArgList *args;
  String *stdin;
  String *stdout;
  int argv;
  struct Command *before;
  struct Command *after;
} Command;
char **getArgFromCommand(Command *output);
void parse(String *input, Command *output);
void initCommand(Command *c);
void deleteArgList(ArgList *arg);
Command *deleteFullCommandList(Command *c);
#endif
