#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

#include "builtins.h"
#include "input.h"

int kysh_launch(char **args) {
  pid_t pid, wpid;
  int status;

  pid = fork();
  if (pid == 0) {
    // Child process
    if (execvp(args[0], args) == -1) {
      perror("kysh\n");
    }
    exit(EXIT_FAILURE);
  } else if (pid < 0) {
    // Error forking
    perror("kysh\n");
  } else {
    // Parent process
    do {
      wpid = waitpid(pid, &status, WUNTRACED);
    } while (!WIFEXITED(status) && !WIFSIGNALED(status));
  }

  return 1;
}

int kysh_execute(int argc, char **args) {
  int i;
  if (argc == 0) {
    // An empty command entered
    return 1;
  }

  for (i = 0; i < kysh_num_builtins(); i++) {
    if (strcmp(args[0], kysh_builtins[i].name) == 0) {
      return kysh_builtins[i].func(args);
    }
  }

  return kysh_launch(args);
}

void kysh_loop(void) {
  char *line;
  char **args;
  int argc, status;

  do {
    kysh_display_cwd();
    line = kysh_read_line();
    args = kysh_split_line(line, &argc);
    status = kysh_execute(argc, args);

    free(line);
    free(args);
  } while (status);
}

int main(int argc, char *argv[]) {
  // Load config here

  // Run command loop
  kysh_loop();

  // Perform any shutdown/cleanup

  return EXIT_SUCCESS;
}
