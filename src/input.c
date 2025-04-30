#include "input.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "builtins.h"

char **kysh_split_line(char *line) {
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
#define KYSH_RL_BUFSIZE (1024)
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
