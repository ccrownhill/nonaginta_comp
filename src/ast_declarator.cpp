#include "ast_declarator.hpp"
#include "ast_direct_declarator.hpp"

void Declarator::EmitCFG(IR &ir, IRGenContext &ctx) const
{
    if (pointer_ != nullptr) {
        pointer_->EmitCFG(ir, ctx);
    }
    dir_decl_->EmitCFG(ir, ctx);
}
void Declarator::Print(std::ostream &stream) const
{
    if (pointer_ != nullptr) {
        pointer_->Print(stream);
    }
    dir_decl_->Print(stream);
}

std::string Declarator::get_name()
{
    return static_cast<DirectDeclarator *>(dir_decl_)->GetName();
}
