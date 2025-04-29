#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

/* Functions declarations for builtin shell commands */
// TODO: Move builtin functions to a separate file
int kysh_cd(char **args);
int kysh_help(char **args);
int kysh_exit(char **args);

/* List of builtin shell commands; followed by their corresponding functions */
char *builtin_str[] = {"cd", "help", "exit"};
int (*builtin_func[])(char **) = {&kysh_cd, &kysh_help, &kysh_exit};

int kysh_num_builtins() { return sizeof(builtin_str) / sizeof(char *); }

/* Builtin functions implementations */
int kysh_cd(char **args) {
  // TODO: Add number of args check
  if (args[1] == NULL) {
    fprintf(stderr, "kysh: cd: expected argument\n");
  } else {
    if (chdir(args[1]) != 0) {
      perror("kysh\n");
    }
  }
  return 1;
}

int kysh_help(char **args) {
  int i;
  printf("kysh\n");
  printf("Type program names and arguments, and hit enter.\n");
  printf("The following are built in:\n");

  for (i = 0; i < kysh_num_builtins(); i++) {
    printf("  %s\n", builtin_str[i]);
  }

  printf("Use the man command for information on other programs.\n");
  return 1;
}

int kysh_exit(char **args) { return EXIT_SUCCESS; }

#define KYSH_TOK_BUFSIZE (64)
#define KYSH_TOK_DELIM " \t\r\n\a"
char **kysh_split_line(char *line) {
  // TODO: Handle quoting, backslash escaping and pipeline in the command line
  // arguments
  int bufsize = KYSH_TOK_BUFSIZE, position = 0;
  char **tokens = malloc(sizeof(char *) * bufsize);
  char *token;

  if (tokens == NULL) {
    fprintf(stderr, "kysh: memory allocation error\n");
    exit(EXIT_FAILURE);
  }

  token = strtok(line, KYSH_TOK_DELIM);
  while (token != NULL) {
    tokens[position] = token;
    position++;

    if (position >= bufsize) {
      bufsize += KYSH_TOK_BUFSIZE;
      tokens = realloc(tokens, sizeof(char *) * bufsize);
      if (tokens == NULL) {
        fprintf(stderr, "kysh: memory allocation error\n");
        exit(EXIT_FAILURE);
      }
    }

    token = strtok(NULL, KYSH_TOK_DELIM);
  }

  tokens[position] = NULL;
  return tokens;
}

#define KYSH_USE_STD_GETLINE
char *kysh_read_line(void) {
#ifdef KYSH_USE_STD_GETLINE
  char *line = NULL;
  size_t bufsize = 0;
  if (getline(&line, &bufsize, stdin) == -1) {
    if (feof(stdin)) {
      exit(EXIT_SUCCESS);
    } else {
      perror("readline error\n");
      exit(EXIT_FAILURE);
    }
  }

  return line;
#else
  int bufsize = KYSH_RL_BUFSIZE;
  int position = 0;
  char *buffer = malloc(sizeof(char) * bufsize);
  int c;

  if (buffer == NULL) {
    fprintf(stderr, "kysh: memory allocation error\n");
    exit(EXIT_FAILURE);
  }

  while (1) {
    // Read a character
    c = getchar();

    // If we hit the EOF, replace it with a hull character and return
    if (c == EOF || c == '\n') {
      buffer[position] = '\0';
      return buffer;
    } else {
      buffer[position] = c;
    }
    position++;

    // If we have exceeded the buffer size, reallocate
    if (position >= bufsize) {
      bufsize += KYSH_RL_BUFSIZE;
      buffer = realloc(buffer, bufsize);
      if (buffer == NULL) {
        fprintf(stderr, "kysh: memory allocation error\n");
        exit(EXIT_FAILURE);
      }
    }
  }
#endif
}

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

void kysh_loop(void) {
  char *line;
  char **args;
  int status;

  do {
    printf("> ");
    line = kysh_read_line();
    args = kysh_split_line(line);
    // status = kysh_execute(args);

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
