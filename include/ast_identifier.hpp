#ifndef AST_IDENTIFIER_HPP
#define AST_IDENTIFIER_HPP

#include "ast_node.hpp"

class Identifier : public Node
{
public:
    Identifier(std::string identifier) : identifier_(identifier){};
    ~Identifier(){};
    void EmitCFG(IR &ir, IRGenContext &ctx) const override;
    void Print(std::ostream &stream) const override;

    std::string GetName();
private:
    std::string identifier_;
};

#endif
