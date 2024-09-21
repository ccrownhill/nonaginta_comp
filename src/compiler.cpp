#include <fstream>
#include <iostream>

#include "cli.h"
#include "ast.hpp"

Node *Parse(CommandLineArguments &args)
{
    std::cout << "Parsing: " << args.compile_source_path << std::endl;
    auto root = ParseAST(args.compile_source_path);
    std::cout << "AST parsing complete" << std::endl;
    return root;
}

// Output the pretty print version of what was parsed to the .printed output
// file.
void PrettyPrintAst(Node *root, CommandLineArguments &args)
{
    auto ast_output_path = args.compile_output_path + ".ast_printed";

    std::cout << "Printing parsed AST..." << std::endl;
    std::ofstream output(ast_output_path, std::ios::trunc);
    root->Print(output);
    output.close();
    std::cout << "Printed parsed AST to: " << ast_output_path << std::endl;
}

void PrettyPrintIR(IR &ir, CommandLineArguments &args)
{
    auto ir_output_path = args.compile_output_path + ".ir_printed";
    std::ofstream output(ir_output_path, std::ios::trunc);
    std::cout << "Printing IR..." << std::endl;
    ir.Print(output);
    output.close();
    std::cout << "Printed IR to: " << ir_output_path << std::endl;
}

IR ast2ir(Node *root)
{
    // TODO make it enter a global scope
    
    IR ir;
    IRGenContext ir_ctx(ir.get_global_scope());
    ir_ctx.set_is_global(true);
	ir.add_const({0}, 8);
    root->EmitCFG(ir, ir_ctx);
	

    return ir;
}
// TODO make separate function for IR generation
// Compile from the root of the AST and output this to the
// args.compiledOutputPath file.
void Compile(IR &ir, CommandLineArguments &args)
{
    // Create a Context. This can be used to pass around information about
    // what's currently being compiled (e.g. function scope and variable names).

    CodeGenContext c_ctx;

    std::cout << "Compiling using IR..." << std::endl;
    std::ofstream output(args.compile_output_path, std::ios::trunc);
    ir.EmitRISC(output, c_ctx);
    output.close();
    std::cout << "Compiled to: " << args.compile_output_path << std::endl;
}

int main(int argc, char **argv)
{
    // Parse CLI arguments to fetch the source file to compile and the path to output to.
    // This retrives [source-file.c] and [dest-file.s], when the compiler is invoked as follows:
    // ./bin/c_compiler -S [source-file.c] -o [dest-file.s]
    auto command_line_arguments = ParseCommandLineArgs(argc, argv);

    // Parse input and generate AST
    auto ast_root = Parse(command_line_arguments);
    if (ast_root == nullptr)
    {
        // Check something was actually returned by parseAST().
        std::cerr << "The root of the AST was a null pointer. Likely the root was never initialised correctly during parsing." << std::endl;
        return 3;
    }
    PrettyPrintAst(ast_root, command_line_arguments);
    IR ir = ast2ir(ast_root);
    PrettyPrintIR(ir, command_line_arguments);
    Compile(ir, command_line_arguments);

    // Clean up afterwards.
    delete ast_root;
    return 0;
}
