#ifndef PARSER_H
#define PARSER_H

#include "lexer.h"
int process_line(struct dlist *l, struct dlist *ol, char **s, int *pos);
struct node *build_ast(char **s, int *pos, struct root *root);
struct root *parse_line(char **args, int arc);

#endif /* PARSER_H */
