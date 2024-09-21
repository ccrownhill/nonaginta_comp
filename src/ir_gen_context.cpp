#include "ir_gen_context.hpp"


void IRGenContext::add_fn(IR &ir, const std::string &name)
{
    current_fn_ = name;
    current_fn_decl_ = name;
    ir.add_fn(name, get_type());
    enter_local_scope(ir);
}

void IRGenContext::add_fn_decl(IR &ir, const std::string &name)
{
    current_fn_decl_ = name;
    ir.add_fn_decl(name, get_type());
}

int IRGenContext::add_cfg_block(IR &ir)
{
    block_num_ = ir.add_cfg_block(current_fn_);
    return block_num_;
}

void IRGenContext::goto_block(int block_num)
{
    block_num_ = block_num;
}

int IRGenContext::get_block_num() const
{
	return block_num_;
}

void IRGenContext::add_CFGEl(IR &ir, CFGElPtr cfg_el)
{
    ir.add_CFGEl(cfg_el, current_fn_, block_num_);
}


void IRGenContext::set_vr(const std::string &ident, OpType type)
{
    vr_num_++;
    scope_stack_.front()->insert({ident, std::make_pair(vr_num_, type)});
}

std::pair<int, OpType> IRGenContext::get_vr(const std::string &ident)
{
    for (ScopeMapPtr scope_m : scope_stack_) {
        if (scope_m->find(ident) != scope_m->end()) {
            return scope_m->at(ident);
        }
    }
	// try enum
	for(auto enum_map : enum_map_) // thinks 
	{
		// OpType type;
		// type.t = Type::INT
		if(enum_map.second.find(ident) != enum_map.second.end())
		{
			std::cout << "found enum" << std::endl;
			return {enum_map.second.at(ident), {Type::ENUM, 0, 0}}; // may want enum type
			
		}
	}
	// if(enum_map_.find(current_enum_) != enum_map_.end())
	
    std::cerr << "could not find identifier '" << ident << "' in any scope" << std::endl;
    exit(1);
}

void IRGenContext::enter_local_scope(IR &ir)
{
    ScopeMapPtr out = std::make_shared<ScopeMap>();
    scope_stack_.push_front(out);
    ir.add_fn_scope(current_fn_, out);
}

void IRGenContext::leave_scope()
{
    scope_stack_.pop_front();
}

int IRGenContext::next_vr_num()
{
    return ++vr_num_;
}

bool IRGenContext::is_evaluating_lhs() const
{
    return eval_lhs_;
}
void IRGenContext::start_eval_lhs()
{
    eval_lhs_ = true;
}
void IRGenContext::stop_eval_lhs()
{
    eval_lhs_ = false;
}



void IRGenContext::add_param(IR &ir, const std::string &name)
{
    ir.add_param(current_fn_, name, get_type());
}

void IRGenContext::add_arg_to_decl(IR &ir, OpType arg_type)
{
    std::cout << "adding param to function: " << current_fn_decl_ << std::endl;
    ir.add_arg_to_decl(current_fn_decl_, arg_type);
}

OpType IRGenContext::get_type() const
{
    return op_type_;
}

void IRGenContext::set_type(OpType type)
{
    op_type_ = type;
}

OpType IRGenContext::get_ret_type(IR &ir) const
{
    return ir.get_ret_type(current_fn_);
}

void IRGenContext::set_ret_type(IR &ir, OpType ret_type)
{
    ir.set_ret_type(current_fn_, ret_type);
}

std::string IRGenContext::get_current_fn()
{
    return current_fn_;
}

void IRGenContext::set_is_global(bool is_global)
{
    is_global_ = is_global;
}
bool IRGenContext::get_is_global() const
{
    return is_global_;
}

void IRGenContext::set_break_goto(int block_num)
{
	break_goto_ = block_num;
}

int IRGenContext::get_break_goto()
{
	return break_goto_;
}

void IRGenContext::set_continue_goto(int block_num)
{
	continue_goto_ = block_num;
}

int IRGenContext::get_continue_goto()
{
	return continue_goto_;
}


Op IRGenContext::get_rhs_op()
{
    return rhs_op_;
}

void IRGenContext::set_rhs_op(Op op)
{
    rhs_op_ = op;

}

void IRGenContext::add_enum(IR &ir, const std::string &name , std::string enum_type)
{
	
	enum_map_[enum_type].insert({name, enum_val_++});
}

void IRGenContext::add_enum(IR &ir, const std::string &name, int val, std::string enum_type)
{
	/*if(val < enum_val_){
		std::cout << "enum value must be greater than previous value" << std::endl;
		std::cout << "this will break" << std::endl;
	}*/
	enum_map_[enum_type].insert({name, val}); // this will not work for mixture of assigned and not
}

void IRGenContext::add_enum(IR &ir, const std::string &name)
{
	enum_map_[current_enum_].insert({name, enum_val_++});
}

void IRGenContext::add_enum_var(IR &ir, const std::string &name)
{
	std::unordered_map<std::string, int> value_map;
	enum_map_.insert({name, value_map});
	enum_val_ = 0;
	set_current_enum(name);
}

void IRGenContext::add_enum_var(IR &ir)
{
	std::unordered_map<std::string, int> value_map;
	enum_map_.insert({"enum", value_map }); // change this to work with multiple enums TODO
	enum_val_ = 0;
	set_current_enum("enum");
}

std::string IRGenContext::get_current_enum()
{
	return current_enum_;
}

void IRGenContext::set_current_enum(std::string enum_type)
{
	current_enum_ = enum_type;
}

void IRGenContext::set_enum_val(int val)
{
	enum_val_ = val;
}

int IRGenContext::get_enum_val() // this may be pointless
{
	return enum_val_++;
}

int IRGenContext::find_enum_val(std::string enum_type, std::string enum_var) // gives enum type and name to find value
{
	return enum_map_[enum_type][enum_var];
}

std::shared_ptr<SwitchOp> IRGenContext::get_cur_switch() const
{
    return cur_switch_;
}

void IRGenContext::set_cur_switch(std::shared_ptr<SwitchOp> switch_op)
{
    cur_switch_ = switch_op;
}


void IRGenContext::set_struct_type(const std::string &struct_name, OpType type)
{
    struct_type_map_[struct_name] = type;
}

OpType IRGenContext::get_struct_type(const std::string &struct_name)
{
    return struct_type_map_[struct_name];
}

std::vector<OpType> IRGenContext::get_struct_type_list() const
{
    return struct_types_;
}

void IRGenContext::add_to_struct_type_list(OpType type, const std::string &elem_name)
{
    std::cout << "adding type: ";
    print_op_type(std::cout, type);
    int offset = 0;
    for (OpType type : struct_types_) {
        offset += get_op_type_size(type);
    }
    struct_types_.push_back(type);
    std::cout << "offset computed: " << offset << std::endl;
    
    if (cur_struct_ != ".undef") {
        struct_el_map_[cur_struct_][elem_name] = {type, offset};
    }
}

bool IRGenContext::is_evaluating_struct_decl()
{
    return eval_struct_decl_;
}

void IRGenContext::start_eval_struct_decl()
{
    eval_struct_decl_ = true;
}

void IRGenContext::stop_eval_struct_decl()
{
    eval_struct_decl_ = false;
}

bool IRGenContext::is_evaluating_typedef()
{
    return eval_typedef_;
}

void IRGenContext::start_eval_typedef()
{
    eval_typedef_ = true;
}

void IRGenContext::stop_eval_typedef()
{
    eval_typedef_ = false;
}

void IRGenContext::set_cur_struct(const std::string &struct_name)
{
    cur_struct_ = struct_name;
}

std::pair<OpType, int> IRGenContext::get_struct_offset(const std::string &member_name)
{
    return struct_el_map_[cur_struct_][member_name];
}

void IRGenContext::set_struct_name(const std::string &name)
{
    name_struct_map_[name] = cur_struct_;
}

std::string IRGenContext::get_struct_name(const std::string &name)
{
    return name_struct_map_[name];
}

void IRGenContext::add_typedef(const std::string &name, OpType type)
{
    typedef_map_[name] = type;
}

OpType IRGenContext::get_typedef(const std::string &name)
{
    return typedef_map_[name];
}

void IRGenContext::toggle_eval_enum()
{
	is_eval_enum_ = !is_eval_enum_;
}

bool IRGenContext::is_eval_enum()
{
	return is_eval_enum_;
}
