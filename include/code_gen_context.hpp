#ifndef CONTEXT_HPP
#define CONTEXT_HPP

#include <string>
#include <vector>
#include <unordered_map>
#include <deque>
#include "op.hpp"

class CodeGenContext
{
public:
    CodeGenContext() : label_num_(0)
	{}
    ~CodeGenContext() {}

    void reset_regs();
    void set_reg(int vr);
    int get_reg(int vr);
    void free_reg(int vr);
    int get_tmp_reg();
    void free_tmp_reg(int reg);
    
    void set_float_reg(int vr);
    int get_float_reg(int vr);
    void free_float_reg(int vr);


    int get_tmp_float_reg();
    void free_tmp_float_reg(int reg);


	int get_label_num();


    static int calc_vr_offset(int vr);
    void set_vr_offset(int vr, int off);
    int get_vr_offset(int vr);

    void set_current_fn(const std::string &fn_name);
    std::string get_current_fn();
    std::string get_ret_label();


    void load_int_const_into_reg(std::ostream &stream, int reg, int val);
    void load_float_const_into_reg(std::ostream &stream, int reg, bool is_double, int val);

    void save_registers(std::ostream &stream);
    void load_registers(std::ostream &stream);

	void set_break_goto(int block_num);
	int get_break_goto();

    void store_int_var(int vr, int store_reg, int size, bool deref_vr_as_ptr, std::ostream &stream);
    void store_float_var(int vr, int store_reg, bool is_double, bool deref_vr_as_ptr, std::ostream &stream);
    void addr_of_vr_to_reg(int vr, int target_reg, std::ostream &stream);

    int load_op_into_reg(Op op, OpType type, std::ostream &stream);
    void free_op_reg(int reg, Op op, OpType type);

private:
    std::unordered_map<int, int> vr_reg_map_;
    std::unordered_map<int, int> vr_float_reg_map_;
    std::unordered_map<int, int> vr_off_map_;
    std::deque<int> avail_regs_;
    std::deque<int> avail_float_regs_;
	int label_num_;
    std::string ret_label_;
    std::string current_fn_;
	int break_goto_;
};

#endif
