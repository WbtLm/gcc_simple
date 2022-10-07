::push_global_func __var_main
::push_global_var_begin
::push_global_var_end
goto __var_main

__var_main:
::push_registers
::push_stack_var 0
::var_stack_alloc_begin
::var_stack_alloc_end
::push_stack_tmp 4
::push_stack_tmp_unfree 0
::tmp_var_stack_alloc_begin
__T_0 4
::tmp_var_stack_alloc_end
__T_0 = 1 + 3
%__FUNC_RET__ = __T_0
::free_tmp_var __T_0 
goto __sign_0_func_return
__block_1_exit:
__sign_0_func_return:
::pop_stack_tmp_unfree 0
::pop_stack_tmp 4
::pop_stack_var 0
::pop_registers
::return
