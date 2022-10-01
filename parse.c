#include "parse.h"
#include "String.h"
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
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

StringList *seperateString(String *input) {

  printf("message : %d %d %s ,%d,%d\n",input->len,input->used,input->start,(int)input->mallocStart,input);


  String *sL[1025];  // less than 1024, 1 as buffer
  int sLNum = 0;
  while (true) {
    int place = findString(input, '>');
    printf("times %d\n",place);
    fflush(NULL);
    if (place == -1) {
      break;
    }

    if ((place != 0) && (input->start[place + 1] == '>'))  // workwith >>
    {
      String *temp = spiltStringByIndexLevel2(input, place - 1);
      sL[sLNum] = input;
      sLNum++;
      input = temp;
      printf("derffede");
      fflush(stdout);
      continue;
    }
    if (place != 0) {
      String *temp = spiltStringByIndex(input, place - 1);
      sL[sLNum] = input;
      sLNum++;
      input = temp;

    } else {
      place = findStringEscape(input, '>');
        printf("times %d\n",place);
    fflush(NULL);
      if (place == -1) {
        break;
      }
      // place can not be 0
      String *temp = spiltStringByIndex(input, place - 1);
      sL[sLNum] = input;

      sLNum++;
      input = temp;
    }
  }
  sL[sLNum] = input;

  sLNum++;
  StringList *output = malloc(sizeof(StringList));
  output->length = sLNum;
  output->str = malloc(sizeof(String *) * (size_t)sLNum);
  for (int i = 0; i < sLNum; i++) {
    (output->str)[i] = sL[i];
  }

  return output;
}

void deleteStringList(StringList *list) {
  // for (int i = 0; i < list->length; i++) {
  //   deleteString((list->str)[i]);
  // }
  free(list->str);
  free(list);
  return;
}

void parse(String *input, Command *output) {
  if (output == NULL) {
    return;
  }
  // Command *temp = output->after;
  // while (temp != NULL) {
  //   Command *temp2 = temp->after;
  //   deleteString(temp->mainCommand);
  //   deleteArgList(temp->args);
  //   free(temp);
  //   if (temp2 != NULL) {
  //     temp = temp2;
  //   } else {
  //     break;
  //   }
  // }
  // output->after = NULL;
  // deleteArgList(output->args);
  // deleteString(output->mainCommand);

  // output->before = NULL;
  output->isValid = false;
  output->after = NULL;
  output->inFile = 0;
  output->outFile = 1;
  printf("parse %s with pointer %d  with before %d \n",input->start,(int) output,(int) output->before);
  fflush(NULL);
  String *tempInput = input;

  // printf("now command %s \n", (tempInput->start));

  if (((tempInput->start)[0] == '>') && ((tempInput->start)[1] == '>'))  // >>
  {
    char *i;
    for (i = (tempInput->start) + 2;; i++) {
      if ((*i) != ' ') break;
    }
    output->before->outFile =
        open(i, O_APPEND | O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);
    printf("open %s in %d\n", i,output->before->outFile);
    deleteString(tempInput);
    // free(tempInput);
    return;
  }

  if ((tempInput->start)[0] == '>')  // >
  {
    char *i;
    for (i = (tempInput->start) + 1;; i++) {
      if ((*i) != ' ') break;
    }
    output->before->outFile =
        open(i, O_CREAT | O_RDWR | O_TRUNC, S_IRUSR | S_IWUSR);
    printf("open %s in %d\n", i,output->before->outFile);
    deleteString(tempInput);
    // free(tempInput);
    return;
  }
  output->args = malloc(sizeof(ArgList));
  initArgList(output->args);
  output->mainCommand = copyString(input);
  // printf("part: %s \n", (tempInput->start));

  while (true) {
    String *tempInputNew = spiltString(tempInput, ' ');
    emplaceArgList(&output->args, tempInput);
    tempInput = tempInputNew;
    if (tempInput == NULL) {
      break;
    }
  }
  output->isValid = true;
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
  // deleteFullCommandList(c->after);
  deleteCommand(c);
  return NULL;
}
