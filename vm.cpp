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

typedef struct {
    int stack[10];
    int stackIndex;
    Symbol sym[10];
} Proto;

typedef struct {
    char *name;
    AST *tree;
    Symbol argv[10];
    int argc;
    Proto proto;
} Func;


static Func funcTable[10];

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

    if (NULL != tree) {
        if (NUMBER == tree->getRootNode()->getToken()->token) {
            return tree->getRootNode()->getToken()->semInfo.r;
        } else if (NAME == tree->getRootNode()->getToken()->token) {
            printf("NAME\n");
            for (int i = 0; i < SYMBOL_NUMBER; i ++) {
                if (NULL != sym[i].name 
                        && 0 == strcmp(sym[i].name, tree->getRootNode()->getToken()->semInfo.s)) {
                    printf("return %d\n", sym[i].number);
                    return sym[i].number;
                }
            }
            return 0;
        }

        switch (tree->getRootNode()->getToken()->token) {
        case '+':
            lv = this->exe(tree->getLeft());
            rv = this->exe(tree->getRight());
            val = lv + rv;
            break;
        case '-':
            lv = this->exe(tree->getLeft());
            rv = this->exe(tree->getRight());
            val = lv - rv;
            break;
        case '*':
            lv = this->exe(tree->getLeft());
            rv = this->exe(tree->getRight());
            val = lv * rv;
            break;
        case '/':
            lv = this->exe(tree->getLeft());
            rv = this->exe(tree->getRight());
            val = lv / rv;
            break;
        case '<':
            lv = this->exe(tree->getLeft());
            rv = this->exe(tree->getRight());
            val = lv < rv;
            break;
        case '>':
            lv = this->exe(tree->getLeft());
            rv = this->exe(tree->getRight());
            val = lv > rv;
            break;
        case EQ:
            lv = this->exe(tree->getLeft());
            rv = this->exe(tree->getRight());
            val = lv == rv;
            break;
        case NE:
            lv = this->exe(tree->getLeft());
            rv = this->exe(tree->getRight());
            val = lv != rv;
            break;
        case OR:
            lv = this->exe(tree->getLeft());
            rv = this->exe(tree->getRight());
            val = lv || rv;
            break;
        case AND:
            lv = this->exe(tree->getLeft());
            rv = this->exe(tree->getRight());
            val = lv && rv;
            break;
        case '=':
        {
            lv = this->exe(tree->getLeft());
            rv = this->exe(tree->getRight());
            for (int i = 0; i < SYMBOL_NUMBER; i ++) {
                if (NULL != sym[i].name 
                        && 0 == strcmp(sym[i].name, tree->getLeft()->getRootNode()->getToken()->semInfo.s)) {
                    sym[i].number = rv;
                    printf("name [%s] reassign [%d]\n", sym[i].name, sym[i].number);
                    return 0;
                }
            }
            for (int i = 0; i < SYMBOL_NUMBER; i ++) {
                if (NULL == sym[i].name) {
                    sym[i].name = strdup(tree->getLeft()->getRootNode()->getToken()->semInfo.s);
                    sym[i].number = rv;
                    printf("name [%s] = [%d]\n", sym[i].name, sym[i].number);
                    return 0;
                }
            }
        }
        case IF:
        {
            lv = this->exe(tree->getLeft());
            if (0 != lv) {
                this->exe(tree->getRight());
            } else {
                this->exe(tree->getThird());
            }
            break;
        }
        case WHILE:
        {
            while (0 != this->exe(tree->getLeft())) {
                this->exe(tree->getRight());
            }
            break;
        }
        default:
            lv = this->exe(tree->getLeft());
            rv = this->exe(tree->getRight());
            break;
        }
    }
    return val;
}

void VM::execute()
{
    this->exe(_tree);
}
