//
//  background.cpp
//  test
//
//  Created by dawn on 2020/1/9.
//  Copyright © 2020 chuyi. All rights reserved.
//
#include "background.h"
#include "../alloc.h"
#include <sstream>
#include "instruction.h"
#include "back_var_ctrl.h"
#include "macro_inst.h"
string mcode;
string new_code;
string get_debug_str(){
    return back_var_ctrl::get_share_impl()-> get_debug_str();
}
int pos=0;
string get_next(int &pos){
    if(mcode.length() && mcode[mcode.length()-1]!='\n'){
        mcode=mcode+"\n";
    }
    if(pos>=mcode.length()){
        cout<<"pos越界"<<endl;
        exit(-1);
    }
    string ret;
    while (1) {
        ret="";
        int i;
        for(i=pos;i<mcode.length();i++){
            if(mcode[i]=='\n'){
                ret = mcode.substr(pos,i-pos);
                break;
            }
        }
        pos=i+1;
        if(ret.length()){
            break;
        }
    }
    for(int i=(int)ret.length()-1;i>=0;i--){
        if(ret[i]!=' ' && ret[i]!='\t'){
            ret=ret.substr(0,i+1);
            break;
        }
    }
    return ret;
}
bool is_order(string line){
    if(line.length()>=2 && line[0]==':' && line[1]==':'){
        return true;
    }
    return false;
}
bool is_note(string line){
    if(line.length()>=2 && line[0]=='/' && line[1]=='/')
        return true;
    return false;
}


//orders
typedef void (*order_func_ptr)(string);
map<string,order_func_ptr>order_map;
reg_t reg_lst_push[]={r_ra,r_ax,r_bx,r_cx,r_dx,r_di,r_si,r_ex,r_fx,r_fp,r_tmp_1,r_tmp_2};

void push_registers(string line){
    /*
     r_ax=8,     //通用寄存器
     r_bx=9,
     r_cx=10,
     r_dx=11,
     r_di=12,
     r_si=13,
     r_ex=14,
     r_bp=15,
     r_ax 到 r_fx、r_fp为函数调用时保存的上下文;
     函数调用前需要将参数压栈(若需)，将返回值空间压栈(若需)，然后将r_ra压栈，记录新的断点地址r_ra，然后进入函数;
     进入函数后，函数将上下文压栈，将fp指向sp，然后将局部变量压栈;
     函数return时，将返回值放入返回值空间，将局部变量退栈，将上下文退栈还原，返回断点;
     返回断点后，将r_ra退栈还原，保存返回值，将返回值空间退栈(若需),将参数空间退栈(若需);
     注意：r_tmp寄存器为临时寄存器，函数调用不应当穿过r_tmp_1和r_tmp_2，但是为了安全起见还是保存一下
     */
//    ins_push_reg(r_ax);
//    ins_push_reg(r_bx);
//    ins_push_reg(r_cx);
//    ins_push_reg(r_dx);
//
//    ins_push_reg(r_di);
//    ins_push_reg(r_si);
//    ins_push_reg(r_ex);
//    ins_push_reg(r_fx);
//
//    ins_push_reg(r_fp);
//
//    ins_push_reg(r_tmp_1);
//    ins_push_reg(r_tmp_2);
    int size=sizeof(reg_lst_push)/sizeof(reg_t);
    for (int i=0; i<size ; i++) {
        ins_push_reg(reg_lst_push[i]);
    }
}
void push_stack_var(string line){
    stringstream in(line);
    int arg;
    string tmp;
    in>>tmp;
    if(!(in>>arg)){
        cout<<"push_stack_var:参数过少"<<endl;
        exit(-1);
    }
    ins_move_reg_to_reg(r_fp, r_sp);
    ins_push_for_size(arg);
    /*
     *  下面两句在局部变量声明时也会设置。故无效。
     */
    back_var_ctrl &var_ctrl = *back_var_ctrl::get_share_impl();
    var_ctrl.push_stack_offset_pos(arg);
    if(in>>arg){
        cout<<"push_stack_var:参数过多"<<endl;
        exit(-1);
    }
}
void var_stack_alloc_begin(string line){
    int stack_pos=0;
    back_var_ctrl &var_ctrl = * back_var_ctrl::get_share_impl();
    while (pos<mcode.length()) {
        line=get_next(pos);
        if(!line.length()){
            break;
        }
        if(line=="::var_stack_alloc_end"){
            var_ctrl.set_stack_offset_pos(stack_pos);
            return;
        }
        stringstream in(line);
        string name;
        int size;
        if(in>>name>>size){
          //  size/=4;
            var_node_t vnode(name,true);
            vnode.set_pos(stack_pos);
            vnode.set_sizeof(size);
            stack_pos+=size;
            var_ctrl.push_var(vnode);
        }
    }
    cout<<"error:var_stack_alloc_begin:到达尾部，没有找到end"<<endl;
    exit(-1);
}
void push_global_func(string line){
    stringstream in(line);
    string next;
    in>>next; //order;
    back_var_ctrl &var_ctrl = * back_var_ctrl::get_share_impl();
    while(in>>next){
        var_node_t vnode(next,false);
        vnode.set_sizeof(0);
        vnode.set_func();
        var_ctrl.push_var(vnode);
    }
}
void push_global_var_begin(string line){
    string next;
    back_var_ctrl &var_ctrl = * back_var_ctrl::get_share_impl();
    int size;
    int g_pos=var_ctrl.get_start_mem_pos();
    while(pos<mcode.length()){
        line=get_next(pos);
        stringstream in(line);
        if(line=="::push_global_var_end"){
            return;
        }
        if(in>>next>>size){
            var_node_t vnode(next,false);
            if(size&0x3){
                cout<<"push_global_var_begin:size&0x3 is true"<<endl;
                cout<<size<<endl;
                exit(-1);
            }
           // size/=4;
            vnode.set_sizeof(size);
            vnode.set_pos(g_pos);
            vnode.set_in_mem_global();
            g_pos+=size;
            var_ctrl.push_var(vnode);
        }
        else{
            cout<<"push_global_var_begin:条目格式不正确"<<endl<<line<<endl;
            exit(-1);
        }
    }
    cout<<"push_global_var_begin:没有找到end"<<endl;
}
void push_stack_tmp_unfree(string line){
    back_var_ctrl &var_ctrl = * back_var_ctrl::get_share_impl();
    var_ctrl.check_reg_pool_init_cond();
}
void non_action(string line){
    
}
void free_tmp_var(string line){
    back_var_ctrl &var_ctrl = * back_var_ctrl::get_share_impl();
    stringstream in(line);
    string tmp_var;
    in>>tmp_var;
    while (in>>tmp_var) {
        cout<<"alloc:free "<<tmp_var<<endl;
        var_ctrl.free_tmp_var(tmp_var);
    }
}
void pop_stack_var(string line){
    stringstream in(line);
    int arg;
    string tmp;
    in>>tmp;
    if(!(in>>arg)){
        cout<<"push_stack_var:参数过少"<<endl;
        exit(-1);
    }
    ins_move_reg_to_reg(r_fp, r_sp);
    ins_pop_for_size(arg);
    if(in>>arg){
        cout<<"push_stack_var:参数过多"<<endl;
        exit(-1);
    }
}
void pop_registers(string line){
    int size=sizeof(reg_lst_push)/sizeof(reg_t);
    for (int i=size-1; i>=0 ; i--) {
        ins_pop_reg(reg_lst_push[i]);
    }
}
void tmp_var_stack_alloc_begin(string line){
    int stack_pos=0;
    back_var_ctrl &var_ctrl = * back_var_ctrl::get_share_impl();
    while (pos<mcode.length()) {
        line=get_next(pos);
        if(!line.length()){
            break;
        }
        if(line=="::tmp_var_stack_alloc_end"){
            var_ctrl.push_stack_offset_pos(stack_pos);
            return;
        }
        
        stringstream in(line);
        string name;
        int size;
        
        if(in>>name>>size){
//            size/=4;
//            var_node_t vnode(name,true);
//            vnode.set_pos(stack_pos);
//            vnode.set_sizeof(size);
//            stack_pos+=size;
//            var_ctrl.push_var(vnode);
        }
    }
    cout<<"error:tmp_var_stack_alloc_begin:到达尾部，没有找到end"<<endl;
    exit(-1);
}
void return_func(string line){
    ins_move_reg_to_pc(r_ra);
}
void set_func_arg(string line){
    stringstream in(line);
    string tmp,arg;
    in>>tmp;
    if(in>>arg){
        move_var_to_reg(arg, r_arg_1);//only for one int argument.
        if(in>>tmp){
            cout<<"error:too much arg for order ::set_func_arg"<<endl;
            exit(-1);
        }
    }
    else{
        cout<<"error:set_func_arg:arg str is needed"<<endl;
        exit(-1);
    }
}
void get_func_arg(string line){
    stringstream in(line);
    string tmp,arg;
    in>>tmp;
    if(in>>arg){
        move_reg_to_ans(r_arg_1, arg);//only for one int argument.
        if(in>>tmp){
            cout<<"error:too much arg for order ::get_func_arg"<<endl;
            exit(-1);
        }
    }
    else{
        cout<<"error:get_func_arg:arg str is needed"<<endl;
        exit(-1);
    }
}
//regist orders
void regist_order_func(){
    order_map["::push_registers"]=push_registers;
    order_map["::push_stack_var"]=push_stack_var;
    order_map["::var_stack_alloc_begin"]=var_stack_alloc_begin;
    order_map["::push_global_func"]=push_global_func;
    order_map["::push_global_var_begin"]=push_global_var_begin;
    order_map["::push_stack_tmp"]=non_action;
    order_map["::push_stack_tmp_unfree"]=push_stack_tmp_unfree;
    order_map["::free_tmp_var"]=free_tmp_var;
    order_map["::pop_stack_tmp_unfree"]=non_action;
    order_map["::pop_stack_tmp"]=non_action;
    order_map["::pop_stack_var"]=pop_stack_var;
    order_map["::pop_registers"]=pop_registers;
    order_map["::tmp_var_stack_alloc_begin"]=tmp_var_stack_alloc_begin;
    order_map["::return"]=return_func;
    order_map["::set_func_arg"]=set_func_arg;
    order_map["::get_func_arg"]=get_func_arg;
}

/*
 * 函数内使用if-else-if枚举操作符，实际上若操作符比较少这样是合理的，若操作符较多，可使用map+函数指针优化比较次数。
 */
void trans(string line){
    back_var_ctrl &var_ctrl = * back_var_ctrl::get_share_impl();

    stringstream in(line);
    string fst,snd;
    in>>fst;
    if(fst=="goto"){
        string address,tmp;
        if(in>>address){
            ins_jmp("sign_"+address);
            if(in>>tmp){
                cout<<"error:trans:goto:in>>tmp:多余的参数"<<endl;
                cout<<line<<endl;
                exit(-1);
            }
        }
        else{
            cout<<"error:trans:goto:in>>address failed"<<endl;
            cout<<line<<endl;
            exit(-1);
        }
    }
    else if(fst=="if"){
        string cond,goto_str,then_sign,tmp;
        if(in>>cond){
            if(in>>tmp){
                cout<<"if error"<<endl;
                exit(0);
            }
            //将if的条件取出放到reg中,同时判立即数
            reg_t cond_reg=r_tmp_1;
            int imm=0;//0为非立即数，1为立即跳转，2为忽略
            {
                var_node_t vnode;
                if(var_ctrl.count_var_by_name(cond)){//若是记录在册的变量
                    vnode=var_ctrl.get_var_by_name(cond);
                    //首先把值转移到寄存器cond_reg中
                    if(vnode.is_func()){
                        ins_move_imm_to_reg(cond_reg, vnode.get_pos());
                    }
                    else if(vnode.is_in_reg()){
                        cond_reg=vnode.get_reg();
                    }
                    else{
                        move_val_to_reg(vnode, cond_reg);
                    }
                    
                    
                    
                }
                else{
                    //若是纯十进制，则直接跳转或者忽略，若不为纯十进制，则报错
                    bool flag=false;
                    for (int i=0; i<cond.length(); i++) {
                        if(cond[i]>='0' && cond[i]<=9){
                            flag=true;
                            break;
                        }
                    }
                    if (flag) {
                        cout<<"error:trans:if:错误的条件格式"<<endl;
                        exit(0);
                    }
                    if(cond=="0"){
                        //条件为假，直接忽略。
                        imm=2;
                    }
                    else{
                        //条件为真，直接跳转
                        imm=1;
                        
                    }
                    
                }
            }
            
            
            
            line=get_next(pos);
            stringstream in2(line);
            string line2_fst;
            in2>>line2_fst;
            new_code+="//"+line+"\n";
            if(line2_fst=="::free_tmp_var"){
                free_tmp_var(line);
                line=get_next(pos);
                new_code+="//"+line+"\n";
                stringstream in3(line);
                in3>>goto_str;
                if(!(in3>>then_sign)){
                    cout<<"error:"<<"if-goto:cannot find then_sign"<<endl;
                    exit(0);
                }
            }
            else if(line2_fst!="goto"){
                cout<<"error:line2_fst is neither free nor goto"<<endl;
                exit(0);
            }
            else{
                goto_str="goto";
                if(!(in2>>then_sign)){
                    cout<<"error:"<<"if-goto:cannot find then_sign"<<endl;
                    exit(0);
                }
            }
            
            
            if(goto_str!="goto"){
                cout<<"error:trans:if:in>>tmp:goto is expected"<<endl;
                cout<<line<<endl;
                exit(-1);
            }
            //跳转
            if(imm==1){
                ins_jmp(string("sign_")+then_sign);
            }
            else if(imm==2){
                //忽略
            }
            else{
                //根据cond_reg跳转
                ins_if(cond_reg, string("sign_")+then_sign);
            }
            
            
            if (in>>tmp) {
                cout<<"error:trans:if:in>>tmp:多余的参数"<<endl;
                cout<<line<<endl;
                exit(0);
            }
        }
    }
    else if(fst=="call"){
        string address,tmp;
        if(in>>address){
            ins_call_imm("sign_"+address);
            
            if(in>>tmp){
                cout<<"error:trans:call:in>>tmp:多余的参数"<<endl;
                cout<<line<<endl;
                exit(0);
            }
        }
        else{
            cout<<"error:trans:call:in>>address failed"<<endl;
            cout<<line<<endl;
            exit(-1);
        }
        
    }
    else if(var_ctrl.is_sign(fst)){
        if(in>>snd){
            cout<<"error:标号格式不正确"<<endl;
            cout<<line<<endl;
            exit(0);
        }
        new_code+="sign:\t"+line.substr(0,line.length()-1);
    }
    else{
        if(in>>snd){
            if(snd=="="){
                string a,b,op;
                var_node_t vnode_a,vnode_b;
                /*
                    string ans，a，b。
                    vnode_ ans(无) a b.
                 
                 */
                string ans=fst;
                string co[3];
                int i=0;
                while(in>>co[i]){
                    i++;
                }
                if(i==1){
                    op="=";
                    a=co[0];
                    if(a[0]>='0'&&a[0]<='9'){
                        vnode_a.set_imm(atoi(a.c_str()));
                    }
                    else
                        vnode_a=var_ctrl.get_var_by_name(a);
                }
                else if(i==2){
                    op=co[0];
                    a=co[1];
                    if(a[0]>='0'&&a[0]<='9'){
                        vnode_a.set_imm(atoi(a.c_str()));
                    }
                    else
                        vnode_a=var_ctrl.get_var_by_name(a);
                }
                else if(i==3){
                    a=co[0];
                    op=co[1];
                    b=co[2];
                    if(a[0]>='0'&&a[0]<='9'){
                        vnode_a.set_imm(atoi(a.c_str()));
                    }
                    else
                        vnode_a=var_ctrl.get_var_by_name(a);
                    if(b[0]>='0'&&b[0]<='9'){
                        vnode_b.set_imm(atoi(b.c_str()));
                    }
                    else
                        vnode_b=var_ctrl.get_var_by_name(b);
                }else{
                    cout<<"error:trans:四元式解析错误：a，b，op个数不正确："<<i<<endl;
                    exit(0);
                }
                Tmp_var_ctrl &tmp_var_ctrl=*Tmp_var_ctrl::get_shared_impl();
                /*
                 *  switch(op)
                 *  值有三种可能(判断先后顺序)：imm,reg，stack，global
                 */
                if(op=="+"){
                    reg_t rega=r_tmp_1,regb=r_tmp_2;
                    if(vnode_a.is_in_reg()){
                        rega=vnode_a.get_reg();
                    }
                    else{
                        move_val_to_reg(vnode_a, rega);
                    }
                    if (vnode_b.is_in_reg()) {
                        regb=vnode_b.get_reg();
                    }
                    else{
                        move_val_to_reg(vnode_b, regb);
                    }
                    ins_add(r_tmp_1, rega, regb);
                    move_reg_to_ans(r_tmp_1, ans);
                    return;
                }
                if(op=="-"){
                    reg_t rega=r_tmp_1,regb=r_tmp_2;
                    if(vnode_a.is_in_reg()){
                        rega=vnode_a.get_reg();
                    }
                    else{
                        move_val_to_reg(vnode_a, rega);
                    }
                    if (vnode_b.is_in_reg()) {
                        regb=vnode_b.get_reg();
                    }
                    else{
                        move_val_to_reg(vnode_b, regb);
                    }
                    ins_sub(r_tmp_1, rega, regb);
                    move_reg_to_ans(r_tmp_1, ans);
                    return;
                }
                if(op=="*"){
                    reg_t rega=r_tmp_1,regb=r_tmp_2;
                    if(vnode_a.is_in_reg()){
                        rega=vnode_a.get_reg();
                    }
                    else{
                        move_val_to_reg(vnode_a, rega);
                    }
                    if (vnode_b.is_in_reg()) {
                        regb=vnode_b.get_reg();
                    }
                    else{
                        move_val_to_reg(vnode_b, regb);
                    }
                    ins_mult(r_tmp_1, rega, regb);
                    move_reg_to_ans(r_tmp_1, ans);
                    return;
                }
                else if(op=="="){
                    var_node_t vnode_ans;
                    if(var_ctrl.count_var_by_name(ans)){
                        vnode_ans = var_ctrl.get_var_by_name(ans);
                        if(vnode_ans.is_left_able()){   //若ans存在，且ans有特殊的左值设定，则取用此设定并将其看做指针，改为向ans指向的内容赋值
                            vnode_ans.get_left_addr(r_tmp_1);
                            //将a赋给r_tmp_1指向的mem
                            move_val_to_ptr_mem(vnode_a, r_tmp_1);//会污染r_tmp_2
                            
                           
//                            return;
                        }
                    }
                    //若特殊的左值指针无效，则进行普通的赋值。
                    //若特殊的左值指针有效，则除了上文进行的对左值变量进行赋值以外，还要对本身进行普通的赋值。以同步临时变量和左值变量的val。
//                    此处r_tmp_1允许被污染
                    move_val_to_ans(vnode_a, ans);//可能会污染r_tmp_1;
                    return;
                }
                else if(op=="<"){
                    reg_t rega=r_tmp_1,regb=r_tmp_2;
                    if(vnode_a.is_in_reg()){
                        rega=vnode_a.get_reg();
                    }
                    else{
                        move_val_to_reg(vnode_a, rega);
                    }
                    if (vnode_b.is_in_reg()) {
                        regb=vnode_b.get_reg();
                    }
                    else{
                        move_val_to_reg(vnode_b, regb);
                    }
                    ins_is_less_than(r_tmp_1, rega, regb);
                    move_reg_to_ans(r_tmp_1, ans);
                    return;
                }
                else if(op=="=="){
                    reg_t rega=r_tmp_1,regb=r_tmp_2;
                    if(vnode_a.is_in_reg()){
                        rega=vnode_a.get_reg();
                    }
                    else{
                        move_val_to_reg(vnode_a, rega);
                    }
                    if (vnode_b.is_in_reg()) {
                        regb=vnode_b.get_reg();
                    }
                    else{
                        move_val_to_reg(vnode_b, regb);
                    }
                    ins_sub(r_tmp_1, rega, regb);
                    ins_get_bool_not(r_tmp_1, r_tmp_1);
                    move_reg_to_ans(r_tmp_1, ans);
                    return;
                }
                else if(op=="&&"){
                    reg_t rega=r_tmp_1,regb=r_tmp_2;

                    if(vnode_a.is_in_reg()){
                        rega=vnode_a.get_reg();
                    }
                    else{
                        move_val_to_reg(vnode_a, rega);
                    }
                    if (vnode_b.is_in_reg()) {
                        regb=vnode_b.get_reg();
                    }
                    else{
                        move_val_to_reg(vnode_b, regb);
                    }
                    /**
                     x || y = bool(x) | bool(y)
                     a && b = !(!a || !b) //又因为!a = bool_not(a)
                     = !(!a  | !b)
                     */
                    //首先，为了不污染原来的寄存器。要将寄存器值迁移出来。好在值不会向临时寄存器里持久存放。
                    if(rega!=r_tmp_1){
                        ins_move_reg_to_reg(rega, r_tmp_1);
                        rega=r_tmp_1;
                    }
                    if(regb!=r_tmp_2){
                        ins_move_reg_to_reg(regb, r_tmp_2);
                        regb=r_tmp_2;
                    }
                    //取逻辑not
                    ins_get_bool_not(rega, rega);
                    ins_get_bool_not(regb, regb);
                    //剩余计算
                    ins_or(rega, rega, regb);
                    ins_logic_get_not(rega, rega);
                    
                    move_reg_to_ans(rega, ans);
                    return;
                }
                else if(op=="||"){
                    reg_t rega=r_tmp_1,regb=r_tmp_2;
                    
                    if(vnode_a.is_in_reg()){
                        rega=vnode_a.get_reg();
                    }
                    else{
                        move_val_to_reg(vnode_a, rega);
                    }
                    if (vnode_b.is_in_reg()) {
                        regb=vnode_b.get_reg();
                    }
                    else{
                        move_val_to_reg(vnode_b, regb);
                    }
                    /**
                     x || y = bool(x) | bool(y)
                            != bool(x+y)
                     */
                    //首先，为了不污染原来的寄存器。要将寄存器值迁移出来。好在值不会向临时寄存器里持久存放。
                    if(rega!=r_tmp_1){
                        ins_move_reg_to_reg(r_tmp_1, rega);
                        rega=r_tmp_1;
                    }
                    if(regb!=r_tmp_2){
                        ins_move_reg_to_reg(r_tmp_2, regb);
                        regb=r_tmp_2;
                    }
                    //取逻辑
                    ins_get_bool(rega, rega);
                    ins_get_bool(regb, regb);
                    //剩余计算
                    ins_or(rega, rega, regb);
                    
                    move_reg_to_ans(rega, ans);
                    return;
                }
                else if(op=="%__ADDRESS_OF__"){
                    if(var_ctrl.count_var_by_name(a)){
                        if(vnode_a.is_left_able()){
                            vnode_a.get_left_addr(r_tmp_1);
                            move_reg_to_ans(r_tmp_1, ans);
                            return;
                        }
                        if(vnode_a.is_stack_var()){
                            //将倒叙的栈地址转为正序的全局地址，注意，从0开始减，故sizeof要减去unit
                            ins_addi(r_tmp_1, r_fp, -(vnode_a.get_pos()+vnode_a.get_sizeof()-unit));
                            move_reg_to_ans(r_tmp_1, ans);
                        }
                        else if(vnode_a.is_global()){
                            ins_move_imm_to_reg(r_tmp_1, vnode_a.get_pos());
                            move_reg_to_ans(r_tmp_1, ans);
                        }
                        else{
                            //reg , imm均error
                            cout<<"error:trans:四元式:%__ADDRESS_OF__，对象非栈变量or全局变量"<<endl;
                            cout<<line<<endl;
                            exit(0);
                        }
                        return;
                    }
                    else{
                        cout<<"error:trans:四元式:%__ADDRESS_OF__，取地址对象不存在:"<<a<<endl;
                        cout<<line<<endl;
                        exit(0);
                    }
                }
                else if(op=="%__MEMORY_OF__"){
                    /* a            op          b
                     * var_addr %__MEMORY_OF__ size
                     */
                    
                    /*: get right value for ans*/
                    int size=vnode_b.get_imm();//
                    if(size&0x3){
                        cout<<"trans:解引用:size内存对齐错误:size&0x3 == true"<<endl;
                        cout<<size<<endl;
                        exit(0);
                    }
//                    size/=4;
                    if(var_ctrl.count_var_by_name(a)){
                        reg_t reg_addr=r_tmp_1;
                        reg_t reg_mid=r_tmp_2;
                        if(vnode_a.is_stack_var()){
                            ins_move_mem_to_reg(r_fp, -vnode_a.get_pos(), r_tmp_1);//把地址从内存中取出，存放在r_tmp_1中。
                        }
                        else if(vnode_a.is_global()){
                            ins_move_mem_to_reg(r_zero, vnode_a.get_pos(), r_tmp_1);
                        }
                        else if(vnode_a.is_in_reg()){
                            reg_addr=vnode_a.get_reg();
                        }
                        else{
                            //imm均error
                            cout<<"error:trans:四元式:%__MEMORY_OF__，地址对象不能为立即数"<<endl;
                            cout<<line<<endl;
                            exit(0);
                        }
                        if(reg_addr==r_tmp_2){
                            reg_mid=r_tmp_1; //reg_addr和reg_mid不能相同
                        }
                        release_ptr_to_ans(reg_addr, size, ans);
                    }
                    else{
                        cout<<"error:trans:四元式:%__MEMORY_OF__，地址对象不存在:"<<a<<endl;
                        cout<<line<<endl;
                        exit(0);
                    }
                    /* regist left value for ans
                     { 地址变量(运行时确定，临时变量或者正常变量，暂不支持真值) ，sizeof }
                     now,ans exist;
                     */
                    var_node_t &vnode_ans = var_ctrl.get_var_by_name(ans);
                    if(vnode_a.is_imm()){
                        vnode_ans.set_left_value(vnode_a.get_imm(), vnode_b.get_imm());
                    }
                    else{
                        vnode_ans.set_left_value(a, vnode_b.get_imm());
                    }
                    return;
                }
                else{
                    cout<<"error:trans:四元式：操作符op不存在: "<<op<<endl;
                    exit(0);
                }
            }
            /**
             *  fst<=val  fst为指针，其内存值(或寄存器值)为地址。需要将fst内存值p取出，然后执行[p]=val
             *  暂时没用
             */
            else if(snd=="<="){
                int i=0;
                string s;
                var_node_t vnode_a;
                while(in>>s){
                    i++;
                }
                if(i>1){
                    cout<<"error:<= operation arg num error:"<<i<<endl;
                    exit(0);
                }
                
                if(s[0]>='0'&&s[0]<='9'){
                    vnode_a.set_imm(atoi(s.c_str()));
                }
                else
                    vnode_a=var_ctrl.get_var_by_name(s);
                //现将fst变量取出，准备取真值
                var_node_t vnode_ans = var_ctrl.get_var_by_name(fst);
                //取fst变量的值。
                move_val_to_reg(vnode_ans, r_tmp_1);
                /**
                 * [r_tmp_1]=vnode_a; r_tmp_1为真地址，不需要计算偏移。故取地址时也要保存真值
                 */
                move_val_to_reg(vnode_a, r_tmp_2);
                ins_move_reg_to_mem(r_tmp_2, r_tmp_1, 0);
                
                return;
            }
        }
        
        cout<<"trans error:fst not found"<<endl;
        cout<<fst<<endl;
        cout<<line<<endl;
        exit(0);
    }
}
string background(string code){
    // cout<<code<<endl;
    pos=0;
    mcode=code;
    regist_order_func();
    string out_code="//start\n";
    while (pos<mcode.length()) {
        string line;
        line=get_next(pos);
        new_code="";
        if(!line.length()){
            break;
        }
        cout<<"<line>"<<line<<endl;
        new_code="//"+line;
        if(new_code[new_code.length()-1]!='\n'){
            new_code+="\n";
        }
        if(is_note(line)){
            new_code=line;
        }
        else if(is_order(line)){
            stringstream ins(line);
            string order;
            ins>>order;
            if(!order_map.count(order)){
                cout<<"找不到该命令"<<endl;
                cout<<line<<endl;
                exit(0);
            }
            order_map[order](line);
        }
        else{   //语句
            
            trans(line);
        }
        
        cout<<"<line>new_code="<<new_code<<endl;
        out_code+=new_code;
        if(out_code.length() && out_code[out_code.length()-1]!='\n'){
            out_code+="\n";
        }
    }
    return out_code;
}

