#ifndef AST_HPP
#define AST_HPP

#include <iostream>
#include <string>
#include <vector>

#include "ir.hpp"
#include "ir_els.hpp"
#include "ir_gen_context.hpp"
#include "ast_node.hpp"
#include "ast_direct_declarator.hpp"
#include "ast_function_definition.hpp"
#include "ast_identifier.hpp"
#include "ast_statement.hpp"
#include "ast_type_specifier.hpp"
#include "ast_constant.hpp"
#include "code_gen_context.hpp"
#include "ast_init_declarator.hpp"
#include "ast_declaration.hpp"
#include "ast_parameter_declaration.hpp"
#include "ast_expression.hpp"
#include "ast_declarator.hpp"
#include "ast_pointer.hpp"
#include "ast_enumerator.hpp"
#include "ast_struct_declaration.hpp"
#include "ast_struct_specifier.hpp"
#include "ast_typedef_declaration.hpp"

extern Node *ParseAST(std::string file_name);

#endif
