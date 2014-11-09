#ifndef parser_h
#define parser_h

#include "common.h"
#include "lexer.h"
#include "AST.h"

class Parser {

private:
    Lexer *_lexer;
    Token _currentToken;

    void check(int c);
    AST* E();
    AST* T();
    AST* F();
    AST* assignment();
    AST* statement();
    AST* stmtList();

public:
    Parser(const char *source);
    ~Parser();
    Token nextToken();
    AST* parse();
};

#endif /* parser_h */
