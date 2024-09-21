What is not needed because it is for old style K&R function declarations:

Direct declarator:

```
| direct_declarator '(' identifier_list ')'
```

Function definition

```
: declaration_specifiers declarator declaration_list compound_statement
| declarator declaration_list compound_statement
```

However, need to support this

```
int foo(int a, double b);
int bar(char, float);
float baz(void);
```

and this:

```
int foo(char a, b);
```

where b will default to int.
HOW to do that??

What is an abstract_declarator??