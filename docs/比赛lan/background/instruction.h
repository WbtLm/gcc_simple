//
//  instruction.hpp
//  test
//
//  Created by dawn on 2020/1/9.
//  Copyright © 2020 chuyi. All rights reserved.
//

#ifndef instruction_h
#define instruction_h

#include "../macro.h"
#include "../alloc.h"
void ins_move_imm_to_reg(reg_t reg,uint imm);
void ins_push_reg(reg_t reg);

void ins_addi(reg_t dest_reg,reg_t a_reg,uint b_imm);
void ins_push_for_size(int size);
void ins_pop_for_size(int size);

void ins_move_reg_to_reg(reg_t reg_d,reg_t reg_s);

void ins_jal(uint address);
void ins_jalr(reg_t reg);

void move_reg_to_mem();
void ins_sw(reg_t r_addr,uint imm_offset,reg_t r_val);
void ins_lw(reg_t r_dest,reg_t r_addr,uint imm_offset);
void ins_move_reg_to_mem(reg_t reg,reg_t r_mem_addr,uint imm_offset);
void ins_move_mem_to_reg(reg_t r_mem_addr,uint imm_offset,reg_t reg_dest);

void ins_add(reg_t r_dest,reg_t r_a,reg_t r_b);
void ins_is_less_than(reg_t dest,reg_t r_a,reg_t r_b);

void ins_pop_reg(reg_t reg);
void ins_call_imm(string address_imm);
void ins_move_reg_to_pc(reg_t reg);
void ins_ret();
void ins_jmp(string sign_imm);
void ins_if(reg_t reg_cond,string then_sign);
void ins_mult(reg_t reg_dest,reg_t reg_a,reg_t reg_b);
void ins_mflo(reg_t reg_dest);
void ins_sub(reg_t reg_dest,reg_t reg_a,reg_t reg_b);
void ins_sltu(reg_t reg_dest,reg_t reg_a,reg_t reg_b);
void ins_or(reg_t reg_dest,reg_t reg_a,reg_t reg_b);
#endif /* instruction_hpp */
