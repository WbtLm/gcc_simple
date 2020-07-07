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
//::push_stack_var 4
or 30 29 0
addi 29 29 65532
//::var_stack_alloc_begin
//::push_stack_tmp 12
//::push_stack_tmp_unfree 0
//::tmp_var_stack_alloc_begin
//__block_1__var_i = 2
lui 24 0
ori 24 24 2
sw 30 0 24
//__T_0 = __block_1__var_i == 1
lw 24 30 0
lui 25 0
ori 25 25 1
sub 24 24 25
sltu 24 0 24
addi 24 24 65535
sub 24 0 24
or 13 24 0
//__T_1 = __block_1__var_i == 2
lw 24 30 0
lui 25 0
ori 25 25 2
sub 24 24 25
sltu 24 0 24
addi 24 24 65535
sub 24 0 24
or 12 24 0
//__T_2 = __T_0 || __T_1
or 24 13 0
or 25 12 0
sltu 24 0 24
sltu 25 0 25
or 24 24 25
or 15 24 0
//if __T_2
//::free_tmp_var __T_0 __T_1 __T_2
//goto __sign_1_if_then
bne 15 0 sign___sign_1_if_then
//__sign_1_if_else:
sign:	__sign_1_if_else
//__var_ret = 9
lui 24 0
ori 24 24 9
sw 0 4 24
//goto __sign_1_if_endif
j sign___sign_1_if_endif
//__sign_1_if_then:
sign:	__sign_1_if_then
//__var_ret = 1
lui 24 0
ori 24 24 1
sw 0 4 24
//__sign_1_if_endif:
sign:	__sign_1_if_endif
//%__FUNC_RET__ = __var_ret
lw 2 0 4
//goto __sign_0_func_return
j sign___sign_0_func_return
//__block_1_exit:
sign:	__block_1_exit
//__sign_0_func_return:
sign:	__sign_0_func_return
//::pop_stack_tmp_unfree 0
//::pop_stack_tmp 12
//::pop_stack_var 4
or 30 29 0
addi 29 29 4
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
