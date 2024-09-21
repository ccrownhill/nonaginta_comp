** GamePlan: **
*** monday ***
Constantin: Finish function arguments, 
Orlan:++ --, for loops, Control flow,

*** tuesday *** 
finish of intermediate features
Constantin: 
Orlan: 

*** wednesday ***
Start advanced features
Constantin:
Orlan:

*** thursday ***
Constantin:
Orlan:

*** friday ***
Constantin:
Orlan:



CFGEL:
Control Flow Graph Element. 
This is an instruction and every function is one control flow graph.

ctx.get_reg(vr_out_):
	gets register assigned to vr_out virtual register
vr_out:
	ctx.set_reg(vr_out_);
	this is the register where we wish to store the result and is automatically set by the line above.

ctx.get_reg(op_.val):
	This gets the register assigned to op.val virtual register

LLVM:
	to see on godbolt use clang with flags -S -emit-llvm
	stream << "%" << vr_out_ << " = " << op_type_ << " i32 ";
	this for example shows a % before a virtual reg
	you then show virutal reg = operator i32 
	where i32 is a 32 bit signed integer
