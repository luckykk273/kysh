#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

#include "builtins.h"
#include "input.h"

void kysh_redirect(redirection_t *redirections) {
  redirection_t *cur = redirections;
  while (cur) {
    if (cur->dir == 0) {
      // Input redirection
      int fd = open(cur->file, O_RDONLY);
      if (fd < 0) {
        perror(cur->file);
        exit(EXIT_FAILURE);
      }
      dup2(fd, STDIN_FILENO);
      close(fd);
    } else if (cur->dir == 1) {
      // Output redirection
      int fd = open(cur->file, O_WRONLY | O_CREAT | O_TRUNC, 0644);
      if (fd < 0) {
        perror(cur->file);
        exit(EXIT_FAILURE);
      }
      dup2(fd, STDOUT_FILENO);
      close(fd);
    }
    cur = cur->next;
  }
}

int kysh_launch(command_t *commands) {
  int fd[2];
  pid_t pid, wpid;
  int status, fdd = 0;

  while (commands != NULL) {
    pipe(fd);
    pid = fork();
    if (pid == 0) {
      // Child process
      // Set the current input as the reading from the PREVIOUS pipe
      dup2(fdd, STDIN_FILENO);
      if (commands->next != NULL && commands->next->argv != NULL) {
        // Set the current output as the writing to the NEXT pipe
        dup2(fd[1], STDOUT_FILENO);
      }
      close(fd[0]);
      kysh_redirect(commands->redir);
      if (execvp(commands->argv[0], commands->argv) == -1) {
        perror("kysh: execvp");
      }
      exit(EXIT_FAILURE);
    } else if (pid < 0) {
      perror("kysh: fork");
    } else {
      // Parent process
      // wait(NULL);
      do {
        wpid = waitpid(pid, &status, WUNTRACED);
      } while (!WIFEXITED(status) && !WIFSIGNALED(status));
      close(fd[1]);
      // Backup the reading from the current pipe as the input for the NEXT
      // pipe
      fdd = fd[0];
      commands = commands->next;
    }
  }

  return 1;
}

int kysh_execute(command_t *commands) {
  int i;
  int cmd_num = kysh_commands_num(commands);
  if (cmd_num == 1) {
    // Builtin function is valid only if there is no pipe
    for (i = 0; i < kysh_num_builtins(); i++) {
      if (strcmp(commands->argv[0], kysh_builtins[i].name) == 0) {
        return kysh_builtins[i].func(commands->argv);
      }
    }
  }

  if (commands == NULL || commands->argv[0] == NULL) {
    // An empty command entered
    return 1;
  }

  return kysh_launch(commands);
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
    commands = kysh_parse_tokens(tokens);
    kysh_parse_commands(commands);
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
