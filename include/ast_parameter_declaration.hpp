#ifndef AST_PARAMETER_DECLARATION_HPP
#define AST_PARAMETER_DECLARATION_HPP
#include "ast_node.hpp"


class ParameterDeclaration : public Node
{
public:
	ParameterDeclaration(Node *declaration_specifier, Node *declarator)
	: declaration_specifier_ (declaration_specifier),
	  declarator_ (declarator)
	{}
	
	~ParameterDeclaration(){
		delete declarator_;
		delete declaration_specifier_;
	}

	void EmitCFG(IR &ir, IRGenContext &ctx) const override;
    void Print(std::ostream &stream) const override;
private:
	Node *declaration_specifier_;
	Node *declarator_;
};



#endif