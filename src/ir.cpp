#include <iostream>
#include <algorithm>
#include "ir.hpp"
#include "op.hpp"




void IR::EmitRISC(std::ostream &stream, CodeGenContext &ctx)
{
    for (auto &[fn_name, fn] : fn_map_) {
        stream << ".text" << std::endl;
		//stream << ".align 3" << std::endl; // align on 4 byte boundary

        stream << ".globl " << fn_name << std::endl;

        stream << fn_name << ":" << std::endl;

        

        fn->EmitRISC(stream, ctx);
    }

    if (consts_.size() > 0) {
        stream << ".section .srodata" << std::endl;
		//stream << ".align 3" << std::endl; // align on 8 byte boundary
        for (int i = 0; i < consts_.size(); i++) {
            stream << ".lro" << i << ":" << std::endl;
            for (uint64_t val : consts_[i].vals) {
                if (consts_[i].size == 4) {
                    stream << ".word " << static_cast<uint32_t>(val) << std::endl;
                } else if (consts_[i].size == 8) {
                    stream << ".quad " << val << std::endl;
                }
            }
            
        }
    }

    if (global_inits_.size() > 0) {
        stream << ".section .sdata" << std::endl;
        for (auto [c, vr] : global_inits_) {
            stream << ".gd" << vr << ":" << std::endl; // use i here to keep increasing name count
            for (uint64_t val : c.vals) {
                if (c.size == 4) {
                    stream << ".word " << static_cast<uint32_t>(val) << std::endl;
                } else if (c.size == 8) {
                    stream << ".quad " << val << std::endl;
                }
            }
        }
    }

    if (global_uninits_.size() > 0) {
        stream << ".section .sbss" << std::endl;
        for (auto [c, vr] : global_uninits_) {
            stream << ".gd" << vr << ":" << std::endl; // use i here to keep increasing name count
            for (uint64_t val : c.vals) {
                if (c.size == 4) {
                    stream << ".word " << static_cast<uint32_t>(val) << std::endl;
                } else if (c.size == 8) {
                    stream << ".quad " << val << std::endl;
                }
            }
        }
    }

    if (string_literals_.size() > 0) {
        stream << ".section .srodata" << std::endl;
		//stream << ".align 2" << std::endl; // align on 4 byte boundary
        for (auto [s, vr] : string_literals_) {
            stream << ".gd" << vr << ":" << std::endl;
            stream << ".asciz \"" << s << "\"" << std::endl;
            stream << ".size .gd" << vr << ", " << s.size() + 1 << std::endl;
        }
    }
    
}

void IR::Print(std::ostream &stream) const
{
    if (string_literals_.size() > 0) {
        for (int i = 0; i < string_literals_.size(); i++) {
            stream << "@.str." << i << " = private unnamed_addr constant ["
                << string_literals_[i].first.size() + 1 << " x i8] c" << "\""
                << string_literals_[i].first << "\\00\", align 1" << std::endl;
        }
    }
    for (const auto &[fn_name, fn] : fn_map_) {
        
        // TODO print return type here
        stream << "define @" << fn_name << "() {" << std::endl;

        fn->Print(stream);

        stream << "}" << std::endl;
    }
}

void IR::add_fn(const std::string &name, OpType ret_type)
{
    fn_map_[name] = std::make_shared<Function>(name);
    add_fn_decl(name, ret_type);
}

void IR::add_fn_decl(const std::string &name, OpType ret_type)
{
    fn_decl_map_[name] = std::make_shared<FunctionDecl>();
    fn_decl_map_[name]->ret_type = ret_type;
}

void IR::add_arg_to_decl(const std::string &name, OpType arg_type)
{
    fn_decl_map_[name]->param_types.push_back(arg_type);
}


int IR::add_cfg_block(const std::string &fn_name)
{
    return fn_map_[fn_name]->add_cfg_block();
}

void IR::add_CFGEl(CFGElPtr cfg_el, const std::string &fn_name, int block_num)
{
    fn_map_[fn_name]->add_CFGEl(cfg_el, block_num);
}


void IR::add_param(const std::string &fn_name, const std::string &name, OpType type)
{
    fn_map_[fn_name]->add_param(name);
}

int IR::add_const(std::vector<uint64_t> vals, int size)
{
    consts_.push_back({.size = size, .vals = vals});
    return consts_.size() - 1;
}

int IR::add_global(std::vector<uint64_t> vals, int size)
{
    int vr_num = global_inits_.size() + global_uninits_.size() + string_literals_.size();
    Const c = {.size = size, .vals = vals};
    global_uninits_.push_back(std::make_pair(c, vr_num));

    return vr_num;
    
}

int IR::set_last_uninit_global(std::vector<uint64_t> vals)
{
    global_inits_.push_back(global_uninits_.back());
    global_uninits_.pop_back();
    global_inits_.back().first.vals = vals;
    return global_inits_.back().second;
}

int IR::add_string_literal(const std::string &str, int vr_num)
{
    string_literals_.push_back(std::make_pair(str, vr_num));
    return vr_num;
}

void IR::add_fn_scope(const std::string &fn_name, ScopeMapPtr scope_map)
{
    fn_map_[fn_name]->add_scope(scope_map);
}

std::vector<uint64_t> IR::get_const(int idx)
{
    return consts_[idx].vals;
}

std::vector<uint64_t> IR::get_global_init(int idx)
{
    return global_inits_[idx].first.vals;
}
void IR::set_const(int idx, std::vector<uint64_t> vals)
{
    consts_[idx].vals = vals;
}

FunctionDeclPtr IR::get_fn_decl(const std::string &fn_name)
{
    return fn_decl_map_[fn_name];
}

OpType IR::get_ret_type(const std::string &fn_name)
{
    return fn_map_[fn_name]->get_ret_type();
}
void IR::set_ret_type(const std::string &fn_name, OpType type)
{
    fn_map_[fn_name]->set_ret_type(type);
}

ScopeMapPtr IR::get_global_scope() const
{
    return global_scope_;
}


void CFG::EmitRISC(std::ostream &stream, CodeGenContext &ctx, const std::string &label_prefix)
{
    for (int label = 0; label < blocks_.size(); label++) {
        stream << "." << label_prefix << "_l" << label << ":" << std::endl;
        for (auto cfg_el : blocks_[label]) {
            cfg_el->EmitRISC(stream, ctx);
        }
    }
}

void CFG::Print(std::ostream &stream, const std::string &label_prefix)
{
    for (int label = 0; label < blocks_.size(); label++) {
        stream << "." << label_prefix << "_l" << label << ":" << std::endl;
        for (auto cfg_el : blocks_[label]) {
            cfg_el->Print(stream);
        }
    }
}

int CFG::add_cfg_block()
{
    blocks_.emplace_back();
    return blocks_.size() - 1;
}

void CFG::add_CFGEl(CFGElPtr cfg_el, int block_num)
{
    blocks_[block_num].push_back(cfg_el);
}

void Function::EmitRISC(std::ostream &stream, CodeGenContext &ctx)
{
    ctx.reset_regs();
    // the 8 is for storing ra, s0
    

    // store the parameters in memory
    int offset = 8; // 8 to make space for ra and s0 which are already backed up on stack
    for (ScopeMapPtr scope_m : scope_maps_) {
        for (const auto &[_, vr] : *scope_m) {
            offset += get_op_type_size(vr.second);
            ctx.set_vr_offset(vr.first, -offset);
        }
    }
    
    // align on 128 bit / 32 byte boundary as required by RISC-V ABI
    int frame_size = 32 * (offset / 32 + 1);
    ctx.set_current_fn(name_);
    stream << "addi sp, sp, -" << frame_size << std::endl;
    stream << "sw ra, " << frame_size - 4 << "(sp)" << std::endl;
    stream << "sw s0, " << frame_size - 8 << "(sp)" << std::endl;
    stream << "addi	s0, sp, " << frame_size << std::endl;

    int num_float_args = 0;
    int num_args = 0;
    int i;
    for (i = 0; num_args < 8 && i < params_.size() && num_float_args < 8; i++) { // TODO use float and ints together
	 	std::cout << "params over 8, off " << offset << std::endl;
        auto [vr_num, type] = scope_maps_.front()->at(params_[i]); // get vr values from first scope map in list
        if (type.t == Type::INT || type.t == Type::UNSIGNED || type.p_level > 0) {
            stream << "sw a" << num_args++ << ", " << ctx.get_vr_offset(vr_num) << "(s0)" << std::endl;
        } else if (type.t == Type::CHAR) {
            stream << "sb a" << num_args++ << ", " << ctx.get_vr_offset(vr_num) << "(s0)" << std::endl;
        } else if (type.t == Type::FLOAT) {
            stream << "fsw fa" << num_float_args++ << ", " << ctx.get_vr_offset(vr_num) << "(s0)" << std::endl;
        } else if (type.t == Type::DOUBLE) {
            stream << "fsd fa" << num_float_args++ << ", " << ctx.get_vr_offset(vr_num) << "(s0)" << std::endl;
        } else {
            std::cerr << "function emitrisc: unknown parameter type" << std::endl;
            exit(1);
        }
    }
	for(;i < params_.size(); i++){
		std::cout << "handling extra function params" << std::endl;
		auto [vr_num, type] = scope_maps_.front()->at(params_[i]);
		offset = (i - 8) * get_op_type_size(type);
		ctx.set_vr_offset(vr_num, offset);
		if(num_float_args >= 8 && num_args < 8){
			stream << "sw a" << num_args++ << ", " << ctx.get_vr_offset(vr_num) << "(s0)" << std::endl;
			num_float_args++;
		}
	}
	/*auto [vr_num, type] = scope_maps_.front()->at(params_[i+1]);
	std::cout << "params over 8, on " << offset << std::endl;
    offset = (i - 8) * 4; // TODO change * 4 to correct size
    ctx.set_vr_offset(vr_num, offset);*/
    // TODO handling excess parameters is now wrong due to adding floats, doubles
    // for (; i < params_.size(); i++) {
    //     std::cout << "params over 8, on " << offset << std::endl;
    //     offset = (i - 8) * 4; // TODO change * 4 to correct size
    //     ctx.set_vr_offset(vr_map_[params_[i]].first, offset);
    // }

    cfg_->EmitRISC(stream, ctx, name_);
    stream << ctx.get_ret_label() << ":" << std::endl;
    stream << "lw ra, " << frame_size - 4 << "(sp)" << std::endl;
    stream << "lw s0, " << frame_size - 8 << "(sp)" << std::endl;
    stream << "addi	sp, sp, " << frame_size << std::endl;
    stream << "ret" << std::endl;
}

void Function::Print(std::ostream &stream) const
{
    
    for (ScopeMapPtr scope_m : scope_maps_) {
        for (auto [_, vr] : *scope_m) {
            stream << "%" << vr.first << " = alloca ";
            print_op_type(stream, vr.second);
            stream << ", align " << get_op_align(vr.second) << std::endl;
        }
    }
    cfg_->Print(stream, name_);
}

void Function::add_scope(ScopeMapPtr scope_map)
{
    scope_maps_.push_back(scope_map);
}



int Function::add_cfg_block()
{
    return cfg_->add_cfg_block();
}

void Function::add_CFGEl(CFGElPtr cfg_el, int block_num)
{
    cfg_->add_CFGEl(cfg_el, block_num);
}

void Function::add_param(const std::string &name)
{
    params_.push_back(name);
}

void Function::set_ret_type(OpType type)
{
    ret_type_ = type;
}

OpType Function::get_ret_type() const
{
    return ret_type_;
}