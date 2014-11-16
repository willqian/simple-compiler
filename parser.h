#ifndef parser_h
#define parser_h

#include "common.h"
#include "lexer.h"
#include "AST.h"

class Parser {

private:
    Lexer *_lexer;
    Token _currentToken;
    void (*_func)(char *, AST *, SemInfo *);

    void check(int c);
    void funcGetterCallback(char *name, AST *tree, SemInfo *infos);
    AST* E();
    AST* T();
    AST* F();
    AST* boolv();
    AST* T2();
    AST* F2();
    AST* stmtIf();
    AST* stmtWhile();
    AST* assignment();
    AST* argList();
    AST* argDeclareList();
    AST* funCall(Token id);
    AST* stmtFunc();
    AST* stmtReturn();
    AST* statement();
    AST* stmtList();

public:
    Parser(const char *source);
    ~Parser();
    Token nextToken();
    AST* parse();
    void registerFuncGetter(void (*func)(char *name, AST *tree, SemInfo *argv));
};

#endif /* parser_h */
