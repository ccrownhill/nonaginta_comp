#include "ast_node.hpp"

// Node::~Node()
// {
//     for (auto branch : branches_)
//     {
//         delete branch;
//     }
// }

std::vector<Node *>::iterator NodeList::begin()
{
    return nodes_.begin();
}

std::vector<Node *>::iterator NodeList::end()
{
    return nodes_.end();
}

void NodeList::PushBack(Node *item)
{
    nodes_.push_back(item);
}

void NodeList::Concat(NodeList *list)
{
    nodes_.insert(nodes_.end(), list->begin(), list->end());
}

void NodeList::EmitCFG(IR &ir, IRGenContext &ctx) const
{
    for (auto node : nodes_)
    {
        if (node == nullptr)
        {
            continue;
        }
        node->EmitCFG(ir, ctx);
    }
}

void NodeList::Print(std::ostream &stream) const
{
    for (auto node : nodes_)
    {
        if (node == nullptr)
        {
            continue;
        }
        node->Print(stream);
    }
}

std::vector<ExprNode *>::iterator ExprNodeList::begin()
{
    return nodes_.begin();
}

std::vector<ExprNode *>::iterator ExprNodeList::end()
{
    return nodes_.end();
}

void ExprNodeList::PushBack(ExprNode *item)
{
    nodes_.push_back(item);
}

Op ExprNodeList::EmitCFG(IR &ir, IRGenContext &ctx, int lhs_vr) const
{
    for (auto node : nodes_)
    {
        if (node == nullptr)
        {
            continue;
        }
        node->EmitCFG(ir, ctx, -1);
    }
    return {.src=OpSrc::con, .val=0};
}

void ExprNodeList::Print(std::ostream &stream) const
{
    for (auto node : nodes_)
    {
        if (node == nullptr)
        {
            continue;
        }
        node->Print(stream);
    }
}
