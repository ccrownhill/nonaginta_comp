#include "ast_expression.hpp"
#include "ast_constant.hpp"
#include "ast_identifier.hpp"

Op AssignmentExpression::EmitCFG(IR &ir, IRGenContext &ctx, int lhs_vr) const
{
    
    
    Op rhs_eval = rhs_op_->EmitCFG(ir, ctx, lhs_vr);
	ctx.set_rhs_op(rhs_eval);
	ctx.start_eval_lhs();
    Op lhs_eval = lhs_op_->EmitCFG(ir, ctx, rhs_eval.val);
    ctx.stop_eval_lhs();
    
    // we know that lhs_eval.val will be a vr number since otherwise
    // lhs_op_->EmitCFG(ir, ctx, lhs_vr) would have thrown an error
	if (!(lhs_eval.src == OpSrc::vr && lhs_eval.val == -1)) {
		CFGElPtr store = std::make_shared<Store>(ctx.get_type(), lhs_eval.val, rhs_eval, false, false);
		ctx.add_CFGEl(ir, store);
	}
    return rhs_eval;
}

void AssignmentExpression::Print(std::ostream &stream) const
{
    lhs_op_->Print(stream);
    stream << " = ";
    rhs_op_->Print(stream);
}

Op ArithmeticExpression::EmitCFG(IR &ir, IRGenContext &ctx, int lhs_vr) const
{
    Op out;
    Op op1_before_mul = op1_->EmitCFG(ir, ctx, lhs_vr);
	Op op1_eval;
	bool is_first_pointer = false;
	int t1_size = 1;
	
	// don't do pointer arithmetic for struct pointers
	if ((ctx.get_type().p_level > 0 || op1_eval.src == OpSrc::ptr) && !(ctx.get_type().type_l.size() > 0)) {
		is_first_pointer = true;
		t1_size = get_op_align(ctx.get_type());
	}

    Op op2_before_mul = op2_->EmitCFG(ir, ctx, lhs_vr);
	bool is_second_pointer = false;
	Op op2_eval;
	int t2_size = 1;
	// don't do pointer arithmetic for struct pointers
	if ((ctx.get_type().p_level > 0 || op1_eval.src == OpSrc::ptr) && !(ctx.get_type().type_l.size() > 0)) {
		is_second_pointer = true;
		t2_size = get_op_align(ctx.get_type());
	}

	std::cout << "before first mul: " << op1_before_mul.val << std::endl;
	if (is_first_pointer && !is_second_pointer) {
		if (op2_before_mul.src == OpSrc::con) {
			op2_eval.src = OpSrc::con;
			op2_eval.val = t1_size * op2_before_mul.val;
		} else {
			op2_eval.src = OpSrc::vr;
			op2_eval.val = ctx.next_vr_num();

			Op const_mul_fac = {.src = OpSrc::con, .val = t1_size};
			CFGElPtr mul = std::make_shared<ArithmeticOp>(ctx.get_type(), "mul", op2_eval.val, op2_before_mul, const_mul_fac);
			ctx.add_CFGEl(ir, mul);
		}
	} else {
		op2_eval = op2_before_mul;
	}
	std::cout << "after first mul: "  << ((op1_eval.src == OpSrc::vr) ? "(vr)" : "(con)") << op1_eval.val << std::endl;

	std::cout << "before second mul: " << op2_before_mul.val << std::endl;
	if (!is_first_pointer && is_second_pointer) {
		if (op1_before_mul.src == OpSrc::con) {
			op1_eval.src = OpSrc::con;
			op1_eval.val = t2_size * op1_before_mul.val;
		} else {
			op1_eval.src = OpSrc::vr;
			op1_eval.val = ctx.next_vr_num();

			Op const_mul_fac = {.src = OpSrc::con, .val = t2_size};
			CFGElPtr mul = std::make_shared<ArithmeticOp>(ctx.get_type(), "mul", op1_eval.val, op1_before_mul, const_mul_fac);
			ctx.add_CFGEl(ir, mul);
		}
	} else {
		op1_eval = op1_before_mul;
	}
	std::cout << "after second mul: " << ((op2_eval.src == OpSrc::vr) ? "(vr)" : "(con)") << op2_eval.val << std::endl;

    if (op1_eval.src == OpSrc::con && op2_eval.src == OpSrc::con) {
        out.src = OpSrc::con;
        if (operation_ == "add")
            out.val = op1_eval.val + op2_eval.val;
        else if (operation_ == "sub")
            out.val = op1_eval.val - op2_eval.val;
        else if (operation_ == "mul")
            out.val = op1_eval.val * op2_eval.val;
        else if (operation_ == "div")
            out.val = op1_eval.val / op2_eval.val;
        else if (operation_ == "rem")
            out.val = op1_eval.val % op2_eval.val;
                    
    } else {
        out.src = OpSrc::vr;
        out.val = ctx.next_vr_num();
        CFGElPtr add = std::make_shared<ArithmeticOp>(ctx.get_type(), operation_, out.val, op1_eval, op2_eval);
        ctx.add_CFGEl(ir, add);
    }
    return out;
}

void ArithmeticExpression::Print(std::ostream &stream) const
{
    op1_->Print(stream);
    if (operation_ == "add")
        stream << " + ";
    else if (operation_ == "sub")
        stream << " - ";
    else if (operation_ == "mul")
        stream << " * ";
    else if (operation_ == "div")
        stream << " / ";
    else if (operation_ == "rem")
        stream << " % ";
    op2_->Print(stream);
}

Op PrimaryExpression::EmitCFG(IR &ir, IRGenContext &ctx, int lhs_vr) const
{
    Op out;
	std::string name = static_cast<Identifier *>(value_)->GetName();
	auto [vr_num, type] = ctx.get_vr(name); 
	ctx.set_type(type);
	if (type.t == Type::STRUCT) {
		std::cout << "setting current struct to: " << ctx.get_struct_name(name) << std::endl;
		ctx.set_cur_struct(ctx.get_struct_name(name));
	}
	if (type.num_els > 0) {
		out.src = OpSrc::ptr;
		out.val = vr_num;
	} else if (type.type_l.size() > 0 && type.p_level == 0) {
		std::cout << "return pointer to struct" << std::endl;
		out.src = OpSrc::ptr;
		out.val = vr_num;
	} else if (!(ctx.is_evaluating_lhs())) {
        out.src = OpSrc::vr;
        out.val = ctx.next_vr_num();
        if(type.t == Type::ENUM){
			out.src = OpSrc::con;
			out.val = vr_num;
			return out;
		}
		else{
			CFGElPtr load = std::make_shared<Load>(type, out.val, vr_num, false);
			ctx.add_CFGEl(ir, load);
		}
    } else if (ctx.is_evaluating_lhs()) {
        out.src = OpSrc::vr;
        out.val = vr_num;
    }
	std::cout << "Primary expression sets type: ";
	print_op_type(std::cout, ctx.get_type());
	std::cout << std::endl;
    return out;
}

void PrimaryExpression::Print(std::ostream &stream) const
{
    value_->Print(stream);
}

std::string PrimaryExpression::get_name() const
{
	return static_cast<Identifier *>(value_)->GetName();
}

Op UnaryExpression::EmitCFG(IR &ir, IRGenContext &ctx, int lhs_vr) const
{
	Op out;
	if (operation_ == "and") {
		ctx.start_eval_lhs();
	}
	Op op_eval = op_->EmitCFG(ir, ctx, lhs_vr);
	if (operation_ == "and") {
		ctx.stop_eval_lhs();
	}
	if (op_eval.src == OpSrc::con) {
		out.src = OpSrc::con;
		if (operation_ == "and") {
			std::cerr << "can't get address of constant" << std::endl;
			std::exit(1);
		} else if (operation_ == "star") {
			std::cerr << "can't dereference constant" << std::endl;
			std::exit(1);
		} else if (operation_ == "plus") {
				// should work for all types
				out.val = op_eval.val;
			
		} else if (operation_ == "minus") {
			std::cout << "using minus" << std::endl;
			if (ctx.get_type().t == Type::INT) {
				out.val = -op_eval.val;
			} else if (ctx.get_type().t == Type::FLOAT) {
				std::cout << "float type" << std::endl;
				uint64_t val = ir.get_const(op_eval.val)[0];
				val = val ^ (1ULL << 31); // flip sign bit
				ir.set_const(op_eval.val, {val});
				out.val = op_eval.val;
			} else if (ctx.get_type().t == Type::DOUBLE) {
				std::cout << "double type" << std::endl;
				uint64_t val = ir.get_const(op_eval.val)[0];
				val = val ^ (1ULL << 63); // flip sign bit
				ir.set_const(op_eval.val, {val});
				out.val = op_eval.val;
			}
			
		// TODO for float update the number at the given index of constants
		} else if (operation_ == "tilde") {
			
			if (ctx.get_type().t == Type::INT) {
				out.val = ~op_eval.val;
			}
			else if (ctx.get_type().t == Type::FLOAT) { // not needed
				std::cout << "float type" << std::endl;
				uint64_t val = ir.get_const(op_eval.val)[0];
				val = ~val;
				ir.set_const(op_eval.val, {val});
				out.val = op_eval.val;

			}
			else if (ctx.get_type().t == Type::DOUBLE) { // not needed
				std::cout << "double type" << std::endl;
				uint64_t val = ir.get_const(op_eval.val)[0];
				val = ~val;
				ir.set_const(op_eval.val, {val});
				out.val = op_eval.val;
			}
		} else if (operation_ == "exclamation") { 
			if (ctx.get_type().t == Type::INT) {
				out.val = !op_eval.val;
			} else if (ctx.get_type().t == Type::FLOAT || ctx.get_type().t == Type::DOUBLE) { // not needed
				uint64_t val = ir.get_const(op_eval.val)[0];
				if (val == 0)
					ir.set_const(op_eval.val, {1065353216ULL});
				else
					ir.set_const(op_eval.val, {0ULL});
				out.val = op_eval.val;
			}
		}
	} else {
		out.src = OpSrc::vr;
		out.val = ctx.next_vr_num();
		if (operation_ == "and") {
			// note that op_eval will have been evaluated as if it were a lhs expression
			out.src = OpSrc::ptr;
			out.val = op_eval.val;
		} else if (operation_ == "star") {
			if (ctx.is_evaluating_lhs()) {
				// op_eval.val will be the vr of the variable after the *
				// lhs_vr will be the virtual register where the result of the
				// right hand side operation is stored
				CFGElPtr store_ptr = std::make_shared<Store>(ctx.get_type(), op_eval.val, ctx.get_rhs_op(), false, true);
				ctx.add_CFGEl(ir, store_ptr);
				out.val = -1; // tell assignment expression not to do a store
			} else {
				CFGElPtr load_ptr = std::make_shared<Load>(ctx.get_type(), out.val, op_eval.val, true);
				ctx.add_CFGEl(ir, load_ptr);
				OpType type = ctx.get_type();
				type.p_level--;
				ctx.set_type(type);
			}
		} else if (operation_ == "plus") {
			out.val = op_eval.val;
		} else if (operation_ == "minus") {
			if(ctx.get_type().t == Type::INT){	
				Op tmp = {OpSrc::con, 0};
				
				CFGElPtr unary_sub = std::make_shared<ArithmeticOp>(ctx.get_type(), "sub", out.val, tmp, op_eval);
				ctx.add_CFGEl(ir, unary_sub);
			}
			else if (ctx.get_type().t == Type::FLOAT){
				// TODO make this work
				
				Op tmp = {OpSrc::con, 0};

				
				CFGElPtr unary_sub = std::make_shared<ArithmeticOp>(ctx.get_type(), "sub", out.val, tmp, op_eval);
				ctx.add_CFGEl(ir, unary_sub);
			}
			else if (ctx.get_type().t == Type::DOUBLE){
				// TODO make this work
				Op tmp = {OpSrc::con, 0};
				
				CFGElPtr unary_sub = std::make_shared<ArithmeticOp>(ctx.get_type(), "sub", out.val, tmp, op_eval);
				ctx.add_CFGEl(ir, unary_sub);
			}
				
		}
		else if (operation_ == "tilde"){ // for tilde constant ~x = -x - 1
			Op tmp1 = {OpSrc::con, -1};
			CFGElPtr bitwise_tilde = std::make_shared<BitwiseOp>(ctx.get_type(),"xor", out.val, tmp1, op_eval); 
			ctx.add_CFGEl(ir, bitwise_tilde);
		}
		else if (operation_ == "exclamation"){ // assume !0 will not occur for now. Will break for this
			// if a !0 occurs you should xor with 1 not 0
			
			//Op tmp2 = {Type::con, op_eval.val == 0}; // this still breaks for !0
			Op tmp2 = {OpSrc::con, 0};
			//int tmp_reg = ctx.next_vr_num();
			CFGElPtr check = std::make_shared<ArithmeticOp>(ctx.get_type(), "eq", op_eval.val, op_eval, tmp2);
			CFGElPtr bitwise_exclamation = std::make_shared<BitwiseOp>(ctx.get_type(), "xor", out.val, op_eval, op_eval);
			ctx.add_CFGEl(ir, bitwise_exclamation);
		}
		
	} 
	
		
	return out; // all possibly wrong
}

void UnaryExpression::Print(std::ostream &stream) const
{
	stream << operation_;
	op_->Print(stream);
}

Op SizeExpression::EmitCFG(IR &ir, IRGenContext &ctx, int lhs_vr) const
{
	if(type_ != nullptr){
		type_->EmitCFG(ir, ctx);
	}
	else if (expression_ != nullptr){
		expression_->EmitCFG(ir, ctx, lhs_vr);
	}
	Op out;

	out.src = OpSrc::con;
	OpType type;
	type = ctx.get_type();
	std::cout << "type is: ";
	print_op_type(std::cout,type);
	
	
	out.val = get_op_type_size(type);
	
// 	else if(type == OpType::UNSIGNED){
// 		out.val = 4;
// 	}
	out.src = OpSrc::con;

	//ctx.add_CFGEl(ir, std::make_shared<Store>(type, out.val, out));
	return out;
}

void SizeExpression::Print(std::ostream &stream) const
{}

Op BitwiseExpression::EmitCFG(IR &ir, IRGenContext &ctx, int lhs_vr) const
{
	Op out;
	Op op1_eval = op1_->EmitCFG(ir, ctx, lhs_vr);
	Op op2_eval = op2_->EmitCFG(ir, ctx, lhs_vr);
	if (op1_eval.src == OpSrc::con && op2_eval.src == OpSrc::con) {
		out.src = OpSrc::con;
		if (operation_ == "and")
			out.val = op1_eval.val & op2_eval.val;
		else if (operation_ == "or")
			out.val = op1_eval.val | op2_eval.val;
		else if (operation_ == "xor")
			out.val = op1_eval.val ^ op2_eval.val;
		else if (operation_ == "shl")
			out.val = op1_eval.val << op2_eval.val;
		else if (operation_ == "shr"){
			if (ctx.get_type().t == Type::UNSIGNED){
				out.val = (unsigned)op1_eval.val >> op2_eval.val;		
			}
			out.val = op1_eval.val >> op2_eval.val;
		}
	} else {
		
		out.src = OpSrc::vr;
		out.val = ctx.next_vr_num();
		CFGElPtr bitwise = std::make_shared<BitwiseOp>(ctx.get_type(), operation_, out.val, op1_eval, op2_eval);
		ctx.add_CFGEl(ir, bitwise);
	}
	return out;
}

void BitwiseExpression::Print(std::ostream &stream) const // this is probably wrong
{
	op1_->Print(stream);
	if (operation_ == "and")
		stream << " & ";
	else if (operation_ == "or")
		stream << " | ";
	else if (operation_ == "xor")
		stream << " ^ ";
	else if (operation_ == "shl")
		stream << " << ";
	else if (operation_ == "shr")
		stream << " >> ";
	op2_->Print(stream);
}

Op LogicalExpression::EmitCFG(IR &ir, IRGenContext &ctx, int lhs_vr) const // this may be wrong
{
	std::cout << "adding LogicalOp" << std::endl;
	Op out;
	Op op1_eval = op1_->EmitCFG(ir, ctx, lhs_vr);
	Op op2_eval = op2_->EmitCFG(ir, ctx, lhs_vr);
	if (op1_eval.src == OpSrc::con && op2_eval.src == OpSrc::con) {
		out.src = OpSrc::con;
		if (operation_ == "and")
			out.val = op1_eval.val && op2_eval.val;
		else if (operation_ == "or")
			out.val = op1_eval.val || op2_eval.val;
		else if (operation_ == "cmp eq")
			out.val = op1_eval.val == op2_eval.val;
		else if (operation_ == "cmp ne")
			out.val = op1_eval.val != op2_eval.val;
		else if (operation_ == "cmp lt")
			out.val = op1_eval.val < op2_eval.val;
		else if (operation_ == "cmp le")
			out.val = op1_eval.val <= op2_eval.val;
		else if (operation_ == "cmp gt")
			out.val = op1_eval.val > op2_eval.val;
		else if (operation_ == "cmp ge")
			out.val = op1_eval.val >= op2_eval.val;
	} else {
		out.src = OpSrc::vr;
		out.val = ctx.next_vr_num();
		CFGElPtr logical = std::make_shared<LogicalOp>(ctx.get_type(), operation_, out.val, op1_eval, op2_eval);
		ctx.add_CFGEl(ir, logical);
	}
	return out;
}

void LogicalExpression::Print(std::ostream &stream) const // change later
{
	op1_->Print(stream);
	if (operation_ == "and")
		stream << " && ";
	else if (operation_ == "or")
		stream << " || ";
	else if (operation_ == "cmp eq")
		stream << " == ";
	else if (operation_ == "cmp ne")
		stream << " != ";
	else if (operation_ == "cmp lt")
		stream << " < ";
	else if (operation_ == "cmp le")
		stream << " <= ";
	else if (operation_ == "cmp gt")
		stream << " > ";
	else if (operation_ == "cmp ge")
		stream << " >= ";
	op2_->Print(stream);
}

Op FunctionCallExpression::EmitCFG(IR &ir, IRGenContext &ctx, int lhs_vr) const // this may be wrong
{
	Op out;
	out.src = OpSrc::vr;
	out.val = ctx.next_vr_num();
	std::vector<Op> args;
	Op tmp;
	if (args_ != nullptr) {
		std::vector<ExprNode *>::iterator it = args_->begin();
		for (; it != args_->end(); it++) {
			tmp = (*it)->EmitCFG(ir, ctx, lhs_vr);
			args.push_back(tmp);
		}
	}
	std::string fn_name = static_cast<PrimaryExpression *>(prim_expr_)->get_name();
	
	CFGElPtr call = std::make_shared<CallOp>(ctx.get_type(),
		fn_name, out.val, args, ir.get_fn_decl(fn_name));
	ctx.add_CFGEl(ir, call);
	
	return out;
}

void FunctionCallExpression::Print(std::ostream &stream) const // change later
{
	std::string fn_name = static_cast<PrimaryExpression *>(prim_expr_)->get_name();
	stream << fn_name;
	stream << "(";
	if (args_ != nullptr) {
		args_->Print(stream);
	}
	stream << ")";
}

Op PointerIndexExpression::EmitCFG(IR &ir, IRGenContext &ctx, int lhs_vr) const
{
	Op ptr_eval = ptr_expr_->EmitCFG(ir, ctx, lhs_vr);
	OpType ptr_type = ctx.get_type();

	bool was_eval_lhs = ctx.is_evaluating_lhs();
	ctx.stop_eval_lhs();
	Op idx_eval = idx_expr_->EmitCFG(ir, ctx, lhs_vr);
	if (was_eval_lhs) {
		ctx.start_eval_lhs();
	}
	Op mul_out;
	mul_out.src = OpSrc::vr;
	mul_out.val = ctx.next_vr_num();

	Op const_mul_fac = {.src = OpSrc::con, .val = get_op_align(ctx.get_type())};
	CFGElPtr mul = std::make_shared<ArithmeticOp>(ctx.get_type(), "mul", mul_out.val, idx_eval, const_mul_fac);
	ctx.add_CFGEl(ir, mul);

	Op add_out;
	add_out.src = OpSrc::vr;
	add_out.val = ctx.next_vr_num();
	CFGElPtr add = std::make_shared<ArithmeticOp>(ctx.get_type(), "add", add_out.val, ptr_eval, mul_out);
	ctx.add_CFGEl(ir, add);

	Op out;
	if (!ctx.is_evaluating_lhs()) { // on right hand side of assignment
		out.src = OpSrc::vr;
		out.val = ctx.next_vr_num();
		CFGElPtr load = std::make_shared<Load>(ptr_type, out.val, add_out.val, true);
		ctx.add_CFGEl(ir, load);
	} else {
		out.src = OpSrc::vr;
		out.val = -1; // tell assignment expression to not emit its own store since this is done here already
		CFGElPtr store = std::make_shared<Store>(ptr_type, add_out.val, ctx.get_rhs_op(), false, true);
		ctx.add_CFGEl(ir, store);
	}
	
	return out;
}

void PointerIndexExpression::Print(std::ostream &stream) const
{
	ptr_expr_->Print(stream);
	stream << "[";
	idx_expr_->Print(stream);
	stream << "]";
}

Op PostCrementExpression::EmitCFG(IR &ir, IRGenContext &ctx, int lhs_vr) const
{
	// has to be a virtual register since constants don't support ++ and --
	ctx.start_eval_lhs();
	Op expr_out = expr_->EmitCFG(ir, ctx, -1);
	ctx.stop_eval_lhs();

	Op load_out;
	load_out.src = OpSrc::vr;
	load_out.val = ctx.next_vr_num();
	CFGElPtr load = std::make_shared<Load>(ctx.get_type(), load_out.val, expr_out.val, false);
	ctx.add_CFGEl(ir, load);

	Op add_out;
	Op backup;
	add_out.src = OpSrc::vr;
	add_out.val = ctx.next_vr_num();
	//backup = add_out;
	OpType type = ctx.get_type();
	Op one = {.src = OpSrc::con, .val = 1};
	CFGElPtr add = std::make_shared<ArithmeticOp>(type, operation_, add_out.val, load_out, one, true);
	ctx.add_CFGEl(ir, add);

	//backup.val = ctx.next_vr_num();
	CFGElPtr store = std::make_shared<Store>(ctx.get_type(), expr_out.val, add_out, false, false); // this store breaks
	ctx.add_CFGEl(ir, store);
	return load_out;
}
void PostCrementExpression::Print(std::ostream &stream) const
{
	expr_->Print(stream);
	if (operation_ == "add")
		stream << "++";
	else
		stream << "--";
}

Op StructAccessExpression::EmitCFG(IR &ir, IRGenContext &ctx, int lhs_vr) const
{
	Op s_ptr = expr_->EmitCFG(ir, ctx, -1);
	std::string element_name = static_cast<Identifier *>(ident_)->GetName();

	Op off_op;
	off_op.src = OpSrc::con;

	auto [type, offset] = ctx.get_struct_offset(element_name);
	off_op.val = offset;
	std::cout << "offset for element '" << element_name << "' : " << off_op.val << std::endl;

	OpType ptr_type = {.t = Type::STRUCT, .p_level = ctx.get_type().p_level + 1, .num_els = 0, .type_l = {}};
	Op add_out = {.src = OpSrc::vr, .val = ctx.next_vr_num()};
	CFGElPtr add = std::make_shared<ArithmeticOp>(ptr_type, "add", add_out.val, s_ptr, off_op);
	ctx.add_CFGEl(ir, add);

	ptr_type.t = type.t;
	Op out;
	if (!ctx.is_evaluating_lhs()) { // on right hand side of =
		out.src = OpSrc::vr;
		out.val = ctx.next_vr_num();
		// TODO need correct type of pointer here
		CFGElPtr load = std::make_shared<Load>(ptr_type, out.val, add_out.val, true);
		ctx.add_CFGEl(ir, load);
		ptr_type.p_level--;
		ctx.set_type(ptr_type);
	} else { // on left hand side of =
		out.src = OpSrc::vr;
		out.val = -1; // tell assignment expression to not emit its own store since this is done here already
		CFGElPtr store = std::make_shared<Store>(ptr_type, add_out.val, ctx.get_rhs_op(), false, true);
		ctx.add_CFGEl(ir, store);
	}
	return out;
}
void StructAccessExpression::Print(std::ostream &stream) const
{
	stream << "(";
	expr_->Print(stream);
	stream << ")";
	if (is_pointer_) {
		stream << "->";
	} else {
		stream << ".";
	}
	ident_->Print(stream);
}

Op Initializer::EmitCFG(IR &ir, IRGenContext &ctx, int lhs_vr) const
{
	Op out;
	out.src = OpSrc::vr;
	out.val = -1; // tell assignment above not to output another store

	Op off_op;
	off_op.src = OpSrc::con;
	off_op.val = 0;
	std::vector<ExprNode *>::iterator it = expr_list_->begin();
	for (; it != expr_list_->end(); it++) {
		Op con = (*it)->EmitCFG(ir, ctx, lhs_vr);
		OpType ptr_type = ctx.get_type();
		ptr_type.p_level++;
		Op mul_out;
		mul_out.src = OpSrc::vr;
		mul_out.val = ctx.next_vr_num();

		Op left_ptr = {.src = OpSrc::ptr, .val = lhs_vr};
		Op const_mul_fac = {.src = OpSrc::con, .val = get_op_align(ctx.get_type())};
		CFGElPtr mul = std::make_shared<ArithmeticOp>(ctx.get_type(), "mul", mul_out.val, off_op, const_mul_fac);
		ctx.add_CFGEl(ir, mul);

		Op add_out;
		add_out.src = OpSrc::vr;
		add_out.val = ctx.next_vr_num();
		CFGElPtr add = std::make_shared<ArithmeticOp>(ctx.get_type(), "add", add_out.val, left_ptr, mul_out);
		ctx.add_CFGEl(ir, add);
		off_op.val += get_op_align(ctx.get_type());

		CFGElPtr store = std::make_shared<Store>(ptr_type, add_out.val, con, false, true);
		ctx.add_CFGEl(ir, store);
	}

	


	return out;
}

void Initializer::Print(std::ostream &stream) const
{
	stream << "initializer";
}

Op InitializerList::EmitCFG(IR &ir, IRGenContext &ctx, int lhs_vr) const
{
	Op out;
	std::vector<Op> ops;
	std::vector<ExprNode *>::iterator it = initializer_list_->begin();
	for (; it != initializer_list_->end(); it++) {
		ops.push_back((*it)->EmitCFG(ir, ctx, lhs_vr));
	}
	return out;
}

void InitializerList::Print(std::ostream &stream) const
{
	std::vector<ExprNode *>::iterator it = initializer_list_->begin();
	for (; it != initializer_list_->end(); it++) {
		(*it)->Print(stream);
	}
}