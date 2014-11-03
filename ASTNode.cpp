#include "ASTNode.h"
#include "lexer.h"
#include <stdio.h>
#include <stdlib.h>

ASTNode::ASTNode()
{
    _token = {EOS, {-1, NULL}}; 
    _left = _right = _third = NULL;   
}

ASTNode::ASTNode(Token t)
{
    _token = t;
    _left = _right = _third = NULL;   
}

ASTNode::~ASTNode()
{
    if (NULL != _token.semInfo.s) {
        free(_token.semInfo.s);
        _token.semInfo.s = NULL;
    }
    _token.token = EOS;
    _token.semInfo.r = -1;

    if (NULL != _left) {
        delete _left;
    }
    if (NULL != _right) {
        delete _right;
    }
    if (NULL != _third) {
        delete _third;
    }
}

void ASTNode::setToken(Token t)
{
    _token = t;
}

Token *ASTNode::getToken()
{
    return &_token;
}

void ASTNode::addLeft(Token t)
{
    _left = new ASTNode(t);
}

void ASTNode::addLeft(ASTNode *n)
{
    _left = n;
}

void ASTNode::addRight(Token t)
{
    _right = new ASTNode(t);
}

void ASTNode::addRight(ASTNode *n)
{
    _right = n;
}

void ASTNode::addThird(Token t)
{
    _third = new ASTNode(t);
}

void ASTNode::addThird(ASTNode *n)
{
    _third = n;
}

ASTNode* ASTNode::getRootNode()
{
    return this;
}

ASTNode* ASTNode::getLeft()
{
    return _left;
}

ASTNode* ASTNode::getRight()
{
    return _right;
}

ASTNode* ASTNode::getThird()
{
    return _third;
}
