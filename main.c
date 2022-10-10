
#include <stdbool.h>
#include <stddef.h>

#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>
#include <errno.h>
#include "String.h"
#include "parse.h"
#include "flush.h"
char input[1030];
char singleLine[1030];  // It is used to handle questions directly after input
void SIGINTReaction() {
  if (strlen(input) == 0) {
    printf("mumsh $ ");
  }
  return;
}
int main() {
  signal(SIGQUIT, SIG_IGN);
  signal(SIGINT, SIGINTReaction);
  signal(SIGTSTP, SIG_IGN);
  signal(SIGTTIN, SIG_IGN);
  signal(SIGTTOU, SIG_IGN);
  setpgid(getpid(), getpid());
  tcsetpgrp(STDOUT_FILENO, getpid());
  tcsetpgrp(STDIN_FILENO, getpid());
  Job *firstJob = malloc(sizeof(Job));
  firstJob->isValid = false;
  Job *lastJob = firstJob;
  int currentJobId = 1;
  char *lastPath = NULL;
  lastPath = getcwd(NULL, 0);
  while (true) {
  RESTART:
    printf("mumsh $ ");
    memset(input, 0, sizeof(char) * 1026);
    memset(singleLine, 0, sizeof(char) * 1026);
    int offset = 0;
    bool running = true;
  READINPUT:
    fgets(input + offset, 1030 - 1, stdin);
    memcpy(singleLine, input, 1026);
    if (strlen(input) == 0) {
      printf("exit\n");
      break;
    }
    if (allSpace(input)) {
      goto RESTART;
    }
    while (true) {
      pid_t temp = waitpid(-1, NULL, WNOHANG);
      Job *tempJob = firstJob;
      if (temp <= 0) {
        break;
      }
      while (true) {
        if ((tempJob != NULL) && (tempJob->isValid) && (tempJob->pid == temp)) {
          tempJob->state = done;
          break;
        }
        if (tempJob == NULL) {
          break;
        }
        tempJob = tempJob->nextJob;
      }
    }
    changeQuote(singleLine);
    bool flagIsValid = isValid(singleLine);
    if (!flagIsValid) {
      goto RESTART;
    }
    bool ifComplete = checkIfComplete(input);
    if (!ifComplete) {
      printf("> ");
      offset = (int)strlen(input);
      goto READINPUT;
    }
    if (input[strlen(input) - 1] == '\n') {
      input[strlen(input) - 1] = '\0';  // remove the \n at the end.
    }
    bool isNotEnd = checkIfNotEnd(input);
    if (isNotEnd) {
      printf("> ");
      offset = (int)strlen(input);
      goto READINPUT;
    }
    changeQuote(input);
    if (strcmp(input, "exit") == 0) {
      printf("exit\n");
      break;
    }
    if (strlen(input) == 0) {
      goto RESTART;
    }
    int backNum = checkIsBack(input);
    bool isBack;
    if (backNum == -1) {
      isBack = false;
    } else {
      isBack = true;
    }
    if (isBack) {
      Job *temp;
      if (!(lastJob->isValid)) {
        temp = lastJob;
      } else {
        temp = malloc(sizeof(Job));
        lastJob->nextJob = temp;
      }
      temp->nextJob = NULL;
      temp->isValid = true;
      temp->jobid = currentJobId;
      currentJobId++;
      temp->state = running;
      temp->jobcmd = malloc(sizeof(char) * (strlen(input) + 1));
      strcpy(temp->jobcmd, input);
      temp->emptySpaceBehind = backNum;
      printf("[%d] %s&", temp->jobid, temp->jobcmd);
      for (int i = 0; i < temp->emptySpaceBehind; i++) printf(" ");
      printf("\n");
      lastJob = temp;
    }
    String *inputS = initString(input);
    StringList *piped = dividesByPipe(inputS);
    int pipFile[2] = {-1, -1};
    CommandList commandList;
    commandList.length = piped->length;
    commandList.lst = malloc(sizeof(Command *) * (size_t)commandList.length);
    for (int i = 0; i < piped->length; i++) {
      StringList *stringList = seperateString(piped->str[i]);
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
        if (firstCommand->inFile != 0) {
          firstCommand->inFile = -4;
        } else {
          firstCommand->inFile = pipFile[0];
        }
      }
      if ((piped->length) - i != 1) {
        pipe(pipFile);
        if (firstCommand->outFile != 1) {
          firstCommand->outFile = -5;
        } else {
          lastCommand->outFile = pipFile[1];
        }
      }
      commandList.lst[i] = firstCommand;
      deleteStringList(stringList);
    }
    int pidList[1025];
    for (int i = 0; i < piped->length; i++) {
      Command *firstCommand = commandList.lst[i];
      while (firstCommand != NULL) {
        Command *command = firstCommand;
        firstCommand = firstCommand->after;
        if (command->isValid == false) {
          pidList[i] = -1;
          free(command);
          continue;
        }
        if (command->inFile == -2) {
          printf(": No such file or directory\n");
          running = false;
        }
        if (command->outFile == -3) {
          printf(": Permission denied\n");
          running = false;
        }
        if (command->inFile == -4) {
          printf("error: duplicated input redirection\n");
          running = false;
        }
        if (command->outFile == -5) {
          printf("error: duplicated output redirection\n");
          running = false;
        }
        char **usArg = getArgFromCommand(command);
        char **usArgChanged = changeFromArg(usArg);
        if (usArgChanged[0] == NULL) {
          printf("error: missing program\n");
          running = false;
        }
        int stdin_ = 0;
        int stdout_ = 1;
        if ((running) && (strcmp(usArgChanged[0], "cd") == 0)) {
          pidList[i] = -1;
          char *aim;
          if (usArgChanged[1] == NULL) {
            // no argument, directly go to home page
            char *login = getlogin();
            if (strcmp(login, "root") == 0) {
              char aim[] = "/root";
              errno = 0;
              char *tempPath = getcwd(NULL, 0);
              chdir(aim);
              if (errno != 0) {
                printf("%s: No such file or directory\n", aim);
                free(tempPath);
                goto Parent;
              }
              free(lastPath);
              lastPath = tempPath;
              goto Parent;

            } else {
              char homePath[6 + 1 +
                            strlen(login)];  // man useradd can allow name
                                             // of maximum of 32 chars.
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
              errno = 0;
              char *tempPath = getcwd(NULL, 0);
              chdir(homePath);
              if (errno != 0) {
                printf("%s: No such file or directory\n", homePath);
                free(tempPath);
                goto Parent;
              }
              free(lastPath);
              lastPath = tempPath;
              goto Parent;
            }
          }
          if (usArgChanged[1][0] == '-') {
            errno = 0;
            char *tempPath = getcwd(NULL, 0);
            chdir(lastPath);
            if (errno != 0) {
              printf("%s: No such file or directory\n", lastPath);
              free(tempPath);
              goto Parent;
            }
            printf("%s\n", lastPath);
            free(lastPath);
            lastPath = tempPath;
            goto Parent;
          }
          if (usArgChanged[1][0] == '~') {
            char *login = getlogin();
            char *homePath;
            if (strcmp(login, "root") == 0) {
              char aim[] = "/root";
              homePath = malloc(sizeof(char) * (strlen(aim) + 1));
              strcpy(homePath, aim);
            } else {
              char *aim = malloc(sizeof(char) * (6 + 1 + strlen(login)));
              aim[0] = '/';
              aim[1] = 'h';
              aim[2] = 'o';
              aim[3] = 'm';
              aim[4] = 'e';
              aim[5] = '/';
              for (int i = 0; i < (int)strlen(login); i++) {
                aim[6 + i] = login[i];
              }
              aim[strlen(login) + 6] = '\0';
              homePath = malloc(sizeof(char) * (strlen(aim) + 1));
              strcpy(homePath, aim);
              free(aim);
            }

            char newPath[strlen(homePath) + 2 + strlen(usArgChanged[1])];
            strcpy(newPath, homePath);
            free(homePath);
            size_t temp = strlen(newPath);
            strcpy(newPath + temp, usArgChanged[1] + 1);

            errno = 0;
            char *tempPath = getcwd(NULL, 0);
            chdir(newPath);
            if (errno != 0) {
              printf("%s: No such file or directory\n", newPath);
              free(tempPath);
              goto Parent;
            }
            free(lastPath);
            lastPath = tempPath;

            goto Parent;
          }
          if (usArgChanged[1] != NULL &&
              usArgChanged[2] != NULL)  // to many arguments
          {
            goto Parent;
          }
          if ((usArgChanged[1][0] != '/'))  // relative path
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
          errno = 0;
          char *tempPath = getcwd(NULL, 0);
          chdir(aim);
          if (errno != 0) {
            printf("%s: No such file or directory\n", usArgChanged[1]);
            free(tempPath);
            goto Parent;
          }
          free(aim);
          free(lastPath);
          lastPath = tempPath;

          goto Parent;
        }
        if (!running) {
          pidList[i] = -1;
          goto Parent;
        }
        pid_t pid = fork();
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
        if (pid == 0) {
          signal(SIGINT, SIG_DFL);
          signal(SIGQUIT, SIG_DFL);
          signal(SIGTSTP, SIG_DFL);
          signal(SIGTTIN, SIG_DFL);
          signal(SIGTTOU, SIG_DFL);
          signal(SIGCHLD, SIG_DFL);
          if ((strcmp(usArgChanged[0], "jobs") == 0)) {
            Job *temp = firstJob;
            while (true) {
              if (temp == NULL) {
                break;
              }
              if (temp->isValid == false) {
                break;
              }
              printf("[%d] ", temp->jobid);
              if (temp->state == running) {
                printf("running ");
              } else {
                printf("done ");
              }
              printf("%s&", temp->jobcmd);
              for (int i = 0; i < temp->emptySpaceBehind; i++) {
                printf(" ");
              }
              printf("\n");
              temp = temp->nextJob;
            }
            exit(0);
          } else if (strcmp(usArgChanged[0], "pwd") == 0) {
            char *pwdPath = NULL;
            pwdPath = getcwd(NULL, 0);
            printf("%s\n", pwdPath);
            free(pwdPath);
            deleteFullCommandList(command);
            free(usArg);
            deleteChar2Array(usArgChanged);
            free(commandList.lst);
            exit(0);
          }
          errno = 0;
          int status_code = execvp(usArgChanged[0], usArgChanged);
          if (status_code == -1) {
            printf("%s", usArgChanged[0]);
            printf(": command not found\n");
            exit(0);
          }
          exit(0);
        }
        if (!isBack) {
          pidList[i] = pid;
        } else {
          pidList[i] = -1;
          lastJob->pid = pid;
        }
      Parent:
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
        deleteFullCommandList(command);
        free(usArg);
        deleteChar2Array(usArgChanged);
      }
    }
    for (int i = 0; i < piped->length; i++) {
      if (pidList[i] != -1) {
        waitpid(pidList[i], NULL, 0);
      }
    }
    free(piped->str);
    free(piped);
    free(commandList.lst);
  }
  exit(0);
}
