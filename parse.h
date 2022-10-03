#ifndef WZNMICKEY_PARSE
#define WZNMICKEY_PARSE
#include "String.h"
typedef struct ArgList {
  String **argc;
  int argv;
  int argvIndex;  // index in ARRAY_LEN
} ArgList;

typedef struct Command {
  String *mainCommand;
  ArgList *args;
  int inFile;
  int outFile;
  struct Command *before;
  struct Command *after;
  bool isValid;
} Command;

typedef struct StringList {
  String **str;
  int length;
} StringList;

typedef struct CommandList {
  Command **lst;
  int length;
} CommandList;

enum token {
  SINGLELEFT,
  DOUBLELEFT,
  SINGLERIGHT,
};

char **getArgFromCommand(Command *output);
void parse(String *input, Command *output, int *tempInFile, int *tempOutFile);
void initCommand(Command *c);
void deleteArgList(ArgList *arg);
Command *deleteFullCommandList(Command *c);
StringList *seperateString(String *input);
void deleteStringList(StringList *list);
StringList *dividesByPipe(String *st);
#endif
