#ifndef lexer_h
#define lexer_h

#include "common.h"

#define FIRST_RESERVED 257

typedef enum {
    IF = FIRST_RESERVED, THEN, ELSE, WHILE, DO, BEGIN, BREAK, END,
    AND, OR, NOT,
    LE, GE, EQ, NE,
    FUNCTION, RETURN,
    EOS, NUMBER, NAME, STMT_LIST
} Reserved;

class Lexer {

private:
    int _current;
    int _lineNumber;
    int _lastLine;
    Token _currentToken;
    Token _lookAheadToken;
    
    char *_buf;
    int _bufSize;
    int _bufIndex;

    char *_sourceBuf;
    int _sourceBufSize;
    int _sourceIndex;

    void next();
    int currIsNewLine();
    void save(int c);
    void saveAndNext();
    void readNumber(SemInfo *semInfo);
    int lex(SemInfo *semInfo);
    void incLineNumber();
    void resetBuf();
    int isReserved(char *s);

public:
    Lexer(const char *filename);
    ~Lexer();

    Token getCurrentToken();
    int getCurrentLineNumber();

    void nextToken();
    int lookAhead();
};

#endif /* lexer_h */
