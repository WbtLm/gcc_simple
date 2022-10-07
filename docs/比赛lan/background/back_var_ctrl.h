//
//  back_var_ctrl.h
//  test
//
//  Created by dawn on 2020/1/9.
//  Copyright © 2020 chuyi. All rights reserved.
//

#ifndef back_var_ctrl_h
#define back_var_ctrl_h
#include "../alloc.h"
#include "../macro.h"
#include "instruction.h"
class var_node_t{
    bool book_imm=false;
    int imm;
    string var_name;
    int var_size=0;
    bool book_stack=true;
    uint pos=0;
    reg_t reg_pos=r_undefined;
    bool book_global=false;
    bool book_func=false;
    // left value{left_addr,left_sizeof};
    bool book_left_able=false;
    uint left_addr;
    uint left_sizeof;
    string left_ptr_var;
    bool book_left_var=false;
public:
    void set_left_value(uint addr,uint size){
        left_addr=addr;
        left_sizeof=size;
        book_left_able=true;
        book_left_var=false;
    }
    void set_left_value(string var,uint size){
        left_ptr_var=var;
        left_sizeof=size;
        book_left_able=true;
        book_left_var=true;
    }
    void get_left_addr(reg_t reg_dest);
    uint get_left_sizeof(){
        return left_sizeof;
    }
    bool is_left_able(){
        return book_left_able;
    }
    void set_not_func(){
        book_func=false;
    }
    void set_func(){
        book_func=true;
    }
    bool is_func(){
        return book_func;
    }
    bool is_imm(){
        return book_imm;
    }
    int get_imm()
    {
        if(is_imm()==false){
            cout<<"get_imm:is_imm == false"<<endl;
            exit(-1);
        }
        return imm;
    }
    void set_imm(int _imm){
        book_imm=true;
        imm=_imm;
        set_not_reg();
        set_not_func();
        set_not_stack();
        book_global=false;
        
    }
    void set_not_imm(){
        book_imm=false;
    }
    void set_not_reg(){
        reg_pos=r_undefined;
    }
    void set_reg(reg_t reg){
        if(reg!=r_undefined){
            set_not_imm();
            set_not_stack();
            set_sizeof(unit);
        }
        reg_pos=reg;
        
    }
    reg_t get_reg(){
        if(reg_pos==r_undefined){
            cout<<"error:get_reg == r_undefined"<<endl;
            exit(-1);
        }
        return reg_pos;
    }
    bool is_in_reg(){
        if(is_imm())
            return false;
        return reg_pos!=r_undefined;
    }
    int get_sizeof(){
        if(is_imm()||is_in_reg()){
            var_size=unit;
        }
        return var_size;
    }
    void set_sizeof(int size){
        var_size=size;
    }
    int get_pos(){
        return pos;
    }
    void set_pos(int new_pos){
        pos=new_pos;
    }
    bool is_stack_var(){
        return book_stack;
    }
    string get_name(){
        return var_name;
    }
    void set_name(string new_name){
        var_name=new_name;
    }
    void set_in_stack(){
        book_stack=true;
    }
    void set_not_stack(){
        book_stack=false;
    }
    void set_in_mem_global(){
        set_not_stack();
        set_reg(r_undefined);
        book_global=true;
    }
    bool is_global(){
        return book_global;
    }
    var_node_t()=default;
    var_node_t(string name){
        var_name=name;
    }
    var_node_t(string name,bool is_stack){
        var_name=name;
        book_stack=is_stack;
    }
};
class back_var_ctrl{    //单例
    string debug_str;
    int start_mem_pos=unit;   //无论如何0地址一定要预留出来，0地址是为了判断地址是否合法的重要标志（包括编译器）。
    map<string,int>var_map;
    int stack_offset_pos=0;
    back_var_ctrl(){
        for(int i=0;i<sizeof(useable)/sizeof(reg_t);i++){
            reg_pool.push(useable[i]);
        }
        push_ret();
    }
    back_var_ctrl(back_var_ctrl&copy)=delete;
    static back_var_ctrl* back_var_ctrl_impl;
    stack_random<var_node_t>var_stack;
    stack_random<reg_t>reg_pool;
    const reg_t useable[8]={r_ax, r_bx, r_cx, r_dx, r_ex, r_fx, r_di, r_si};
    stack_random<var_node_t>ret_stack;
    /*
     function arguments
     */
    stack_random<int>arg_size_stack;//no use now. because only one argument must be put in r_arg_1
    
public:
    void free_var(string var_name){
        if(!var_map.count(var_name)){
            cout<<"free的变量不存在"<<endl;
            cout<<var_name<<endl;
            exit(-1);
        }
        
        var_map.erase(var_map.find(var_name));
    }

    string get_debug_str(){
        return debug_str;
    }
    /**
     * 调用函数时，将函数返回值声明的内容进行分析，区分返回值存放地点并记录。以便为%__FUNC_RET__赋值。现阶段只支持int类型，故返回值均存放在r_ret_1寄存器中，也无需操作ret_stack,因此本函数为支持多类型返回值而预留。
     */
    void push_ret(){
        /**
         *  因为只支持int返回值，故临时这么处理用作初始化，仅在类初始化时调用一次
         */
        var_node_t vnode_ret;
        vnode_ret.set_reg(r_ret_1);
        vnode_ret.set_name("%__FUNC_RET__");
        push_var(vnode_ret);
    }
    /**
     *  同 push_ret()
     */
    void pop_ret(){
        
    }
    
    /**
     *  stack_offset_pos:局部变量保存在栈底，此变量记录栈有效位置距离栈帧的偏移量。
     */
    void set_stack_offset_pos(int pos){
        stack_offset_pos = pos;
    }
    void pop_stack_offset_pos(int pop_offset){
        stack_offset_pos-=pop_offset;
    }
    void push_stack_offset_pos(int push_offset){
        stack_offset_pos+=push_offset;
    }
    /**
     *  stack_offset_pos:局部变量保存在栈底，此变量记录栈有效位置距离栈帧的偏移量。
     */
    int get_stack_offset_pos(){
        return stack_offset_pos;
    }
    
    bool can_alloc_in_reg_pool(){
        return reg_pool.size();
    }
    /**
     * 从reg_pool中取一个reg
     */
    reg_t alloc_reg(){
        reg_t ret;
        if(reg_pool.size()){
            ret=reg_pool.top();
            reg_pool.pop();
        }
        else{
            ret=r_undefined;
        }
        cout<<"alloc:alloc_reg reg="<<ret<<" size="<<reg_pool.size()<<endl;
        return ret;
    }
    /**
     * 释放一个寄存器
     */
    void free_reg(reg_t reg){
        for(int i=0;i<reg_pool.size();i++){
            if(reg_pool.get_by_idx(i)==reg){
                cout<<"error:free_reg:寄存器重复释放"<<endl;
                cout<<get_reg_name(reg)<<endl;
                exit(-1);
            }
        }
        bool flag=0;
        for(int i=0;i<sizeof(useable)/sizeof(reg_t);i++){
            if(useable[i]==reg){
                flag=1;
            }
        }
        if(!flag){
            cout<<"error:free_reg:非法的寄存器释放"<<endl;
            exit(-1);
        }
        reg_pool.push(reg);
        cout<<"释放reg="<<reg<<endl;
    }
    void free_tmp_var(var_node_t var){
        if(!Tmp_var_ctrl::get_shared_impl()->is_tmp_var(var.get_name())){
            exit(-1);
        }
        var_node_t vnode = get_var_by_name(var.get_name());
        if(vnode.is_in_reg()){
            back_var_ctrl::get_share_impl()->free_reg(vnode.get_reg());
        }
        else{
            ins_pop_for_size(vnode.get_sizeof());
            pop_stack_offset_pos(vnode.get_sizeof());
        }
        free_var(var.get_name());
    
    }
    /**
     * 若进入函数时reg_pool不为满，说明之前有未释放的reg资源，不平衡程序错误。
     */
    void check_reg_pool_init_cond(){
        if(reg_pool.size()!=get_reg_num_useable()){
            cout<<"error:check_reg_pool_init_cond failed"<<endl;
            cout<<reg_pool.size()<<"!="<<get_reg_num_useable()<<endl;
            exit(-1);
        }
        return;
    }
    /**
     *  获取运算可用的通用寄存器个数。
     */
    int get_reg_num_useable(){
        int reg_num = sizeof(useable)/sizeof(reg_t);
        return reg_num;
    }
    int get_start_mem_pos(){
        return start_mem_pos;
    }
    static back_var_ctrl* get_share_impl(){
        if(back_var_ctrl_impl)
            return back_var_ctrl_impl;
        return back_var_ctrl_impl=new back_var_ctrl();
    }
    void push_var(var_node_t node){
        if(var_map.count(node.get_name())){
            cout<<"back_var_ctrl.set_pos_of_var:变量重复"<<node.get_name()<<endl;
            exit(-1);
        }
        if(node.is_stack_var()){
            debug_str+=""+node.get_name()+" s"+to_string(node.get_pos())+"\n";
        }
        else if(node.is_global()){
            debug_str+=""+node.get_name()+" "+to_string(node.get_pos())+"\n";
        }
        else if(node.is_in_reg()){
            debug_str+=""+node.get_name()+" r"+to_string(node.get_reg())+"\n";

        }
        var_map[node.get_name()]=var_stack.size();
        var_stack.push(node);//=node;
    }
    int count_var_by_name(string var){
        return (int)var_map.count(var);
    }
    var_node_t& get_var_by_name(string var){
        if(!var_map.count(var)){
            cout<<"查询的变量不存在"<<endl;
            cout<<var<<endl;
            exit(-1);
        }
        return var_stack.get_by_idx(var_map[var]);
    }
    bool is_sign(string line){
        if(line.length()>=2 && line[line.length()-1]==':')
            return true;
        return false;
        
    }

};
#endif /* back_var_ctrl_hpp */
