#ifndef IR_ELS_HPP
#define IR_ELS_HPP

#include "ir.hpp"
#include "code_gen_context.hpp"


class Return : public CFGEl
{
public:
    Return(OpType op_type, Op op) : op_(op) {
        op_type_ = op_type;
    }
    ~Return() {}

    void EmitRISC(std::ostream &stream, CodeGenContext &ctx) const override;
    void Print(std::ostream &stream) const override;

private:
    Op op_;
};



class Store : public CFGEl
{
public:
    Store(OpType op_type, int vr, Op op, bool get_pointer, bool deref_vr)
		: vr_ (vr), op_ (op), get_pointer_(get_pointer), deref_vr_ (deref_vr)
	{
        op_type_ = op_type;
    }
    ~Store() {}

    void EmitRISC(std::ostream &stream, CodeGenContext &ctx) const override;
    void Print(std::ostream &stream) const override;
private:
    int vr_;
    Op op_;
	bool get_pointer_;
	bool deref_vr_;
};

class Load : public CFGEl
{
public:
    Load(OpType op_type, int vr_to, int vr_from, bool from_pointer)
		: vr_to_ (vr_to), vr_from_ (vr_from), from_pointer_(from_pointer)
    {
        op_type_ = op_type;
		if (from_pointer_)
			std::cout << "loading from pointer" << std::endl;
		else
			std::cout << "loading not from pointer" << std::endl;
    }
    ~Load() {}

    void EmitRISC(std::ostream &stream, CodeGenContext &ctx) const override;
    void Print(std::ostream &stream) const override;
private:
    int vr_to_;
    int vr_from_;
	bool from_pointer_;
};

class ArithmeticOp : public CFGEl
{
public:
	ArithmeticOp(OpType op_type, std::string operation, int vr_out, Op op1, Op op2, bool is_postcrement = false)
		: operation_(operation), vr_out_ (vr_out), op1_ (op1), op2_ (op2), is_postcrement_ (is_postcrement)
	{
		op_type_ = op_type;
	}

    ~ArithmeticOp() {}

    void EmitRISC(std::ostream &stream, CodeGenContext &ctx) const override;
    void Print(std::ostream &stream) const override;

    static std::string to_float_operation(std::string operation, Type type);
private:
    std::string operation_;
    int vr_out_;
    Op op1_;
    Op op2_;
	bool is_postcrement_;

};

// class UnaryOp : public CFGEl
// {

// public:
// 	UnaryOp(std::string op_type, int vr_out, Op op)
// 		: op_type_ (op_type), vr_out_ (vr_out), op_ (op)
// 	{}
// 	~UnaryOp() {}

// 	void EmitRISC(std::ostream &stream, CodeGenContext &ctx) const;
// 	void Print(std::ostream &stream) const;
// private:
// 	std::string op_type_;
// 	int vr_out_;
// 	Op op_;

// };

class BitwiseOp : public CFGEl
{
public:
	BitwiseOp(OpType op_type,std::string operation, int vr_out, Op op1, Op op2)
		: operation_(operation), vr_out_ (vr_out), op1_ (op1), op2_ (op2)
	{op_type_ = op_type;}

	~BitwiseOp() {}

	void EmitRISC(std::ostream &stream, CodeGenContext &ctx) const override;
	void Print(std::ostream &stream) const override;
private:
	std::string operation_;
	int vr_out_;
	Op op1_;
	Op op2_;

};

class LogicalOp : public CFGEl
{
public:
	LogicalOp(OpType op_type, std::string operation, int vr_out, Op op1, Op op2)
		: operation_(operation), vr_out_ (vr_out), op1_ (op1), op2_ (op2)
	{op_type_ = op_type;}

	~LogicalOp() {}

	void EmitRISC(std::ostream &stream, CodeGenContext &ctx) const override;
	void Print(std::ostream &stream) const override;
private:
	std::string operation_;
	int vr_out_;
	Op op1_;
	Op op2_;
};

class BranchOp : public CFGEl
{
public:
	BranchOp(OpType op_type, const std::string &fn_name, Op condition, int block_true, int block_false)
	: fn_name_ (fn_name), condition_ (condition), block_true_ (block_true), block_false_ (block_false) 
	{op_type_ = op_type;}
	BranchOp(OpType op_type, std::string &fn_name, int block)
	 // if false is -1 it is unconditional jump
	{
        fn_name_ = fn_name;
		block_true_ = block; block_false_ = -1;
		op_type_ = op_type;
	}

	void EmitRISC(std::ostream &stream, CodeGenContext &ctx) const override;
	void Print(std::ostream &stream) const override;
private:
    std::string fn_name_;
	Op condition_;
	int block_true_;
	int block_false_;
};

class CallOp : public CFGEl
{
public:
	CallOp(OpType op_type, const std::string &fn_name, int vr_out, std::vector<Op> &args, FunctionDeclPtr decl)
	: fn_name_ (fn_name), vr_out_(vr_out), args_(args), decl_ (decl)
	{op_type_ = op_type;}

	void EmitRISC(std::ostream &stream, CodeGenContext &ctx) const override;
	void Print(std::ostream &stream) const override;
    void emit_call_arg_setup(std::ostream &stream, CodeGenContext &ctx) const;
private:
    std::string fn_name_;
    int vr_out_;
    std::vector<Op> args_;
    FunctionDeclPtr decl_;
};

class LabelOp : public CFGEl
{
public:
	LabelOp(std::string label) : label_ (label) {}
	~LabelOp() {}

	void EmitRISC(std::ostream &stream, CodeGenContext &ctx) const override;
	void Print(std::ostream &stream) const override;
private:
	std::string label_;
};

class SwitchOp : public CFGEl
{
public:
	SwitchOp(OpType op_type, Op condition, int default_block)
	: condition_ (condition), default_block_(default_block)
	{
		op_type_ = op_type;
	}

	void add_case(int case_val, int block_num);
	void set_default(int default_block);
	void EmitRISC(std::ostream &stream, CodeGenContext &ctx) const override;
	void Print(std::ostream &stream) const override;
private:
	Op condition_;
	int default_block_;
	std::vector<std::pair<int, int>> case_map_; // case value to label number
};
#endif