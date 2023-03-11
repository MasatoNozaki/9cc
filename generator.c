#include "generator.h"

int labelseq = 0; // ジャンプラベルの通し番号用

// void gen() {
// 	// 式の最初は数でなければならないので、それをチェックして
// 	// 最初のmov命令を出力
// 	printf("  mov rax, %d\n", expect_number());

// 	while (!at_eof()) {
// 		if (consume('+')) {
// 			printf("  add rax, %d\n", expect_number());
//       		continue;
// 		}
// 		else if (consume('-')) {
// 			printf("  sub rax, %d\n", expect_number());
// 			continue;
// 		}

// 		printf("error");
// 	}
// }

void gen_lval(Node *node) {
  if (node->kind != ND_LVAR)
    error("代入の左辺値が変数ではありません");

  printf("  mov rax, rbp\n");
  printf("  sub rax, %d\n", node->offset);
  printf("  push rax\n");
}

void gen_from_abstructTree(Node *node) {

  	switch (node->kind) {
		case ND_NUM:
			printf("  push %d\n", node->val);
			return;
		case ND_LVAR: // 変数だったら変数の場所を特定（gen_lval）し、その値を読み取る
			gen_lval(node);
			printf("  pop rax\n");
			printf("  mov rax, [rax]\n");
			printf("  push rax\n");
			return;
		case ND_ASSIGN: // 代入だったら、変数を特定し、右辺を計算した後、代入する
			gen_lval(node->lhs);
			gen_from_abstructTree(node->rhs);

			printf("  pop rdi\n");
			printf("  pop rax\n");
			printf("  mov [rax], rdi\n");
			printf("  push rdi\n");
			return;
		case ND_RETURN:
			gen_from_abstructTree(node->lhs);
			printf("  pop rax\n");
			printf("  mov rsp, rbp\n");
			printf("  pop rbp\n");
			printf("  ret\n");
			return;
		case ND_IF: {
			int seq = labelseq++;
			if (node->els) {
				gen_from_abstructTree(node->cond);
				printf("	pop rax\n");
				printf("	cmp rax, 0\n");
				printf("	je .Lelse%d\n", seq); // 条件がtrueでないならjmpする
				gen_from_abstructTree(node->then);
				printf("	jmp .Lend%d\n", seq);
				printf(".Lelse%d:\n", seq);
				gen_from_abstructTree(node->els);
				printf(".Lend%d:\n", seq);
			}
			else {
				gen_from_abstructTree(node->cond);
				printf("	pop rax\n");
				printf("	cmp rax, 0\n");
				printf("	je .Lend%d\n", seq);
				gen_from_abstructTree(node->then);
				printf(".Lend%d:\n", seq);
			}
			return;
		}
	}

	gen_from_abstructTree(node->lhs);
	gen_from_abstructTree(node->rhs);

	printf("  pop rdi\n");
	printf("  pop rax\n");

	switch (node->kind) {
		case ND_ADD:
			printf("  add rax, rdi\n");
			break;
		case ND_SUB:
			printf("  sub rax, rdi\n");
			break;
		case ND_MUL:
			printf("  imul rax, rdi\n");
			break;
		case ND_DIV:
			printf("  cqo\n");
			printf("  idiv rdi\n");
			break;
		case ND_EQ:
			printf("  cmp rax, rdi\n");
			printf("  sete al\n");
			printf("  movzb rax, al\n");
			break;
		case ND_NE:
			printf("  cmp rax, rdi\n");
			printf("  setne al\n");
			printf("  movzb rax, al\n");
			break;
		case ND_LT:
			printf("  cmp rax, rdi\n");
			printf("  setl al\n");
			printf("  movzb rax, al\n");
			break;
		case ND_LE:
			printf("  cmp rax, rdi\n");
			printf("  setle al\n");
			printf("  movzb rax, al\n");
			break;
	}

	printf("  push rax\n");
}