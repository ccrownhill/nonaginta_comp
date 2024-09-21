#ifndef IR_GEN_CONTEXT_HPP
#define IR_GEN_CONTEXT_HPP

#include <iostream>
#include <deque>
#include "ir.hpp"
#include "ir_els.hpp"

class IRGenContext
{
public:
    IRGenContext(ScopeMapPtr global_scope)
        : scope_stack_(), vr_num_ (-1), eval_lhs_ (false), eval_param_(false), is_global_(true), eval_struct_decl_ (false), eval_typedef_(false)
    {
        scope_stack_.push_front(global_scope);
    }
    ~IRGenContext() {}
    void add_fn(IR &ir, const std::string &name);
    int add_cfg_block(IR &ir);
    void goto_block(int block_num);
	int get_block_num() const;
    void add_CFGEl(IR &ir, CFGElPtr cfg_el);
    void add_param(IR &ir, const std::string &name);
    void add_fn_decl(IR &ir, const std::string &name);

    void add_arg_to_decl(IR &ir, OpType arg_type);
    void set_vr(const std::string &ident, OpType type);
    std::pair<int, OpType> get_vr(const std::string &ident);
    int next_vr_num();

    void enter_local_scope(IR &ir);
    void leave_scope();

    bool is_evaluating_lhs() const;
    void start_eval_lhs();
    void stop_eval_lhs();

    OpType get_type() const;
    void set_type(OpType type);

    OpType get_ret_type(IR &ir) const;
    void set_ret_type(IR &ir, OpType type);

	std::string get_current_fn();

    void set_is_global(bool is_global);
    bool get_is_global() const;

	void set_break_goto(int block_num);
	int get_break_goto();
	void set_continue_goto(int block_num);
	int get_continue_goto();


	Op get_rhs_op();
    void set_rhs_op(Op op);


	void add_enum(IR &ir, const std::string &name, int val, std::string enum_type);
	void add_enum(IR &ir, const std::string &name, std::string enum_type);
	void add_enum(IR &ir, const std::string &name);
	
	std::string get_current_enum();
	void set_current_enum(std::string enum_type);

	void add_enum_var(IR &ir, const std::string &name);
	void add_enum_var(IR &ir);

	void set_enum_val(int val);
	int get_enum_val();

	int find_enum_val(std::string enum_type, std::string enum_var);
	void toggle_eval_enum();
	bool is_eval_enum();


    std::shared_ptr<SwitchOp> get_cur_switch() const;
    void set_cur_switch(std::shared_ptr<SwitchOp> switch_op);

    void set_struct_type(const std::string &struct_name, OpType type);
    OpType get_struct_type(const std::string &struct_name);

    std::vector<OpType> get_struct_type_list() const;
    void add_to_struct_type_list(OpType type, const std::string &elem_name);

    bool is_evaluating_struct_decl();
    void start_eval_struct_decl();
    void stop_eval_struct_decl();

    bool is_evaluating_typedef();
    void start_eval_typedef();
    void stop_eval_typedef();
    
    void set_cur_struct(const std::string &struct_name);
    //void set_struct_offset(const std::string &member_name, int offset);
    std::pair<OpType, int> get_struct_offset(const std::string &member_name);

    void set_struct_name(const std::string &name);
    std::string get_struct_name(const std::string &name);

    void add_typedef(const std::string &name, OpType type);
    OpType get_typedef(const std::string &name);
private:
    std::deque<ScopeMapPtr> scope_stack_;
    std::string current_fn_;
    std::string current_fn_decl_;
    int block_num_;
    int vr_num_;
    bool eval_lhs_;
    bool eval_param_;
    OpType op_type_;
    bool is_global_;
	int break_goto_;
	int continue_goto_;

	int enum_val_;
	std::string current_enum_;
	// map of enum variables to their names and values
	// for enum x = {a = 0, b = 3} finding x returns {a = 0, b = 3} finding b now returns 3
	std::unordered_map<std::string, std::unordered_map<std::string, int> > enum_map_;
	bool is_eval_enum_;


    Op rhs_op_;
    std::shared_ptr<SwitchOp> cur_switch_;

    std::unordered_map<std::string, OpType> struct_type_map_;
    std::vector<OpType> struct_types_;
    bool eval_struct_decl_;
    std::string cur_struct_;

    std::unordered_map<std::string, std::string> name_struct_map_;
    std::unordered_map<std::string, std::unordered_map<std::string, std::pair<OpType, int>>> struct_el_map_;

    bool eval_typedef_;
    std::unordered_map<std::string, OpType> typedef_map_;
};

#endif