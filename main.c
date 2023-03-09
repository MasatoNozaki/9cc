#include <stdlib.h>
#include "common.h"
#include "tokenize.h"
#include "generator.h"

int main(int argc, char **argv) {
	if (argc != 2) {
		error_at(token->str, "引数の個数が正しくありません");
		return 1;
	}

	user_input = argv[1];
	token = tokenize(user_input);
	Node *node = expr();

	// アセンブリの前半部分を出力
	printf(".intel_syntax noprefix\n");
	printf(".globl main\n");
	printf("main:\n");

	gen_from_abstructTree(node);

	// スタックトップに式全体の値が残っているはずなので
	// それをRAXにロードして関数からの返り値とする
	printf("  pop rax\n");
	printf("  ret\n");
	
	return 0;
}