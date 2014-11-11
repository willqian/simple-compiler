#include "parser.h"
#include "vm.h"
#include <stdio.h>

int main(int argc, char *argv[])
{
    if (2 != argc) {
        printf("usage: ./complier sourcefile\n");
        return -1;
    }
    Parser *p = new Parser(argv[1]);
    AST* tree = p->parse();
    VM *vm = new VM(tree);
    vm->execute();
    return 0;
}
