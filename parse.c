#include "parse.h"

bool consume(char *op) {
  if (token->kind != TK_SYMBOL || strlen(op) != token->len || memcmp(token->str, op, token->len)) // 種類と文字数でチェック
    return false;
  token = token->next;
  return true;
}

bool consume_return() {
	if (token->kind != TK_RETURN) {
		return false;
	}
	token = token->next;
	return true;
}

bool consume_if() {
	if (token->kind != TK_IF) {
		return false;
	}
	token = token->next;
	return true;
}

bool consume_else() {
	if (token->kind != TK_ELSE) {
		return false;
	}
	token = token->next;
	return true;
}

bool consume_while() {
	if (token->kind != TK_WHILE) {
		return false;
	}
	token = token->next;
	return true;
}

bool consume_for() {
	if (token->kind != TK_FOR) {
		return false;
	}
	token = token->next;
	return true;
}

Token* consume_ident() {
	if (token->kind != TK_IDENT) {
		return NULL;
	}
	Token *t = token;
	token = token->next;
	return t;
}

void expect(char *op) {
  if (token->kind != TK_SYMBOL || strlen(op) != token->len || memcmp(token->str, op, token->len))
    error("'%c'ではありません", op);
  token = token->next;
}

int expect_number() {
  if (token->kind != TK_NUM)
    error_at(token->str, "数ではありません");
  int val = token->val;
  token = token->next;
  return val;
}

Node *new_node(NodeKind kind, Node *lhs, Node *rhs) {
  Node *node = calloc(1, sizeof(Node));
  node->kind = kind;
  node->lhs = lhs;
  node->rhs = rhs;
  return node;
}

Node *new_node_num(int val) {
  Node *node = calloc(1, sizeof(Node));
  node->kind = ND_NUM;
  node->val = val;
  return node;
}

void program() {
	int i = 0;
	while (!at_eof())
		code[i++] = stmt();
	code[i] = NULL;
}

Node *stmt() {
	Node *node;

	if (consume_return()) {
		node = calloc(1, sizeof(Node));
		node->kind = ND_RETURN;
		node->lhs = expr();
	}
	else if (consume_if()) {
		node = calloc(1, sizeof(Node));
		node->kind = ND_IF;
		expect("(");
		node->cond = expr(); // 条件式
		expect(")");
		node->then = stmt(); // ifブロックの中身
		if (consume_else())
			node->els = stmt(); // elseブロックの中身
		return node;
	}
	else if (consume_while()) {
		node = calloc(1, sizeof(Node));
		node->kind = ND_WHILE;
		expect("(");
		node->cond = expr();
		expect(")");
		node->then = stmt();
		return node;
	}
	else if (consume_for()) {
		node = calloc(1, sizeof(Node));
		node->kind = ND_FOR;
		expect("(");
		if (!consume(";")) {
			node->init = expr();
			expect(";");
		}
		if (!consume(";")) {
			node->cond = expr();
			expect(";");
		}
		if (!consume(")")) {
			node->inc = expr();
			expect(")");
		}
		node->then = stmt();
		return node;
	}
	else if (consume("{")) {
		Node head;
		head.next = NULL;
		Node *cur = &head;

		while (!consume("}")) {
			cur->next = stmt();
			cur = cur->next;
		}

		Node *node = calloc(1, sizeof(Node));
		node->kind = ND_BLOCK;
		node->body = head.next;
		return node;
	}
	else {
		node = expr();
	}

	if (!consume(";"))
		error_at(token->str, "';'ではないトークンです");
	return node;
}

Node *expr() {
	return assign();
}

Node *assign() {
	Node *node = equality();
	if (consume("="))
		node = new_node(ND_ASSIGN, node, assign());
	return node;
}

Node *equality() {
	Node *node = relational();

	for (;;) {
		if (consume("=="))
			node = new_node(ND_EQ, node, relational());
		else if(consume("!="))
			node = new_node(ND_NE, node, relational());
		else
			return node;
	}
}

Node *relational() {
	Node *node = add();

	for (;;) {
		if (consume("<="))
			node = new_node(ND_LE, node, add());
		else if(consume(">="))
			node = new_node(ND_LE, add(), node);
		else if(consume("<"))
			node = new_node(ND_LT, node, add());
		else if(consume(">"))
			node = new_node(ND_LT, add(), node);
		else
			return node;
	}
}

Node *add() {
  Node *node = mul();

  for (;;) {
    if (consume("+"))
      node = new_node(ND_ADD, node, mul());
    else if (consume("-"))
      node = new_node(ND_SUB, node, mul());
    else
      return node;
  }
}

Node *mul() {
  Node *node = unary();

  for (;;) {
    if (consume("*"))
      node = new_node(ND_MUL, node, unary());
    else if (consume("/"))
      node = new_node(ND_DIV, node, unary());
    else
      return node;
  }
}

Node *unary() {
  if (consume("+"))
    return unary();
  if (consume("-"))
    return new_node(ND_SUB, new_node_num(0), unary()); // 0-xとして実装
  return primary();
}

Node *primary() {
	// 次のトークンが"("なら、"(" expr ")"のはず
	if (consume("(")) {
		Node *node = expr();
		expect(")");
		return node;
	}

	// 変数
	Token *tok = consume_ident();
	if (tok) {
		Node *node = calloc(1, sizeof(Node));
		node->kind = ND_LVAR;

		LVar *lvar = find_lvar(tok);
		if (lvar) {
			 node->offset = lvar->offset;
		}
		else {
			// 後ろにつなぐのは、毎回最後尾にアクセスする必要があって面倒なため、前方に伸ばしていく
			lvar = calloc(1, sizeof(LVar));
			lvar->next = locals;
			lvar->name = tok->str;
			lvar->len = tok->len;
			// localsが未初期化の場合、エラーになるので分岐。参考元のコードがそもそも作りが違うため、独自に実装
			if (locals) {
				lvar->offset = locals->offset+8;
			}
			else {
				lvar->offset = 8;
			}
			node->offset = lvar->offset;
			locals=lvar;
		}
		return node;
	}

	// そうでなければ数値のはず
	return new_node_num(expect_number());
}

LVar *find_lvar(Token *tok) {
  for (LVar *var = locals; var; var = var->next)
    if (var->len == tok->len && !memcmp(tok->str, var->name, var->len)) // memcmpは一致した場合0を返す
      return var;
  return NULL;
}