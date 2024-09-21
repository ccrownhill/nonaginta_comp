#ifndef IR_HPP
#define IR_HPP

#include <memory>
#include <list>
#include <unordered_map>
#include <vector>
#include <cstdint>

#include "code_gen_context.hpp"
#include "op.hpp"


// whether a given operand is a virtual register number or a constant






struct Const
{
    int size;
    std::vector<uint64_t> vals;
};


class CFGEl {
public:
    CFGEl() {}
    virtual ~CFGEl() {}

    virtual void EmitRISC(std::ostream &stream, CodeGenContext &ctx) const = 0;
    virtual void Print(std::ostream &stream) const = 0;
protected:
    OpType op_type_;
};


typedef std::shared_ptr<CFGEl> CFGElPtr;

class CFG
{
public:
    CFG() {}
    ~CFG() {}

    void EmitRISC(std::ostream &stream, CodeGenContext &ctx, const std::string &label_prefix);
    void Print(std::ostream &stream, const std::string &label_prefix);

    // returns index of added block
    int add_cfg_block();
    void add_CFGEl(CFGElPtr cfg_el, int block_num);
private:
    // the label for every block (list of CFGEls) is its index
    // with "l" prepended
    std::vector<std::list<CFGElPtr>> blocks_;
};

typedef std::shared_ptr<CFG> CFGPtr;

struct FunctionDecl
{
    OpType ret_type;
    std::vector<OpType> param_types;
};

typedef std::shared_ptr<FunctionDecl> FunctionDeclPtr;

typedef std::unordered_map<std::string, std::pair<int, OpType>> ScopeMap;

typedef std::shared_ptr<ScopeMap> ScopeMapPtr;

class Function
{
public:
    Function(const std::string &name) : name_ (name), scope_maps_{}, params_{}, cfg_ (std::make_shared<CFG>()) {}
    ~Function() {}

    void EmitRISC(std::ostream &stream, CodeGenContext &ctx);
    void Print(std::ostream &stream) const;
    std::pair<int, OpType> get_vr(const std::string &ident);
    void set_vr(const std::string &ident, OpType type);

    int add_cfg_block();
    void add_CFGEl(CFGElPtr cfg_el, int block_num);
    void add_param(const std::string &name);

    void set_ret_type(OpType type);
    OpType get_ret_type() const;

    void add_scope(ScopeMapPtr scope_map);
private:
    std::string name_;
    std::list<ScopeMapPtr> scope_maps_;
    std::vector<std::string> params_;
    CFGPtr cfg_;
    OpType ret_type_;
};

typedef std::shared_ptr<Function> FunctionPtr;

class IR
{
public:
    IR() : global_scope_ (std::make_shared<ScopeMap>()) {}
    ~IR() {}

    // move constructor for returning it directly
    IR(IR&& other)
    : fn_map_ (std::move(other.fn_map_))
    {}

    void Print(std::ostream &stream) const;
    void EmitRISC(std::ostream &stream, CodeGenContext &ctx);

    void add_fn(const std::string &name, OpType ret_type);
    void add_fn_decl(const std::string &name, OpType ret_type);
    void add_arg_to_decl(const std::string &name, OpType arg_type);
    int add_cfg_block(const std::string &fn_name);
    void add_CFGEl(CFGElPtr cfg_el, const std::string &fn_name, int block_num);
    void add_fn_scope(const std::string &fn_name, ScopeMapPtr scope_map);

    std::pair<int, OpType> get_vr(const std::string &fn_name, const std::string &ident);
    void set_vr(const std::string &fn_name, const std::string &ident, OpType type);

    void add_param(const std::string &fn_name, const std::string &param_name, OpType type);
    int add_const(std::vector<uint64_t> vals, int size);
    int add_global(std::vector<uint64_t> vals, int size);
    int add_string_literal(const std::string &str, int vr_num);
    int set_last_uninit_global(std::vector<uint64_t> vals);

    std::vector<uint64_t> get_const(int idx);
    std::vector<uint64_t> get_global_init(int idx);
    void set_const(int idx, std::vector<uint64_t> vals);

    OpType get_ret_type(const std::string &fn_name);
    void set_ret_type(const std::string &fn_name, OpType type);

    FunctionDeclPtr get_fn_decl(const std::string &fn_name);

    ScopeMapPtr get_global_scope() const;
private:
    std::unordered_map<std::string, FunctionPtr> fn_map_;
    std::unordered_map<std::string, FunctionDeclPtr> fn_decl_map_;
    std::vector<Const> consts_;
    std::vector<std::pair<Const, int>> global_inits_;
    std::vector<std::pair<Const, int>> global_uninits_;
    std::vector<std::pair<std::string, int>> string_literals_;
    ScopeMapPtr global_scope_;
};


#endif