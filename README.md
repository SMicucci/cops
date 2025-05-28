![COPS logo](cops-logo.svg)

# COPS

## Naming Convention

for collision avoidance are used prefix:  
- macro case => `COPS_`
- pascal case => `Cops`
- camel case => `cops_`
- snake case => `cops_`
- flat case => `cops_`

**macro**:  
- constant = macro case `CONST_MACRO`
- function = camel case `macroFunction()`

**struct**:  
- standard = snake case `struct my_struct`
- value = flat case `int myint`
- alias = snake case + '_t' `my_struct_t`
- pointer = pascal case `MyStruct`

**enum**:  
- standard = snake case `enum my_enum`
- value = macro case `ENUM_VAL`
- alias = 'e_' + snake case + '_t' `e_my_enum_t`
- pointer = 'E' + pascal case `EMyEnum`

**union**:  
- standard = snake case `union my_union`
- value = flat case `int intcase`
- alias = 'u_' + snake case + '_t' `u_my_enum_t`
- pointer = 'U' + pascal case `UMyEnum`

**function**:  
- standard = pascal case `void MyFunc()`
- static = camel case `static void myFunc()`
- argument = snake case `void MyFunc(int first_arg, int last_arg)`
- "method" = '<struct pointer>_' + pascal case `void MyStruct_MyFunc()`
- function pointer = flat case + '_FnPtr' `void (*myfunc_FnPtr)()`

**variable**:  
- local = snake case `int my_var`
- global = camel case `int myVar`
