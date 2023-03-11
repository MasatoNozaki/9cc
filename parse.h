#ifndef PARSE_H
#define PARSE_H

#include <stdbool.h>
#include "tokenize.h"

// 抽象構文木のノードの種類
typedef enum {
  ND_ADD, // +
  ND_SUB, // -
  ND_MUL, // *
  ND_DIV, // /
  ND_EQ,  // ==
  ND_NE,  // !=
  ND_LT,  // <
  ND_LE,  // <=
  ND_ASSIGN, // =
  ND_LVAR,   // ローカル変数
  ND_NUM, // 整数
  ND_RETURN,
  ND_IF,
  ND_ELSE,
  ND_WHILE,
  ND_FOR,
  ND_BLOCK,
  ND_FUNCALL
} NodeKind;

typedef struct Node Node;

// 抽象構文木のノードの型
struct Node {
	NodeKind kind; // ノードの型
	Node *lhs;     // 左辺
	Node *rhs;     // 右辺

	Node *init;
	Node *cond;
	Node *inc;
	Node *then;
	Node *els;

	Node *body;
	Node *next;

	char *funcname;
	
	int val;       // kindがND_NUMの場合のみ使う
	int offset;    // kindがND_LVARの場合のみ使う
};

Node *code[100];

// 次のトークンが期待している記号のときには、トークンを1つ読み進めて
// 真を返す。それ以外の場合には偽を返す。
bool consume(char *op);
bool consume_return();
bool consume_if();
bool consume_else();
bool consume_while();
bool consume_for();
Token* consume_ident();

// 次のトークンが期待している記号のときには、トークンを1つ読み進める。
// それ以外の場合にはエラーを報告する。
// consumeとの差別化点は、返り値がないこと。(expr)の解析の時、(exprまである場合は、)をチェックする場合わざわざif文でチェックする必要はない。
void expect(char *op);

// 次のトークンが数値の場合、トークンを1つ読み進めてその数値を返す。
// それ以外の場合にはエラーを報告する。
int expect_number();

// 数字ノードは葉、それ以外は内部ノードであるため、分けている
Node *new_node(NodeKind kind);
Node *new_binary_node(NodeKind kind, Node *lhs, Node *rhs);
Node *new_node_num(int val);

void program();
Node *stmt();
Node *expr();
Node *assign();
Node *equality();
Node *relational();
Node *add();
Node *mul();
Node *unary();
Node *primary();

// 変数を名前で検索する。見つからなかった場合はNULLを返す。
LVar *find_lvar(Token *tok);
#endif // __PARSE_H__