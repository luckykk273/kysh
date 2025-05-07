#include "input.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "builtins.h"

void kysh_free_tokens(char **tokens) {
  int i;

  if (tokens == NULL) {
    return;
  }

  for (i = 0; tokens[i] != NULL; i++) {
    free(tokens[i]);
  }
  free(tokens);
}

int kysh_commands_num(command_t *commands) {
  int num = 0;
  while (commands != NULL) {
    num++;
    commands = commands->next;
  }
  return num;
}

void kysh_print_commands(command_t *commands) {
  int i, cmd_num = 0;
  while (commands != NULL) {
    printf("Command %d: argc = %d\n", cmd_num++, commands->argc);
    for (i = 0; i < commands->argc; i++) {
      printf("  argv[%d]: %s\n", i, commands->argv[i]);
    }
    commands = commands->next;
  }
}

void kysh_free_commands(command_t *commands) {
  command_t *next;
  int i;
  while (commands != NULL) {
    next = commands->next;
    for (i = 0; i < commands->argc; i++) {
      free(commands->argv[i]);
    }
    free(commands->argv);
    free(commands);
    commands = next;
  }
}

char **kysh_tokenize(char *line) {
  // arguments
  int bufsize = KYSH_TOK_BUFSIZE, position = 0;
  char **tokens = malloc(sizeof(char *) * bufsize);
  const char *p_line = line;

  if (tokens == NULL) {
    fprintf(stderr, "kysh: memory allocation error\n");
    exit(EXIT_FAILURE);
  }

  while (*p_line != '\0') {
    // Skip the delimiters
    while (*p_line != '\0' && strchr(KYSH_TOK_DELIM, *p_line) != NULL) {
      p_line++;
    }

    if (*p_line == '\0') {
      break;
    }

    if (strchr(KYSH_TOK_SPEC_DELIM, *p_line) != NULL) {
      // Handle special characters
      tokens[position] = strndup(p_line, 1);
      p_line++;
    } else {
      // Normal case
      const char *start = p_line;
      while (*p_line != '\0' &&
             strchr(KYSH_TOK_DELIM KYSH_TOK_SPEC_DELIM, *p_line) == NULL) {
        p_line++;
      }
      tokens[position] = strndup(start, p_line - start);
    }
    position++;

    if (position >= bufsize) {
      bufsize += KYSH_TOK_BUFSIZE;
      tokens = realloc(tokens, sizeof(char *) * bufsize);
      if (tokens == NULL) {
        fprintf(stderr, "kysh: memory allocation error\n");
        exit(EXIT_FAILURE);
      }
    }
  }

  tokens[position] = NULL;
  return tokens;
}

/*
  1. Handle '|', '<', '>' only;
  2. '>' and '>' can only be placed to the left/right most;
  */
command_t *kysh_parse_tokens(char **tokens) {
  command_t *head = NULL, *current = NULL, *node;
  int i, start, end, argc;

  start = 0;
  while (1) {
    // Find next pipe or end
    end = start;
    while (tokens[end] != NULL && strcmp(tokens[end], "|") != 0) {
      end++;
    }

    // Count args and allocate
    argc = end - start;
    char **argv = malloc(sizeof(char *) * (argc + 1));
    if (argv == NULL) {
      fprintf(stderr, "kysh: memory allocation error\n");
      exit(EXIT_FAILURE);
    }

    for (i = 0; i < argc; i++) {
      argv[i] = strdup(tokens[start + i]);
    }
    argv[argc] = NULL;

    // Create new node
    node = malloc(sizeof(command_t));
    if (node == NULL) {
      fprintf(stderr, "kysh: memory allocation error\n");
      exit(EXIT_FAILURE);
    }
    node->argc = argc;
    node->argv = argv;
    node->next = NULL;

    if (head == NULL) {
      head = node;
    } else {
      current->next = node;
    }

    current = node;

    // Move to next command
    if (tokens[end] == NULL) {
      break;
    }

    start = end + 1;
  }

  return head;
}

char *kysh_read_line(void) {
#ifdef KYSH_USE_STD_GETLINE
  char *line = NULL;
  size_t bufsize = 0;
  if (getline(&line, &bufsize, stdin) == -1) {
    if (feof(stdin)) {
      exit(EXIT_SUCCESS);
    } else {
      perror("kysh: feof");
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
    if (c == EOF) {
      exit(EXIT_SUCCESS);
    } else if (c == '\n') {
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
