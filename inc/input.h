/*
 * Handle input commands
 */
#ifndef INPUT_H
#define INPUT_H

#ifdef __cplusplus
extern "C" {
#endif

#define KYSH_TOK_BUFSIZE (64)
#define KYSH_TOK_DELIM " \t\r\n\a"
#define KYSH_TOK_SPEC_DELIM "|"
// #define KYSH_USE_STD_GETLINE

typedef struct command {
  int argc;
  char **argv;
  struct command *next;
} command_t;

char *kysh_read_line(void);
char **kysh_tokenize(char *line);
command_t *kysh_parse_tokens(char **tokens);
void kysh_print_commands(command_t *commands);
void kysh_free_commands(command_t *commands);
void kysh_free_tokens(char **tokens);
int kysh_commands_num(command_t *commands);

#ifdef __cplusplus
}
#endif

#endif  // INPUT_H
