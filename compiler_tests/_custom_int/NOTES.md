## Known Problems
### evaluating !0 DONE
currently due to the way it is implemented evaluating !0 will incorrectly return 0 when it should return 1
### shifting right for unsigned numbers DONE
if you shift an unsigned number right this will currently do an arithmetic shift where it should do a logical shift.
### nesting if statements DONE
as we do arithemetic to calculate jump targets i believe it may break. Currently it assumes jump label for continuation of code is block_false + 1 but this may be wrong.

### return statement DONE

### problem with string
int f(char *s, char *t){
	while((*s++ = *t++) != '\0'){
		;
	}
	return 0;
}
this program is an example of one that does not work as it memory leaks. (SEGV)

### problem with i++
the inc operator does not pass value and then increment but increments and then returns its value


## TODO
### printing for llvm DONE
printing for most of the llvm is not implemented but it is commented where it is not

### add multiple enums working DONE
currently for multiple unnamed enum it will break

