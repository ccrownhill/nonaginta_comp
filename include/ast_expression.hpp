#ifndef AST_PRIMARY_EXPRESSION_HPP
#define AST_PRIMARY_EXPRESSION_HPP
#include "ast_node.hpp"
#include <string>

class AssignmentExpression : public ExprNode
{
public:
    AssignmentExpression(ExprNode *lhs_op, ExprNode *rhs_op)
        : lhs_op_ (lhs_op), rhs_op_ (rhs_op)
    {std::cout << "adding assignment expression" << std::endl;}
    ~AssignmentExpression()
    {
        delete lhs_op_;
        delete rhs_op_;
    }

    Op EmitCFG(IR &ir, IRGenContext &ctx, int lhs_vr) const override;
    void Print(std::ostream &stream) const override;

private:
    ExprNode *lhs_op_;
    ExprNode *rhs_op_;
};

class ArithmeticExpression : public ExprNode
{
public:
    ArithmeticExpression(const std::string &operation, ExprNode *op1, ExprNode *op2)
        : operation_ (operation), op1_ (op1), op2_ (op2)
    {}
    ~ArithmeticExpression()
    {
        delete op1_;
        delete op2_;
    }

    Op EmitCFG(IR &ir, IRGenContext &ctx, int lhs_vr) const override;
    void Print(std::ostream &stream) const override;

private:
    std::string operation_;
    ExprNode *op1_;
    ExprNode *op2_;
};


class PrimaryExpression : public ExprNode
{
public:
    PrimaryExpression(Node *value) : value_(value)
    {std::cout << "adding primary expression '" << value << "'" << std::endl;}

    ~PrimaryExpression(){
        delete value_;
    }

    Op EmitCFG(IR &ir, IRGenContext &ctx, int lhs_vr) const override;
    void Print(std::ostream &stream) const override;
    std::string get_name() const;

private:
    Node *value_;
};

class UnaryExpression : public ExprNode
{	
public:
	UnaryExpression(const std::string &operation, ExprNode *op)
		: operation_ (operation), op_ (op)
	{std::cout << "adding unary expression" << std::endl;}
	~UnaryExpression()
	{
		delete op_;
	}

	Op EmitCFG(IR &ir, IRGenContext &ctx, int lhs_vr) const override;
	void Print(std::ostream &stream) const override;

private:
	std::string operation_; 
	ExprNode *op_;
};

class SizeExpression : public ExprNode
{
public:
	SizeExpression(ExprNode *expression, Node *type)
		: expression_ (expression), type_ (type)
	{}
	~SizeExpression()
	{
		delete expression_;
		delete type_;
	}

	Op EmitCFG(IR &ir, IRGenContext &ctx, int lhs_vr) const override;
	void Print(std::ostream &stream) const override;

private:
	ExprNode *expression_;
	Node *type_;
};

class BitwiseExpression : public ExprNode
{
public:
	BitwiseExpression(const std::string &operation, ExprNode *op1, ExprNode *op2)
		: operation_ (operation), op1_ (op1), op2_ (op2)
	{}
	~BitwiseExpression()
	{
		delete op1_;
		delete op2_;
	}

	Op EmitCFG(IR &ir, IRGenContext &ctx, int lhs_vr) const override;
	void Print(std::ostream &stream) const override;

private:
	std::string operation_;
	ExprNode *op1_;
	ExprNode *op2_;
};

class LogicalExpression : public ExprNode
{
public:
	LogicalExpression(const std::string &operation, ExprNode *op1, ExprNode *op2)
		: operation_ (operation), op1_ (op1), op2_ (op2)
	{}
	~LogicalExpression()
	{
		delete op1_;
		delete op2_;
	}

	Op EmitCFG(IR &ir, IRGenContext &ctx, int lhs_vr) const override;
	void Print(std::ostream &stream) const override;

private:
	std::string operation_;
	ExprNode *op1_;
	ExprNode *op2_;
};

class FunctionCallExpression : public ExprNode
{	
public:
	FunctionCallExpression(ExprNode *prim_expr, ExprNodeList *args)
		: prim_expr_ (prim_expr), args_ (args)
	{std::cout << "function call expression" << std::endl;}
	~FunctionCallExpression()
	{
		delete args_;
	}

	Op EmitCFG(IR &ir, IRGenContext &ctx, int lhs_vr) const override;
	void Print(std::ostream &stream) const override;

private:
	ExprNode *prim_expr_;
	ExprNodeList *args_;
};

class PointerIndexExpression : public ExprNode
{
public:
	PointerIndexExpression(ExprNode *ptr_expr, ExprNode  *idx_expr)
	: ptr_expr_ (ptr_expr), idx_expr_ (idx_expr)
	{}

	~PointerIndexExpression()
	{
		delete ptr_expr_;
		delete idx_expr_;
	}

	Op EmitCFG(IR &ir, IRGenContext &ctx, int lhs_vr) const override;
	void Print(std::ostream &stream) const override;
	
private:
	ExprNode *ptr_expr_;
	ExprNode *idx_expr_;
};

class PostCrementExpression : public ExprNode
{
public:
	PostCrementExpression(const std::string &operation, ExprNode  *expr)
	: operation_ (operation), expr_ (expr)
	{}

	~PostCrementExpression()
	{
		delete expr_;
	}

	Op EmitCFG(IR &ir, IRGenContext &ctx, int lhs_vr) const override;
	void Print(std::ostream &stream) const override;
	
private:
	std::string operation_;
	ExprNode *expr_;
};

class StructAccessExpression : public ExprNode
{
public:
	StructAccessExpression(ExprNode *expr, Node *ident, bool is_pointer)
	: expr_ (expr), ident_ (ident), is_pointer_ (is_pointer)
	{}

	~StructAccessExpression()
	{
		delete expr_;
		delete ident_;
	}

	Op EmitCFG(IR &ir, IRGenContext &ctx, int lhs_vr) const override;
	void Print(std::ostream &stream) const override;
	
private:
	ExprNode *expr_;
	Node *ident_;
	bool is_pointer_;
};
class Initializer : public ExprNode
{
	public:
		Initializer(ExprNodeList *expr_list)
		: expr_list_ (expr_list)
		{}
		~Initializer()
		{
			delete expr_list_;
		}

		Op EmitCFG(IR &ir, IRGenContext &ctx, int lhs_vr) const override;
		void Print(std::ostream &stream) const override;
		
	private:
		ExprNodeList *expr_list_;
};

class InitializerList : public ExprNode
{
	public:
		InitializerList(ExprNodeList *initializer_list)
		: initializer_list_ (initializer_list)
		{}
		~InitializerList()
		{
			delete initializer_list_;
		}

		Op EmitCFG(IR &ir, IRGenContext &ctx, int lhs_vr) const override;
		void Print(std::ostream &stream) const override;

	private:
		ExprNodeList *initializer_list_;
};

#endif