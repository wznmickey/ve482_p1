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
#define printf(...)                                                            \
  printf(__VA_ARGS__);                                                         \
  flush();
#define fgets(...)                                                             \
  fgets(__VA_ARGS__);                                                          \
  flush();

int main() {
  char input[2048];
  char *arg[2048];
  while (true) {
    printf("mumsh $ ");
    memset(input, 0, sizeof(char) * 2048);
    fgets(input, 2048 - 1, stdin);
    input[strlen(input) - 1] = '\0'; // remove the \n at the end.
    if (strcmp(input, "exit") == 0) {
      printf("exit\n");
      return 0;
    }
    arg[0] = input;
    arg[1] = NULL;
    pid_t pid = fork();
    if (pid == 0) {
      int status_code = execvp(input, arg);
      if (status_code == -1) {
        printf("Command wrong with error code %d.\n", status_code);
        return status_code;
      }
    } else {
      waitpid(pid, NULL, 0);
    }
  }
}
