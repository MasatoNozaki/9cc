#include <stdlib.h>
#include "common.h"
#include "tokenize.h"
#include "generator.h"

int main(int argc, char **argv) {
	if (argc != 2) {
		error("引数の個数が正しくありません");
		return 1;
	}

	 token = tokenize(argv[1]);

	gen();
	return 0;
}