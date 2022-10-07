//
//  alloc.h
//  test
//
//  Created by dawn on 2019/12/14.
//  Copyright © 2019 chuyi. All rights reserved.
//

#ifndef alloc_h
#define alloc_h

#include "macro.h"
/*
 小端模式：低位放在低地址
 栈：空递减
 */
/**
@函数调用过程
 r_ax=8,     //通用寄存器
 r_bx=9,
 r_cx=10,
 r_dx=11,
 r_di=12,
 r_si=13,
 r_ex=14,
 r_bp=15,
 r_ax 到 r_fx、r_fp为函数调用时保存的上下文;(具体见后端实现，可能有出入)，注意r_ra的保存。
 函数调用前需要将参数压栈(若需)，将返回值空间压栈(若需，现阶段不需要，因为只允许返回int)，然后将r_ra压栈，记录新的断点地址r_ra，然后进入函数;
 进入函数后，函数将上下文压栈，将fp指向sp，然后将局部变量压栈;
 函数return时，将返回值放入返回值空间，将局部变量退栈，将上下文退栈还原，返回断点;
 返回断点后，将r_ra退栈还原，保存返回值，将返回值空间退栈(若需),将参数空间退栈(若需);
 */
enum reg_t{
    r_undefined=-1,
    r_zero=0,   //零寄存器
    //r_at=1,     //为汇编器预留
    r_ret_1=2,  //返回值
    r_ret_2=3,
    r_arg_1=4,  //函数参数
    r_arg_2=5,
    r_arg_3=6,
    r_arg_4=7,
    
    r_ax=8,     //通用寄存器
    r_bx=9,
    r_cx=10,
    r_dx=11,
    r_di=12,
    r_si=13,
    r_ex=14,
    r_fx=15,
    
    //16-23 考虑到可能有特殊含义，暂未使用
    r_tmp_1=24,    //通用寄存器，编译器后端使用
    r_tmp_2=25,
    //26-27 预留给中断
    //r_gp=28,  //全局指针，先预留下来
    r_sp=29,    //栈顶指针寄存器
    r_fp=30,    //栈帧指针寄存器
    r_ra=31,   //函数返回地址ret addr
};
string get_reg_name(reg_t reg);
class Tmp_var_ctrl{ //单例
    string prefix=PREFIX_TMP_VAR;
    set<string>using_var;
    vector<string>free_var;
    int free_num=0;
    int total_num=0;
    Tmp_var_ctrl(){};
    Tmp_var_ctrl& operator=(Tmp_var_ctrl& copy)=delete;
    static Tmp_var_ctrl* shared_impl;
public:
    static Tmp_var_ctrl* get_shared_impl(){
        if(shared_impl==NULL)
            return shared_impl=new Tmp_var_ctrl();
        return shared_impl;
    }
    /**
     *  根据前缀判断是否是临时变量。
     */
    bool is_tmp_var(string var){
        if(var.length()>=prefix.length() && var.substr(0,prefix.length())==prefix){
            return true;
        }
        return false;
    }
    string alloc_tmp_var();
    void free_tmp_var(string var_name){
        if(using_var.count(var_name)==0){
            cout<<"临时变量不存在，无法释放"<<endl;
            cout<<var_name<<endl;
            exit(-1);
        }
        free_var[free_num]=var_name;
        free_num++;
        using_var.erase(var_name);
    }
};
#endif /* alloc_h */
