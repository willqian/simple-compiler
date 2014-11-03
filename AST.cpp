#include "AST.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

AST::AST(Token t)
{
    _root = new ASTNode(t);
}

AST::AST(ASTNode *n)
{
    this->setRootNode(n);
}

AST::AST(AST *tree)
{
    this->setRootNode(tree->getRootNode());
}

AST::AST()
{
    this->setRootNode();
}

AST::~AST()
{
    if (NULL != _root) {
        delete _root;
    }
}

void AST::setRootNode(Token t)
{
    _root = new ASTNode(t);
}

void AST::setRootNode(ASTNode *n)
{
    if (NULL == n) {
        _root = new ASTNode();
    } else {
        _root = n;
    }
}

void AST::setRootNode()
{
    _root = new ASTNode();
}

void AST::addLeft(Token t)
{
    _root->_left = new ASTNode(t);
}

void AST::addLeft(AST *tree)
{
    if (NULL == tree) {
        return;
    }
    _root->_left = tree->getRootNode();
}

void AST::addRight(Token t)
{
    _root->_right = new ASTNode(t);
}

void AST::addRight(AST *tree)
{
    if (NULL == tree) {
        return;
    }
    _root->_right = tree->getRootNode();
}

void AST::addThird(Token t)
{
    _root->_third = new ASTNode(t);
}

void AST::addThird(AST *tree)
{
    if (NULL == tree) {
        return;
    }
    _root->_third = tree->getRootNode();
}

ASTNode* AST::getRootNode()
{
    return _root;
}

AST* AST::getLeft()
{
    if (NULL == _root->getLeft()) {
        return NULL;
    }
    return new AST(_root->getLeft());
}

AST* AST::getRight()
{
    if (NULL == _root->getRight()) {
        return NULL;
    }
    return new AST(_root->getRight());
}
AST* AST::getThird()
{
    if (NULL == _root->getThird()) {
        return NULL;
    }
    return new AST(_root->getThird());
}
