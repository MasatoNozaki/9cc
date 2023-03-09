#ifndef GENERATOR_H
#define GENERATOR_H

#include "tokenize.h"
#include "parse.h"

void gen();

void gen_from_abstructTree(Node *node);

#endif // __GENERATOR_H__