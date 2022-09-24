
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>
#include "String.h"
#include "parse.h"
void flush() {
  fflush(stdin);
  fflush(stdout);
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
    printf("mumsh $ ");
    memset(input, 0, sizeof(char) * 2048);
    fgets(input, 2048 - 1, stdin);
    input[strlen(input) - 1] = '\0';  // remove the \n at the end.
    if (strcmp(input, "exit") == 0) {
      printf("exit\n");
      return 0;
    }
    String *inputS = initString(input);
    Command *command = malloc(sizeof(Command));
    initCommand(command);
    parse(inputS, command);
    char **usArg = getArgFromCommand(command);
    pid_t pid = fork();
    if (pid == 0) {
      int status_code = execvp(usArg[0], usArg);
      if (status_code == -1) {
        printf("Command wrong with error code %d.\n", status_code);
        deleteFullCommandList(command);

        free(usArg);
        return status_code;
      }
    } else {
      waitpid(pid, NULL, 0);
    }
    deleteFullCommandList(command);

    free(usArg);
  }
}
