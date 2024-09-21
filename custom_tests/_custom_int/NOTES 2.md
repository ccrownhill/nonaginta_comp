## Known Problems
### evaluating !0
currently due to the way it is implemented evaluating !0 will incorrectly return 0 when it should return 1
### shifting right for unsigned numbers
if you shift an unsigned number right this will currently do an arithmetic shift where it should do a logical shift.
### nesting if statements
as we do arithemetic to calculate jump targets i believe it may break. Currently it assumes jump label for continuation of code is block_false + 1 but this may be wrong.
### return statement

## TODO
### printing for llvm
printing for most of the llvm is not implemented but it is commented where it is not

