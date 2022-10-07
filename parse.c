#include "parse.h"
#include "String.h"
#include <errno.h>
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
    if (strcmp(getCharArray((arg->argc[i])), "") == 0)
      continue;  // remove empty args.

    val[toPush] = getCharArray((arg->argc[i]));
    toPush++;
  }
  val[toPush] = NULL;
  return val;
}
char **getArgFromCommand(Command *output) {
  return getArgFromArgList((output->args));
}

int getPlace(String *input, int before, int offset) {
  for (int i = before + offset; i < input->len; i++) {
    switch ((input->start)[i]) {
      case '>':
        return i;
      case '<':
        return i;
    }
  }
  return -1;
}

StringList *dividesByPipe(String *st) {
  String *sL[1025];  // less than 1024, 1 as buffer
  int sLNum = 0;
  while (true) {
    String *temp = spiltString(st, '|');
    sL[sLNum] = st;
    sLNum++;
    if (temp == NULL) {
      break;
    }
    st = temp;
  }
  StringList *output = malloc(sizeof(StringList));
  output->length = sLNum;
  output->str = malloc(sizeof(String *) * (size_t)sLNum);
  for (int i = 0; i < sLNum; i++) {
    (output->str)[i] = sL[i];
  }
  return output;
}

StringList *seperateString(String *input) {
  String *sL[1025];  // less than 1024, 1 as buffer
  int sLNum = 0;
  int lastPlace = 0;
  int offset = 0;
  while (true) {
    int place = getPlace(input, lastPlace, offset);

    fflush(NULL);
    if (place == -1) {
      break;
    }
    if ((input->start[place + 1] == '>') &&
        (input->start[place] == '>'))  // workwith >>
    {
      int end = findStringEscape(input, ' ', place + 2);
      if (end == -1) {
        end = input->len;
      }
      twoString sp = getString(input, place, end);
      sL[sLNum] = (sp.st2);
      sLNum++;
      offset = 0;
      input = sp.st1;
      fflush(stdout);
      lastPlace = 0;
      continue;
    }
    if (((input->start[place] == '>') ||
         (input->start[place] == '<')))  // workwith < and >
    {
      int end = findStringEscape(input, ' ', place + 1);
      if (end == -1) {
        end = input->len;
      }
      twoString sp = getString(input, place, end);
      sL[sLNum] = (sp.st2);
      sLNum++;
      offset = 0;
      input = sp.st1;
      fflush(stdout);
      lastPlace = 0;
      continue;
    } else {
      printf("error");
      fflush(NULL);
      lastPlace = 0;
      break;
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
  free(list->str);
  free(list);
  return;
}

void parse(String *input, Command *output, int *tempInFile, int *tempOutFile) {
  if (output == NULL) {
    return;
  }
  output->isValid = false;
  output->after = NULL;
  if (*tempInFile != -1) {
    output->inFile = *tempInFile;
  } else {
    output->inFile = 0;
  }
  if (*tempOutFile != -1) {
    output->outFile = *tempOutFile;
  } else {
    output->outFile = 1;
  }
  fflush(NULL);
  String *tempInput = input;

  if (((tempInput->start)[0] == '>') && ((tempInput->start)[1] == '>'))  // >>
  {
    char *i;
    for (i = (tempInput->start) + 2;; i++) {
      if ((*i) != ' ') break;
    }
    char *tt = changeSingleCharArray(i);
    errno = 0;
    int temp = open(tt, O_APPEND | O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);
    if (errno != 0) {
      printf("%s", tt);
      temp = -3;
    }
    free(tt);
    if (*tempOutFile != -1) {
      *tempOutFile = -5;
      temp = -5;
    }
    if ((output->before != NULL) && (output->before->isValid)) {
      if (output->before->outFile != 1) {
        output->before->outFile = -5;
      } else {
        output->before->outFile = temp;
      }

    } else {
      *tempOutFile = temp;
    }

    deleteString(tempInput);

    return;
  }

  if ((tempInput->start)[0] == '>')  // >
  {
    char *i;
    for (i = (tempInput->start) + 1;; i++) {
      if ((*i) != ' ') break;
    }
    char *tt = changeSingleCharArray(i);
    errno = 0;
    int temp = open(tt, O_CREAT | O_RDWR | O_TRUNC, S_IRUSR | S_IWUSR);
    if (errno != 0) {
      printf("%s", tt);
      temp = -3;
    }
    free(tt);
    if (*tempOutFile != -1) {
      *tempOutFile = -5;
      temp = -5;
    }
    if ((output->before != NULL) && (output->before->isValid)) {
      if (output->before->outFile != 1) {
        output->before->outFile = -5;
      } else {
        output->before->outFile = temp;
      }
    } else {
      *tempOutFile = temp;
    }
    deleteString(tempInput);
    return;
  }

  if ((tempInput->start)[0] == '<')  // <
  {
    char *i;
    for (i = (tempInput->start) + 1;; i++) {
      if ((*i) != ' ') break;
    }
    char *tt = changeSingleCharArray(i);
    errno = 0;
    int temp = open(tt, O_RDWR, S_IRUSR | S_IWUSR);
    if (errno == 2) {
      temp = -2;
      printf("%s", tt);
    }
    free(tt);
    if (*tempInFile != -1) {
      *tempInFile = -4;
      temp = -4;
    }
    if ((output->before != NULL) && (output->before->isValid)) {
      if (output->before->inFile != 1) {
        output->before->inFile = -4;
      } else {
        output->before->inFile = temp;
      }
    } else {
      *tempInFile = temp;
    }
    deleteString(tempInput);
    return;
  }

  output->args = malloc(sizeof(ArgList));
  initArgList(output->args);
  output->mainCommand = copyString(input);

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
  deleteCommand(c);
  return NULL;
}
bool checkIfComplete(char *input) {
  int index = 0;
  bool inDouble = false;
  bool inSingle = false;
  while (true) {
    if (input[index] == '\'') {
      if (!inDouble) {
        inSingle = !inSingle;
      }
    }
    if (input[index] == '"') {
      if (!inSingle) {
        inDouble = !inDouble;
      }
    }
    if (input[index] == '\0') {
      break;
    }
    index++;
  }
  return ((!inDouble) && (!inSingle));
}
bool checkIfNotEnd(char *input) {
  int offset = 0;
AGAIN:
  switch (input[(int)strlen(input) - 1 - (int)offset]) {
    case '>':
      return true;
    case '<':
      return true;
    case '|':
      return true;
    case ' ': {
      offset++;
      goto AGAIN;
    }
    default:
      return false;
  }
}
void changeQuote(char *input) {
  int index = 0;
  bool inDouble = false;
  bool inSingle = false;
  while (true) {
    if (input[index] == '\0') {
      break;
    }
    if (input[index] == '\'') {
      if (!inDouble) {
        inSingle = !inSingle;
        input[index] = DROP;
      } else {
        input[index] = SINGLE;
      }
      index++;
      continue;
    }
    if (input[index] == '"') {
      if (!inSingle) {
        inDouble = !inDouble;
        input[index] = DROP;
      } else {
        input[index] = DOUBLE;
      }
      index++;
      continue;
    }
    if (inDouble || inSingle) {
      if (input[index] == ' ') {
        input[index] = SPACE;
      }
      if (input[index] == '>') {
        input[index] = RIGHTARROW;
      }
      if (input[index] == '<') {
        input[index] = LEFTARROW;
      }
      if (input[index] == '|') {
        input[index] = PIPE;
      }
    }
    index++;
  }
  return;
}
char **changeFromArg(char **arg) {
  int outIndex = 0;
  while (true) {
    if (arg[outIndex] == NULL) {
      break;
    }
    outIndex++;
  }
  char **val = malloc(sizeof(char *) * (size_t)(outIndex + 1));
  for (int i = 0; i < outIndex; i++) {
    val[i] = changeSingleCharArray(arg[i]);
  }
  val[outIndex] = NULL;
  return val;
}

char *changeSingleCharArray(char *st) {
  int index = 0;
  int len = (int)strlen(st);

  char *val = malloc(sizeof(char) * (size_t)(len + 1));
  for (int i = 0; i < len; i++) {
    if (((st)[i]) != DROP) {
      (val)[index] = st[i];
      switch ((val)[index]) {
        case LEFTARROW: {
          ((val)[index]) = '<';
          index++;
          continue;
        }
        case RIGHTARROW: {
          ((val)[index]) = '>';
          index++;

          continue;
        }
        case SPACE: {
          ((val)[index]) = ' ';
          index++;

          continue;
        }
        case PIPE: {
          ((val)[index]) = '|';
          index++;

          continue;
        }
        case SINGLE: {
          ((val)[index]) = '\'';
          index++;

          continue;
        }
        case DOUBLE: {
          ((val)[index]) = '"';
          index++;

          continue;
        }
        default: {
          index++;

          continue;
        }
      }
    }
  }
  val[index] = '\0';
  return val;
}
bool isValid(char *input) {
  size_t len = strlen(input);
  bool isSpaceGiven =
      true;  // whether there is a space before (only for >> cases)
  bool isSyntaxed = false;   // whether there is a syntax before
  bool isLastRight = false;  // whether there is a > before (only for >> cases)
  bool isLastPipe = false;   // whether last one is |
  for (size_t i = 0; i < len; i++) {
    switch (input[i]) {
      case ' ': {
        isSpaceGiven = true;
        continue;
      }
      case '>': {
        if (!(((isSpaceGiven == false) && (isLastRight)) ||
              (isSyntaxed == false))) {
          printf("syntax error near unexpected token `>'\n");
          return false;
        }
        isSpaceGiven = false;
        isSyntaxed = true;
        isLastRight = true;
        isLastPipe = false;
        continue;
      }
      case '<': {
        if (isSyntaxed) {
          printf("syntax error near unexpected token `<'\n");
          return false;
        }
        isSpaceGiven = false;
        isSyntaxed = true;
        isLastRight = false;
        isLastPipe = false;
        continue;
      }
      case '|': {
        if (isLastPipe) {
          printf("error: missing program\n");
          return false;
        }
        if (isSyntaxed) {
          printf("syntax error near unexpected token `|'\n");
          return false;
        }
        isSpaceGiven = false;
        isSyntaxed = true;
        isLastRight = false;
        isLastPipe = true;
        continue;
      }
      default: {
        isSpaceGiven = false;
        isSyntaxed = false;
        isLastRight = false;
        isLastPipe = false;
      }
    }
  }
  return true;
}
bool checkIsBack(char *input) {
  size_t len =
      strlen(input);  // The case of len ==0 should not happen in this function.
  if (input[len - 1] == '&') {
    input[len - 1] = '\0';  // delete token
    return true;
  }
  return false;
}
