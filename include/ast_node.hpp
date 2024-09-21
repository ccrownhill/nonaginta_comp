#ifndef AST_NODE_HPP
#define AST_NODE_HPP

#include <iostream>
#include <vector>

#include "ir.hpp"
#include "ir_els.hpp"
#include "ir_gen_context.hpp"

class Node
{
public:
    Node() {}
    virtual ~Node() {}
    virtual void EmitCFG(IR &ir, IRGenContext &ctx) const = 0;
    virtual void Print(std::ostream &stream) const = 0;

// protected:
//     std::vector<Node *> branches_;
};


// this node is added since it has a different EmitCFG
// because expressions are the only things with return value
// and hence they need to indicate where they output their results to
class ExprNode
{
public:
    ExprNode() {}
    virtual ~ExprNode() {}
    virtual Op EmitCFG(IR &ir, IRGenContext &ctx, int lhs_vr) const = 0;
    virtual void Print(std::ostream &stream) const = 0;

// protected:
//     std::vector<ExprNode *> branches_;
};

// Represents a list of nodes.
class NodeList : public Node
{
public:
    NodeList(Node *first_node) : nodes_({first_node}) {}

    ~NodeList()
    {
        for (auto node : nodes_)
        {
            delete node;
        }
    }

    void PushBack(Node *item);
    void Concat(NodeList *list);
    virtual void EmitCFG(IR &ir, IRGenContext &ctx) const override;
    virtual void Print(std::ostream &stream) const override;
    std::vector<Node *>::iterator begin();
    std::vector<Node *>::iterator end();
private:
    std::vector<Node *> nodes_;
};

class ExprNodeList : public ExprNode
{
public:
    ExprNodeList(ExprNode *first_node) : nodes_({first_node}) {}

    ~ExprNodeList()
    {
        for (auto node : nodes_)
        {
            delete node;
        }
    }

    void PushBack(ExprNode *item);
    virtual Op EmitCFG(IR &ir, IRGenContext &ctx, int lhs_vr) const override;
    virtual void Print(std::ostream &stream) const override;
    std::vector<ExprNode *>::iterator begin();
    std::vector<ExprNode *>::iterator end();
private:
    std::vector<ExprNode *> nodes_;
};

#endif
