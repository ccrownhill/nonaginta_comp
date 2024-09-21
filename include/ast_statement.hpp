#ifndef AST_STATEMENT_HPP
#define AST_STATEMENT_HPP

#include "ast_node.hpp"


class CompoundStatement : public Node
{
public:
	CompoundStatement(NodeList *declarations, NodeList *statements)
	: declarations_ (declarations),
	  statements_ (statements)
	{}
	
	~CompoundStatement(){
    	if (declarations_ != nullptr)
        	delete declarations_;
      	if (statements_ != nullptr)
		    delete statements_;
	}

	void EmitCFG(IR &ir, IRGenContext &ctx) const override;
  	void Print(std::ostream &stream) const override;
private:
	NodeList *declarations_;
	NodeList *statements_;
};

class ExpressionStatement : public Node
{
public:
    ExpressionStatement(ExprNode *expression) : expression_(expression) 
	{std::cout << "creating ExpressionStatement" << std::endl;}
    ~ExpressionStatement()
    {
        delete expression_;
    };
	
    void EmitCFG(IR &ir, IRGenContext &ctx) const override;
    void Print(std::ostream &stream) const override;
private:
    ExprNode *expression_;
};

class ReturnStatement : public Node
{
public:
    ReturnStatement(ExprNode *expression) : expression_(expression)
	{std::cout << "creating ReturnStatement"  << std::endl;
	if (expression_ == nullptr) {
		std::cout << "nullptr expression" << std::endl;
	}}
    ~ReturnStatement()
    {
        delete expression_;
    };

    void EmitCFG(IR &ir, IRGenContext &ctx) const override;
    void Print(std::ostream &stream) const override;
private:
    ExprNode *expression_;
};

class JumpStatement : public Node
{
public:
	JumpStatement(std::string keyword)
	: keyword_ (keyword)
	{std::cout << "creating JumpStatement" << std::endl;}
	
	~JumpStatement()
	{}


	void EmitCFG(IR &ir, IRGenContext &ctx) const override;
	void Print(std::ostream &stream) const override;
private:
	std::string keyword_;
};

class SelectionStatement : public Node
{
public:
	SelectionStatement(ExprNode *condition, Node *if_statement, Node *else_statement)
	: condition_ (condition),
	  if_statement_ (if_statement),
	  else_statement_ (else_statement)
	{std::cout << "creating SelectionStatement" << std::endl;}
	~SelectionStatement()
	{
		delete condition_;
		delete if_statement_;
		delete else_statement_;
	}

	void EmitCFG(IR &ir, IRGenContext &ctx) const override;
	void Print(std::ostream &stream) const override;
private:
	ExprNode *condition_;
	Node *if_statement_;
	Node *else_statement_;
};


class IterationStatement : public Node
{
public:
	IterationStatement(ExprNode *condition, Node *body, Node *statement_before, Node *statement_end, bool is_do) 
		: condition_ (condition), body_ (body),
		statement_before_ (statement_before), statement_end_ (statement_end), is_do_ (is_do)
	{std::cout << "creating IterationStatement" << std::endl;}
	~IterationStatement()
	{
		delete condition_;
		delete body_;
	}

	void EmitCFG(IR &ir, IRGenContext &ctx) const override;
	void Print(std::ostream &stream) const override;
private:
	ExprNode *condition_;
	Node *body_;
	Node *statement_before_;
	Node *statement_end_;
	bool is_do_;
};

class SwitchStatement : public Node
{
public:
	SwitchStatement(ExprNode *cond_expr, Node *statement)
		: cond_expr_(cond_expr), statement_ (statement)
	{}
	~SwitchStatement()
	{
		delete cond_expr_;
		delete statement_;
	}

	void EmitCFG(IR &ir, IRGenContext &ctx) const override;
	void Print(std::ostream &stream) const override;
private:
	ExprNode *cond_expr_;
	Node *statement_;
};

class CaseStatement : public Node
{
public:
	CaseStatement(ExprNode *const_expr, Node *statement)
		: const_expr_(const_expr), statement_ (statement)
	{}
	~CaseStatement()
	{
		if (const_expr_ != nullptr) {
			delete const_expr_;
		}
		delete statement_;
	}

	void EmitCFG(IR &ir, IRGenContext &ctx) const override;
	void Print(std::ostream &stream) const override;
private:
	ExprNode *const_expr_;
	Node *statement_;
};



#endif