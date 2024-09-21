#include <iostream>
#include "op.hpp"

void print_op_type(std::ostream &stream, OpType type)
{
    for (int i = 0; i < type.p_level; i++) {
        stream << "ptr ";
    }
    if (type.t == Type::INT) {
        stream << "i32";
    } else if (type.t == Type::FLOAT) {
        stream << "float";
    } else if (type.t == Type::DOUBLE) {
        stream << "double";
    } else if (type.t == Type::CHAR) {
        stream << "i8";
    } else if (type.t == Type::UNSIGNED) {
        stream << "u32";
    } else if (type.t == Type::STRUCT) {
        stream << "struct";
    } else if (type.t == Type::ENUM) {
        stream << "enum";
    } else {
        stream << "raw number of type: " << (int)type.t << std::endl;
    }
}

int get_op_type_size(OpType type)
{
    if (type.t == Type::STRUCT && type.p_level == 0) {
        int size = 0;
        for (OpType t : type.type_l) {
            size += get_op_type_size(t);
        }
        return size;
    } else if (type.t == Type::ENUM) {
        return 4 * ((type.num_els > 0) ? type.num_els : 1);
    } else if (type.p_level > 0) {
        return 4 * ((type.num_els > 0) ? type.num_els : 1);
    } else if (type.t == Type::INT) {
        return 4 * ((type.num_els > 0) ? type.num_els : 1);
    } else if (type.t == Type::FLOAT) {
        return 4 * ((type.num_els > 0) ? type.num_els : 1);
    } else if (type.t == Type::DOUBLE) {
        return 8 * ((type.num_els > 0) ? type.num_els : 1);
    } else if (type.t == Type::CHAR) {
        return 1 * ((type.num_els > 0) ? type.num_els : 1);
    } else if (type.t == Type::UNSIGNED) {
        return 4 * ((type.num_els > 0) ? type.num_els : 1);
    }
}

int get_op_align(OpType type)
{
    if (type.t == Type::INT) {
        return 4;
    } else if (type.t == Type::FLOAT) {
        return 4;
    } else if (type.t == Type::DOUBLE) {
        return 8;
    } else if (type.t == Type::CHAR) {
        return 1;
    } else if (type.t == Type::UNSIGNED) {
        return 4;
    } else if (type.t == Type::ENUM) {
        return 4;
    } else if (type.t == Type::STRUCT) {
        int size = 0;
        for (OpType t : type.type_l) {
            size += get_op_type_size(t);
        }
        return size;
    }
}

void Op::Print(std::ostream &stream) const
{
    if (src == OpSrc::vr) {
        stream << "%";
    } else if (src == OpSrc::ptr) {
        stream << "ptr %";
    }
    stream << val;
}