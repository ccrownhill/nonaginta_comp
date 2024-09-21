#include "ast_identifier.hpp"

void Identifier::EmitCFG(IR &ir, IRGenContext &ctx) const
{
    // if (context.IsGlobal()) {
    //     stream << identifier_;
    // }
}

void Identifier::Print(std::ostream &stream) const
{
    stream << identifier_;
};

std::string Identifier::GetName()
{
    return identifier_;
}
