#include "generator.h"

void gen() {
	// アセンブリの前半部分を出力
	printf(".intel_syntax noprefix\n");
	printf(".globl main\n");
	printf("main:\n");

	// 式の最初は数でなければならないので、それをチェックして
	// 最初のmov命令を出力
	printf("  mov rax, %d\n", expect_number());

	while (!at_eof()) {
		if (consume('+')) {
			printf("  add rax, %d\n", expect_number());
      		continue;
		}
		else if (consume('-')) {
			printf("  sub rax, %d\n", expect_number());
			continue;
		}

		printf("error");
	}

	printf("	ret\n");
}