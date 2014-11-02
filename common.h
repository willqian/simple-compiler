#ifndef common_h
#define common_h

#define MAX_INT 10000

typedef struct {
    int r;
    char *s;
} SemInfo;

typedef struct {
    int token;
    SemInfo semInfo;
} Token;

#endif /* common_h */
