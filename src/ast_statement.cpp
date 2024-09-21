#include "ast_statement.hpp"
#include "ast_expression.hpp"
#include "ir.hpp"

void CompoundStatement::EmitCFG(IR &ir, IRGenContext &ctx) const
{
	ctx.enter_local_scope(ir);
    if (declarations_ != nullptr)
        declarations_->EmitCFG(ir, ctx);
    if (statements_ != nullptr)
    	statements_->EmitCFG(ir, ctx);
	ctx.leave_scope();
}

void CompoundStatement::Print(std::ostream &stream) const
{
	stream << "{" << std::endl;
    if (declarations_ != nullptr)
        declarations_->Print(stream);
    if (statements_ != nullptr)
        statements_->Print(stream);
	stream << "}" << std::endl;
}

void ExpressionStatement::EmitCFG(IR &ir, IRGenContext &ctx) const
{
    // don't care about return value of expression
	if (expression_ != nullptr)
		expression_->EmitCFG(ir, ctx, -1);
}

void ExpressionStatement::Print(std::ostream &stream) const
{
    if (expression_ != nullptr)
    {
        expression_->Print(stream);
    }
    stream << ";" << std::endl;
}

void ReturnStatement::EmitCFG(IR &ir, IRGenContext &ctx) const
{
    //context.SetTargetReg(10);
    // if (expression_ != nullptr)
    // {
    //     expression_->EmitCFG(ir, ctx);
    // }

    // TODO make this not rely on public member data
    Op ret_op = expression_->EmitCFG(ir, ctx, -1);
	std::cout << "retop is: " << ret_op.val << " " << ((ret_op.src == OpSrc::con) ? "constant" : "vr") << std::endl;
    CFGElPtr ret_el = std::make_shared<Return>(ctx.get_ret_type(ir), ret_op);
    ctx.add_CFGEl(ir, ret_el);
}

void ReturnStatement::Print(std::ostream &stream) const
{
    stream << "return";
    if (expression_ != nullptr)
    {
        stream << " ";
        expression_->Print(stream);
    }
    stream << ";" << std::endl;
}

void JumpStatement::EmitCFG(IR &ir, IRGenContext &ctx) const
{
	OpType btype = {.t = Type::INT, .p_level = 0, .num_els = 0, .type_l = {}};
	std::string cur_fn = ctx.get_current_fn();
	int topblock = ctx.get_block_num();
	CFGElPtr branch_unc;
	if (keyword_ == "break")
		branch_unc = std::make_shared<BranchOp>(btype,cur_fn, ctx.get_break_goto());
	else if (keyword_ == "continue")
		branch_unc = std::make_shared<BranchOp>(btype,cur_fn, ctx.get_continue_goto());
	ctx.add_CFGEl(ir, branch_unc);
	
	
}

void JumpStatement::Print(std::ostream &stream) const
{
	stream << keyword_ << ";" << std::endl;
}

void SelectionStatement::EmitCFG(IR &ir, IRGenContext &ctx) const
{
	
	OpType btype = {.t = Type::INT, .p_level = 0, .num_els = 0, .type_l = {}};
	// if no else statement
	// create a new block for the if statement
	Op cond = condition_->EmitCFG(ir, ctx, -1);
	
	int top_block_num = ctx.get_block_num();
	
	int if_body_block_num = ctx.add_cfg_block(ir);
	if_statement_->EmitCFG(ir, ctx);

	
	int else_body_block_num;
	if(else_statement_ != nullptr){
		
		else_body_block_num = ctx.add_cfg_block(ir);
		
		else_statement_->EmitCFG(ir, ctx);
		
	}
	int after_block_num = ctx.add_cfg_block(ir);

	std::string cur_fn = ctx.get_current_fn();
	if (else_statement_ != nullptr) {
		ctx.goto_block(if_body_block_num);
		CFGElPtr branch_unc1 = std::make_shared<BranchOp>(btype, cur_fn, after_block_num);
		// create a new block for the else statement
		ctx.add_CFGEl(ir, branch_unc1);
	}

	ctx.goto_block(top_block_num);
	CFGElPtr branch;
	if (else_statement_ != nullptr) {
		branch = std::make_shared<BranchOp>(btype, cur_fn, cond, if_body_block_num, else_body_block_num);
	} else {
		branch = std::make_shared<BranchOp>(btype, cur_fn, cond, if_body_block_num, after_block_num);
	}
	ctx.add_CFGEl(ir, branch);

	ctx.goto_block(after_block_num);
}

void SelectionStatement::Print(std::ostream &stream) const // probably wrong
{ 
	stream << "if (";
	condition_->Print(stream);
	stream << ") " << std::endl;
	if_statement_->Print(stream);
	if (else_statement_ != nullptr)
	{
		stream << "else " << std::endl;
		else_statement_->Print(stream);
	}

}

void IterationStatement::EmitCFG(IR &ir, IRGenContext &ctx) const
{	
	int block_num = ctx.get_block_num();

	std::string cur_fn = ctx.get_current_fn();
	OpType btype = {.t = Type::INT, .p_level = 0, .num_els = 0, .type_l = {}};
	if(is_do_) { // do while loop
		int body_block = ctx.add_cfg_block(ir);
		int after_block = ctx.add_cfg_block(ir);
		ctx.set_break_goto(after_block);
		ctx.set_continue_goto(body_block);
		ctx.goto_block(body_block);
		body_->EmitCFG(ir, ctx);
		Op cond = condition_->EmitCFG(ir, ctx, -1);
		//int after_block = ctx.add_cfg_block(ir);

		//ctx.goto_block(body_block);
		CFGElPtr branch = std::make_shared<BranchOp>(btype, cur_fn, cond, body_block, after_block);
		ctx.add_CFGEl(ir, branch);
		ctx.goto_block(after_block);
	} else { // while and for loop
		if (statement_before_ != nullptr)
			statement_before_->EmitCFG(ir, ctx);
		
		int cond_block = ctx.add_cfg_block(ir);
		int body_block = ctx.add_cfg_block(ir);
		int after_block = ctx.add_cfg_block(ir);
		ctx.set_break_goto(after_block);
		ctx.set_continue_goto(cond_block);

		ctx.goto_block(body_block);
		body_->EmitCFG(ir, ctx);


		if (statement_end_ != nullptr)
			statement_end_->EmitCFG(ir, ctx);
		
		// jump back to condition evaluation
		CFGElPtr branch_unc = std::make_shared<BranchOp>(btype, cur_fn, cond_block);
		ctx.add_CFGEl(ir, branch_unc);

		
		ctx.goto_block(cond_block);
		CFGElPtr branch;
		if (condition_ != nullptr) {
			Op cond = condition_->EmitCFG(ir, ctx, -1);
			branch = std::make_shared<BranchOp>(btype, cur_fn, cond, body_block, after_block);
		} else {
			branch = std::make_shared<BranchOp>(btype, cur_fn, body_block);
		}
		ctx.add_CFGEl(ir, branch);
		ctx.goto_block(after_block);
	}

}

void IterationStatement::Print(std::ostream &stream) const // fix this later
{

	if(is_do_) {
		stream << "do" << std::endl;
		body_->Print(stream);
		stream << "while (";
		condition_->Print(stream);
		stream << ")" << std::endl;
	} else {
		if (statement_before_ == nullptr && statement_end_ == nullptr) {
			stream << "while (";
		} else {
			stream << "for (";
		}
		if (statement_before_ != nullptr)
			statement_before_->Print(stream);
		stream << " ";
		if (condition_ != nullptr)
			condition_->Print(stream);
		stream << "; ";
		if (statement_end_ != nullptr)
			statement_end_->Print(stream);
		stream << ") ";
		body_->Print(stream);
		stream << std::endl;		
	}

}

void SwitchStatement::EmitCFG(IR &ir, IRGenContext &ctx) const
{
	Op cond_expr_eval = cond_expr_->EmitCFG(ir, ctx, -1);
	int cur_block = ctx.get_block_num();
	int after_block = ctx.add_cfg_block(ir);
	ctx.goto_block(cur_block);
	CFGElPtr switch_op = std::make_shared<SwitchOp>(ctx.get_type(), cond_expr_eval, after_block);
	ctx.add_CFGEl(ir, switch_op);
	ctx.set_break_goto(after_block);
	ctx.set_cur_switch(std::static_pointer_cast<SwitchOp>(switch_op));
	statement_->EmitCFG(ir, ctx);

	// add unconditional branch to after_block
	std::string cur_fn = ctx.get_current_fn();
	CFGElPtr branch_back = std::make_shared<BranchOp>(ctx.get_type(), cur_fn, after_block);
	ctx.add_CFGEl(ir, branch_back);
	ctx.goto_block(after_block);
}

void SwitchStatement::Print(std::ostream &stream) const
{
	stream << "switch (";
	cond_expr_->Print(stream);
	stream << ") ";
	statement_->Print(stream);
	stream << std::endl;
}

void CaseStatement::EmitCFG(IR &ir, IRGenContext &ctx) const
{
	std::shared_ptr<SwitchOp> cur_switch = ctx.get_cur_switch();
	int block_num = ctx.add_cfg_block(ir);
	statement_->EmitCFG(ir, ctx);
	if (const_expr_ != nullptr) { // case
		Op const_out = const_expr_->EmitCFG(ir, ctx, -1);
		cur_switch->add_case(const_out.val, block_num);
	} else { // default
		cur_switch->set_default(block_num);
	}
}

void CaseStatement::Print(std::ostream &stream) const
{
	if (const_expr_ != nullptr) {
		stream << "case ";
		const_expr_->Print(stream);
	} else {
		stream << "default";
	}
	stream << ":" << std::endl;
	statement_->Print(stream);
}
