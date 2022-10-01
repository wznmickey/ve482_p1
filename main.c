
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>
#include "String.h"
#include "parse.h"
void flush() {
  fflush(NULL);
  fflush(NULL);
  return;
}
#define printf(...)    \
  printf(__VA_ARGS__); \
  flush();
#define fgets(...)    \
  fgets(__VA_ARGS__); \
  flush();

int main() {
  char input[2048];
  while (true) {
    fflush(NULL);
    printf("mumsh $ ");
    fflush(NULL);
    memset(input, 0, sizeof(char) * 2048);
    fgets(input, 2048 - 1, stdin);
    input[strlen(input) - 1] = '\0';  // remove the \n at the end.
    if (strcmp(input, "exit") == 0) {
      printf("exit\n");
      return 0;
    }
    // printf("1\n");

    fflush(stdout);
    String *inputS = initString(input);
    // Command *command = malloc(sizeof(Command));
    // initCommand(command);
    StringList *stringList = seperateString(inputS);
    // printf("2\n");

    fflush(stdout);
    Command *firstCommand = NULL;
    {
      Command *lastCommand = NULL;
      for (int i = 0; i < stringList->length; i++) {
        Command *tempCommand = malloc(sizeof(Command));
        if (lastCommand != NULL) {
          tempCommand->before = lastCommand;
          lastCommand->after = tempCommand;
        } else {
          firstCommand = tempCommand;
          firstCommand->before=NULL;
        }
        lastCommand = tempCommand;
        parse(stringList->str[i], tempCommand);
      }
    }
    // printf("3\n");

    fflush(stdout);
    while (firstCommand != NULL) {
      fflush(NULL);
      printf("1111\n");
      fflush(NULL);

      Command *command = firstCommand;
      unsigned long long int temp = command;
      printf("use command %lld", temp);
      firstCommand = firstCommand->after;
      if (command->isValid == false) {
        free(command);
        continue;
      }
      char **usArg = getArgFromCommand(command);

      for (int i = 0; i < 1000; i++) {
        if (usArg[i] == NULL) break;

        // printf(" arg %s \n",usArg[i]);
      }

      pid_t pid = fork();
      int stdin_ = 0;
      int stdout_ = 1;

      printf("in %d out %d\n", command->inFile, command->outFile);
      fflush(NULL);
      if (command->inFile != 0) {
        stdin_ = dup(0);
        dup2(command->inFile, 0);
        close(command->inFile);
      }
      if (command->outFile != 1) {
        stdout_ = dup(1);
        dup2(command->outFile, 1);
        close(command->outFile);
      }
      fflush(NULL);

      if (pid == 0) {
        fflush(NULL);
        int status_code = execvp(usArg[0], usArg);
        fflush(NULL);

        if (status_code == -1) {
          printf("Command wrong with error code %d.\n", status_code);
          deleteFullCommandList(command);

          free(usArg);
          fflush(NULL);
          return 0;
        }
        fflush(NULL);
        return 0;
      } else {
        fflush(NULL);
        waitpid(pid, NULL, 0);
        fflush(NULL);
        if (stdin_ != 0) {
          close(0);
          dup2(stdin_, 0);
          close(stdin_);
        }

        if (stdout_ != 1) {
          close(1);
          dup2(stdout_, 1);
          close(stdout_);
        }
        fflush(NULL);
      }
      deleteFullCommandList(command);

      free(usArg);
    }
    deleteStringList(stringList);
  }
  return 0;
}
