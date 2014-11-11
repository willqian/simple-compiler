#include "parser.h"
#include <stdio.h>
#include <stdlib.h>

/*
 * E -> T | E A T
 * T -> F | T M F
 * F -> (E) | i 
 * A -> + | - 
 * M -> * | /
 *
 */

/*
 * if exp then block {elseif exp then block } else block end
 * exp -> 0 | 1
 */

/*
 * bool -> T2 | bool A1 T2
 * T2 -> F2 | T2 M1 F2
 * F2 -> E | F2 N1 E
 * A1 -> or
 * M1 -> and
 * N1 -> < | > | == | !=
 *
 */

/*
 * stmt_list -> stmt | stmt_list stmt_list | stmt_list ; stmt_list | e
 */

Parser::Parser(const char *source)
{
    _lexer = new Lexer(source);
    _currentToken = {EOS, {-1, NULL}};
}

Parser::~Parser()
{
    delete _lexer;
    if (NULL != _currentToken.semInfo.s) {
        free(_currentToken.semInfo.s);
    }
}

void Parser::check(int c)
{
    if (c != _currentToken.token) {
        printf("check [%d] error\n", c);
        exit(1);
    }
    this->nextToken();
}

Token Parser::nextToken()
{
    _lexer->nextToken();
    _currentToken = _lexer->getCurrentToken();
    return _currentToken;
}

AST* Parser::F()
{
    printf("F\n");

    AST *tree = NULL;

    if ('(' == _currentToken.token) {
        printf("enter (\n");
        this->nextToken();
        tree = this->E();
        if (')' == _currentToken.token) {
            printf("enter )\n");
            this->nextToken();
        } else {
            printf("( and ) error\n");
            exit(1);
        }
    } else if (NUMBER == _currentToken.token) {
        printf("number is %d\n", _currentToken.semInfo.r);
        tree = new AST(_currentToken);
        this->nextToken();
    } else {
        printf("F error\n");
        exit(1);
    }
    return tree;
}

AST* Parser::T()
{
    printf("T\n");

    AST *tmp = NULL;
    AST *tree = this->F();

    if (NULL == tree) {
        return NULL;
    }

    while ('*' == _currentToken.token || '/' == _currentToken.token) {
        tmp = new AST(_currentToken);
        tmp->addLeft(tree);
        tree = tmp;
        this->nextToken();

        tmp = this->F();
        if (NULL != tmp) {
            tree->addRight(tmp);
        } else {
            printf("T error\n");
            exit(1);
        }
    } 
    return tree;
}

AST* Parser::E()
{
    printf("E\n");

    AST *tmp = NULL;
    AST *tree = this->T();

    if (NULL == tree) {
        return NULL;
    }

    while ('+' == _currentToken.token || '-' == _currentToken.token) {
        tmp = new AST(_currentToken);
        tmp->addLeft(tree);
        tree = tmp;
        this->nextToken();

        tmp = this->T();
        if (NULL != tmp) {
            tree->addRight(tmp);
        } else {
            printf("E error\n");
            exit(1);
        }
    } 
    return tree;
}

AST* Parser::boolv()
{
    printf("boolv\n");

    AST *tmp = NULL;
    AST *tree = this->T2();

    if (NULL == tree) {
        return NULL;
    }

    while (OR == _currentToken.token) {
        tmp = new AST(_currentToken);
        tmp->addLeft(tree);
        tree = tmp;
        this->nextToken();

        tmp = this->T2();
        if (NULL != tmp) {
            tree->addRight(tmp);
        } else {
            printf("boolv error\n");
            exit(1);
        }
    } 
    return tree;
}

AST* Parser::T2()
{
    printf("T2\n");

    AST *tmp = NULL;
    AST *tree = this->F2();

    if (NULL == tree) {
        return NULL;
    }

    while (AND == _currentToken.token) {
        tmp = new AST(_currentToken);
        tmp->addLeft(tree);
        tree = tmp;
        this->nextToken();

        tmp = this->F2();
        if (NULL != tmp) {
            tree->addRight(tmp);
        } else {
            printf("T2 error\n");
            exit(1);
        }
    } 
    return tree;
}

AST* Parser::F2()
{
    printf("F2\n");

    AST *tmp = NULL;
    AST *tree = this->E();

    if (NULL == tree) {
        return NULL;
    }

    while ('<' == _currentToken.token || '>' == _currentToken.token 
            || EQ == _currentToken.token || NE == _currentToken.token) {
        tmp = new AST(_currentToken);
        tmp->addLeft(tree);
        tree = tmp;
        this->nextToken();

        tmp = this->E();
        if (NULL != tmp) {
            tree->addRight(tmp);
        } else {
            printf("F2 error\n");
            exit(1);
        }
    } 
    return tree;
}

AST* Parser::stmtIf()
{
    AST *tree = new AST(_currentToken);
    this->nextToken();
    AST *bv = this->boolv();
    AST *tmp = NULL;
    AST *tmp2 = NULL;

    if (NULL == bv) {
        printf("error bool\n");
        exit(1);
    }
    tree->addLeft(bv);

    if (THEN != _currentToken.token) {
        printf("no then\n");
        exit(1);
    }
    this->nextToken();
    tmp = this->stmtList();

    if (NULL == tmp) {
        printf("error if block\n");
        exit(1);
    }
    tree->addRight(tmp);
    
    if (ELSE == _currentToken.token) {
        this->nextToken();
        tmp2 = this->stmtList();
        
        if (NULL == tmp2) {
            printf("error else block\n");
            exit(1);
        } 
        tree->addThird(tmp2);
    }

    if (END != _currentToken.token) {
        printf("no end\n");
        exit(1);
    }
    this->nextToken();
    return tree;
}

AST* Parser::assignment()
{
    printf("assignment current token %d\n", _currentToken.token);

    AST *tree = NULL;

    if ('=' == _currentToken.token) {
        Token assign = _currentToken;
        this->nextToken();
        AST *tmp = this->E();
        if (NULL != tmp) {
            tree = new AST(assign);
            tree->addRight(tmp); 
        }
    } else {
        printf("assignment do nothing\n");
        return NULL;
    }
    return tree;
}

AST* Parser::statement()
{
    AST *tree = NULL;

    printf("statement current token %d\n", _currentToken.token);
    switch (_currentToken.token) {
    case EOS:
        return NULL;
    case NAME:
    {
        Token id = _currentToken;
        this->nextToken();
        tree = this->assignment();
        if (NULL != tree) {
            tree->addLeft(id);
        }
        break;
    }
    case IF:
        tree = this->stmtIf();
        break;
    default:
        break;
    }
    return tree;
}

AST* Parser::stmtList()
{
    printf("statement list\n");

    AST *tmp = NULL;
    AST *tree = this->statement();
    Token t = {STMT_LIST, {-1, NULL}};

    if (NULL == tree) {
        printf("empty\n");
        return NULL;
    }

    while (';' == _currentToken.token) {
        this->nextToken();
    }

    tmp = new AST(t);
    tmp->addLeft(tree);
    tree = tmp;

    tmp = this->stmtList();
    tree->addRight(tmp);
    
    return tree;
}

AST* Parser::parse()
{
    AST *tree = NULL;
    this->nextToken();
    tree = this->stmtList();
    return tree;
}
