#include "ast_struct_declaration.hpp"
#include "ast_declarator.hpp"

void StructDeclaration::EmitCFG(IR &ir, IRGenContext &ctx) const
{
    declaration_specifier_->EmitCFG(ir, ctx);
    

    if (declarator_list_ != nullptr) {
        declarator_list_->EmitCFG(ir, ctx);
    }
}

void StructDeclaration::Print(std::ostream &stream) const
{
    declaration_specifier_->Print(stream);
    stream << " ";
    if (declarator_list_ != nullptr) {
        declarator_list_->Print(stream);
    }
    stream << ";";
}