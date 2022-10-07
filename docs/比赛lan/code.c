//start
//::push_global_func __var_main
//::push_global_var_begin
//goto __var_main
j sign___var_main
//__var_main:
sign:	__var_main
//::push_registers
sw 29 0 31
addi 29 29 65532
sw 29 0 8
addi 29 29 65532
sw 29 0 9
addi 29 29 65532
sw 29 0 10
addi 29 29 65532
sw 29 0 11
addi 29 29 65532
sw 29 0 12
addi 29 29 65532
sw 29 0 13
addi 29 29 65532
sw 29 0 14
addi 29 29 65532
sw 29 0 15
addi 29 29 65532
sw 29 0 30
addi 29 29 65532
sw 29 0 24
addi 29 29 65532
sw 29 0 25
addi 29 29 65532
//::push_stack_var 0
or 30 29 0
addi 29 29 0
//::var_stack_alloc_begin
//::push_stack_tmp 4
//::push_stack_tmp_unfree 0
//::tmp_var_stack_alloc_begin
//__T_0 = 1 + 3
lui 24 0
ori 24 24 1
lui 25 0
ori 25 25 3
add 24 24 25
or 13 24 0
//%__FUNC_RET__ = __T_0
or 2 13 0
//::free_tmp_var __T_0
//goto __sign_0_func_return
j sign___sign_0_func_return
//__block_1_exit:
sign:	__block_1_exit
//__sign_0_func_return:
sign:	__sign_0_func_return
//::pop_stack_tmp_unfree 0
//::pop_stack_tmp 4
//::pop_stack_var 0
or 30 29 0
addi 29 29 0
//::pop_registers
addi 29 29 4
lw 25 29 0
addi 29 29 4
lw 24 29 0
addi 29 29 4
lw 30 29 0
addi 29 29 4
lw 15 29 0
addi 29 29 4
lw 14 29 0
addi 29 29 4
lw 13 29 0
addi 29 29 4
lw 12 29 0
addi 29 29 4
lw 11 29 0
addi 29 29 4
lw 10 29 0
addi 29 29 4
lw 9 29 0
addi 29 29 4
lw 8 29 0
addi 29 29 4
lw 31 29 0
//::return
jr 31
