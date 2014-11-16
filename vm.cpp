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
static Symbol syms[SYMBOL_NUMBER];
static Symbol *sym;

typedef struct {
    Symbol syms[10];
    int symIndex;
} Proto;

typedef struct {
    char *name;
    AST *tree;
    Symbol argv[10];
    int argc;
    Proto proto;
} Func;

static int ret = 0;
static int status[10] = {-1}; /* 0-9 in function */
static int statusIndex = 1;
static Func funcTable[10];
static Func funcEnv[10];

VM::VM(AST *tree)
{
    _tree = tree;
    memset(syms, 0, sizeof(Symbol) * SYMBOL_NUMBER);
    memset(funcTable, 0, sizeof(Func) * 10);
    sym = syms;
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
        case FUNCTION:
        {
            int i = 0;
            AST* tmp = NULL;
            printf("coming function \n");
            for (i = 0; i < 10; i ++) {
                if (NULL == funcTable[i].name) {
                    break;
                }
            }
            if (i >= 10) {
                printf("function is more than 10\n");
                exit(1);
            }
            funcTable[i].name = strdup(tree->getLeft()->getRootNode()->getToken()->semInfo.s);
            funcTable[i].tree = tree->getThird();
            tmp = tree;
            while (1) {
                if (NULL == tmp->getRight()) {
                    break;
                }
                tmp = tmp->getRight();
                funcTable[i].argv[funcTable[i].argc].name = strdup(tmp->getLeft()->getRootNode()->getToken()->semInfo.s);
                printf("argument is %s\n", funcTable[i].argv[funcTable[i].argc].name);
                funcTable[i].argc ++;
            }
            printf("out argument\n");
            break;
        }
        case FUNCALL:
        {
            int i = 0;
            AST *tmp = NULL;
            Proto *proto = NULL;
            int index = 0;
            printf("coming function call \n");
            for (i = 0; i < 10; i ++) {
                if (NULL != funcTable[i].name
                        && 0 == strcmp(funcTable[i].name, tree->getRootNode()->getToken()->semInfo.s)) {
                    printf("find the function %s\n", funcTable[i].name);
                    break;
                }
            }
            status[statusIndex] = i;
            statusIndex ++;
            if (statusIndex >= 10) {
                printf("status stack overflow\n");
                exit(1);
            }
            memcpy(&funcEnv[i], &(funcTable[i]), sizeof(Func));

            tmp = tree;
            proto = &funcEnv[i].proto;
            /* copy argument */
            while (1) {
                if (NULL == tmp->getRight()) {
                    break;
                }
                tmp = tmp->getRight();
                proto->syms[proto->symIndex].number = this->exe(tmp->getLeft());
                printf("function call argument is %d\n", proto->syms[proto->symIndex].number);
                proto->syms[proto->symIndex].name = strdup(funcEnv[i].argv[index].name);
                index ++;
                funcEnv[i].proto.symIndex ++;
                if (index > funcEnv[i].argc) {
                    printf("error argument\n");
                    exit(1);
                }
            }

            sym = funcEnv[i].proto.syms;
            this->exe(funcEnv[i].tree);
            val = ret;
            statusIndex --;
            if (statusIndex < 1) {
                printf("status stack is too low\n");
                exit(1);
            }
            if (status[statusIndex - 1] >= 0) {
                sym = funcEnv[status[statusIndex] - 1].proto.syms;
            } else {
                sym = syms;
            }
            break;
        }
        case RETURN:
        {
            ret = this->exe(tree->getLeft());
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
