#ifndef AST_DECLARATOR_HPP
#define AST_DECLARATOR_HPP

#include "ast_node.hpp"

class Declarator : public Node
{
public:
    Declarator(Node *pointer, Node *dir_decl)
        : pointer_ (pointer), dir_decl_ (dir_decl)
    {}
    ~Declarator()
    {
        if (pointer_ != nullptr)
            delete pointer_;

        delete dir_decl_;
    }
    void EmitCFG(IR &ir, IRGenContext &ctx) const override;
    void Print(std::ostream &stream) const override;

    std::string get_name();
private:
    Node *pointer_;
    Node *dir_decl_;
};

#endif