#include "ast_type_specifier.hpp"

void TypeSpecifier::EmitCFG(IR &ir, IRGenContext &ctx) const
{
    if (type_ == "int") {
        ctx.set_type({.t = Type::INT, .p_level = 0, .num_els = 0, .type_l = {}});
    } else if (type_ == "float") {
        ctx.set_type({.t = Type::FLOAT, .p_level = 0, .num_els = 0, .type_l = {}});
    } else if (type_ == "double") {
        ctx.set_type({.t = Type::DOUBLE, .p_level = 0, .num_els = 0, .type_l = {}});
	} else if (type_ == "unsigned") {
		ctx.set_type({.t = Type::UNSIGNED, .p_level = 0, .num_els = 0, .type_l = {}});
	} else if (type_ == "char") {
		ctx.set_type({.t = Type::CHAR, .p_level = 0, .num_els = 0, .type_l = {}});
    } else if (type_ == "void") {
        ctx.set_type({.t = Type::VOID, .p_level = 0, .num_els = 0, .type_l = {}});
    } else {
        ctx.set_type(ctx.get_typedef(type_));
    }
}

void TypeSpecifier::Print(std::ostream &stream) const
{
    stream << type_;
}
