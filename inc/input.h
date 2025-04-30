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
// #define KYSH_USE_STD_GETLINE

char **kysh_split_line(char *line, int *argc);
char *kysh_read_line(void);

#ifdef __cplusplus
}
#endif

#endif  // INPUT_H
