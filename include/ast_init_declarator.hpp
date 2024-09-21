#ifndef AST_INIT_DECLARATOR_HPP
#define AST_INIT_DECLARATOR_HPP

#include "ast_node.hpp"

class InitDeclarator : public Node
{
public:
    InitDeclarator(Node *declarator, ExprNode *assignment_expr)
        : declarator_(declarator),
          assignment_expr_(assignment_expr)
    {std::cout << "new InitDeclarator '" << assignment_expr << "'" << std::endl;}

    ~InitDeclarator()
    {
        delete declarator_;
        if (assignment_expr_ != nullptr)
            delete assignment_expr_;
    }

    void EmitCFG(IR &ir, IRGenContext &ctx) const override;
    void Print(std::ostream &stream) const override;

private:
    Node *declarator_;
    ExprNode *assignment_expr_;
};

#endif