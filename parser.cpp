#include "parser.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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
 * exp -> bool
 */

/*
 * while exp do block end
 * exp -> bool
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

/*
 * function name(arg1, arg2 ...)
 *     return i;
 * end
 */

/*
 * name(i1, i2, ...)
 */

Parser::Parser(const char *source)
{
    _lexer = new Lexer(source);
    _currentToken = {EOS, {-1, NULL}};
    _func = NULL;
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
    } else if (NAME == _currentToken.token) {
        printf("name is %s\n", _currentToken.semInfo.s);
        Token t = _currentToken;
        this->nextToken();
        if ('(' == _currentToken.token) {
            tree = this->funCall(t);
        } else {
            tree = new AST(t);
        }
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

AST* Parser::stmtWhile()
{
    AST *tree = new AST(_currentToken);
    this->nextToken();
    AST *bv = this->boolv();
    AST *tmp = NULL;

    if (NULL == bv) {
        printf("error bool\n");
        exit(1);
    }
    tree->addLeft(bv);

    if (DO != _currentToken.token) {
        printf("no do\n");
        exit(1);
    }
    this->nextToken();
    tmp = this->stmtList();

    if (NULL == tmp) {
        printf("error while block\n");
        exit(1);
    }
    tree->addRight(tmp);
    
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
    printf("assignment DONE\n");
    return tree;
}

void Parser::funcGetterCallback(char *name, AST *tree, SemInfo *infos)
{
    if (NULL == _func) {
        return;
    }
    _func(name, tree, infos);
}

AST* Parser::argList()
{
    AST *argList = NULL;
    AST *tmp = NULL;
    Token argl = {ARG_LIST, {-1, NULL}};

    printf("arg list create\n");

    this->nextToken();
    if (')' == _currentToken.token) {
        this->nextToken();
        return NULL;
    } 
    argList = new AST(argl);
    if (NAME != _currentToken.token 
         && NUMBER != _currentToken.token) {
        
        printf("error function call, no argv\n");
        exit(1);
    }
    tmp = new AST(_currentToken);
    argList->addLeft(tmp);
    
    this->nextToken();
    if (')' == _currentToken.token) {
        this->nextToken();
        return argList;
    }
    if (',' != _currentToken.token) {
        printf("error function statment, no ,\n");
        exit(1);
    }
    argList->addRight(this->argList());
    return argList;
}

AST* Parser::argDeclareList()
{
    AST *argList = NULL;
    AST *tmp = NULL;
    Token argl = {ARG_LIST, {-1, NULL}};

    printf("arg list create\n");

    this->nextToken();
    if (')' == _currentToken.token) {
        this->nextToken();
        return NULL;
    } 
    argList = new AST(argl);
    if (NAME != _currentToken.token) {
        
        printf("error function call, no argv\n");
        exit(1);
    }
    tmp = new AST(_currentToken);
    argList->addLeft(tmp);
    
    this->nextToken();
    if (')' == _currentToken.token) {
        this->nextToken();
        return argList;
    }
    if (',' != _currentToken.token) {
        printf("error function statment, no ,\n");
        exit(1);
    }
    argList->addRight(this->argDeclareList());
    return argList;
}
AST* Parser::funCall(Token id)
{
    AST *tree = NULL;
    Token token = {FUNCALL, {-1, NULL}};

    token.semInfo.s = strdup(id.semInfo.s);
    printf("function %s call\n", token.semInfo.s);
    tree = new AST(token);
    tree->addRight(this->argList());
    return tree;
}

AST* Parser::stmtFunc()
{
    AST *tree = NULL;
    AST *tmp = NULL;

    printf("ENTER function statement\n");
    tree = new AST(_currentToken);
    this->nextToken();
    if (NAME != _currentToken.token) {
        printf("error function statement\n");
        exit(1);
    }
    tmp = new AST(_currentToken);
    tree->addLeft(tmp);
    printf("function name is %s\n", _currentToken.semInfo.s);

    this->nextToken();

    if ('(' != _currentToken.token) {
        printf("function no (\n");
        exit(1);
    }

    tree->addRight(this->argDeclareList());
    
    tmp = this->stmtList();

    if (NULL != tmp) {
        tree->addThird(tmp);
    }

    if (END != _currentToken.token) {
        printf("function no end\n");
        exit(1);
    }
    this->nextToken();
    return tree;
}

AST* Parser::stmtReturn()
{
    AST *tree = NULL;
    AST *tmp = NULL;

    printf("RETURN statement\n");
    tree = new AST(_currentToken);
    this->nextToken();
    tmp = this->E();
    if (NULL != tmp) {
        tree->addLeft(tmp);
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
        } else if ('(' == _currentToken.token) {
            printf("funcall\n");
            tree = this->funCall(id); 
        }
        break;
    }
    case IF:
        tree = this->stmtIf();
        break;
    case WHILE:
        tree = this->stmtWhile();
        break;
    case FUNCTION:
        tree = this->stmtFunc();
        break;
    case RETURN:
        tree = this->stmtReturn();
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

void Parser::registerFuncGetter(void (*func)(char *, AST *, SemInfo *))
{
    _func = func;
}
