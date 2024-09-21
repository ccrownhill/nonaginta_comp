#include "ast_typedef_declaration.hpp"

void TypedefDeclaration::EmitCFG(IR &ir, IRGenContext &ctx) const
{
    declaration_specifier_->EmitCFG(ir, ctx);
    ctx.start_eval_typedef();
    init_declarator_list_->EmitCFG(ir, ctx);
    ctx.stop_eval_typedef();
}

void TypedefDeclaration::Print(std::ostream &stream) const
{
    std::cout << "before printint specifiers" << std::endl;
    stream << "typedef ";
    declaration_specifier_->Print(stream);
    stream << " ";
    std::cout << "before printint init declarator" << std::endl;
    init_declarator_list_->Print(stream);
    std::cout << "after printing" << std::endl;
    stream << ";" << std::endl;
}