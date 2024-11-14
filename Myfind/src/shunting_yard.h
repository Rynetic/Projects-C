#ifndef SHUNTING_YARD_H
#define SHUNTING_YARD_H

#include "dlist.h"
#include "lexer.h"
#include "sym.h"

int shunting_yard(struct dlist *l, struct dlist *ol, struct node *node);

#endif /* SHUNTING_YARD_H */
