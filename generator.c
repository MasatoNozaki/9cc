#include "generator.h"

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

void gen_from_abstructTree(Node *node) {
	if (node->kind == ND_NUM) {
		printf("  push %d\n", node->val);
		return;
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
	}

	printf("  push rax\n");
}