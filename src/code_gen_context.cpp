#include <memory>

#include <iostream>


#include "code_gen_context.hpp"

int CodeGenContext::calc_vr_offset(int vr)
{
    return -(vr * 4 + 12);
}

void CodeGenContext::set_vr_offset(int vr, int off)
{
    vr_off_map_[vr] = off;
}

int CodeGenContext::get_vr_offset(int vr)
{
    if (vr_off_map_.find(vr) == vr_off_map_.end())
        return -1;
    return vr_off_map_[vr];
}

void CodeGenContext::reset_regs()
{
    // all registers that need not be saved can act as temporary value stores
    // in functions
    // https://github.com/riscv-non-isa/riscv-elf-psabi-doc/blob/9a77e8801592b3d194796ea5ba6ec670e4fe054f/riscv-cc.adoc
    avail_regs_.clear();
    avail_regs_ = {5, 6, 7, 28, 29, 30, 31};
    vr_reg_map_.clear();
    avail_float_regs_.clear();
    avail_float_regs_ = {0, 1, 2, 3, 4, 5, 6, 7, 28, 29, 30, 31};
    vr_float_reg_map_.clear();
}

void CodeGenContext::set_reg(int vr)
{
    std::cout << "allocating int reg " << vr << std::endl;
    vr_reg_map_[vr] = avail_regs_.front();
    avail_regs_.pop_front();
}

int CodeGenContext::get_reg(int vr)
{
    if (vr_reg_map_.find(vr) == vr_reg_map_.end())
        return -1;
    return vr_reg_map_[vr];
}
void CodeGenContext::free_reg(int vr)
{
    std::cout << "freeing int reg " << vr << std::endl; 
    avail_regs_.push_back(vr_reg_map_[vr]);
    vr_reg_map_.erase(vr);
}

int CodeGenContext::get_tmp_reg()
{
    int out = avail_regs_.front();
    avail_regs_.pop_front();
    return out;
}

void CodeGenContext::free_tmp_reg(int reg)
{
    avail_regs_.push_back(reg);
}




void CodeGenContext::set_float_reg(int vr)
{
    std::cout << "allocating float reg " << vr << std::endl;
    vr_float_reg_map_[vr] = avail_float_regs_.front();
    avail_float_regs_.pop_front();
}

int CodeGenContext::get_float_reg(int vr)
{
    if (vr_float_reg_map_.find(vr) == vr_float_reg_map_.end())
        return -1;
    return vr_float_reg_map_[vr];
}
void CodeGenContext::free_float_reg(int vr)
{
    std::cout << "freeing float reg " << vr << std::endl;
    avail_float_regs_.push_back(vr_float_reg_map_[vr]);
    vr_float_reg_map_.erase(vr);
}

int CodeGenContext::get_tmp_float_reg()
{
    int out = avail_float_regs_.front();
    avail_float_regs_.pop_front();
    return out;
}

void CodeGenContext::free_tmp_float_reg(int reg)
{
    avail_float_regs_.push_back(reg);
}

int CodeGenContext::get_label_num()
{
	return label_num_++;

}

void CodeGenContext::set_current_fn(const std::string &fn_name)
{
    current_fn_ = fn_name;
}

std::string CodeGenContext::get_current_fn()
{
    return current_fn_;
}

std::string CodeGenContext::get_ret_label()
{
    return "." + current_fn_ + "_END";

}

void CodeGenContext::load_int_const_into_reg(std::ostream &stream, int reg, int val)
{
    // DONE if it exceeds bit count of li use additional lui with addi 
    	stream << "li x" << reg << ", " << val << std::endl;
	
}
void CodeGenContext::load_float_const_into_reg(std::ostream &stream, int reg, bool is_double, int val)
{
    int tmp_reg = get_tmp_reg();
    stream << "lui x" << tmp_reg << ", %hi(.lro" << val << ")" << std::endl;
    if (!is_double) {
        stream << "flw f" << reg << ", %lo(.lro" << val << ")(x" << tmp_reg << ")" << std::endl;
    } else {
        stream << "fld f" << reg << ", %lo(.lro" << val << ")(x" << tmp_reg << ")" << std::endl;
    }
    free_tmp_reg(tmp_reg);

}

void CodeGenContext::save_registers(std::ostream &stream)
{
    stream << "addi sp, sp, -" << (vr_float_reg_map_.size() * 8 + vr_reg_map_.size() * 4) << std::endl;

    int offset = 0;
    for (auto [vr, reg] : vr_reg_map_) {
        stream << "sw x" << reg << ", " << offset << "(sp)" << std::endl;
        offset += 4;
    }

    for (auto [vr, reg] : vr_float_reg_map_) {
        stream << "fsw.d f" << reg << ", " << offset << "(sp)" << std::endl;
        offset += 8;
    }
}
void CodeGenContext::load_registers(std::ostream &stream)
{

    int offset = 0;
    for (auto [vr, reg] : vr_reg_map_) {
        stream << "lw x" << reg << ", " << offset << "(sp)" << std::endl;
        offset += 4;
    }

    for (auto [vr, reg] : vr_float_reg_map_) {
        stream << "flw.d f" << reg << ", " << offset << "(sp)" << std::endl;
        offset += 8;
    }

    stream << "addi sp, sp, " << (vr_float_reg_map_.size() * 8 + vr_reg_map_.size() * 4) << std::endl;
}

void CodeGenContext::store_int_var(int vr, int store_reg, int size, bool deref_vr_as_ptr, std::ostream &stream)
{
    std::string store_op = (size == 1) ? "sb" : "sw";
    if (get_vr_offset(vr) == -1 && get_reg(vr) == -1) {
        int tmp_reg = get_tmp_reg();
        stream << "lui x" << tmp_reg << ", %hi(.gd" << vr << ")" << std::endl;
        if (deref_vr_as_ptr) {
            stream << "lw x" << tmp_reg << ", %lo(.gd" << vr << ")(x" << tmp_reg << ")" << std::endl;
            // at this point tmp_reg contains the address stored in vr
            // store given value of store_reg in tmp_reg
            stream << store_op << " x" << store_reg << ", 0(x" << tmp_reg << ")" << std::endl;
        } else {
            stream << store_op << " x" << store_reg << ", %lo(.gd" << vr << ")(x" << tmp_reg << ")" << std::endl;

        }
        free_tmp_reg(tmp_reg);
    } else if (deref_vr_as_ptr && get_reg(vr) != -1) {
        stream << store_op << " x" << store_reg << ", 0(x" << get_reg(vr) << ")" << std::endl;
    } else {
        if (deref_vr_as_ptr) {
            int tmp_reg = get_tmp_reg();
            stream << "lw x" << tmp_reg << ", " << get_vr_offset(vr) << "(s0)" << std::endl;
            stream << store_op << " x" << store_reg << ", 0(x" << tmp_reg << ")" << std::endl;
            free_tmp_reg(tmp_reg);
        } else {
            stream << store_op << " x" << store_reg << ", " << get_vr_offset(vr) << "(s0)" << std::endl;
        }
    }
}

void CodeGenContext::store_float_var(int vr, int store_reg, bool is_double, bool deref_vr_as_ptr, std::ostream &stream)
{
    std::string store_op = (is_double) ? "fsd" : "fsw";
    if (get_vr_offset(vr) == -1 && get_reg(vr) == -1) {
        int tmp_reg = get_tmp_reg();
        stream << "lui x" << tmp_reg << ", %hi(.gd" << vr << ")" << std::endl;
        if (deref_vr_as_ptr) {
            stream << "lw x" << tmp_reg << ", %lo(.gd" << vr << ")(x" << tmp_reg << ")" << std::endl;
            // at this point tmp_reg contains the address stored in vr
            // store given value of store_reg in tmp_reg
            stream << store_op << " f" << store_reg << ", 0(x" << tmp_reg << ")" << std::endl;
        } else {
            stream << store_op << " f" << store_reg << ", %lo(.gd" << vr << ")(x" << tmp_reg << ")" << std::endl;
        }
        free_tmp_reg(tmp_reg);
    } else if (deref_vr_as_ptr && get_reg(vr) != -1) {
        // store at address stored in get_float_reg(vr)
        stream << store_op << " f" << store_reg << ", 0(x" << get_reg(vr) << ")" << std::endl;
    } else {
        if (deref_vr_as_ptr) {
            int tmp_reg = get_tmp_reg();
            stream << "lw x" << tmp_reg << ", " << get_vr_offset(vr) << "(s0)" << std::endl;
            stream << store_op << " f" << store_reg << ", 0(x" << tmp_reg << ")" << std::endl;
            free_tmp_reg(tmp_reg);
        } else {
            stream << store_op << " f" << store_reg << ", " << get_vr_offset(vr) << "(s0)" << std::endl;
        }
    }
}

void CodeGenContext::addr_of_vr_to_reg(int vr, int target_reg, std::ostream &stream)
{
    if (get_vr_offset(vr) == -1) { // if it is a global variable
        stream << "lui x" << target_reg << ", %hi(.gd" << vr << ")" << std::endl;
        stream << "addi x" << target_reg << ", x" << target_reg << ", %lo(.gd" << vr << ")" << std::endl;
    } else {
        int tmp_reg = get_tmp_reg();
        // do this because addi might not be fine for very very big offsets
        stream << "li x" << tmp_reg << ", " << get_vr_offset(vr) << std::endl;
        stream << "add x" << target_reg << ", s0, x" << tmp_reg << std::endl;
        free_tmp_reg(tmp_reg);
    }
}

int CodeGenContext::load_op_into_reg(Op op, OpType type, std::ostream &stream)
{
    if (op.src == OpSrc::ptr) {
        int tmp_reg = get_tmp_reg();
        addr_of_vr_to_reg(op.val, tmp_reg, stream);
        return tmp_reg;
    } else if (op.src == OpSrc::vr) {
        if (type.t == Type::FLOAT || type.t == Type::DOUBLE) {
			std::cout << "loading float reg with vr: " << op.val << std::endl;
            return get_float_reg(op.val);
        } else {
            return get_reg(op.val);
        }
    } else if (op.src == OpSrc::con) {
        if (type.t == Type::FLOAT || type.t == Type::DOUBLE) {
			std::cout << "loading float reg with const: " << op.val << std::endl;
            int tmp_reg = get_tmp_float_reg();
            load_float_const_into_reg(stream, tmp_reg, (type.t == Type::DOUBLE), op.val);
            return tmp_reg;
        } else {
            int tmp_reg = get_tmp_reg();
            load_int_const_into_reg(stream, tmp_reg, op.val);
            return tmp_reg;
        }
    } else {
        std::cerr << "unknown opsrc" << std::endl;
        std::exit(1);
    }
}

void CodeGenContext::free_op_reg(int reg, Op op, OpType type)
{
    if (op.src == OpSrc::con) {
        if (type.t == Type::FLOAT || type.t == Type::DOUBLE) {
            free_tmp_float_reg(reg);
        } else {
            free_tmp_reg(reg);
        }
    } else if (op.src == OpSrc::ptr) {
        free_tmp_reg(reg);
    } else if (op.src == OpSrc::vr) {
        if (type.t == Type::FLOAT || type.t == Type::DOUBLE) {
            free_float_reg(op.val);
        } else {
            free_reg(op.val);
        }
    }
}
