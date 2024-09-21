#include <iostream>
#include <stdlib.h>
#include <utility>
#include "ir_els.hpp"



void Return::EmitRISC(std::ostream &stream, CodeGenContext &ctx) const
{
    std::cout << "loading in Return" << std::endl;
    int op_reg = ctx.load_op_into_reg(op_, op_type_, stream);
    if (op_type_.t == Type::INT || op_type_.t == Type::UNSIGNED || op_type_.t == Type::CHAR) {
        
        stream << "mv a0, x" << op_reg << std::endl;
    } else if (op_type_.t == Type::FLOAT || op_type_.t == Type::DOUBLE) {
        
        std::string mv_op = (op_type_.t == Type::DOUBLE) ? "fmv.d" : "fmv.s";
        stream << mv_op << " fa0, f" << op_reg << std::endl;
    }
    ctx.free_op_reg(op_reg, op_, op_type_);

	stream << "j " << ctx.get_ret_label() << std::endl;
}

void Return::Print(std::ostream &stream) const
{
    stream << "ret ";
    print_op_type(stream, op_type_);
    stream << " ";
    op_.Print(stream);
    stream << std::endl;
}



void Store::EmitRISC(std::ostream &stream, CodeGenContext &ctx) const
{
    std::cout << "emit risc for store" << std::endl;
    int op_reg = ctx.load_op_into_reg(op_, op_type_, stream);
    if (op_type_.p_level > 0 || op_type_.t == Type::INT || op_type_.t == Type::UNSIGNED || op_type_.t == Type::CHAR) {
        if (op_type_.t == Type::CHAR) {
            std::cout << "storing char, ptr level: " << op_type_.p_level << std::endl;
        }
        if (deref_vr_) {
            std::cout << "storing at address pointed to by vr: " << vr_ << std::endl;
        }
        ctx.store_int_var(vr_, op_reg, (((op_type_.t == Type::CHAR && op_type_.p_level == 0) || (op_type_.t == Type::CHAR && op_type_.p_level == 1 && deref_vr_)) ? 1 : 4), deref_vr_, stream);
    } else if (op_type_.t == Type::FLOAT || op_type_.t == Type::DOUBLE) {
        std::string store_op = (op_type_.t == Type::DOUBLE) ? "fsd" : "fsw";
        ctx.store_float_var(vr_, op_reg, (op_type_.t == Type::DOUBLE), deref_vr_, stream);
    }
    std::cout << "store trying to free reg " << op_.val << std::endl;
    if (deref_vr_) {
        ctx.free_reg(vr_);
    }
    ctx.free_op_reg(op_reg, op_, op_type_);
}

void Store::Print(std::ostream &stream) const
{
    stream << "store ";
    if (get_pointer_) {
        stream << "ptr ";
    }

    print_op_type(stream, op_type_);
    stream << " ";
    op_.Print(stream);
    stream << ", ptr %" << vr_ << ", align " << get_op_align(op_type_) << std::endl;
}

void Load::EmitRISC(std::ostream &stream, CodeGenContext &ctx) const
{
    if (op_type_.p_level > 0 || op_type_.t == Type::INT || op_type_.t == Type::UNSIGNED || op_type_.t == Type::CHAR) {
        ctx.set_reg(vr_to_);
        if (op_type_.t == Type::CHAR) {
            std::cout << "loading char, ptr level: " << op_type_.p_level  << ((from_pointer_) ? "(from pointer)" : "()") << std::endl;
        }
        std::string load_op = ((op_type_.t == Type::CHAR && op_type_.p_level == 0) || (op_type_.t == Type::CHAR && op_type_.p_level == 1 && from_pointer_)) ? "lb" : "lw";
        if (from_pointer_) {
            stream << load_op << " x" << ctx.get_reg(vr_to_) << ", 0(x" << ctx.get_reg(vr_from_) << ")" << std::endl;
        } else if (ctx.get_vr_offset(vr_from_) == -1) {
            int tmp_reg = ctx.get_tmp_reg();
            stream << "lui x" << tmp_reg << ", %hi(.gd" << vr_from_ << ")" << std::endl;
            stream << load_op << " x" << ctx.get_reg(vr_to_) << ", %lo(.gd" << vr_from_ << ")(x" << tmp_reg << ")" << std::endl;

            
            ctx.free_tmp_reg(tmp_reg);
        } else {
            stream << load_op << " x" << ctx.get_reg(vr_to_) << ", " << ctx.get_vr_offset(vr_from_) << "(s0)" << std::endl;
        }
    } else if (op_type_.t == Type::FLOAT || op_type_.t == Type::DOUBLE) {
        ctx.set_float_reg(vr_to_);
        std::string load_op = (op_type_.t == Type::DOUBLE) ? "fld" : "flw";
        if (from_pointer_) {
            stream << load_op << " f" << ctx.get_float_reg(vr_to_) << ", 0(x" << ctx.get_reg(vr_from_) << ")" << std::endl;
        } else if (ctx.get_vr_offset(vr_from_) == -1) {
            int tmp_reg = ctx.get_tmp_reg();
            stream << "lui x" << tmp_reg << ", %hi(.gd" << vr_from_ << ")" << std::endl;
            stream << load_op << " f" << ctx.get_float_reg(vr_to_) << ", %lo(.gd" << vr_from_ << ")(x" << tmp_reg << ")" << std::endl;
            ctx.free_tmp_reg(tmp_reg);
        } else {
            
            stream << load_op << " f" << ctx.get_float_reg(vr_to_) << ", " << ctx.get_vr_offset(vr_from_) << "(s0)" << std::endl;
        }
    }
}

void Load::Print(std::ostream &stream) const
{
    stream << "%" << vr_to_ << " = load ";
    print_op_type(stream, op_type_);
    stream << ", " << ((from_pointer_) ? " ptr " : "") << " ptr %" << vr_from_ << ", align " << get_op_type_size(op_type_) << std::endl;
}

void ArithmeticOp::EmitRISC(std::ostream &stream, CodeGenContext &ctx) const
{
    std::cout << "arithmetic op '" << operation_ << "': type: ";
    print_op_type(std::cout, op_type_);
    stream << std::endl;
    if (op_type_.p_level > 0 || op_type_.t == Type::INT || op_type_.t == Type::UNSIGNED || op_type_.t == Type::CHAR) {
        std::cout << "setting in int arith" << std::endl;
        ctx.set_reg(vr_out_);
        Op op1 = op1_;
        Op op2 = op2_;
        // there can only be one constant since otherwise the CFG generation would already have calculated
        // the result of the addition
        std::cout << "arithmetic type ";
        print_op_type(std::cout, op_type_);
        // stream << std::endl;
        // if (op_type_.p_level > 0 || op_type_.num_els > 0) {
        //     std::cout << "pointer arithmetic" << std::endl;
        //     if (op1.src == OpSrc::con) {
        //         op1.val *= get_op_type_size({.t = op_type_.t, .p_level = 0, .num_els = 0});
        //     }

        //     if (op2.src == OpSrc::con) {
        //         op2.val *= get_op_type_size({.t = op_type_.t, .p_level = 0, .num_els = 0});
        //     }
        // }

        int op1_reg = ctx.load_op_into_reg(op1, op_type_, stream);
        int op2_reg = ctx.load_op_into_reg(op2, op_type_, stream);


        // if (op1.src == OpSrc::con && operation_ == "add" && op1.val >= -2048 && op1.val <= 2047) {
        //     stream << operation_ << "i x" << ctx.get_reg(vr_out_) << ", x" << ctx.get_reg(op2.val) << ", " << op1.val << std::endl;
        //     ctx.free_reg(op2.val);
        // } else if (op2.src == OpSrc::con && (operation_ == "add") && op2.val >= -2048 && op2.val <= 2047) {
        //     stream << operation_ << "i x" << ctx.get_reg(vr_out_) << ", x" << ctx.get_reg(op1.val) << ", " << op2.val << std::endl;
        //     ctx.free_reg(op1.val);

        // } else {
        // int tmp_reg1 = -1, tmp_reg2 = -1;
        // if (op1.src == OpSrc::con) {
        //     tmp_reg1 = ctx.get_tmp_reg();

        //     ctx.load_int_const_into_reg(stream, tmp_reg1, op1.val);
        // }
        // if (op2.src == OpSrc::con) {
        //     tmp_reg2 = ctx.get_tmp_reg();
        //     ctx.load_int_const_into_reg(stream, tmp_reg2, op2.val);

        // }
            
        // stream << operation_
        //     << " x" << ctx.get_reg(vr_out_)
        //     << ", x" << ((tmp_reg1 == -1) ? ctx.get_reg(op1.val) : tmp_reg1)
        //     << ", x" << ((tmp_reg2 == -1) ? ctx.get_reg(op2.val) : tmp_reg2)
        //     << std::endl;
        stream << operation_
            << " x" << ctx.get_reg(vr_out_)
            << ", x" << op1_reg
            << ", x" << op2_reg
            << std::endl;
		if(!is_postcrement_)
       		ctx.free_op_reg(op1_reg, op1, op_type_);
        ctx.free_op_reg(op2_reg, op2, op_type_);
        // if (tmp_reg1 == -1)
        //     ctx.free_reg(op1.val);
        // else
        //     ctx.free_tmp_reg(tmp_reg1);
        
        // if (tmp_reg2 == -1)
        //     ctx.free_reg(op2.val);
        // else
        //     ctx.free_tmp_reg(tmp_reg2);
        // }
    } else if (op_type_.t == Type::FLOAT || op_type_.t == Type::DOUBLE) {
        ctx.set_float_reg(vr_out_);
        int op1_reg = ctx.load_op_into_reg(op1_, op_type_, stream);
        int op2_reg = ctx.load_op_into_reg(op2_, op_type_, stream);
        // int tmp_float_reg1 = -1, tmp_float_reg2 = -1;
        // int tmp_reg = -1;
        // if (op1_.src == OpSrc::con) {
        //     tmp_float_reg1 = ctx.get_tmp_float_reg();

        //     ctx.load_float_const_into_reg(stream, tmp_float_reg1, (op_type_.t == Type::DOUBLE), op1_.val);
        // }
        // if (op2_.src == OpSrc::con) {
        //     tmp_float_reg2 = ctx.get_tmp_float_reg();
        //     ctx.load_float_const_into_reg(stream, tmp_float_reg2, (op_type_.t == Type::DOUBLE), op2_.val);
        // }


        stream << ArithmeticOp::to_float_operation(operation_, op_type_.t)
            << " f" << ctx.get_float_reg(vr_out_)
            << ", f" << op1_reg
            << ", f" << op2_reg
            << std::endl;
        ctx.free_op_reg(op1_reg, op1_, op_type_);
        ctx.free_op_reg(op2_reg, op2_, op_type_);
        // if (tmp_float_reg1 == -1)
        //     ctx.free_float_reg(op1_.val);
        // else
        //     ctx.free_tmp_float_reg(tmp_float_reg1);
        
        // if (tmp_float_reg2 == -1)
        //     ctx.free_float_reg(op2_.val);
        // else
        //     ctx.free_tmp_float_reg(tmp_float_reg2);
    }
}

void ArithmeticOp::Print(std::ostream &stream) const
{
    stream << "%" << vr_out_ << " = " << operation_ << " ";
    print_op_type(stream, op_type_);
    stream << " ";
    op1_.Print(stream);
    stream << ", ";
    op2_.Print(stream);
    stream << std::endl;
}

std::string ArithmeticOp::to_float_operation(std::string operation, Type type)
{
    if (type == Type::FLOAT) {
        return "f" + operation + ".s";
    } else if (type == Type::DOUBLE) {
        return "f" + operation + ".d";
    }
}



void BitwiseOp::EmitRISC(std::ostream &stream, CodeGenContext &ctx) const // check this
{
	bool shift = false;
	std::string shift_op = "";
	//if(op_type_.t == "shl") // this is fine
	if (operation_ == "shr" ){

		// check for unsigned/signed and then lshr or ashr
		// for now always sra and sll 
		// also broken for !0
		shift = true;
		if (op_type_.t == Type::UNSIGNED)
			shift_op = "srl";
		else 
			shift_op = "sra";
	}
	else if (operation_ == "shl"){
		shift = true;
		shift_op = "sll";
	}
    std::cout << "setting in bitwise" << std::endl;
	ctx.set_reg(vr_out_);
	
    if (op1_.src == OpSrc::con && op2_.src == OpSrc::con) {
		std::cerr << "ERROR: Two constants in logical operation" << std::endl;
        exit(1);
    } else {
        // int tmp_reg1 = -1, tmp_reg2 = -1;
        // if (op1_.src == OpSrc::con) {
        //     tmp_reg1 = ctx.get_tmp_reg();

        //     ctx.load_int_const_into_reg(stream, tmp_reg1, op1_.val);
        // }
        // if (op2_.src == OpSrc::con) {
        //     tmp_reg2 = ctx.get_tmp_reg();
        //     ctx.load_int_const_into_reg(stream, tmp_reg2, op2_.val);

        // }

        int op1_reg = ctx.load_op_into_reg(op1_, op_type_, stream);
        int op2_reg = ctx.load_op_into_reg(op2_, op_type_, stream);
        if(shift)
            stream << shift_op;
        else 
            stream << operation_;
        
        stream	<< " x" << ctx.get_reg(vr_out_)
                << ", x" << op1_reg
                << ", x" << op2_reg
                << std::endl;
        
        ctx.free_op_reg(op1_reg, op1_, op_type_);
        ctx.free_op_reg(op2_reg, op2_, op_type_);
        // if (tmp_reg1 == -1)
        //     ctx.free_reg(op1_.val);
        // else
        //     ctx.free_tmp_reg(tmp_reg1);
        
        // if (tmp_reg2 == -1)
        //     ctx.free_reg(op2_.val);
        // else
        //     ctx.free_tmp_reg(tmp_reg2);
    }
}

void BitwiseOp::Print(std::ostream &stream) const // change this later
{
	stream << "%" << vr_out_ << " = " << operation_ << " ";
    print_op_type(stream, op_type_);
	op1_.Print(stream);
	stream << ", ";
	op2_.Print(stream);
	stream << std::endl;
}

void LogicalOp::EmitRISC(std::ostream &stream, CodeGenContext &ctx) const
{
    std::cout << "setting in logical" << std::endl;
	ctx.set_reg(vr_out_);

	int op1_reg = ctx.load_op_into_reg(op1_, op_type_, stream);
    int op2_reg = ctx.load_op_into_reg(op2_, op_type_, stream);

    if (op_type_.t == Type::INT || op_type_.t == Type::UNSIGNED || op_type_.t == Type::CHAR)  { // TODO make work for unsigned
        // int tmp_reg1 = -1, tmp_reg2 = -1;
        // if (op1_.src == OpSrc::con) {
        //     tmp_reg1 = ctx.get_tmp_reg();
        //     ctx.load_int_const_into_reg(stream, tmp_reg1, op1_.val);
        // } else if (op2_.src == OpSrc::con) {
        //     tmp_reg2 = ctx.get_tmp_reg();
        //     ctx.load_int_const_into_reg(stream, tmp_reg2, op2_.val);
        // }
        
        if (operation_ == "and") {
            // and op1 and op2 then check if result is greater than 0
            int label_num0 = ctx.get_label_num();
            int label_num1 = ctx.get_label_num();
            stream << "beqz x" << op1_reg << ", .tmpL" << label_num0 << std::endl
            	<< "beqz x" << op2_reg << ", .tmpL" << label_num0 << std::endl;
			ctx.load_int_const_into_reg(stream, ctx.get_reg(vr_out_), 1);
            stream << "j .tmpL" << label_num1 << std::endl
            	<< ".tmpL" << label_num0 << ":" << std::endl;
				ctx.load_int_const_into_reg(stream, ctx.get_reg(vr_out_), 0);
            	stream<< ".tmpL" << label_num1 << ":" << std::endl;
            // check if any equal to zero and set output to 0 or 1

        } else if (operation_ == "or") {
            stream << "or x" << ctx.get_reg(vr_out_) 
            << ", x" << op1_reg
            << ", x" << op2_reg << std::endl;
            stream << "sgt x" << ctx.get_reg(vr_out_) << ", x"
            << ctx.get_reg(vr_out_) << ", x0" << std::endl;
            // or op1 and op2 then check greater than 0

        } else if (operation_ == "cmp eq") {
            stream << "xor x" << ctx.get_reg(vr_out_) 
                << ", x" << op1_reg
                << ", x" << op2_reg << std::endl;

            stream << "seqz x" << ctx.get_reg(vr_out_) << ", x" 
                << ctx.get_reg(vr_out_) << std::endl;
            
            
            // xor op1 and op2 then check if result is 0
        } else if (operation_ == "cmp ne") {
            stream << "xor x" << ctx.get_reg(vr_out_) 
            << ", x" << op1_reg
            << ", x" << op2_reg << std::endl;
            stream << "sgt x" << ctx.get_reg(vr_out_) << ", x" 
            << ctx.get_reg(vr_out_) << ", x0" << std::endl;
            // xor op1 and op2 then check if result is greater than 0
        } else if (operation_ == "cmp lt") {
			if (op_type_.t == Type::UNSIGNED)
				stream << "sltu x" << ctx.get_reg(vr_out_);
			else
            	stream << "slt x" << ctx.get_reg(vr_out_); 
			stream
            << ", x" << op1_reg
            << ", x" << op2_reg << std::endl;
            // set less than and put in output register
        } else if (operation_ == "cmp le") {
			if (op_type_.t == Type::UNSIGNED)
				stream << "sgtu x" << ctx.get_reg(vr_out_);
			else
				stream << "sgt x" << ctx.get_reg(vr_out_);
            stream
                << ", x" << op1_reg
                << ", x" << op2_reg << std::endl;
            stream << "seqz x" << ctx.get_reg(vr_out_) << ", x"
            << ctx.get_reg(vr_out_) << std::endl;
            // check if op1 is greater than op2 and then invert value.

        } else if (operation_ == "cmp gt") {
			if (op_type_.t == Type::UNSIGNED)
				stream << "sgtu x" << ctx.get_reg(vr_out_);
			else
				stream << "sgt x" << ctx.get_reg(vr_out_);

            stream 
            << ", x" << op1_reg
            << ", x" << op2_reg << std::endl;
            // set greater than and put in output register
        } else if (operation_ == "cmp ge") {
			if (op_type_.t == Type::UNSIGNED)
				stream << "sltu x" << ctx.get_reg(vr_out_);
			else
				stream << "slt x" << ctx.get_reg(vr_out_);
            stream
            << ", x" << op1_reg
            << ", x" << op2_reg << std::endl;
            stream << "seqz x" << ctx.get_reg(vr_out_) << ", x"
            << ctx.get_reg(vr_out_) << std::endl;
            // check if op1 is less than op2 and then invert value.
        }
        // if (tmp_reg1 == -1)
        //     ctx.free_reg(op1_.val);
        // else
        //     ctx.free_tmp_reg(tmp_reg1);
        
        // if (tmp_reg2 == -1)
        //     ctx.free_reg(op2_.val);
        // else
        //     ctx.free_tmp_reg(tmp_reg2);
    } else if (op_type_.t == Type::FLOAT || op_type_.t == Type::DOUBLE) {
        // int tmp_float_reg1 = -1, tmp_float_reg2 = -1;
        // if (op1_.src == OpSrc::con) {
        //     tmp_float_reg1 = ctx.get_tmp_reg();
        //     ctx.load_float_const_into_reg(stream, tmp_float_reg1, (op_type_.t == Type::DOUBLE), op1_.val);
        // } else if (op2_.src == OpSrc::con) {
        //     tmp_float_reg2 = ctx.get_tmp_reg();
        //     ctx.load_float_const_into_reg(stream, tmp_float_reg2, (op_type_.t == Type::DOUBLE), op2_.val);
        // }
        
        if (operation_ == "and") {
            std::cerr << "float logical and not implemented" << std::endl; // not to be done
            exit(1);
        } else if (operation_ == "or") {
            std::cerr << "float logical or not implemented" << std::endl; // not to be done
            exit(1);
        } else if (operation_ == "cmp eq") {
            // IMPORTANT: output register is not a float
            stream << "feq." << ((op_type_.t == Type::DOUBLE)  ? "d" : "s") << " x" << ctx.get_reg(vr_out_) << ", "
                << "f" << op1_reg << ", "
                << "f" << op2_reg << std::endl;
			stream << "snez x" << ctx.get_reg(vr_out_) << ", x" 
				<< ctx.get_reg(vr_out_) << std::endl;
			stream << "andi x" << ctx.get_reg(vr_out_) << ", x"
				<< ctx.get_reg(vr_out_) << ", 0xFF" << std::endl;
			// set equal and put in output register
        } else if (operation_ == "cmp ne") {
            //std::cerr << "float logical cmp ne not implemented" << std::endl;
            stream << "feq." << ((op_type_.t == Type::DOUBLE)  ? "d" : "s") <<" x" << ctx.get_reg(vr_out_) << ", "
				<< "f" << op1_reg << ", "
				<< "f" << op2_reg << std::endl;
			stream << "seqz x" << ctx.get_reg(vr_out_) << ", x" // IMPORTANT: output register is not a float
				<< ctx.get_reg(vr_out_) << std::endl;
			stream << "andi x" << ctx.get_reg(vr_out_) << ", x"
				<< ctx.get_reg(vr_out_) << ", 0xFF" << std::endl;
			// set not equal and put in output register
        } else if (operation_ == "cmp lt") {
            // set less than and put in output register
            //std::cerr << "float logical cmp lt not implemented" << std::endl;
			stream << "flt." << ((op_type_.t == Type::DOUBLE)  ? "d" : "s") << " x" << ctx.get_reg(vr_out_) << ", " // problem possibly
				<< "f" << op1_reg << ", "
				<< "f" << op2_reg << std::endl;
			stream << "snez x" << ctx.get_reg(vr_out_) << ", x" // IMPORTANT: output register is not a float
				<< ctx.get_reg(vr_out_) << std::endl;
			
            
        } else if (operation_ == "cmp le") {
            //std::cerr << "float logical cmp le not implemented" << std::endl;
            stream << "fle." << ((op_type_.t == Type::DOUBLE)  ? "d" : "s") << " x" << ctx.get_reg(vr_out_) << ", " // problem possibly
				<< "f" << op1_reg << ", "
				<< "f" << op2_reg << std::endl;
			//stream << "snez x" << ctx.get_reg(vr_out_) << ", x" // IMPORTANT: output register is not a float
			//	<< ctx.get_reg(vr_out_) << std::endl;
		
        } else if (operation_ == "cmp gt") {
            //std::cerr << "float logical cmp gt not implemented" << std::endl;
            stream << "fgt." << ((op_type_.t == Type::DOUBLE)  ? "d" : "s") << " x" << ctx.get_reg(vr_out_) << ", " // problem possibly
				<< "f" << op1_reg << ", "
				<< "f" << op2_reg << std::endl;
			stream << "snez x" << ctx.get_reg(vr_out_) << ", x" // IMPORTANT: output register is not a float
				<< ctx.get_reg(vr_out_) << std::endl;

            // set greater than and put in output register
        } else if (operation_ == "cmp ge") {
            //std::cerr << "float logical cmp ge not implemented" << std::endl;
            stream << "fge." << ((op_type_.t == Type::DOUBLE)  ? "d" : "s") << " x" << ctx.get_reg(vr_out_) << ", " // problem possibly
				<< "f" << op1_reg << ", "
				<< "f" << op2_reg << std::endl;
			stream << "seqz x" << ctx.get_reg(vr_out_) << ", x" // IMPORTANT: output register is not a float
				<< ctx.get_reg(vr_out_) << std::endl;
        }
        // if (tmp_float_reg1 == -1)
        //     ctx.free_float_reg(op1_.val);
        // else
        //     ctx.free_tmp_float_reg(tmp_float_reg1);
        
        // if (tmp_float_reg2 == -1)
        //     ctx.free_float_reg(op2_.val);
        // else
        //     ctx.free_tmp_float_reg(tmp_float_reg2);
    }
    
	ctx.free_op_reg(op1_reg, op1_, op_type_);
    ctx.free_op_reg(op2_reg, op2_, op_type_);
}

void LogicalOp::Print(std::ostream &stream) const
{
	stream << "%" << vr_out_ << " = " << operation_ << " ";
    print_op_type(stream, op_type_);
    stream << " ";
	op1_.Print(stream);
	stream << ", ";
	op2_.Print(stream);
	stream << std::endl;
}

void BranchOp::EmitRISC(std::ostream &stream, CodeGenContext &ctx) const
{
	std::cout << "in branchop block false == " << block_false_ << std::endl;
	if(block_false_ == -1){
		stream << "j ." << fn_name_ << "_l" << block_true_ << std::endl;
		return;
	}
	int tmp_reg1 = -1;
    int cond_op_reg = ctx.load_op_into_reg(condition_, op_type_, stream);
	// if (condition_.src == OpSrc::con) {
	// 		tmp_reg1 = ctx.get_tmp_reg();
	// 		ctx.load_int_const_into_reg(stream, tmp_reg1, condition_.val);
	// 	}
	stream << "beqz x"
        << cond_op_reg 
        << ", ." << fn_name_ << "_l" << block_false_ << std::endl;
	stream << "j ." << fn_name_ << "_l" << block_true_ << std::endl; // was commented
	ctx.free_op_reg(cond_op_reg, condition_, op_type_);
}

void BranchOp::Print(std::ostream &stream) const // change later
{
	stream << "br i1 ";
	condition_.Print(stream);
	stream << ", label %L" << block_true_ << ", label %L" << block_false_ << std::endl;
}

void CallOp::emit_call_arg_setup(std::ostream &stream, CodeGenContext &ctx) const
{
    int cur_float_arg_reg = 0, cur_arg_reg = 0;

    for (int i = 0; i < args_.size(); i++) {
        int arg_reg = ctx.load_op_into_reg(args_[i], decl_->param_types[i], stream);
        if (decl_->param_types[i].t == Type::FLOAT) {
            std::cout << "float arg" << std::endl;
            // if (args_[i].src == OpSrc::con) {
            //     ctx.load_float_const_into_reg(stream, 10 + cur_float_arg_reg++,
            //         false, args_[i].val);
            // } else {
            stream << "fmv.s fa" << cur_float_arg_reg++ << ", f" << arg_reg << std::endl;
            // }
        } else if (decl_->param_types[i].t == Type::DOUBLE) {
            // if (args_[i].src == OpSrc::con) {
            //     ctx.load_float_const_into_reg(stream, 10 + cur_float_arg_reg++,
            //         true, args_[i].val);
            // } else {
            stream << "fmv.d fa" << cur_float_arg_reg++ << ", f" << arg_reg << std::endl;
            // }
        } else if (decl_->param_types[i].t == Type::INT || decl_->param_types[i].t == Type::UNSIGNED
                || decl_->param_types[i].t == Type::CHAR || decl_->param_types[i].p_level > 0) {
            // std::cout << "int arg" << std::endl;
            // if (args_[i].src == OpSrc::con) {
            //     std::cout << "constant: " << args_[i].val << std::endl;
            //     ctx.load_int_const_into_reg(stream, 10 + cur_arg_reg++, args_[i].val);
            // } else {
            //     std::cout << "variable" << std::endl;
            stream << "mv a" << cur_arg_reg++ << ", x" << arg_reg << std::endl;
            // }
        } else {
            std::cerr << "unknown function argument type" << std::endl;
            exit(1);
        }
        if (args_[i].src != OpSrc::vr) {
            if (decl_->param_types[i].t == Type::FLOAT || decl_->param_types[i].t == Type::DOUBLE)
                ctx.free_tmp_float_reg(arg_reg);
            else
                ctx.free_tmp_reg(arg_reg);
        }
    }
}

void CallOp::EmitRISC(std::ostream &stream, CodeGenContext &ctx) const
{
    //if (fn_name_ == ctx.get_current_fn()) {
        std::cout << "saving registers" << std::endl;
		ctx.save_registers(stream);
	//}
    if (decl_->ret_type.t == Type::INT) {
        emit_call_arg_setup(stream, ctx);
        stream << "call " << fn_name_ << std::endl;
        //if (fn_name_ == ctx.get_current_fn()) {
            std::cout << "loading registers" << std::endl;
            ctx.load_registers(stream);
        //}
        std::cout << "setting in call" << std::endl;
        ctx.set_reg(vr_out_);
        stream << "mv x" << ctx.get_reg(vr_out_) << ", a0" << std::endl;
    } else if (decl_->ret_type.t == Type::FLOAT || decl_->ret_type.t == Type::DOUBLE) {
        emit_call_arg_setup(stream, ctx);
        stream << "call " << fn_name_ << std::endl;
        std::string mv_op = (decl_->ret_type.t == Type::DOUBLE) ? "fmv.d" : "fmv.s";

        //if (fn_name_ == ctx.get_current_fn()) {
            std::cout << "loading registers" << std::endl;
            ctx.load_registers(stream);
        //}
        ctx.set_float_reg(vr_out_);
        stream << mv_op << " f" << ctx.get_float_reg(vr_out_) << ", fa0" << std::endl;
    } else if (decl_->ret_type.t == Type::VOID) {
        emit_call_arg_setup(stream, ctx);
        stream << "call " << fn_name_ << std::endl;
        //if (fn_name_ == ctx.get_current_fn()) {
            std::cout << "loading registers" << std::endl;
            ctx.load_registers(stream);
        //}
    }
}

void CallOp::Print(std::ostream &stream) const
{
    stream << "%" << vr_out_ << " = call " << fn_name_ << "(";
    for (Op o : args_) {
        o.Print(stream);
        stream << ",";
    }
    stream << ")" << std::endl;
}



void SwitchOp::EmitRISC(std::ostream &stream, CodeGenContext &ctx) const
{
	int cond_reg = ctx.load_op_into_reg(condition_, op_type_, stream);
    for (auto [val, block] : case_map_) {
        Op const_val = {.src = OpSrc::con, .val = val};
        int const_reg = ctx.load_op_into_reg(const_val, op_type_, stream);
        stream << "beq x" << cond_reg << ", x" << const_reg << ", ." << ctx.get_current_fn() << "_l" << block << std::endl;
        ctx.free_op_reg(const_reg, const_val, op_type_);
    }
	stream << "j ." << ctx.get_current_fn() << "_l" << default_block_ << std::endl;
    ctx.free_op_reg(cond_reg, condition_, op_type_);
}

void SwitchOp::add_case(int val, int block_num)
{
    case_map_.push_back({val, block_num});
}

void SwitchOp::set_default(int default_block)
{
    default_block_ = default_block;
}

void SwitchOp::Print(std::ostream &stream) const
{
	
}

