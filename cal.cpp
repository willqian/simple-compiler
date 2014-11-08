#include "parser.h"
#include <stdio.h>

int calc(AST *tree)
{
    int a = 0;
    int b = 0;
    int val = 0;

    if (NULL != tree) {
        if (NUMBER == tree->getRootNode()->getToken()->token) {
            return tree->getRootNode()->getToken()->semInfo.r;
        }
        a = calc(tree->getLeft());
        b = calc(tree->getRight());

        switch (tree->getRootNode()->getToken()->token) {
        case '+':
            val = a + b;
            break;
        case '-':
            val = a - b;
            break;
        case '*':
            val = a * b;
            break;
        case '/':
            val = a / b;
            break;
        default:
            break;
        }
    }
    printf("return %d\n", val);
    return val;
}

int main(int argc, char *argv[])
{
    if (2 != argc) {
        printf("usage: ./complier sourcefile\n");
        return -1;
    }
    Parser *p = new Parser(argv[1]);
    AST* tree = p->parse();
    printf("result is %d\n", calc(tree->getRight()));
    return 0;
}
