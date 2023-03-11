#include "generator.h"

int labelseq = 0; // ジャンプラベルの通し番号用
char *argreg[] = {"rdi", "rsi", "rdx", "rcx", "r8", "r9"}; // 引数を保存しておくレジスタ

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
		case ND_WHILE: {
			int seq = labelseq++;
			printf(".Lbegin%d:\n", seq);
			gen_from_abstructTree(node->cond);
			printf("	pop rax\n");
			printf("	cmp rax, 0\n");
			printf("	je .Lend%d\n", seq);
			gen_from_abstructTree(node->then);
			printf("	jmp .Lbegin%d\n", seq);
			printf(".Lend%d:\n", seq);
			return;
		}
		case ND_FOR: {
			int seq = labelseq++;
			if (node->init)
				gen_from_abstructTree(node->init);
			printf(".Lbegin%d:\n", seq);
			if (node->cond) {
				gen_from_abstructTree(node->cond);
				printf("	pop rax\n");
				printf("	cmp rax, 0\n");
				printf("	je .Lend%d\n", seq);
			}
			gen_from_abstructTree(node->then);
			if (node->inc)
				gen_from_abstructTree(node->inc);
			printf("	jmp .Lbegin%d\n", seq);
			printf(".Lend%d:\n", seq);
			return;
		}
		case ND_BLOCK:
			for (Node *n = node->body; n; n = n->next)
				gen_from_abstructTree(n);
			return;
		case ND_FUNCALL: {
			int nargs = 0;
			for (Node *arg = node->args; arg; arg = arg->next) {
				gen_from_abstructTree(arg);
				nargs++;
			}

			for (int i = nargs - 1; i >= 0; i--)  // 引数は対応するレジスタに保存しておく（最大6個）
				printf("	pop %s\n", argreg[i]);

			// 関数呼び出しの前にRSPが16の倍数になるように調整
			// 8引くことの回答: http://herumi.in.coocan.jp/prog/x64.html
			int seq = labelseq++;
			printf("	mov rax, rsp\n");
			printf("	and rax, 15\n"); // 16の倍数か確認。11111と100000でアンドすると0になる。16の倍数とは、1~5桁目が0である。
			printf("	jnz .Lcall%d\n", seq); // 16の倍数でなかったらジャンプ
			printf("	mov rax, 0\n"); // なぜ0？-> 可変長引数のためらしい...が、よくわからない
			printf("	call %s\n", node->funcname);
			printf("	jmp .Lend%d\n", seq);
			printf(".Lcall%d:\n", seq);
			printf("	sub rsp, 8\n"); // 関数呼び出し直後はリターンアドレス(8byte)がpushされているため、8押し下げることで16の倍数
			printf("	mov rax, 0\n");
			printf("	call %s\n", node->funcname);
			printf("	add rsp, 8\n");
			printf(".Lend%d:\n", seq);
			printf("	push rax\n");
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