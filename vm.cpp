#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "lexer.h"
#include "vm.h"

typedef struct {
    char *name;
    int number;
} Symbol;

#define SYMBOL_NUMBER 10
static Symbol sym[SYMBOL_NUMBER];

VM::VM(AST *tree)
{
    _tree = tree;
    memset(sym, 0, sizeof(Symbol) * SYMBOL_NUMBER);
}

VM::~VM()
{
    _tree = NULL;
}

int VM::exe(AST *tree)
{
    int lv = 0;
    int rv = 0;
    int val = 0;

    printf("exe\n");
    if (NULL != tree) {
        if (NUMBER == tree->getRootNode()->getToken()->token) {
            return tree->getRootNode()->getToken()->semInfo.r;
        }
        lv = this->exe(tree->getLeft());
        rv = this->exe(tree->getRight());

        switch (tree->getRootNode()->getToken()->token) {
        case '+':
            val = lv + rv;
            break;
        case '-':
            val = lv - rv;
            break;
        case '*':
            val = lv * rv;
            break;
        case '/':
            val = lv / rv;
            break;
        case '=':
        {
            printf("=\n");
            for (int i = 0; i < SYMBOL_NUMBER; i ++) {
                if (NULL == sym[i].name) {
                    sym[i].name = strdup(tree->getLeft()->getRootNode()->getToken()->semInfo.s);
                    sym[i].number = rv;
                    printf("name [%s] = [%d]\n", sym[i].name, sym[i].number);
                    break;
                }
            }
        }

        default:
            break;
        }
    }
    printf("return %d\n", val);
    return val;
}

void VM::execute()
{
    this->exe(_tree);
}
