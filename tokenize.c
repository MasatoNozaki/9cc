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

	if (startswith(p, "==") || startswith(p, "!=") ||
        startswith(p, "<=") || startswith(p, ">=")) {
      cur = new_token(TK_SYMBOL, cur, p, 2);
      p += 2;
      continue;
    }

	if (startswith(p, "return") && !is_alnum(p[6])) {
		cur = new_token(TK_RETURN, cur, p, 6);
		p += 6;
		continue;
	}

	if (startswith(p, "if") && !is_alnum(p[2])) {
		cur = new_token(TK_IF, cur, p, 2);
		p += 2;
		continue;
	}

	if (startswith(p, "else") && !is_alnum(p[4])) {
		cur = new_token(TK_ELSE, cur, p, 4);
		p += 4;
		continue;
	}

	if (startswith(p, "while") && !is_alnum(p[5])) {
		cur = new_token(TK_WHILE, cur, p, 5);
		p += 5;
		continue;
	}

	if (startswith(p, "for") && !is_alnum(p[3])) {
		cur = new_token(TK_FOR, cur, p, 3);
		p += 3;
		continue;
	}

    if (strchr("+-*/()<>;={},", *p)) {
      cur = new_token(TK_SYMBOL, cur, p, 1);
	  p += 1;
      continue;
    }

	if (is_alpha(*p)) { // 変数。英字と_始まりのみ許す
		char *q = p++;
		while (is_alnum(*p)) { // どこまでが変数名かチェック。変数名中であれば数字も許す
			p++;
		}
		cur = new_token(TK_IDENT, cur, q, p-q); // 長さはp-q
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

bool startswith(char *p, char *q) {
  return memcmp(p, q, strlen(q)) == 0;
}

bool is_alpha(char c) {
  return ('a' <= c && c <= 'z') || ('A' <= c && c <= 'Z') || c == '_';
}

bool is_alnum(char c) {
  return is_alpha(c) || ('0' <= c && c <= '9');
}

char *strndup(char *p, int len) {
  char *buf = malloc(len + 1);
  strncpy(buf, p, len);
  buf[len] = '\0';
  return buf;
}