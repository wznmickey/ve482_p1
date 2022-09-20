#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>
void flush() {
  fflush(stdin);
  fflush(stdout);
  return;
}
int main() {
  char input[2048];
  char *arg[2048];
  while (true) {
    printf("mumsh $ ");
    flush();
    memset(input, 0, sizeof(char) * 2048);
    fgets(input, 2048 - 1, stdin);
    flush();
    input[strlen(input) - 1] = '\0'; // remove the \n at the end.
    if (strcmp(input, "exit") == 0) {
      printf("exit\n");
      flush();
      return 0;
    }
    arg[0] = input;
    arg[1] = NULL;
    pid_t pid = fork();
    if (pid == 0) {
      int status_code = execvp(input, arg);
      if (status_code == -1) {
        printf("Command wrong with error code %d.", status_code);
        flush();
        return status_code;
      }
    } else {
      waitpid(pid, NULL, 0);
    }
  }
}
