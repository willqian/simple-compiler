#include "parser.h"
#include <stdio.h>

int main(int argc, char *argv[])
{
    if (2 != argc) {
        printf("usage: ./complier sourcefile\n");
        return -1;
    }
    Parser *p = new Parser(argv[1]);
    AST* tree = p->parse();
    Token token;
    while (1) {
        token = p->nextToken();
        if (EOS == token.token) {
            break;
        } else if (NUMBER == token.token) {
            printf("number is %d\n", token.semInfo.r);
        } else if (NAME == token.token) {
            printf("symbol name is %s\n", token.semInfo.s);
        }
    }
    return 0;
}
