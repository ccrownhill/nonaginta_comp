#ifndef AST_FUNCTION_DEFINITION_HPP
#define AST_FUNCTION_DEFINITION_HPP

#include "ast_node.hpp"

class FunctionDefinition : public Node
{
public:
    FunctionDefinition(Node *declaration_specifiers, Node *declarator, Node *compound_statement)
        : declaration_specifiers_(declaration_specifiers),
          declarator_(declarator),
          compound_statement_(compound_statement)
    {std::cout << "function definition" << std::endl;}

    ~FunctionDefinition()
    {
        delete declaration_specifiers_;
        delete declarator_;
        delete compound_statement_;
    };
    void EmitCFG(IR &ir, IRGenContext &ctx) const override;
    void Print(std::ostream &stream) const override;
private:
    Node *declaration_specifiers_;
    Node *declarator_;
    Node *compound_statement_;
};

#endif
