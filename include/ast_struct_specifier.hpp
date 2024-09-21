#ifndef AST_STRUCT_SPECIFIER_HPP
#define AST_STRUCT_SPECIFIER_HPP
#include "ast_node.hpp"

class StructSpecifier : public Node
{
public:
	StructSpecifier(Node *ident, NodeList *struct_decl_list)
	: ident_ (ident),
	  struct_decl_list_ (struct_decl_list)
	{}
	
	~StructSpecifier(){
		if (struct_decl_list_ != nullptr) {
			delete struct_decl_list_;
		}

		if (ident_ != nullptr) {
			delete ident_;
		}
	}

	void EmitCFG(IR &ir, IRGenContext &ctx) const override;
    void Print(std::ostream &stream) const override;
private:
	Node *ident_;
	Node *struct_decl_list_;
};



#endif