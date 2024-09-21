#ifndef AST_ENUMERATOR_HPP
#define AST_ENUMERATOR_HPP

#include "ast_node.hpp"

class Enumerator : public Node
{
public:
	Enumerator(std::string identifier, ExprNode *constant)
		: identifier_(identifier), constant_(constant)
	{std::cout << "Enumerator" << std::endl;}
	~Enumerator()
	{
		delete constant_;
	}
	void EmitCFG(IR &ir, IRGenContext &ctx) const override;
	void Print(std::ostream &stream) const override;

	std::string GetName();
private:
	std::string identifier_;
	ExprNode *constant_;
};

class EnumeratorList : public Node
{
public:
	EnumeratorList(NodeList *enumerator_list, Node *enumerator)
		: enumerator_list_(enumerator_list), enumerator_(enumerator)
	{
		std::cout << "EnumeratorList" << std::endl;
	}
	~EnumeratorList()
	{
		delete enumerator_list_;
		delete enumerator_;
	}

	void EmitCFG(IR &ir, IRGenContext &ctx) const override;
	void Print(std::ostream &stream) const override;

	//std::vector<Enumerator *> GetEnumerators();
private:
	NodeList *enumerator_list_;
	Node *enumerator_;
};

class EnumSpecifier : public Node
{
public:
	EnumSpecifier(std::string identifier, NodeList *enumerator_list)
		: identifier_(identifier), enumerator_list_(enumerator_list)
	{std::cout << "EnumSpecifier" << std::endl;}
	~EnumSpecifier()
	{

		delete enumerator_list_;
	}

	void EmitCFG(IR &ir, IRGenContext &ctx) const override;
	void Print(std::ostream &stream) const override;
private:	
	std::string identifier_;
	NodeList *enumerator_list_;
};
#endif