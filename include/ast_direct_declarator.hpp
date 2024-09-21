#ifndef AST_DIRECT_DECLARATOR_HPP
#define AST_DIRECT_DECLARATOR_HPP

#include "ast_node.hpp"

enum class DirDeclType {var, fn, arr};

class DirectDeclarator : public Node
{
public:
    DirectDeclarator(DirDeclType decl_type, Node *identifier, NodeList *param_list, ExprNode *expr)
        : decl_type_ (decl_type), identifier_ (identifier), param_list_(param_list), expr_(expr)
    {std::cout << "direct declarator of type: " << ((decl_type == DirDeclType::fn) ? "fn" : "var") << std::endl;}
    ~DirectDeclarator()
    {
        delete identifier_;
        if (param_list_ != nullptr)
            delete param_list_;
    }
    void EmitCFG(IR &ir, IRGenContext &ctx) const override;
    void Print(std::ostream &stream) const override;

    std::string GetName();
private:
    DirDeclType decl_type_;
    Node *identifier_;
    NodeList *param_list_;
    ExprNode *expr_;
};

#endif
