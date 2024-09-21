#include "ast_struct_specifier.hpp"
#include "ast_identifier.hpp"


void StructSpecifier::EmitCFG(IR &ir, IRGenContext &ctx) const
{
    if (struct_decl_list_ == nullptr) {
        std::cout << "getting struct type with number of elements: " << ctx.get_struct_type((static_cast<Identifier *>(ident_))->GetName()).type_l.size() << std::endl;
        ctx.set_type(ctx.get_struct_type((static_cast<Identifier *>(ident_))->GetName()));
    } else {
        std::string name = ".undef";
        if (ident_ != nullptr) {
            name = (static_cast<Identifier *>(ident_))->GetName();
        }
        ctx.set_cur_struct(name);
        ctx.start_eval_struct_decl();
        struct_decl_list_->EmitCFG(ir, ctx);
        ctx.stop_eval_struct_decl();
        OpType cur_type = {.t = Type::STRUCT, .p_level = 0, .num_els = 0, .type_l = ctx.get_struct_type_list()};
        if (ident_ != nullptr) {
            ctx.set_struct_type(name, cur_type);
        }
        ctx.set_type(cur_type);
    }
}

void StructSpecifier::Print(std::ostream &stream) const
{
    stream << "struct ";
    if (ident_ != nullptr) {
        ident_->Print(stream);
    }
    if (struct_decl_list_ != nullptr) {
        stream << "{" << std::endl;
        struct_decl_list_->Print(stream);
        stream << "};" << std::endl;
    }
}
