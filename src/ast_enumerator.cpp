#include "ast_enumerator.hpp"
#include "ast_identifier.hpp"
#include "ast_constant.hpp"

void Enumerator::EmitCFG(IR &ir, IRGenContext &ctx) const
{
	// identifier_->EmitCFG(ir, ctx);
	// stream << ":" << std::endl;
	Op value;
	if(constant_ != nullptr)
	{
		std::cout << "Enumerator with assignment" << std::endl;
		ctx.toggle_eval_enum();
		value = constant_->EmitCFG(ir, ctx, -1); // this causes memory leak
		ctx.toggle_eval_enum();
		ctx.add_enum(ir, identifier_, value.val, ctx.get_current_enum());
		//ctx.add_enum(ir, identifier_, 5,ctx.get_current_enum());
	

	}
	if (constant_ == nullptr)
		ctx.add_enum(ir, identifier_, ctx.get_current_enum());
}

void Enumerator::Print(std::ostream &stream) const
{
	stream << "(";
	stream << identifier_;
	stream << "= ";
	//constant_->Print(stream); // this breaks
	stream << "),";
}

std::string Enumerator::GetName()
{
	return identifier_;
}

void EnumeratorList::EmitCFG(IR &ir, IRGenContext &ctx) const
{
	std::cout << "EnumeratorList" << std::endl; // should not happen
	enumerator_->EmitCFG(ir, ctx);
	if(enumerator_list_ != nullptr)
		enumerator_list_->EmitCFG(ir, ctx);
	
}

void EnumeratorList::Print(std::ostream &stream) const
{
	stream << "EnumeratorList(";
	enumerator_->Print(stream);
	if(enumerator_list_ != nullptr)
	{
		stream << ", ";
		enumerator_list_->Print(stream);
	}
	stream << ")";
}


void EnumSpecifier::EmitCFG(IR &ir, IRGenContext &ctx) const
{
	
	if(identifier_ == "")
		ctx.add_enum_var(ir, "enum");
	else{
		ctx.add_enum_var(ir, identifier_);
	}
	if(enumerator_list_ != nullptr)
		enumerator_list_->EmitCFG(ir, ctx);
	
	//enumerator_list_->EmitCFG(ir, ctx);
}

void EnumSpecifier::Print(std::ostream &stream) const
{
	stream << "EnumSpecifier ";
	std::cout << identifier_;
	stream << ", {";
	enumerator_list_->Print(stream);
	stream << "}";
}