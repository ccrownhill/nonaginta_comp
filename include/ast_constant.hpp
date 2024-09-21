#ifndef AST_CONSTANT_HPP
#define AST_CONSTANT_HPP

#include <cstdint>
#include "ast_node.hpp"


class IntConstant : public ExprNode
{
public:
    IntConstant(int value) : value_(value) {
		std::cout << "int const constr: " << value << std::endl;
	}

    int GetValue();
    Op EmitCFG(IR &ir, IRGenContext &ctx, int lhs_vr) const override;
    void Print(std::ostream &stream) const override;
private:
    int value_;

};

class FloatConstant : public ExprNode
{
public:
    FloatConstant(float value) : value_(value) {}

    int GetValue();
    Op EmitCFG(IR &ir, IRGenContext &ctx, int lhs_vr) const override;
    void Print(std::ostream &stream) const override;
private:
    float value_;

};

class DoubleConstant : public ExprNode
{
public:
    DoubleConstant(double value) : value_(value) {std::cout << "double const constr: " << value << std::endl;}

    int GetValue();
    Op EmitCFG(IR &ir, IRGenContext &ctx, int lhs_vr) const override;
    void Print(std::ostream &stream) const override;
private:
    double value_;

};

class UnsignedConstant : public ExprNode
{
public:
	UnsignedConstant(unsigned value) : value_(value) { std::cout << "unsigned const constr: " << value << std::endl;}
    ~UnsignedConstant() {}

	int GetValue(); 
	Op EmitCFG(IR &ir, IRGenContext &ctx, int lhs_vr) const override;
	void Print(std::ostream &stream) const override;
private:
	unsigned value_;

};


class StringLiteral : public ExprNode
{
public:
    StringLiteral(const std::string &lit) : lit_ (lit) {}
    ~StringLiteral() {}

    Op EmitCFG(IR &ir, IRGenContext &ctx, int lhs_vr) const override;
	void Print(std::ostream &stream) const override;
private:
    std::string lit_;
};

uint64_t encode_float(float f);
uint64_t encode_double(double f);
float decode_float(uint64_t i);
double decode_double(uint64_t i);

#endif
