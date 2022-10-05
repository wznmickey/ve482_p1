
#include <stdbool.h>
#include <stddef.h>
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
#define printf(...)      \
  do {                   \
    printf(__VA_ARGS__); \
    flush();             \
  } while (0);
#define fgets(...)      \
  do {                  \
    flush();            \
    fgets(__VA_ARGS__); \
    flush();            \
  } while (0);

char input[2048];
void SIGINTReaction() {
  if (strlen(input) == 0) printf("mumsh $ ");
  return;
}

int main() {
  signal(SIGQUIT, SIG_IGN);
  signal(SIGINT, SIGINTReaction);
  signal(SIGTSTP, SIG_IGN);
  signal(SIGTTIN, SIG_IGN);
  signal(SIGTTOU, SIG_IGN);
  signal(SIGCHLD, SIG_IGN);
  setpgid(getpid(), getpid());
  tcsetpgrp(STDOUT_FILENO, getpid());
  tcsetpgrp(STDIN_FILENO, getpid());

  while (true) {
  RESTART:
    fflush(NULL);
    fflush(stdin);
    fflush(stdout);
    printf("mumsh $ ");
    fflush(stdin);
    fflush(stdout);
    fflush(NULL);
    // AGAIN:
    memset(input, 0, sizeof(char) * 2048);
    // char ch;
    // int tempIndex = 0;
    int offset = 0;
  READINPUT:
    fgets(input + offset, 2048 - 1, stdin);
    fflush(stdin);
    fflush(stdout);
    if (strlen(input) == 0) {
      printf("exit\n");
      return 0;
    }
    if (allSpace(input)){
      goto RESTART;
    }
    bool ifComplete = checkIfComplete(input);
    if (!ifComplete) {
      printf("> ");
      offset = (int)strlen(input);
      goto READINPUT;
    }

    input[strlen(input) - 1] = '\0';  // remove the \n at the end.

    bool isNotEnd = checkIfNotEnd(input);
    if (isNotEnd) {
      printf("> ");
      offset = (int)strlen(input);
      goto READINPUT;
    }

    // TODO : come with new line if not complete

    changeQuote(input);

    if (strcmp(input, "exit") == 0) {
      // EXIT:
      printf("exit\n");
      fflush(NULL);
      return 0;
    }
    if (strlen(input) == 0) {
      goto RESTART;
    }
    // printf("1\n");

    fflush(stdout);
    String *inputS = initString(input);
    // Command *command = malloc(sizeof(Command));
    // initCommand(command);

    StringList *piped = dividesByPipe(inputS);
    int pipFile[2] = {-1, -1};

    CommandList commandList;
    commandList.length = piped->length;
    commandList.lst = malloc(sizeof(Command) * (size_t)commandList.length);
    // printf("after pipe %d \n", piped->length);

    for (int i = 0; i < piped->length; i++) {
      StringList *stringList = seperateString(piped->str[i]);
      // deleteString(inputS);
      // printf("2\n");

      fflush(stdout);
      Command *firstCommand = NULL;
      Command *lastCommand = NULL;
      {
        Command *tempCommand = malloc(sizeof(Command));
        tempCommand->isValid = false;
        int tempInFile = -1;
        int tempOutFile = -1;
        for (int i = 0; i < stringList->length; i++) {
          if (tempCommand->isValid) {
            tempCommand = malloc(sizeof(Command));
            if (lastCommand != NULL) {
              tempCommand->before = lastCommand;
              lastCommand->after = tempCommand;
            }
          }
          if (lastCommand == NULL) {
            firstCommand = tempCommand;
            firstCommand->before = NULL;
          }
          lastCommand = tempCommand;
          parse(stringList->str[i], tempCommand, &tempInFile, &tempOutFile);
        }
      }
      if (pipFile[0] != -1) {
        firstCommand->inFile = pipFile[0];
      }
      if ((piped->length) - i != 1) {
        pipe(pipFile);
        // printf("we have a pipe linked %d %d \n",pipFile[0],pipFile[1]);
        lastCommand->outFile = pipFile[1];
      }

      commandList.lst[i] = firstCommand;
      deleteStringList(stringList);
      // printf("3\n");
    }
    int pidList[1025];
    for (int i = 0; i < piped->length; i++) {
      Command *firstCommand = commandList.lst[i];

      fflush(stdout);
      while (firstCommand != NULL) {
        fflush(NULL);
        // printf("1111\n");
        fflush(NULL);

        Command *command = firstCommand;
        // unsigned long long int temp = command;
        // printf("use command %lld", temp);
        firstCommand = firstCommand->after;
        if (command->isValid == false) {
          free(command);
          continue;
        }
        char **usArg = getArgFromCommand(command);

        char **usArgChanged = changeFromArg(usArg);
        // printf("%s \n",usArgChanged[0]);
        // for (int i = 0; i < 1000; i++) {
        //   if (usArg[i] == NULL) break;

        //   // printf(" arg %s \n",usArg[i]);
        // }
        // int x = (strcmp(usArg[0], "cd"));
        // printf("%s\n",usArg[0]);
        if (strcmp(usArgChanged[0], "cd") == 0) {
          pidList[i] = -1;
          char *aim;
          if (usArgChanged[1] == NULL) {
            // no argument, directly go to home page
            char *login = getlogin();
            if (strcmp(login, "root") == 0) {
              char aim[] = "/root";
              // free(login);
              chdir(aim);
              goto Parent;

            } else {
              char homePath[6 + 1 +
                            strlen(login)];  // man useradd can allow name of
                                             // maximum of 32 chars.
              homePath[0] = '/';
              homePath[1] = 'h';
              homePath[2] = 'o';
              homePath[3] = 'm';
              homePath[4] = 'e';
              homePath[5] = '/';
              for (int i = 0; i < (int)strlen(login); i++) {
                homePath[6 + i] = login[i];
              }
              homePath[strlen(login) + 6] = '\0';
              // strcat(login, homePath);
              // printf("%s \n",homePath);
              fflush(NULL);
              chdir(homePath);
              fflush(NULL);
              // free(login);
              goto Parent;
            }
            // char aim[] = "";
            // chdir(aim);
            // goto Parent;
          }
          if (usArgChanged[1] != NULL &&
              usArgChanged[2] != NULL)  // to many arguments
          {
            goto Parent;
          }
          if ((usArgChanged[1][0] != '/')
              // && ((usArg[1][0] != '~'))
              )  // relative path
          {
            aim = malloc(sizeof(char) * (2 + 1 + strlen(usArgChanged[1])));
            aim[0] = '.';
            aim[1] = '/';
            for (int i = 0; i < (int)strlen(usArgChanged[1]); i++) {
              aim[i + 2] = usArgChanged[1][i];
            }
            aim[2 + strlen(usArgChanged[1])] = '\0';
          } else {
            aim = malloc(sizeof(char) * (1 + strlen(usArgChanged[1])));
            for (int i = 0; i < (int)strlen(usArgChanged[1]); i++) {
              aim[i] = usArgChanged[1][i];
            }
            aim[strlen(usArgChanged[1])] = '\0';
          }
          chdir(aim);
          free(aim);
          fflush(NULL);
          // deleteFullCommandList(command);
          // deleteChar2Array(usArgChanged);
          // free(usArg);
          fflush(NULL);
          goto Parent;
        }

        pid_t pid = fork();
        int stdin_ = 0;
        int stdout_ = 1;

        // printf("in %d out %d\n", command->inFile, command->outFile);
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
          signal(SIGINT, SIG_DFL);
          signal(SIGQUIT, SIG_DFL);
          signal(SIGTSTP, SIG_DFL);
          signal(SIGTTIN, SIG_DFL);
          signal(SIGTTOU, SIG_DFL);
          signal(SIGCHLD, SIG_DFL);
          fflush(NULL);
          if (strcmp(usArgChanged[0], "pwd") == 0) {
            char *pwdPath = NULL;
            pwdPath = getcwd(NULL, 0);
            printf("%s\n", pwdPath);
            fflush(NULL);
            free(pwdPath);
            deleteFullCommandList(command);
            free(usArg);
            deleteChar2Array(usArgChanged);
            fflush(NULL);
            free(commandList.lst);
            exit(0);
          } else {
            int status_code = execvp(usArgChanged[0], usArgChanged);
            fflush(NULL);

            if (status_code == -1) {
              printf("Command wrong with error code %d.\n", status_code);
              // deleteFullCommandList(command);
              // free(usArg);
              // fflush(NULL);
              // free(commandList.lst);
              // free(commandList.lst);
              exit(0);
            }
          }
          fflush(NULL);
          return 0;
        } else {
          pidList[i] = pid;

        Parent:
          fflush(NULL);
          // waitpid(pid, NULL, 0);
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
        deleteChar2Array(usArgChanged);
      }
    }
    // for (int i=0;i<commandList.length;i++)
    // {
    //   deleteFullCommandList(commandList.lst[i]);
    // }
    // for (int i = 0; i < piped->length; i++) {
    for (int i = 0; i < piped->length; i++) {
      if (pidList[i] != -1) {
        waitpid(pidList[i], NULL, 0);
      }
    }
    free(piped->str);
    free(piped);
    // }
    free(commandList.lst);
  }
  return 0;
}
