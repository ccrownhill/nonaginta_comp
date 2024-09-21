%option noyywrap

%{
  // A lot of this lexer is based off the ANSI C grammar:
  // https://www.lysator.liu.se/c/ANSI-C-grammar-l.html#MUL-ASSIGN
  // Avoid error "error: `fileno' was not declared in this scope"
  extern "C" int fileno(FILE *stream);
  #include "typedef_map.hpp"
  #include "parser.tab.hpp"
  #include <iostream>
%}

D	  [0-9]
L	  [a-zA-Z_]
H   [a-fA-F0-9]
E	  [Ee][+-]?{D}+
FS  (f|F|l|L)
IS  (u|U|l|L)*

%%

"/*"(.*)"*/"\n			{/* consumes comment - TODO you might want to process and emit it in your assembly for debugging */}

"auto"			{return(AUTO);}
"break"			{return(BREAK);}
"case"			{return(CASE);}
"char"			{return(CHAR);}
"const"			{return(CONST);}
"continue"  {return(CONTINUE);}
"default"		{return(DEFAULT);}
"do"			  {return(DO);}
"double"		{return(DOUBLE);}
"else"			{return(ELSE);}
"enum"			{return(ENUM);}
"extern"		{return(EXTERN);}
"float"			{return(FLOAT);}
"for"			  {return(FOR);}
"goto"			{return(GOTO);}
"if"			  {return(IF);}
"int"			  {return(INT);}
"long"			{return(LONG);}
"register"	{return(REGISTER);}
"return"		{std::cout << "return" << std::endl; return(RETURN);}
"short"			{return(SHORT);}
"signed"		{return(SIGNED);}
"sizeof"		{return(SIZEOF);}
"static"		{return(STATIC);}
"struct"		{return(STRUCT);}
"switch"		{return(SWITCH);}
"typedef"		{return(TYPEDEF);}
"union"			{return(UNION);}
"unsigned"	{return(UNSIGNED);}
"void"			{return(VOID);}
"volatile"	{return(VOLATILE);}
"while"			{return(WHILE);}

{L}({L}|{D})*		{
  yylval.string = new std::string(yytext);
  if (std::find(typedef_list.begin(), typedef_list.end(), *yylval.string) != typedef_list.end()) {
    std::cout << "typedef: " << yytext << std::endl;
    return TYPE_NAME;
  } else {
  std::cout << "ident: " << yytext << std::endl;
    return(IDENTIFIER);
  }
}

0[xX]{H}+		{yylval.number_int = (int)strtol(yytext, NULL, 0); return(INT_CONSTANT);}
0{D}+		    {yylval.number_int = (int)strtol(yytext, NULL, 0); return(INT_CONSTANT);}
{D}+		      {yylval.number_int = (int)strtol(yytext, NULL, 0); return(INT_CONSTANT);}
L?'(\\.|[^\\'])+'	{
  yytext++;
  yytext[strlen(yytext)-1] = '\0';
  if (yytext[0] == '\\') {
    switch(yytext[1])
    {
      case '0': yylval.number_int=0x00;
      break;
      case '\'': yylval.number_int=0x27;
      break;
      case '"': yylval.number_int=0x22;
      break;
      case '?': yylval.number_int=0x3f;
      break;
      case '\\': yylval.number_int=0x5c;
      break;
      case 'a': yylval.number_int=0x07;
      break;
      case 'b': yylval.number_int=0x08;
      break;
      case 'f': yylval.number_int=0x0c;
      break;
      case 'n': yylval.number_int=0x0a;
      break;
      case 'r': yylval.number_int=0x0d;
      break;
      case 't': yylval.number_int=0x09;
      break;
      case 'v': yylval.number_int=0x0b;
      break;
      default:
        std::cerr << "other escape" << std::endl;
    }
  } else {
    yylval.number_int = yytext[0];
  }
  return(INT_CONSTANT);}

{D}+{E}{FS}		        {yylval.number_float = strtof(yytext, NULL); return(FLOAT_CONSTANT);}
{D}*"."{D}+({E})?{FS}	{yylval.number_float = strtof(yytext, NULL); return(FLOAT_CONSTANT);}
{D}+"."{D}*({E})?{FS}	{yylval.number_float = strtof(yytext, NULL); return(FLOAT_CONSTANT);}

{D}+{E}		        {yylval.number_double = strtod(yytext, NULL); return(DOUBLE_CONSTANT);}
{D}*"."{D}+({E})?	{yylval.number_double = strtod(yytext, NULL); return(DOUBLE_CONSTANT);}
{D}+"."{D}*({E})?	{yylval.number_double = strtod(yytext, NULL); return(DOUBLE_CONSTANT);}

0[xX]{H}+{IS}		{yylval.number_int = (unsigned)strtoul(yytext, NULL, 0); return(UNSIGNED_CONSTANT);}
0{D}+{IS}		    {yylval.number_int = (unsigned)strtoul(yytext, NULL, 0); return(UNSIGNED_CONSTANT);}
{D}+{IS}		      {yylval.number_int = (unsigned)strtoul(yytext, NULL, 0); return(UNSIGNED_CONSTANT);}





L?\"(\\.|[^\\"])*\"	{
  yytext++; // skip " at beginning
  yytext[strlen(yytext) - 1] = '\0';
  yylval.string = new std::string(yytext);
  size_t pos = 0;
  // do {
  //   pos = yylval.string->find("\\", pos);
  //   if (pos != std::string::npos) {
  //     yylval.string->replace(pos, 1, "\\\\");
  //     pos += 2;
  //   }
  // } while (pos != std::string::npos);
  std::cout << "string literal: " << *yylval.string << std::endl;
  return(STRING_LITERAL);
}

"..."      {return(ELLIPSIS);}
">>="			 {return(RIGHT_ASSIGN);}
"<<="      {return(LEFT_ASSIGN);}
"+="			 {return(ADD_ASSIGN);}
"-="       {return(SUB_ASSIGN);}
"*="       {return(MUL_ASSIGN);}
"/="			 {return(DIV_ASSIGN);}
"%="			 {return(MOD_ASSIGN);}
"&="       {return(AND_ASSIGN);}
"^="			 {return(XOR_ASSIGN);}
"|="       {return(OR_ASSIGN);}
">>"       {return(RIGHT_OP);}
"<<"       {return(LEFT_OP);}
"++"			 {return(INC_OP);}
"--"			 {return(DEC_OP);}
"->"			 {return(PTR_OP);}
"&&"			 {return(AND_OP);}
"||"			 {return(OR_OP);}
"<="			 {return(LE_OP);}
">="			 {return(GE_OP);}
"=="			 {return(EQ_OP);}
"!="			 {return(NE_OP);}
";"			   {return(';');}
("{"|"<%") {return('{');}
("}"|"%>") {return('}');}
","			   {return(',');}
":"			   {return(':');}
"="			   {return('=');}
"("		     {std::cout << "(" << std::endl; return('(');}
")"			   {std::cout << "(" << std::endl; return(')');}
("["|"<:") {return('[');}
("]"|":>") {return(']');}
"."			   {return('.');}
"&"			   {return('&');}
"!"			   {return('!');}
"~"			   {return('~');}
"-"			   {return('-');}
"+"			   {return('+');}
"*"			   {return('*');}
"/"			   {return('/');}
"%"			   {return('%');}
"<"			   {return('<');}
">"			   {return('>');}
"^"			   {return('^');}
"|"			   {return('|');}
"?"			   {return('?');}

[ \a\b\t\v\f\n\r]		{/* ignore new lines and special sequences */}
.			              {/* ignore bad characters */}

%%

void yyerror (char const *s)
{
  fprintf(stderr, "Lexing error: %s\n", s);
  exit(1);
}
