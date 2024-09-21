#include "ast_identifier.hpp"
#include "ast_direct_declarator.hpp"

void DirectDeclarator::EmitCFG(IR &ir, IRGenContext &ctx) const
{
    // identifier_->EmitCFG(ir, ctx);
    // stream << ":" << std::endl;
    std::string name = (static_cast<Identifier *>(identifier_))->GetName();
    if (ctx.is_evaluating_typedef()) {
        ctx.add_typedef(name, ctx.get_type());
    } else if (ctx.is_evaluating_struct_decl()) {
        std::cout << "evaluating struct decl" << std::endl;
        ctx.add_to_struct_type_list(ctx.get_type(), name);
    } else if (decl_type_ == DirDeclType::fn) {
        if (!ctx.get_is_global()) {
            ctx.add_fn(ir, name);
            
        } else {
            ctx.add_fn_decl(ir, name);
        }
        if (param_list_ != nullptr) {
            param_list_->EmitCFG(ir, ctx);
        }
    } else if (decl_type_ == DirDeclType::var) {
        OpType type = ctx.get_type();
        if (type.t == Type::STRUCT) {
            ctx.set_struct_name(name);
            std::cout << "setting struct with number of elements: " << type.type_l.size() << std::endl;
        }
        ctx.set_vr(name, type);
        if (ctx.get_is_global()) {
            ir.add_global({0UL}, get_op_type_size(ctx.get_type()));
        }
    } else if (decl_type_ == DirDeclType::arr) {
        OpType type = ctx.get_type();
        
        bool is_global_backup = ctx.get_is_global();
        ctx.set_is_global(false);
        Op const_expr = expr_->EmitCFG(ir, ctx, -1);
        ctx.set_is_global(is_global_backup);
        type = {.t = type.t, .p_level = type.p_level, .num_els = const_expr.val, .type_l = {}};
        ctx.set_vr(name, type);
        if (ctx.get_is_global()) {
            std::vector<uint64_t> vals;
            vals.resize(const_expr.val);
            ir.add_global(vals, get_op_type_size(ctx.get_type()));
        }
		

    }
}

void DirectDeclarator::Print(std::ostream &stream) const
{
    identifier_->Print(stream);
    if (decl_type_ == DirDeclType::fn) {
        stream << "(";
    }

    if (param_list_ != nullptr) {
        param_list_->Print(stream);
    }
    
    if (decl_type_ == DirDeclType::fn) {
        stream << ")";
    }
}

std::string DirectDeclarator::GetName()
{
    return (static_cast<Identifier *>(identifier_))->GetName();
}
