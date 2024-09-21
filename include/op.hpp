#ifndef OP_HPP
#define OP_HPP

#include <vector>

// whether a given operand is a virtual register number or a constant
enum class OpSrc {vr, con, ptr};

enum class Type {VOID, CHAR, INT, UNSIGNED, FLOAT, DOUBLE, ENUM, STRUCT};

struct OpType {
    Type t;
    int p_level;
    int num_els;
    std::vector<OpType> type_l;
};

void print_op_type(std::ostream &stream, OpType type);
int get_op_type_size(OpType type);

int get_op_align(OpType type);

// union OpVal {
//     int vr_val;
//     int int_val;
//     double float_val;
// };

struct Op
{
    OpSrc src;
    int val;

    void Print(std::ostream &stream) const;
};

#endif