#ifndef vm_h
#define vm_h

#include "AST.h"

class VM {

private:
    AST *_tree;
    int exe(AST *tree);

public:
    VM(AST *tree);
    ~VM();
    void execute();
};

#endif /* vm_h */
