#include "ast_function_definition.hpp"
#include "ast_direct_declarator.hpp"

void FunctionDefinition::EmitCFG(IR &ir, IRGenContext &ctx) const
{
    declaration_specifiers_->EmitCFG(ir, ctx);
    OpType ret_type = ctx.get_type();
    ctx.set_is_global(false);
    declarator_->EmitCFG(ir, ctx);
    ctx.add_cfg_block(ir);
    ctx.set_ret_type(ir, ret_type);

    if (compound_statement_ != nullptr)
    {
        compound_statement_->EmitCFG(ir, ctx);
    }
    ctx.leave_scope();
    ctx.set_is_global(true);
}

void FunctionDefinition::Print(std::ostream &stream) const
{
    declaration_specifiers_->Print(stream);
    stream << " ";

    declarator_->Print(stream);

    if (compound_statement_ != nullptr)
    {
        compound_statement_->Print(stream);
    } else {
        std::cout << "null" << std::endl;
    }

    stream << std::endl;
}
