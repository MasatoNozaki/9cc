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

	gen();
	return 0;
}