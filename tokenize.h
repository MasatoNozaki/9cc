#ifndef TOKENIZE_H
#define TOKENIZE_H

#include <stdio.h>
#include <stdbool.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include "common.h"

// トークンの種類
typedef enum {
  TK_SYMBOL, // 記号
  TK_NUM,      // 整数トークン
  TK_EOF,      // 入力の終わりを表すトークン
} TokenKind;

typedef struct Token Token;

// トークン型
struct Token {
  TokenKind kind; // トークンの型
  Token *next;    // 次の入力トークン
  int val;        // kindがTK_NUMの場合、その数値
  char *str;      // トークン文字列
  int len;		  // トークンの長さ
};

// 現在着目しているトークン
Token *token;

// 入力文字列pをトークナイズしてそれを返す
Token *tokenize(char *p);

// 新しいトークンを作成してcurに繋げる
Token *new_token(TokenKind kind, Token *cur, char *str);

// 終端かどうかチェックする
bool at_eof();

#endif