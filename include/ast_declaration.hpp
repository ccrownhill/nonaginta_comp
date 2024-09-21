#ifndef AST_DECLARATION_HPP
#define AST_DECLARATION_HPP
#include "ast_node.hpp"


class Declaration : public Node
{
public:
	Declaration(Node *declaration_specifier, NodeList *init_declarator_list)
	: declaration_specifier_ (declaration_specifier),
	  init_declarator_list_ (init_declarator_list)
	{std::cout << "new declaration" << std::endl;}
	
	Declaration(Node *declaration_specifier)
	: declaration_specifier_ (declaration_specifier)
	{}
	~Declaration(){
		if(init_declarator_list_ != nullptr) 
			delete init_declarator_list_;
		delete declaration_specifier_;
	}

	void EmitCFG(IR &ir, IRGenContext &ctx) const override;
    void Print(std::ostream &stream) const override;
private:
	Node *declaration_specifier_;
	NodeList *init_declarator_list_;
};



#endif