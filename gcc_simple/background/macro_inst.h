//
//  macro_inst.hpp
//  gcc_simple
//
//  Created by dawn on 2020/2/29.
//  Copyright © 2020 chuyi. All rights reserved.
//

#ifndef macro_inst_hpp
#define macro_inst_hpp
#include "../macro.h"
class var_node_t;
#include "back_var_ctrl.h"
/**
 * 注意：本函数会污染r_tmp_1(目标为寄存器的情况除外)，无法用于指针解引用（运行时确定地址）
 */
void move_val_to_ans(var_node_t vnode_a,string ans_name);
void move_val_to_reg(var_node_t vnode_a,reg_t reg);
void move_var_to_reg(string var,reg_t reg);
void move_reg_to_ans(reg_t reg,string ans);
/**
 *  将指针指向的变量赋给ans
 *  注意：本函数会污染r_tmp_1或者r_tmp_2(保证与reg_addr互异）。可用于指针解引用（运行时确定地址的）
 */
void release_ptr_to_ans(reg_t reg_addr,uint size,string ans_name);

/**
 *  将val赋给指针指向的空间
 */
void move_val_to_ptr_mem(var_node_t vnode_val,reg_t reg_ptr);
void ins_get_bool(reg_t reg_dest,reg_t reg_a);
void ins_get_bool_not(reg_t reg_dest,reg_t reg_a);
void ins_logic_get_not(reg_t reg_dest,reg_t reg_a);
#endif /* macro_inst_hpp */
