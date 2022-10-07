//
//  instruction.cpp
//  test
//
//  Created by dawn on 2020/1/9.
//  Copyright © 2020 chuyi. All rights reserved.
//

#include "instruction.h"
extern string new_code;
void ins_move_imm_to_reg(reg_t reg,uint imm){
    string ret="";
    uint heigh,low;
    heigh=imm>>16;
    low=imm&0xffff;
    ret=ret+"lui "+to_string(reg)+" "+to_string(heigh)+"\n";
    ret=ret+"ori "+to_string(reg)+" "+to_string(reg)+" "+to_string(low)+"\n";
    new_code+=ret;
}
void ins_push_reg(reg_t reg){
    ins_move_reg_to_mem(reg, r_sp, 0);
    ins_addi(r_sp, r_sp, -unit);
}
void ins_pop_reg(reg_t reg){
    ins_addi(r_sp,r_sp,unit);
    ins_move_mem_to_reg(r_sp, 0, reg);
}
void ins_addi(reg_t dest_reg,reg_t a_reg,uint b_imm){
    string ret;
    b_imm&=0xffff;
    ret=ret+"addi "+to_string(dest_reg)+" "+to_string(a_reg)+" "+to_string(b_imm)+"\n";
    new_code+=ret;
}
void ins_push_for_size(int size){
    //sp-=size/4;
//    size/=4;
    ins_addi(r_sp, r_sp, -size);
}
void ins_pop_for_size(int size){
    ins_push_for_size(-size);
}
void ins_move_reg_to_reg(reg_t reg_d,reg_t reg_s){
    new_code=new_code+"or "+to_string(reg_d)+" "+to_string(reg_s)+" "+to_string(r_zero)+"\n";
}
void ins_jal(uint address){
    new_code=new_code+"jal "+to_string(address)+"\n";
}
void ins_jalr(reg_t reg){
    new_code=new_code+"jalr "+to_string((int)reg);
}
void ins_sw(reg_t r_addr,uint imm_offset,reg_t r_val){
    new_code=new_code+"sw "+to_string(r_addr)+" "+to_string(imm_offset)+" "+to_string(r_val)+"\n";
}
void ins_lw(reg_t r_dest,reg_t r_addr,uint imm_offset){
    new_code=new_code+"lw "+to_string(r_dest)+" "+to_string(r_addr)+" "+to_string(imm_offset)+"\n";
}
void ins_move_reg_to_mem(reg_t reg,reg_t r_mem_addr,uint imm_offset){
    ins_sw(r_mem_addr, imm_offset, reg);
}
void ins_move_mem_to_reg(reg_t r_mem_addr,uint imm_offset,reg_t reg_dest){
    ins_lw(reg_dest , r_mem_addr, imm_offset);
}

void ins_add(reg_t r_dest,reg_t r_a,reg_t r_b){
    new_code=new_code+"add "+to_string(r_dest)+" "+to_string(r_a)+" "+to_string(r_b)+"\n";
}
void ins_is_less_than(reg_t r_dest,reg_t r_a,reg_t r_b){
    new_code=new_code+"slt "+to_string(r_dest)+" "+to_string(r_a)+" "+to_string(r_b)+"\n";
}
void ins_call_imm(string address_imm_sign){
    new_code=new_code+"jal "+address_imm_sign+"\n";
}
void ins_move_reg_to_pc(reg_t reg){
    new_code=new_code+"jr "+to_string(reg)+"\n";
}
void ins_ret(){
    ins_move_reg_to_pc(r_ra);
}

void ins_jmp(string sign_imm){
    new_code=new_code+"j "+sign_imm+"\n";
}
void ins_if(reg_t reg_cond,string then_sign){
    new_code=new_code+"bne "+to_string(reg_cond)+" "+to_string(r_zero)+" "+then_sign;//debug_needed
}
void ins_mult(reg_t reg_dest,reg_t reg_a,reg_t reg_b){
    new_code=new_code+"mult "+to_string(reg_a)+" "+to_string(reg_b)+"\n";
    ins_mflo(reg_dest);
}
void ins_mflo(reg_t reg_dest){
    new_code=new_code+"mflo "+to_string(reg_dest)+"\n";
}
void ins_sub(reg_t reg_dest,reg_t reg_a,reg_t reg_b){
    new_code=new_code+"sub "+to_string(reg_dest)+" "+to_string(reg_a)+" "+to_string(reg_b)+"\n";
}
void ins_sltu(reg_t reg_dest,reg_t reg_a,reg_t reg_b){
    new_code=new_code+"sltu "+to_string(reg_dest)+" "+to_string(reg_a)+" "+to_string(reg_b)+"\n";
}
//
void ins_is_lager_than(reg_t r_dest,reg_t r_a,reg_t r_b){
    new_code=new_code+"slt "+to_string(r_dest)+" "+to_string(r_b)+" "+to_string(r_a)+"\n";
}
void ins_div(reg_t reg_dest,reg_t reg_a,reg_t reg_b){
    new_code=new_code+"div "+to_string(reg_a)+" "+to_string(reg_b)+"\n";
    
}
void ins_or(reg_t reg_dest,reg_t reg_a,reg_t reg_b){
    new_code=new_code+"or "+to_string(reg_dest)+" "+to_string(reg_a)+" "+to_string(reg_b)+"\n";
}
