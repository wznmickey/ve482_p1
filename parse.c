#include "parse.h"
#include "String.h"
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>
const size_t ARRAY_LEN[10] = {8, 16, 32, 64, 128, 256, 512, 1024, 2048, 4096};
void initArgList(ArgList *arg) {
  if (arg == NULL) {
    return;
  }
  arg->argv = 0;
  arg->argc = malloc(sizeof(String *) * ARRAY_LEN[0]);
  arg->argvIndex = 0;
  return;
}

void deleteArgList(ArgList *arg) {
  if (arg == NULL) {
    return;
  }
  for (int i = 0; i < arg->argv; i++) {
    deleteString(arg->argc[i]);
  }
  arg->argv = 0;
  free(arg->argc);
  free(arg);
}

void pureMoveArgList(ArgList *arg, ArgList *dst) {
  if (arg == NULL || dst == NULL) {
    return;
  }
  dst->argv = arg->argv;
  dst->argvIndex = arg->argvIndex;

  for (int i = 0; i < arg->argv; i++) {
    dst->argc[i] = arg->argc[i];
  }
  free(arg->argc);
  // free(arg);

  return;
}

void extendArgList(ArgList **arg) {
  if (*arg == NULL) {
    return;
  }
  if ((*arg)->argv >=
      (int)ARRAY_LEN[(*arg)->argvIndex] / 2)  // extend in advance
  {
    (*arg)->argvIndex++;
    ArgList *temp = malloc(sizeof(ArgList));
    String **tt = malloc(sizeof(String *) * ARRAY_LEN[(*arg)->argvIndex]);
    temp->argc = tt;
    pureMoveArgList(*arg, temp);
    free(*arg);  // not sure

    *arg = temp;
  }
  return;
}

bool emplaceArgList(ArgList **arg, String *val) {
  if (arg == NULL) {
    return false;
  }

  extendArgList(arg);
  (*arg)->argc[(*arg)->argv] = copyString(val);
  deleteString(val);
  (*arg)->argv++;
  return true;
}
char **getArgFromArgList(ArgList *arg) {
  char **val = malloc(sizeof(char *) * (size_t)(arg->argv + 1));

  int toPush = 0;

  for (int i = 0; i < arg->argv; i++) {
    if (strcmp(getCharArray(arg->argc[i]), "") == 0)
      continue;  // remove empty args.

    val[toPush] = getCharArray(arg->argc[i]);
    toPush++;
  }
  val[toPush] = NULL;
  return val;
}
char **getArgFromCommand(Command *output) {
  return getArgFromArgList(output->args);
}

// void seperateCommand(String *input,Command *output)
// {

// }
void parse(String *input, Command *output) {
  if (output == NULL) {
    return;
  }
  Command *temp = output->after;
  while (temp != NULL) {
    Command *temp2 = temp->after;
    deleteString(temp->mainCommand);
    deleteArgList(temp->args);
    free(temp);
    if (temp2 != NULL) {
      temp = temp2;
    } else {
      break;
    }
  }
  output->after = NULL;
  deleteArgList(output->args);
  deleteString(output->mainCommand);

  output->before = NULL;
  String *tempInput = input;

  output->args = malloc(sizeof(ArgList));
  initArgList(output->args);

  while (true) {
    String *tempInputNew = spiltString(tempInput, ' ');

    emplaceArgList(&output->args, tempInput);

    tempInput = tempInputNew;
    if (tempInput == NULL) {
      break;
    }
  }
  return;
}
void initCommand(Command *c) {
  c->mainCommand = NULL;
  c->after = NULL;
  c->args = NULL;
  c->before = NULL;
  c->inFile = 1;
  c->outFile = 2;
  return;
}
Command *deleteCommand(Command *c) {
  if (c == NULL) {
    return c;
  }
  deleteArgList(c->args);
  deleteString(c->mainCommand);
  free(c);
  return NULL;
}
Command *deleteFullCommandList(Command *c) {
  if (c == NULL) {
    return c;
  }
  deleteFullCommandList(c->after);
  deleteCommand(c);
  return NULL;
}
