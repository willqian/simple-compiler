#ifndef ast_h
#define ast_h

#include "common.h"
#include "ASTNode.h"

class AST {

private:
    ASTNode *_root;

public:
    AST(Token t);
    AST(ASTNode *n);
    AST(AST *tree);
    AST();
    ~AST();

    void setRootNode();
    void setRootNode(Token t);
    void setRootNode(ASTNode *n);

    void addLeft(Token t);
    void addLeft(AST *tree);
    void addRight(Token t);
    void addRight(AST *tree);
    void addThird(Token t);
    void addThird(AST *tree);

    ASTNode* getRootNode();
    AST* getLeft();
    AST* getRight();
    AST* getThird();
};

#endif /* ast_h */
