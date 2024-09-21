#ifndef AST_TYPE_SPECIFIER
#define AST_TYPE_SPECIFIER

#include "ast_node.hpp"

class TypeSpecifier : public Node
{
public:
    TypeSpecifier(std::string type) : type_(type){};
    ~TypeSpecifier(){};
    void EmitCFG(IR &ir, IRGenContext &ctx) const override;
    void Print(std::ostream &stream) const override;
private:
    std::string type_;
};

#endif
