#include "ast_declaration.hpp"

void Declaration::EmitCFG(IR &ir, IRGenContext &ctx) const
{
    declaration_specifier_->EmitCFG(ir, ctx);
    init_declarator_list_->EmitCFG(ir, ctx);
}

void Declaration::Print(std::ostream &stream) const
{
    std::cout << "before printint specifiers" << std::endl;
    declaration_specifier_->Print(stream);
    stream << " ";
    std::cout << "before printint init declarator" << std::endl;
    init_declarator_list_->Print(stream);
    std::cout << "after printing" << std::endl;
    stream << ";" << std::endl;
}