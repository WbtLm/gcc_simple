//
//  macro_inst.cpp
//  gcc_simple
//
//  Created by dawn on 2020/2/29.
//  Copyright © 2020 chuyi. All rights reserved.
//

#include "macro_inst.h"

/**
 *  生成指令达成效果：将val移动到ans_name对应的符号中，若ans_name还未分配空间（不存在），则先分配空间，若已经存在，则校验大小是否相同。编译时有效。运行时无法确定vnode_a，故本函数失效。
 注意：本函数会污染r_tmp_1,目的为寄存器的情况除外
 */
void move_val_to_ans(var_node_t vnode_a,string ans_name){
    back_var_ctrl &var_ctrl = * back_var_ctrl::get_share_impl();
    
    //入参：vnode_ans,vnode_a
    //若ans已经存在，则无需分配空间
    //若需先为ans分配空间，若a的大小大于1，则ans放在stack中，若a的大小==1，则ans放在reg或stack中
    //a的值有三种可能(判断先后顺序)：imm.reg，stack，global
    
    bool ans_in_reg = var_ctrl.count_var_by_name(ans_name) && var_ctrl.get_var_by_name(ans_name).is_in_reg();
    //ans存在且在reg中 or ans不存在且可以在reg中
    if(ans_in_reg || (vnode_a.get_sizeof()==unit && var_ctrl.can_alloc_in_reg_pool() && !var_ctrl.count_var_by_name(ans_name))){
        reg_t reg=r_undefined;
        var_node_t vnode_ans;
        if(var_ctrl.count_var_by_name(ans_name)){   //存在则无需申请
            vnode_ans=var_ctrl.get_var_by_name(ans_name);
            reg=vnode_ans.get_reg();//ans一定在reg中
        }
        else{   //ans不存在，且可以在reg中
            reg=var_ctrl.alloc_reg();
            vnode_ans.set_reg(reg);
            vnode_ans.set_name(ans_name);
            vnode_ans.set_sizeof(unit);
        }
        
        if(!var_ctrl.count_var_by_name(vnode_ans.get_name())){ //若ans不存在，则将新申请的ans提交上去
            var_ctrl.push_var(vnode_ans);
        }
        //找a
        if(vnode_a.is_imm()){
            ins_move_imm_to_reg(reg, vnode_a.get_imm());
        }
        else if(vnode_a.is_in_reg()){//a in reg
            ins_move_reg_to_reg(reg, vnode_a.get_reg());
        }
        else if(vnode_a.is_stack_var()){    //a in stack mem
            ins_move_mem_to_reg(r_fp, -vnode_a.get_pos(), reg);
        }
        else// a is gloable
        {
            ins_move_mem_to_reg(r_zero, vnode_a.get_pos(), reg);
        }
        
    }
    else{   //ans只能放在reg外,此时要注意ans的大小，若大于1需要分段拷贝
        var_node_t vnode_ans;
        if(var_ctrl.count_var_by_name(ans_name)){   //存在则无需申请
            vnode_ans=var_ctrl.get_var_by_name(ans_name);
        }
        else{   //不存在，只能放在栈空间里
            ins_push_for_size(vnode_a.get_sizeof());
            vnode_ans.set_in_stack();
            vnode_ans.set_pos(var_ctrl.get_stack_offset_pos());
            vnode_ans.set_name(ans_name);
            vnode_ans.set_sizeof(vnode_a.get_sizeof());
            var_ctrl.push_stack_offset_pos(vnode_ans.get_sizeof());
        }
        
        if(!var_ctrl.count_var_by_name(vnode_ans.get_name())){ //若ans不存在，则将新申请的ans提交上去
            var_ctrl.push_var(vnode_ans);
        }
        /**
         * 小端，低字节低地址，高字节高地址
         */
        
        
        if(vnode_ans.is_stack_var()){ //若ans在栈中
            for(int i=0;i<vnode_ans.get_sizeof();i+=unit){//交换if和for的先后位置能提高性能
                //找a  //ans is stack
                if(vnode_a.is_imm()){
                    ins_move_imm_to_reg(r_tmp_1, vnode_a.get_imm());
                    ins_move_reg_to_mem(r_tmp_1, r_fp, -vnode_ans.get_pos());
                    break;
                }
                else if(vnode_a.is_in_reg()){//a in reg
                    ins_move_reg_to_mem(vnode_a.get_reg(), r_fp, -vnode_ans.get_pos());
                    break;
                }
                else if(vnode_a.is_stack_var()){    //a in stack mem
                    ins_move_mem_to_reg(r_fp,-(vnode_a.get_pos()+i), r_tmp_1);        //将栈中a取出放到r_tmp
                    ins_move_reg_to_mem(r_tmp_1, r_fp,-vnode_ans.get_pos()-(vnode_ans.get_sizeof()-unit)+i);      //将r_tmp放到栈中ans
                }
                else// a is gloable
                {
                    ins_move_mem_to_reg(r_zero, vnode_a.get_pos()+i, r_tmp_1);      //将全局a取出放到r_tmp
                    ins_move_reg_to_mem(r_tmp_1, r_fp,-(vnode_ans.get_pos()+i));      //将r_tmp放到栈中ans
                }
            }
        }
        else{
            //ans全局变量
            for(int i=0;i<vnode_ans.get_sizeof();i+=unit){//可以交换if和for的先后位置能提高性能
                //找a  //ans is stack
                if(vnode_a.is_imm()){
                    ins_move_imm_to_reg(r_tmp_1, vnode_a.get_imm());
                    ins_move_reg_to_mem(r_tmp_1, r_zero, vnode_ans.get_pos());
                    break;
                }
                else if(vnode_a.is_in_reg()){//a in reg
                    ins_move_reg_to_mem(vnode_a.get_reg(), r_zero, vnode_ans.get_pos());
                    break;
                }
                else if(vnode_a.is_stack_var()){    //a in stack mem
                    ins_move_mem_to_reg(r_fp,-vnode_a.get_pos()-(vnode_a.get_sizeof()-unit)+i, r_tmp_1);        //将栈中a取出放到r_tmp
                    ins_move_reg_to_mem(r_tmp_1, r_zero,(vnode_ans.get_pos()+i));      //将r_tmp放到栈中ans
                }
                else// a is gloable
                {
                    ins_move_mem_to_reg(r_zero, vnode_a.get_pos()+i, r_tmp_1);      //将全局a取出放到r_tmp
                    ins_move_reg_to_mem(r_tmp_1,r_zero,(vnode_ans.get_pos()+i));      //将r_tmp放到栈中ans
                }
            }
        }
        
    }
    
    return;
}
/**
 * 不会污染tmp寄存器
 */
void move_val_to_reg(var_node_t vnode_a,reg_t reg){

    if(vnode_a.is_in_reg()){
        ins_move_reg_to_reg(reg, vnode_a.get_reg());
    }
    else if(vnode_a.is_imm()){
        ins_move_imm_to_reg(reg, vnode_a.get_imm());
    }
    else if(vnode_a.is_stack_var()){
        ins_move_mem_to_reg(r_fp, -vnode_a.get_pos(), reg);
    }
    else//is global
    {
        ins_move_mem_to_reg(r_zero, vnode_a.get_pos(), reg);
    }
}
/**
 *  不会污染tmp寄存器
 */
void move_reg_to_ans(reg_t reg,string ans){
    var_node_t vnode_a;
    vnode_a.set_reg(reg);
    move_val_to_ans(vnode_a, ans);//reg中的情况不涉及污染临时寄存器。
}
/**
 *  不会污染tmp寄存器
 */
void move_var_to_reg(string var,reg_t reg){
    back_var_ctrl &var_ctrl = *back_var_ctrl::get_share_impl();
    if(var.length()&&(var[0]>='0'&&var[0]<='9')){
        var_node_t vnode;
        vnode.set_imm(atoi(var.c_str()));
        move_val_to_reg(vnode, reg);
        return;
    }
    if(var_ctrl.count_var_by_name(var)==0){
        cout<<"move_var_to_reg:var not found"<<endl;
        cout<<var<<endl;
        exit(-1);
    }
    var_node_t vnode = var_ctrl.get_var_by_name(var);
    move_val_to_reg(vnode, reg);
}
/**
 *  将地址指向的内容copy给ans
 *  reg_addr为存放地址的寄存器。因为寄存器的内容为运行时决定，故本函数无法使用vnode.set_pos方法构造var_node;
 *  所以只能按照ans的位置分类，手动将指向的内容copy给ans
 *  size单位是字
 *  注意：本函数会污染r_tmp_1或者r_tmp_2(保证与reg_addr互异）
 */
void release_ptr_to_ans(reg_t reg_addr,uint size,string ans_name){
    back_var_ctrl &var_ctrl = *back_var_ctrl::get_share_impl();
    var_node_t vnode_ans;
    reg_t reg_tmp = r_tmp_1;
    if(reg_addr == r_tmp_1){
        reg_tmp=r_tmp_2;
    }
    if(var_ctrl.count_var_by_name(ans_name)){   //存在则无需申请
        vnode_ans=var_ctrl.get_var_by_name(ans_name);
    }
    else{   //不存在，只能放在栈空间里（暂不考虑reg)
        /**
         *  使用栈空间时，要保证先修改r_sp，再使用栈空间。
         */
        ins_push_for_size(size);
        vnode_ans.set_in_stack();
        vnode_ans.set_pos(var_ctrl.get_stack_offset_pos());
        vnode_ans.set_name(ans_name);
        vnode_ans.set_sizeof(size);
        var_ctrl.push_stack_offset_pos(vnode_ans.get_sizeof());
    }
    if(!var_ctrl.count_var_by_name(vnode_ans.get_name())){ //若ans不存在，则将新申请的ans提交上去
        var_ctrl.push_var(vnode_ans);
    }
    if(vnode_ans.is_in_reg()){
        ins_move_mem_to_reg(reg_addr, r_zero, vnode_ans.get_reg());
    }
    else if(vnode_ans.is_stack_var()){
        for (int i=0; i<size; i+=unit) {
            ins_move_mem_to_reg(reg_addr, i,reg_tmp);
            ins_move_reg_to_mem(reg_tmp, r_fp, -vnode_ans.get_pos()-(vnode_ans.get_sizeof()-unit)+i);
        }
    }
    else if(vnode_ans.is_global()){
        for (int i=0; i<size; i+=unit) {
            ins_move_mem_to_reg(reg_addr, i,reg_tmp);
            ins_move_reg_to_mem(reg_tmp, r_zero, (vnode_ans.get_pos()+i));
        }
    }
    else{
        cout<<"error:release_ptr_to_ans:vnode_ans.else (is_imm)"<<endl;
        exit(-1);
    }
}
/**
 *  会污染r_tmp_2;
 */
void move_val_to_ptr_mem(var_node_t vnode_val,reg_t reg_ptr){
    if(vnode_val.is_in_reg()){
        ins_move_reg_to_mem(vnode_val.get_reg(), reg_ptr, 0);
    }
    else if(vnode_val.is_global()){
        for (int i=0; i<vnode_val.get_sizeof(); i+=4) {
            ins_move_mem_to_reg(r_zero, vnode_val.get_pos()+i, r_tmp_2);  //将值从变量中取出
            ins_move_reg_to_mem(r_tmp_2, reg_ptr, i);   //将值存入指针指向的内存,该内存认为是全局变量，故存的时候要把栈中变量转为全局变量的格式
        }
    }
    else if(vnode_val.is_stack_var()){
        for (int i=0; i<vnode_val.get_sizeof(); i+=4) {
            ins_move_mem_to_reg(r_fp, -vnode_val.get_pos()-(vnode_val.get_sizeof()-unit)+i, r_tmp_2);  //将值从变量中取出
            ins_move_reg_to_mem(r_tmp_2, reg_ptr, i);   //将值存入指针指向的内存,该内存认为是全局变量，故存的时候要把栈中变量转为全局变量的格式
        }
    }
    else if(vnode_val.is_imm()){
        ins_move_imm_to_reg(r_tmp_2, vnode_val.get_imm());
        ins_move_reg_to_mem(r_tmp_2, reg_ptr, 0);
    }
    
}
/**
 * reg_a值若是0，则返回0，若是非0值，则返回1
 * 允许对大于1的值进行操作
 */
void ins_get_bool(reg_t reg_dest,reg_t reg_a){
    ins_sltu(reg_dest, r_zero, reg_a);//zero<a?1:0;
}
/**
 * reg_a值若是0，则返回1，若是非0值，则返回1
 * 允许对大于1的值进行操作
 */
void ins_get_bool_not(reg_t reg_dest,reg_t reg_a){
    ins_get_bool(reg_dest, reg_a);
    //为了不污染临时变量。要使些手段。
    //while a is bool:   1-a == 0-(a-1) == 0-(a+(-1))
    ins_logic_get_not(reg_dest,reg_dest);
}
/**
 * reg_a必须是0或1，不然计算错误
 * 使用减法进行取反 dest = 1-a = 0-(a+(-1))
 */
void ins_logic_get_not(reg_t reg_dest,reg_t reg_a){
    ins_addi(reg_dest, reg_dest, -1);//a+(-1)
    ins_sub(reg_dest, r_zero, reg_dest);
}
void is_equal_to(reg_t reg_dest,reg_t reg_a,reg_t reg_b){
    ins_sub(reg_dest, reg_a, reg_b);
    ins_get_bool(reg_dest, reg_dest);
}
