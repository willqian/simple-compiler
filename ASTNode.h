#ifndef ast_node_h
#define ast_node_h

#include "common.h"

class ASTNode {

private:
    Token _token;
    ASTNode *_left, *_right, *_third;

public:
    ASTNode();
    ASTNode(Token t);
    ~ASTNode();

    void setToken(Token t);
    Token *getToken();

    void addLeft(Token t);
    void addLeft(ASTNode *n);
    void addRight(Token t);
    void addRight(ASTNode *n);
    void addThird(Token t);
    void addThird(ASTNode *n);

    ASTNode* getRootNode();
    ASTNode* getLeft();
    ASTNode* getRight();
    ASTNode* getThird();
};

#endif /* ast_node_h */
