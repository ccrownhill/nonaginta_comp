#include "ast_pointer.hpp"

void Pointer::EmitCFG(IR &ir, IRGenContext &ctx) const
{
    if (pointer_ != nullptr) {
        pointer_->EmitCFG(ir, ctx);
        ctx.set_type({.t = ctx.get_type().t, .p_level = ctx.get_type().p_level + 1, .num_els = 0, .type_l = {}});
    } else {
        ctx.set_type({.t = ctx.get_type().t, .p_level = 1, .num_els = 0, .type_l = {}});
    }
}

void Pointer::Print(std::ostream &stream) const
{
    if (pointer_ != nullptr) {
        pointer_->Print(stream);
    }
    stream << "*";
}