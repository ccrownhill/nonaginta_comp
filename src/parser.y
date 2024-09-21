// Adapted from: https://www.lysator.liu.se/c/ANSI-C-grammar-y.html

// TODO: you can either continue adding to this grammar file or
// rename parser_full.y to parser.y once you're happy with
// how this example works.

%code requires{
    #include "ast.hpp"
	#include "typedef_map.hpp"
	#include <vector>

    extern Node *g_root;
    extern FILE *yyin;
    int yylex(void);
    void yyerror(const char *);
	
}

// Represents the value associated with any kind of AST node.
%union{
  Node         *node;
  ExprNode	   *expr_node;
  NodeList     *nodes;
  ExprNodeList *expr_nodes;
  int          number_int;
  unsigned 	   number_unsigned;
  float        number_float;
  double	   number_double;

  std::string  *string;
  yytokentype  token;
}

%token IDENTIFIER INT_CONSTANT FLOAT_CONSTANT DOUBLE_CONSTANT UNSIGNED_CONSTANT  STRING_LITERAL
%token PTR_OP INC_OP DEC_OP LEFT_OP RIGHT_OP LE_OP GE_OP EQ_OP NE_OP AND_OP OR_OP
%token MUL_ASSIGN DIV_ASSIGN MOD_ASSIGN ADD_ASSIGN SUB_ASSIGN LEFT_ASSIGN RIGHT_ASSIGN AND_ASSIGN XOR_ASSIGN OR_ASSIGN
%token TYPE_NAME TYPEDEF EXTERN STATIC AUTO REGISTER SIZEOF
%token CHAR SHORT INT LONG SIGNED UNSIGNED FLOAT DOUBLE CONST VOLATILE VOID
%token STRUCT UNION ENUM ELLIPSIS
%token CASE DEFAULT IF ELSE SWITCH WHILE DO FOR GOTO CONTINUE BREAK RETURN

%type <node> external_declaration function_definition declaration declaration_specifiers
%type <node> init_declarator type_specifier struct_specifier struct_declaration 
%type <node> struct_declarator enum_specifier enumerator declarator direct_declarator pointer parameter_declaration
%type <node> identifier_list type_name abstract_declarator direct_abstract_declarator statement labeled_statement
%type <node> compound_statement expression_statement selection_statement iteration_statement jump_statement

%type <expr_node> equality_expression and_expression exclusive_or_expression inclusive_or_expression logical_and_expression logical_or_expression
%type <expr_node> unary_expression cast_expression multiplicative_expression additive_expression shift_expression relational_expression
%type <expr_node> conditional_expression assignment_expression expression constant_expression primary_expression postfix_expression initializer

%type <nodes> translation_unit statement_list declaration_list init_declarator_list parameter_list enumerator_list specifier_qualifier_list struct_declarator_list struct_declaration_list
%type <expr_nodes> argument_expression_list initializer_list

%type <string> unary_operator assignment_operator storage_class_specifier

%type <number_int> INT_CONSTANT
%type <string> STRING_LITERAL TYPE_NAME
%type <number_float> FLOAT_CONSTANT
%type <number_double> DOUBLE_CONSTANT
%type <number_unsigned> UNSIGNED_CONSTANT

%type <string> IDENTIFIER

%nonassoc NOELSE
%nonassoc ELSE
%nonassoc LINT
%nonassoc UINT


%start ROOT
%%

ROOT
    : translation_unit { g_root = $1; }

translation_unit
	: external_declaration { $$ = new NodeList($1); }
	| translation_unit external_declaration {
		$1->PushBack($2);
		$$ = $1;
	}
	;

external_declaration
	: function_definition { $$ = $1; }
	| declaration
	;

function_definition
	: declaration_specifiers declarator declaration_list compound_statement
	| declaration_specifiers declarator compound_statement {
		$$ = new FunctionDefinition($1, $2, $3);
	}
	| declarator declaration_list compound_statement
	| declarator compound_statement {
		$$ = new FunctionDefinition(new TypeSpecifier("int"), $1, $2);
	}
	;

declaration
	: declaration_specifiers ';'
	| declaration_specifiers init_declarator_list ';' { $$ = new Declaration($1, $2); }
	| storage_specifier declaration_specifiers ';' {
		$$ = new TypedefDeclaration($2, nullptr);
		typedef_started = false;
	}
	| storage_specifier declaration_specifiers init_declarator_list ';' {
		$$ = new TypedefDeclaration($2, $3);
		typedef_started = false;
	}
	;

storage_specifier
	: TYPEDEF {
		typedef_started = true;
	}

declaration_specifiers
	: type_specifier { $$ = $1; }
	| type_specifier declaration_specifiers
	;



init_declarator_list
	: init_declarator { $$ = new NodeList($1); }
	| init_declarator_list ',' init_declarator {
		$1->PushBack($3);
		$$ = $1;
	}
	;

init_declarator
	: declarator {
		$$ = new InitDeclarator($1, nullptr);
	}
	| declarator '=' initializer {
		$$ = new InitDeclarator($1, $3);
	}
	;

storage_class_specifier
	: TYPEDEF {
		typedef_started = true;
	}
	;

type_specifier
	: VOID {
		$$ = new TypeSpecifier("void");
	}
	| CHAR {
		$$ = new TypeSpecifier("char");
	}
	| INT {
		$$ = new TypeSpecifier("int");
	}
	| FLOAT {
		$$ = new TypeSpecifier("float");
	}
	| DOUBLE {
		$$ = new TypeSpecifier("double");
	}
	| SIGNED {
		$$ = new TypeSpecifier("int");
	}
	| UNSIGNED {
		$$ = new TypeSpecifier("unsigned");
	} %prec  LINT
	| UNSIGNED INT {
		$$ = new TypeSpecifier("unsigned");
	} %prec  UINT
	| SIGNED INT {
		$$ = new TypeSpecifier("int");
	}
	| enum_specifier {
		$$ = $1;
	}
	| struct_specifier {
		$$ = $1;
	}
	| TYPE_NAME {
		$$ = new TypeSpecifier(*$1);
		delete $1;
	}
	;

struct_specifier
	: STRUCT IDENTIFIER '{' struct_declaration_list '}' {
		$$ = new StructSpecifier(new Identifier(*$2), $4);
		delete $2;
	}
	| STRUCT '{' struct_declaration_list '}' {
		$$ = new StructSpecifier(nullptr, $3);
	}
	| STRUCT IDENTIFIER {
		$$ = new StructSpecifier(new Identifier(*$2), nullptr);
		delete $2;
	}
	;

struct_declaration_list
	: struct_declaration {
		$$ = new NodeList($1);
	}
	| struct_declaration_list struct_declaration {
		$1->PushBack($2);
		$$ = $1;
	}
	;

struct_declaration
	: specifier_qualifier_list struct_declarator_list ';' {
		$$ = new StructDeclaration($1, $2);
	}
	;

specifier_qualifier_list
	: type_specifier
	;

struct_declarator_list
	: struct_declarator {
		$$ = new NodeList($1);
	}
	| struct_declarator_list ',' struct_declarator {
		$1->PushBack($3);
		$$ = $1;
	}
	;

struct_declarator
	: declarator {
		$$ = $1;
	}
	| ':' constant_expression
	| declarator ':' constant_expression
	;

enum_specifier
	: ENUM '{' enumerator_list '}' {$$ = new EnumSpecifier("",$3);}
	| ENUM IDENTIFIER '{' enumerator_list '}' {$$ = new EnumSpecifier(*$2, $4);
		delete $2;}
	| ENUM IDENTIFIER {$$ = new EnumSpecifier(*$2,nullptr);
		delete $2;}
	;

enumerator_list
	: enumerator {$$ = new NodeList($1);}
	| enumerator_list ',' enumerator {
		$1->PushBack($3);
		$$ = $1;}
	;

enumerator
	: IDENTIFIER	{
		$$ = new Enumerator(*$1, nullptr); 
		delete $1;
	}
	| IDENTIFIER '=' constant_expression {
		$$ = new Enumerator(*$1, $3); 
		delete $1;
		}
	;

declarator
	: pointer direct_declarator {
		$$ = new Declarator($1, $2);
	}
	| direct_declarator {
		$$ = new Declarator(nullptr, $1);
	}
	;

direct_declarator
	: IDENTIFIER {
		if (typedef_started) {
			typedef_list.push_back(*$1);
		}
		$$ = new DirectDeclarator(DirDeclType::var, new Identifier(*$1), nullptr, nullptr);
		delete $1;
	}
	| IDENTIFIER '(' ')' {
		$$ = new DirectDeclarator(DirDeclType::fn, new Identifier(*$1), nullptr, nullptr);
		delete $1;
	}
	| IDENTIFIER '(' VOID ')' {
		$$ = new DirectDeclarator(DirDeclType::fn, new Identifier(*$1), nullptr, nullptr);
		delete $1;
	}
	| IDENTIFIER '(' parameter_list ')' {
		$$ = new DirectDeclarator(DirDeclType::fn, new Identifier(*$1), $3, nullptr);
		delete $1;
	}
	| IDENTIFIER '[' constant_expression ']' {
		$$ = new DirectDeclarator(DirDeclType::arr, new Identifier(*$1), nullptr, $3);
		delete $1;
	}
	;

pointer
	: '*' {
		$$ = new Pointer(nullptr);
	}
	| '*' pointer {
		$$ = new Pointer($2);
	}
	;


parameter_list
	: parameter_declaration {
		$$ = new NodeList($1);
	}
	| parameter_list ',' parameter_declaration {
		$1->PushBack($3);
		$$ = $1;
	}
	;

parameter_declaration
	: declaration_specifiers declarator {
		$$ = new ParameterDeclaration($1, $2);
	}
	| declaration_specifiers {
		$$ = new ParameterDeclaration($1, nullptr);
	}
	;

type_name
	: specifier_qualifier_list
	| specifier_qualifier_list abstract_declarator
	;

abstract_declarator
	: pointer
	;

initializer
	: assignment_expression {$$ = $1;}
	| '{' initializer_list '}' {$$ = new Initializer($2);}
	| '{' initializer_list ',' '}' {$$ = new Initializer($2);}
	;

initializer_list
	: initializer {$$ = new ExprNodeList($1);}
	| initializer_list ',' initializer {
		$1->PushBack($3);
		$$ = $1;}
	;


statement
	: labeled_statement
	| compound_statement {$$ = $1; }
	| expression_statement { $$ = $1; }
	| selection_statement { $$ = $1; }
	| iteration_statement
	| jump_statement { $$ = $1; }
	;

labeled_statement
	: CASE constant_expression ':' statement {
		$$ = new CaseStatement($2, $4);
	}
	| DEFAULT ':' statement {
		$$ = new CaseStatement(nullptr, $3);
	}
	;



compound_statement
	: '{' '}' {$$ = new CompoundStatement(nullptr, nullptr);}
	| '{' statement_list '}' { $$ = new CompoundStatement(nullptr, $2); }
	| '{' declaration_list '}' {
		$$ = new CompoundStatement($2, nullptr);
	}
	| '{' declaration_list statement_list '}'  {
		$$ = new CompoundStatement($2, $3);
	}
	;

expression_statement
	: ';' { $$ = new ExpressionStatement(nullptr); }
	| expression ';' { $$ = new ExpressionStatement($1); }
	;

selection_statement
	: IF '(' expression ')' statement {$$ = new SelectionStatement($3, $5, nullptr);} %prec NOELSE
	| IF '(' expression ')' statement ELSE statement {$$ = new SelectionStatement($3, $5, $7);}
	| SWITCH '(' expression ')' statement {
		$$ = new SwitchStatement($3, $5);
	}
	;


iteration_statement
	: WHILE '(' expression ')' statement {
		$$ = new IterationStatement($3,$5, nullptr, nullptr, false);
	}
	| DO statement WHILE '(' expression ')' ';' {
		$$ = new IterationStatement($5, $2, nullptr, nullptr, true);
	}
	| FOR '(' expression_statement expression ';' ')' statement {
		$$ = new IterationStatement($4, $7, $3, nullptr, false);
	}
	| FOR '(' expression_statement expression ';' expression ')' statement {
		$$ = new IterationStatement($4, $8, $3, new ExpressionStatement($6), false);
	}
	| FOR '(' expression_statement ';' ')' statement {
		$$ = new IterationStatement(nullptr, $6, $3, nullptr, false);
	}
	;

jump_statement
	: CONTINUE ';' {
		$$ = new JumpStatement("continue");
	}
	| BREAK ';' {
		$$ = new JumpStatement("break");
	}
	| RETURN ';' {
		$$ = new ReturnStatement(nullptr);
	}
	| RETURN expression ';' {
		$$ = new ReturnStatement($2);
	}
	;

statement_list
	: statement { $$ = new NodeList($1); }
	| statement_list statement { $1->PushBack($2); $$=$1; }
	;

declaration_list
	: declaration { $$ = new NodeList($1); }
	| declaration_list declaration { $1->PushBack($2); $$=$1; }
	;



primary_expression
	: INT_CONSTANT {
		$$ = new IntConstant($1);
	}
	| FLOAT_CONSTANT {
		$$ = new FloatConstant($1);
	}
	| DOUBLE_CONSTANT {
		$$ = new DoubleConstant($1);
	}
	| UNSIGNED_CONSTANT {
		$$ = new UnsignedConstant($1);
	}
	| STRING_LITERAL {
		$$ = new StringLiteral(*$1);
		delete $1;
	}
	| IDENTIFIER {
		Node *tmp = new Identifier(*$1);
		delete $1;
		$$ = new PrimaryExpression(tmp);
	}
	| '(' expression ')' {
		$$ = $2;
	}
	;

postfix_expression
	: primary_expression
	| postfix_expression '[' expression ']' {
		$$ = new PointerIndexExpression($1, $3);
	}
	| postfix_expression '(' ')' {
		$$ = new FunctionCallExpression($1, nullptr);
	}
	| postfix_expression '(' VOID ')' {
		$$ = new FunctionCallExpression($1, nullptr);
	}
	| postfix_expression '(' argument_expression_list ')' {
		$$ = new FunctionCallExpression($1, $3);
	}
	| postfix_expression '.' IDENTIFIER {
		$$ = new StructAccessExpression($1, new Identifier(*$3), false);
		delete $3;
	}
	| postfix_expression PTR_OP IDENTIFIER {
		$$ = new StructAccessExpression($1, new Identifier(*$3), true);
		delete $3;
	}
	| postfix_expression INC_OP {
		$$ = new PostCrementExpression("add", $1);
	}
	| postfix_expression DEC_OP {
		$$ = new PostCrementExpression("sub", $1);
	}
	;

argument_expression_list
	: assignment_expression {
		$$ = new ExprNodeList($1);
	}
	| argument_expression_list ',' assignment_expression {
		$1->PushBack($3);
		$$ = $1;
	}
	;

unary_expression
	: postfix_expression
	| INC_OP unary_expression {
		ExprNode *one = new IntConstant(1);
		$$ = new ArithmeticExpression("add", $2, one);
	}
	| DEC_OP unary_expression {
		ExprNode *one = new IntConstant(1);
		$$ = new ArithmeticExpression("sub", $2, one);
	}
	| unary_operator cast_expression {
		$$ = new UnaryExpression(*$1, $2); 
		delete $1;
	}
	| SIZEOF unary_expression {$$ = new SizeExpression($2, nullptr);}
	| SIZEOF '(' type_name ')' {$$ = new SizeExpression(nullptr, $3);}


	;

unary_operator
	: '&' { $$ = new std::string("and"); }
	| '*' { $$ = new std::string("star"); }
	| '+' { $$ = new std::string("plus"); }
	| '-' { $$ = new std::string("minus"); }
	| '~' { $$ = new std::string("tilde"); }
	| '!' { $$ = new std::string("exclamation"); }
	;

cast_expression
	: unary_expression {$$ = $1;}
	;

multiplicative_expression
	: cast_expression
	| multiplicative_expression '*' cast_expression {
		$$ = new ArithmeticExpression("mul", $1, $3);
	}
	| multiplicative_expression '/' cast_expression {
		$$ = new ArithmeticExpression("div", $1, $3);
	}
	| multiplicative_expression '%' cast_expression {
		$$ = new ArithmeticExpression("rem", $1, $3);
	}
	;

additive_expression
	: multiplicative_expression { $$ = $1; }
	| additive_expression '+' multiplicative_expression {
		$$ = new ArithmeticExpression("add", $1, $3);
	}
	| additive_expression '-' multiplicative_expression {
		$$ = new ArithmeticExpression("sub", $1, $3);
	}
	;

shift_expression
	: additive_expression
	| shift_expression LEFT_OP additive_expression {$$ = new BitwiseExpression("shl", $1, $3);}
	| shift_expression RIGHT_OP additive_expression {$$ = new BitwiseExpression("shr", $1, $3);}
	;

relational_expression
	: shift_expression
	| relational_expression '<' shift_expression {$$ = new LogicalExpression("cmp lt", $1, $3);}
	| relational_expression '>' shift_expression {$$ = new LogicalExpression("cmp gt", $1, $3);}
	| relational_expression LE_OP shift_expression {$$ = new LogicalExpression("cmp le", $1, $3);}
	| relational_expression GE_OP shift_expression {$$ = new LogicalExpression("cmp ge", $1, $3);}
	;

equality_expression
	: relational_expression
	| equality_expression EQ_OP relational_expression {$$ = new LogicalExpression("cmp eq", $1, $3);}
	| equality_expression NE_OP relational_expression {$$ = new LogicalExpression("cmp ne", $1, $3);}
	;

and_expression
	: equality_expression
	| and_expression '&' equality_expression {$$ = new BitwiseExpression("and", $1, $3);}
	;

exclusive_or_expression
	: and_expression
	| exclusive_or_expression '^' and_expression {$$ = new BitwiseExpression("xor", $1, $3);}
	;

inclusive_or_expression
	: exclusive_or_expression
	| inclusive_or_expression '|' exclusive_or_expression {$$ = new BitwiseExpression("or", $1, $3);}
	;

logical_and_expression
	: inclusive_or_expression
	| logical_and_expression AND_OP inclusive_or_expression  {$$ = new LogicalExpression("and", $1, $3);}
	;

logical_or_expression
	: logical_and_expression
	| logical_or_expression OR_OP logical_and_expression {$$ = new LogicalExpression("or", $1, $3);}
	;

conditional_expression
	: logical_or_expression
	;

assignment_expression
	: conditional_expression { $$ = $1; }
	| unary_expression assignment_operator assignment_expression {
		if( *($2) == "="){
			$$ = new AssignmentExpression($1, $3);
			delete $2;
		}
		else if( *($2) == "*=" ){
			ExprNode *tmp = new ArithmeticExpression("mul", $1, $3);
			$$ = new AssignmentExpression($1, tmp);
			delete $2;
		}
		else if ( *($2) == "/="){
			ExprNode *tmp = new ArithmeticExpression("div", $1, $3);
			$$ = new AssignmentExpression($1, tmp);
			delete $2;

		}	
		else if ( *($2) == "%="){
			ExprNode *tmp = new ArithmeticExpression("rem", $1, $3); // from here on below names will need changing.
			$$ = new AssignmentExpression($1, tmp);
		}
		else if ( *($2) == "+="){
			ExprNode *tmp = new ArithmeticExpression("add", $1, $3);
			$$ = new AssignmentExpression($1, tmp);
		}
		else if ( *($2) == "-="){
			ExprNode *tmp = new ArithmeticExpression("sub", $1, $3);
			$$ = new AssignmentExpression($1, tmp);
		}
		else if ( *($2) == "<<="){
			ExprNode *tmp = new BitwiseExpression("shl", $1, $3);
			$$ = new AssignmentExpression($1, tmp);
		}
		else if ( *($2) == ">>="){
			ExprNode *tmp = new BitwiseExpression("shr", $1, $3);
			$$ = new AssignmentExpression($1, tmp);
		}
		else if ( *($2) == "&="){
			ExprNode *tmp = new BitwiseExpression("and", $1, $3);
			$$ = new AssignmentExpression($1, tmp);
		}
		else if ( *($2) == "^="){
			ExprNode *tmp = new BitwiseExpression("xor", $1, $3);
			$$ = new AssignmentExpression($1, tmp);
		}
		else if ( *($2) == "|="){
			ExprNode *tmp = new BitwiseExpression("or", $1, $3);
			$$ = new AssignmentExpression($1, tmp);
		}
	}
	;

assignment_operator
	: '=' {$$ = new std::string("=");} // wrong
	| MUL_ASSIGN {$$ = new std::string("*=");}
	| DIV_ASSIGN {$$ = new std::string("/=");}
	| MOD_ASSIGN {$$ = new std::string("%=");}
	| ADD_ASSIGN {$$ = new std::string("+=");}
	| SUB_ASSIGN {$$ = new std::string("-=");}
	| LEFT_ASSIGN {$$ = new std::string("<<=");}
	| RIGHT_ASSIGN {$$ = new std::string(">>=");}
	| AND_ASSIGN {$$ = new std::string("&=");}
	| XOR_ASSIGN {$$ = new std::string("^=");}
	| OR_ASSIGN {$$ = new std::string("|=");}
	;

expression
	: assignment_expression
	;

constant_expression
	: conditional_expression
	;

%%

Node *g_root;

Node *ParseAST(std::string file_name)
{
  yyin = fopen(file_name.c_str(), "r");
  if(yyin == NULL){
    std::cerr << "Couldn't open input file: " << file_name << std::endl;
    exit(1);
  }
  g_root = nullptr;
  yyparse();
  return g_root;
}
