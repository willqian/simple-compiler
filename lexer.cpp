#include "lexer.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#if 0
#define LEX_DEBUG(fmt, args...) \
    printf("[%s %d]\t" fmt, __func__, __LINE__, ##args);
#else
#define LEX_DEBUG(fmt, args...) \
    do {} while(0)

#endif

static const char *tokens[] = {
    "if", "then", "else", "while", "do", "begin", "break", "end",
    "and", "or", "not",
    "<=", ">=", "==", "!=",
    "function", "return",
    "<eof>", "<number>", "<name>", "<stmt_list>",
    "<function_call>", "<arg_list>"
};

int Lexer::currIsNewLine()
{
    return _current == '\n' || _current == '\r';
}

void Lexer::incLineNumber()
{
    int old = _current;
    if (!this->currIsNewLine()) {
        printf("ASSERT currIsNewLine error\n");
        exit(1);
    }
    this->next();
    if (this->currIsNewLine() && _current != old) {
        this->next(); /* skip \r\n or \n\r */
    }
    if (++_lineNumber >= MAX_INT) {
        printf("too large line number\n");
        exit(1);
    }
}

void Lexer::next()
{
    _current = _sourceBuf[_sourceIndex++]; 
}

void Lexer::save(int c)
{
    if (_bufIndex >= _bufSize) {
        _bufSize = _bufSize * 2;
        _buf = (char *)realloc((void *)_buf, _bufSize);
    }
     _buf[_bufIndex++] = (char)c;
}

void Lexer::saveAndNext()
{
    this->save(_current);
    this->next();
}

void Lexer::readNumber(SemInfo *semInfo)
{
    this->saveAndNext();
    for (;;) {
        if (isdigit(_current)) {
            this->saveAndNext();
        } else {
            break;
        }
    }
    this->save('\0');
    semInfo->r = atoi(_buf);
}

void Lexer::resetBuf()
{
    _bufIndex = 0;
}

int Lexer::isReserved(char *s)
{
    for (int i = IF; i < EOS; i ++) {
        if (0 == strcmp(tokens[i - IF], s)) {
            return i;
        }
    }
    return -1;
}

int Lexer::lex(SemInfo *semInfo)
{
    this->resetBuf();
    for (;;) {
        switch (_current) {
            case '\n': case '\r':
            {
                this->incLineNumber();
                break;
            }
            case ' ': case '\f': case '\t': case '\v':
            {
                this->next();
                break;
            }
            case '#': /* comment */
            {
                this->next();
            }
            case '=':
            {
                this->next();
                if (_current != '=') {
                    return '=';
                } else {
                    this->next();
                    return EQ;
                }
            }
            case '<':
            {
                this->next();
                if (_current != '=') {
                    return '<';
                } else {
                    this->next();
                    return LE;
                }
            }
            case '>':
            {
                this->next();
                if (_current != '=') {
                    return '>';
                } else {
                    this->next();
                    return GE;
                }
            }
            case '!':
            {
                this->next();
                if (_current != '=') {
                    return '!';
                } else {
                    this->next();
                    return NE;
                }
            }
            case '0': case '1': case '2': case '3': case '4':
            case '5': case '6': case '7': case '8': case '9':
            {
                this->readNumber(semInfo);
                return NUMBER;
            }
            case -1:
            {
                return EOS;
            }
            default:
            {
                if (isalpha(_current)) {
                    int token = -1;
                    do {
                        this->saveAndNext();
                    } while (isalnum(_current));
                    this->save('\0');
                    semInfo->s = (char *)malloc(strlen(_buf) + 1);
                    strncpy(semInfo->s, _buf, strlen(_buf));
                    token = this->isReserved(semInfo->s);
                    if (token > 0){
                        return token;
                    } else {
                        return NAME;
                    }
                } else {
                    int c = _current;
                    this->next();
                    return c;
                }
            }
        }
    }
}

Lexer::Lexer(const char *filename)
{
    int baseSize = 1024;
    int fd = -1;
    int readBytes = -1;
    int totalBytes = 0;

    _current = -1;
    _lineNumber = 1;
    _lastLine = 1;
    _currentToken = {EOS, {-1, NULL}};
    _lookAheadToken = {EOS, {-1, NULL}};

    _buf = (char *)malloc(baseSize);
    memset(_buf, 0, baseSize);
    _bufSize = baseSize;
    _bufIndex = 0;

    _sourceBuf = (char *)malloc(baseSize);
    memset(_sourceBuf, 0, baseSize);
    _sourceBufSize = baseSize;
    _sourceIndex = 0;

    fd = open(filename, O_RDONLY);
    while (1) {
        readBytes = read(fd, _sourceBuf + totalBytes, baseSize);
        if (-1 == readBytes) {
            printf("read error\n");
            exit(1);
        } else {
            totalBytes += readBytes;
            baseSize = baseSize * 2;
            _sourceBufSize = baseSize;
            _sourceBuf = (char *)realloc((void *)_sourceBuf, baseSize);
            _sourceBuf[totalBytes] = -1;
            if (0 == readBytes) {
                break;
            }
        }
    }
    _current = _sourceBuf[0];
    _sourceIndex = 1;
    close(fd);
    for (int i = 0; i < _sourceBufSize; i ++) {
        if (-1 == _sourceBuf[i]) {
            break;
        }
        printf("%c",_sourceBuf[i]);
    }
}

Lexer::~Lexer()
{
    free(_buf);
    free(_sourceBuf);
    _buf = NULL;
    _sourceBuf = NULL;

    if (NULL != _currentToken.semInfo.s) {
        free(_currentToken.semInfo.s);
    }
    if (NULL != _lookAheadToken.semInfo.s) {
        free(_lookAheadToken.semInfo.s);
    }
}

Token Lexer::getCurrentToken()
{
    return _currentToken;
}

int Lexer::getCurrentLineNumber()
{
    return _lineNumber;
}

void Lexer::nextToken()
{
    _lastLine = _lineNumber;
    if (_lookAheadToken.token != EOS) {
        _currentToken = _lookAheadToken;
        _lookAheadToken.token = EOS;
    } else {
        _currentToken.token = this->lex(&_currentToken.semInfo);
    }
    if (_currentToken.token > 256) {
        LEX_DEBUG("token is %s\n", tokens[_currentToken.token - IF]);
    } else {
        LEX_DEBUG("token is %c\n", _currentToken.token);
    }
}

int Lexer::lookAhead()
{
    if (EOS == _lookAheadToken.token) {
        printf("ASSERT lookAheadToken error\n");
        exit(1);
    }
    _lookAheadToken.token = this->lex(&_lookAheadToken.semInfo);
    return _lookAheadToken.token;
}
