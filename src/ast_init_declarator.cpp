#include "ast_init_declarator.hpp"
#include "ast_declarator.hpp"
#include "ast_constant.hpp"

void InitDeclarator::EmitCFG(IR &ir, IRGenContext &ctx) const
{
    std::string name = static_cast<Declarator *>(declarator_)->get_name();
    declarator_->EmitCFG(ir, ctx);
    if (assignment_expr_ != nullptr) {
		
        Op rhs_op = assignment_expr_->EmitCFG(ir, ctx, ctx.get_vr(name).first);
		
        if (!ctx.get_is_global() && !(rhs_op.src == OpSrc::vr && rhs_op.val == -1)) { // if it is -1 means store already happened on lower level
            auto [vr_num, type] = ctx.get_vr(name);
            CFGElPtr store = std::make_shared<Store>(ctx.get_type(), vr_num, rhs_op, false, false);
            ctx.add_CFGEl(ir, store);
        }
    }
}

void InitDeclarator::Print(std::ostream &stream) const
{
    declarator_->Print(stream);
    if (assignment_expr_ != nullptr) {
        stream << " = ";
        assignment_expr_->Print(stream);
    }
}