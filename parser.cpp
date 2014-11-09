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
 * stmt_list -> stmt | stmt_list
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
        this->nextToken();
        tree = this->assignment();
        if (NULL != tree) {
            Token id = {NAME, {-1, NULL}};
            tree->addLeft(id);
        }
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
