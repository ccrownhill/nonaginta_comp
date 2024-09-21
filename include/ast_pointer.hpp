#ifndef AST_POINTER_HPP
#define AST_POINTER_HPP

#include "ast_node.hpp"

class Pointer : public Node
{
public:
    Pointer(Node *pointer) : pointer_ (pointer) {}
    ~Pointer() {
        if (pointer_ != nullptr)
            delete pointer_;
    }

    void EmitCFG(IR &ir, IRGenContext &ctx) const override;
    void Print(std::ostream &stream) const override;
private:
    Node *pointer_;
};


#endif