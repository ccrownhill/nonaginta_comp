#include "ast_constant.hpp"

Op IntConstant::EmitCFG(IR &ir, IRGenContext &ctx, int lhs_vr) const
{
    Op out;
    if (ctx.get_is_global()) {
		std::cout << "global int const" << std::endl;
        out.src = OpSrc::vr;
        ctx.next_vr_num();
		if(ctx.is_eval_enum())
			out.val = value_;
		else
        	out.val = ir.set_last_uninit_global({static_cast<uint64_t>(value_)}); // this is the leak
		std::cout << "test1" << std::endl;
    } else {
        out.src = OpSrc::con;
        out.val = value_;
		std::cout << "IntConstant::EmitCFG value: " << value_ << std::endl;
    }
    
    ctx.set_type({.t = Type::INT, .p_level = 0, .num_els = 0, .type_l = {}});
    return out;
}

void IntConstant::Print(std::ostream &stream) const
{
    stream << value_;
}

int IntConstant::GetValue()
{
    return value_;
}


Op FloatConstant::EmitCFG(IR &ir, IRGenContext &ctx, int lhs_vr) const
{
    Op out;
    if (ctx.get_is_global()) {
        out.src = OpSrc::vr;
        ctx.next_vr_num();
        out.val = ir.set_last_uninit_global({encode_float(value_)});
    } else {
        out.val = ir.add_const({encode_float(value_)}, 4);
        out.src = OpSrc::con;
    }
    ctx.set_type({.t = Type::FLOAT, .p_level = 0, .num_els = 0, .type_l = {}});
    return out;
}

void FloatConstant::Print(std::ostream &stream) const
{
    stream << value_;
}

Op DoubleConstant::EmitCFG(IR &ir, IRGenContext &ctx, int lhs_vr) const
{
    Op out;
    if (ctx.get_is_global()) {
        out.src = OpSrc::vr;
        out.val = ir.set_last_uninit_global({encode_double(value_)});
        ctx.next_vr_num();
    } else {
        out.src = OpSrc::con;
        out.val = ir.add_const({encode_double(value_)}, 8);
    }
    ctx.set_type({.t = Type::DOUBLE, .p_level = 0, .num_els = 0, .type_l = {}});
    return out;
}

void DoubleConstant::Print(std::ostream &stream) const
{
    stream << value_;
}

Op UnsignedConstant::EmitCFG(IR &ir, IRGenContext &ctx, int lhs_vr) const
{
	Op out;
	if (ctx.get_is_global()) {
        out.src = OpSrc::vr;
        ctx.next_vr_num();
        out.val = ir.set_last_uninit_global({static_cast<uint64_t>(value_)});
    } else {
        out.src = OpSrc::con;
        out.val = value_;
    }
	out.src = OpSrc::con;
	out.val = value_;
	ctx.set_type({.t = Type::UNSIGNED, .p_level = 0, .num_els = 0, .type_l = {}});
	return out;
}

void UnsignedConstant::Print(std::ostream &stream) const
{
	stream << value_;
}

int UnsignedConstant::GetValue()
{
	return value_;
}

Op StringLiteral::EmitCFG(IR &ir, IRGenContext &ctx, int lhs_vr) const
{
    Op out;
    out.src = OpSrc::ptr;
    out.val = ctx.next_vr_num();
    ir.add_string_literal(lit_, out.val);
    ctx.set_type({.t = Type::CHAR, .p_level = 1, .num_els = 0, .type_l = {}});
    return out;
}

void StringLiteral::Print(std::ostream &stream) const
{
    stream << "\"" << lit_ << "\"";
}

uint64_t encode_float(float f)
{
    union {
        float float_var;
        uint32_t out;
    } u;
    u.float_var = f;
    return (uint64_t)u.out;
}

uint64_t encode_double(double f)
{
    union {
        double double_var;
        uint64_t out;
    } u;
    u.double_var = f;
    return u.out;
}


float decode_float(uint64_t i)
{
    union {
        float float_out;
        uint32_t in_int;
    } u;
    u.in_int = i;
    return u.float_out;
}

double decode_double(uint64_t i)
{
    union {
        double float_out;
        uint64_t in_int;
    } u;
    u.in_int = i;
    return u.float_out;
}


