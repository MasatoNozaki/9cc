#include "parse.h"

bool consume(char op) {
  if (token->kind != TK_SYMBOL || token->str[0] != op) // 種類と文字数でチェック
    return false;
  token = token->next;
  return true;
}

int expect_number() {
  if (token->kind != TK_NUM)
    error("数ではありません");
  int val = token->val;
  token = token->next;
  return val;
}