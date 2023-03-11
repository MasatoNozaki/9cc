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
  TK_SYMBOL,   // 記号
  TK_IDENT,    // 識別子
  TK_NUM,      // 整数トークン
  TK_RETURN,
  TK_IF,
  TK_ELSE,
  TK_WHILE,
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

typedef struct LVar LVar;

// ローカル変数の型
struct LVar {
  LVar *next; // 次の変数かNULL
  char *name; // 変数の名前
  int len;    // 名前の長さ
  int offset; // RBPからのオフセット
};

// ローカル変数
LVar *locals;

// 入力文字列pをトークナイズしてそれを返す
Token *tokenize(char *p);

// 新しいトークンを作成してcurに繋げる
Token *new_token(TokenKind kind, Token *cur, char *str, int len);

// 終端かどうかチェックする
bool at_eof();

bool startswith(char *p, char *q);

bool is_alpha(char c);
bool is_alnum(char c);

#endif