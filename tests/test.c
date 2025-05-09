#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

#include "builtins.h"
#include "input.h"

void test_redirect(redirection_t *redirections) {
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

int test_launch(command_t *commands) {
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
      test_redirect(commands->redir);
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
      fdd = fd[0];
      commands = commands->next;
    }
  }

  return 1;
}

int test_execute(command_t *commands) {
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

  return test_launch(commands);
}

void test_kysh(char **lines) {
  char **tokens;
  command_t *commands;
  int i, status;

  if (lines == NULL) {
    return;
  }

  for (i = 0; lines[i] != NULL; i++) {
    printf("---------------------------------------------------------------\n");
    kysh_print_line(lines[i]);

    tokens = kysh_tokenize(lines[i]);
    kysh_print_tokens(tokens);

    commands = kysh_parse_tokens(tokens);
    kysh_parse_commands(commands);
    kysh_print_commands(commands);

    printf("output:\n");
    status = test_execute(commands);
    kysh_free_tokens(tokens);
    kysh_free_commands(commands);
  }
}

int main(int argc, char *argv[]) {
  char *lines_multi_pipes_only[] = {"ls -al | rev | nl | cat -e   ", NULL};
  char *lines_multi_pipes_with_builtins[] = {
      "cd .. | echo hello | echo world", "echo hello | cd .. | echo world",
      "echo hello | echo world | cd ..", NULL};
  char *lines_multi_redirs_only[] = {"echo hello > out1 > out2",
                                     "ls -l > file1 -a > file2",
                                     "grep main < ./test.c > out", NULL};
  char *lines_multi_redirs_with_multi_pipes[] = {
      "echo Kyle 87 > in1.txt  | echo Dog 88 > in2.txt | grep Kyle > out.txt < "
      "in2.txt < in1.txt",
      NULL};
  char *lines_error_catch[] = {"cd..", "cd.. | ls", NULL};
  char *lines_arbitrary_ws[] = {"echo hello     | ls",
                                "echo hello|      ls",
                                "echo hello>         hello.txt|   ls",
                                "echo hello         >hello.txt   |ls",
                                "ls     -al    |rev  | nl |cat    -e",
                                NULL};

  test_kysh(lines_multi_pipes_only);
  test_kysh(lines_multi_pipes_with_builtins);
  test_kysh(lines_multi_redirs_only);
  test_kysh(lines_multi_redirs_with_multi_pipes);
  test_kysh(lines_error_catch);
  test_kysh(lines_arbitrary_ws);

  return 0;
}
