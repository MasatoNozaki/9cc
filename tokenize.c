#include "tokenize.h"

Token *tokenize(char *p) {
  Token head;
  head.next = NULL;
  Token *cur = &head;

  while (*p) {
    // 空白文字をスキップ
    if (isspace(*p)) {
      p++;
      continue;
    }

    if (strchr("+-*/()", *p)) {
      cur = new_token(TK_SYMBOL, cur, p++, 1);
      continue;
    }

    if (isdigit(*p)) { // 数字用のnew_tokenを作ってもいいかもしれない
      cur = new_token(TK_NUM, cur, p, 1);
      cur->val = strtol(p, &p, 10);
      continue;
    }
    error_at(p, "トークナイズできません");
  }

  new_token(TK_EOF, cur, p, 1); // 終端
  return head.next;
}

Token *new_token(TokenKind kind, Token *cur, char *str, int len) {
  Token *tok = calloc(1, sizeof(Token));
  tok->kind = kind;
  tok->str = str;
  tok->len = len;
  cur->next = tok;
  return tok;
}

bool at_eof() {
  return token->kind == TK_EOF;
}