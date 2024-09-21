#include "ast_parameter_declaration.hpp"
#include "ast_declarator.hpp"

void ParameterDeclaration::EmitCFG(IR &ir, IRGenContext &ctx) const
{
    declaration_specifier_->EmitCFG(ir, ctx);
    

    ctx.add_arg_to_decl(ir, ctx.get_type());

    //ctx.set_vr(ir, name, ctx.get_type());
    if (!ctx.get_is_global() && declarator_ != nullptr) {
        std::string name = static_cast<Declarator *>(declarator_)->get_name();
        declarator_->EmitCFG(ir, ctx);
        std::cout << "parameter type: ";
        print_op_type(std::cout, ctx.get_type());
        std::cout << std::endl;
        ctx.add_param(ir, name);
    }
}

void ParameterDeclaration::Print(std::ostream &stream) const
{
    declaration_specifier_->Print(stream);
    stream << " ";
    if (declarator_ != nullptr)
        declarator_->Print(stream);
    stream << ",";
}