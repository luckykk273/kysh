#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

#include "builtins.h"
#include "input.h"

int kysh_launch(command_t *commands) {
  int fd[2];
  pid_t pid, wpid;
  int status, fdd = 0;  // Backup

  while (commands != NULL) {
    pipe(fd);
    pid = fork();
    if (pid == 0) {
      // Child process
      dup2(fdd, STDIN_FILENO);
      if (commands->next != NULL && commands->next->argv != NULL) {
        dup2(fd[1], STDOUT_FILENO);
      }
      close(fd[0]);
      execvp(commands->argv[0], commands->argv);
      exit(EXIT_FAILURE);
    } else if (pid < 0) {
      perror("kysh: fork\n");
    } else {
      // Parent process
      // wait(NULL);
      do {
        wpid = waitpid(pid, &status, WUNTRACED);
      } while (!WIFEXITED(status) && !WIFSIGNALED(status));
      close(fd[1]);
      fdd = fd[0];
      commands = commands->next;
    }
  }

  return 1;
}

int kysh_execute(command_t *commands) {
  int i;
  while (commands != NULL) {
    if (commands->argv[0] == NULL) {
      // An empty command entered
      return 1;
    }

    for (i = 0; i < kysh_num_builtins(); i++) {
      if (strcmp(commands->argv[0], kysh_builtins[i].name) == 0) {
        return kysh_builtins[i].func(commands->argv);
      }
    }

    return kysh_launch(commands);
  }

  return 1;
}

void kysh_loop(void) {
  char *line;
  char **tokens;
  command_t *commands;
  int status = 0;

  do {
    kysh_display_cwd();
    line = kysh_read_line();
    tokens = kysh_tokenize(line);
    // TODO: how to parse tokens for pipes and I/O redirection
    commands = kysh_parse_tokens(tokens);
    status = kysh_execute(commands);

    free(line);
    kysh_free_tokens(tokens);
    kysh_free_commands(commands);
  } while (status);
}

int main(int argc, char *argv[]) {
  // Load config here

  // Run command loop
  kysh_loop();

  // Perform any shutdown/cleanup

  return EXIT_SUCCESS;
}
