#include <stdlib.h>

void compile(void) {
	system("make -C git_src/git > /dev/null 2>&1");
	system("make clean -C git_src/git > /dev/null 2>&1");
}
