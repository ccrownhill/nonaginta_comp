#ifndef AST_STRUCT_DECLARATION_HPP
#define AST_STRUCT_DECLARATION_HPP
#include "ast_node.hpp"

class StructDeclaration : public Node
{
public:
	StructDeclaration(Node *declaration_specifier, NodeList *declarator_list)
	: declaration_specifier_ (declaration_specifier),
	  declarator_list_ (declarator_list)
	{}
	
	~StructDeclaration(){
		delete declarator_list_;
		delete declaration_specifier_;
	}

	void EmitCFG(IR &ir, IRGenContext &ctx) const override;
    void Print(std::ostream &stream) const override;
private:
	Node *declaration_specifier_;
	Node *declarator_list_;
};



#endif