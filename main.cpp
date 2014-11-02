#include "lexer.h"
#include <stdio.h>

int main(int argc, char *argv[])
{
    if (2 != argc) {
        printf("usage: ./complier sourcefile\n");
        return -1;
    }
    Lexer *l = new Lexer(argv[1]);
    int token;
    int lineNumber;
    while (1) {
        l->nextToken();
        token = l->getCurrentToken().token;
        lineNumber = l->getCurrentLineNumber();
        if (EOS == token) {
            break;
        } else if (NUMBER == token) {
            printf("number is %d\n", l->getCurrentToken().semInfo.r);
        } else if (NAME == token) {
            printf("symbol name is %s\n", l->getCurrentToken().semInfo.s);
        }
    }
    return 0;
}
