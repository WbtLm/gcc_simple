::push_global_func __var_main
::push_global_var_begin
__var_ret 4
::push_global_var_end
goto __var_main

__var_main:
::push_registers
::push_stack_var 4
::var_stack_alloc_begin
__block_1__var_i 4
::var_stack_alloc_end
::push_stack_tmp 12
::push_stack_tmp_unfree 0
::tmp_var_stack_alloc_begin
__T_0 4
__T_1 4
__T_2 4
::tmp_var_stack_alloc_end
__block_1__var_i = 2
__T_0 = __block_1__var_i == 1
__T_1 = __block_1__var_i == 2
__T_2 = __T_0 || __T_1
if __T_2
::free_tmp_var __T_0 __T_1 __T_2 
goto __sign_1_if_then
__sign_1_if_else:
__var_ret = 9
goto __sign_1_if_endif
__sign_1_if_then:
__var_ret = 1
__sign_1_if_endif:
%__FUNC_RET__ = __var_ret
goto __sign_0_func_return
__block_1_exit:
__sign_0_func_return:
::pop_stack_tmp_unfree 0
::pop_stack_tmp 12
::pop_stack_var 4
::pop_registers
::return
