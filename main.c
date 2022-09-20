#include <stdbool.h>
#include <stdio.h>
#include <string.h>
int main() {
  char input[2048];
  while (true) {
    printf("mumsh $ ");
    memset(input, 0, sizeof(char) * 2048);
    fgets(input, 2048 - 1, stdin);
    if (strcmp(input, "exit\n") == 0) {
      printf("exit\n");
      return 0;
    }
  }
}
