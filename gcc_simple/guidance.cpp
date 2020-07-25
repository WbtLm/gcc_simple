//
//  guidance.cpp
//  test
//
//  Created by dawn on 2019/12/6.
//  Copyright © 2019 chuyi. All rights reserved.
//

#include "guidance.h"
#include "symble.h"
#include "alloc.h"
#include "struct_type_dic.h"
#include "data_type_func.h"
//=======
string __func_arg; // func can have only one arg. __func_arg tells code blocks the name of the only arg
//=======
Guider* Guider::share_impl=NULL;
string middle_code;
string get_middle_code()
{
    return middle_code;
}
int slr1_ctrl(matrix_t matrix,stack<tree_node_t> word_stack){
    middle_code="";
    stack<int>cond_stack;
    stack<int>sign_stack;
    stack<tree_node_t>sign_info_stack;
    cond_stack.push(1);
    int pointed_sign;
    int cond;
    tree_node_t word;
    while(word_stack.empty()==0){
        cond=cond_stack.top();
        word=word_stack.top();
        pointed_sign=echo_str2num(word.name);
        if(pointed_sign<0){
            cout<<"语法分析自动机：符号错误:"<<word.name<<" "<<pointed_sign<<endl;
            return -1;
        }
        cout<<"========   ======="<<endl;
        cout<<"现在："<<pointed_sign<<" "<<echo_num2str(pointed_sign)<<" cond="<<cond<<endl;
        if(matrix[cond].count(pointed_sign)){
            map_cell_t cell = matrix[cond][pointed_sign];
            if(cell.action==ACTION_S){ //移进
                cout<<"移进"<<endl;
                sign_stack.push(pointed_sign);
                cond_stack.push(cell.go_to);
                cout<<"状态入栈：cond="<<cell.go_to-1<<endl;
                //存储信息
                sign_info_stack.push(word_stack.top());
                word_stack.pop();
            }
            else if(cell.action==ACTION_R || cell.action==ACTION_ACC){ //归约
                cout<<"归约，产生式："<<endl;
                production_t prod=get_prod_by_idx(cell.go_to);
                cout<<"prod.sign=";
                prod.show();
                guid_func_args_t args(prod.val);
                for(int i=0;i<(int)prod.detail.size();i++){
                    cout<<echo_num2str(sign_stack.top());
                    cout<<"出栈"<<endl;
                    if(cond_stack.empty()||sign_stack.empty()){
                        cout<<"stack empty error"<<endl;
                        return -1;
                    }
                    args.push_back(sign_info_stack.top());
                    cond_stack.pop();
                    sign_stack.pop();
                    sign_info_stack.pop();
                }
//                if(args.size() &&
//                   args.get_arg(0).value.str_value.length()){
//                    cout<<args.get_arg(0).value.str_value<<endl;
//                }
                tree_node_t new_left= Guider::get_shared_impl()->call_guid_func(args);
                cout<<echo_num2str(prod.left)<<"入word_stack:"<<endl;
                cout<<new_left.name<<endl;
                if(echo_num2str(prod.left) != new_left.name){
                    cout<<"error:guid:"<<__LINE__<<endl;
                    exit(-1);
                }
                word_stack.push(new_left);
              //  new_left.show();
            
              //  cout<<"算数："<<new_left.int_val<<endl;
                if(cell.action==ACTION_ACC){
                    middle_code=new_left.code;
                    return 1;
                }
            }
            
            else{
                cout<<"error:"<<endl;
                cout<<"cond="<<cond<<endl;
                cout<<"sign="<<echo_num2str(pointed_sign)<<endl;
                cout<<"cell:"<<endl;
                cell.show();
                return -1;
            }
        }
        else{
            cout<<"error:语法错误，转移矩阵无此cell"<<endl;
            cout<<echo_num2str(pointed_sign)<<endl;
            cout<<"行："<<word.coor.y<<"列："<<word.coor.x<<endl;
            return -1;
        }
    }
    return 0;
}

//S'-><程序>
tree_node_t guid_func_0(guid_func_args_t args){
    production_t prod = get_prod_by_idx(args.get_pord_idx());
    Symble_block_stack_t &symble_controller = *Symble_block_stack_t::get_shared_impl();
    tree_node_t arg0=args.get_arg(0);
    tree_node_t ret_word;
    /*
     enum type_t{
     UNDEFINED,
     VAR,
     ADDRESS_SIGN,
     CODE_BLOCK,
     STATIC_VALUE;
     };
     coor_t coor;
     value_t value;
     string code;
     type_t type=UNDEFINED;
     string name;
     */
    ret_word.coor=arg0.coor;
    ret_word.value = arg0.value;
    ret_word.code=arg0.code;
    ret_word.type=arg0.type;
    ret_word.name=echo_num2str(prod.left);
    ret_word.set_left_able(arg0.get_left_albe());
   // ret_word.code="goto __func_main\n"+ret_word.code;
    
    string new_code="";
    Symble_block_t sblock=symble_controller.top();
    
    new_code=new_code+"::push_global_func";
    for (int i=0; i<sblock.size();i++){
        if(sblock.get_snode_by_idx(i).data_type.is_func_ptr()){
            new_code+=" "+sblock.get_snode_by_idx(i).get_name();
        }
    }
    new_code+="\n";
    new_code=new_code+"::push_global_var_begin\n";
    string init_str="";
    for (int i=0; i<sblock.size();i++){
        if(!sblock.get_snode_by_idx(i).data_type.is_func_ptr() && sblock.get_snode_by_idx(i).type!=DATA_TYPE_SYMBLE){
            
            data_type_t type_tmp = sblock.get_snode_by_idx(i).data_type;
//            cout<<type_tmp.get_type_sign()<<endl;
            int type_size = type_tmp.get_sizeof() + (type_tmp.is_array()?unit:0);
            string var_name=sblock.get_snode_by_idx(i).get_name();
            new_code+=var_name+" "+to_string(type_size)+"\n";
            if(type_tmp.is_array()){
                init_str=init_str+var_name+" = "+"%__ADDRESS_OF__ "+var_name+"\n";
                init_str=init_str+var_name+" = "+var_name+" + "+to_string(unit)+"\n";
            }
        }
    }
    new_code=new_code+"::push_global_var_end\n";
    new_code=new_code+init_str;
    new_code+="goto __var_main\n";
    ret_word.code=new_code+ret_word.code;
    cout<<"语法分析完毕"<<endl;
//    cout<<ret_word.code<<endl;
//    cout<<"完毕"<<endl;
    return ret_word;
}
// <程序>-><外部声明>
tree_node_t guid_func_1(guid_func_args_t args){
    production_t prod = get_prod_by_idx(args.get_pord_idx());
    Symble_block_stack_t &symble_controller = *Symble_block_stack_t::get_shared_impl();
    tree_node_t arg0=args.get_arg(0);
  
    tree_node_t ret_word;
    /*
     enum type_t{
     UNDEFINED,
     VAR,
     ADDRESS_SIGN,
     CODE_BLOCK,
     STATIC_VALUE;
     };
     coor_t coor;
     value_t value;
     string code;
     type_t type=UNDEFINED;
     string name;
     */
    ret_word.coor=arg0.coor;
    ret_word.value = arg0.value;
    ret_word.code=arg0.code;
    ret_word.type=arg0.type;
    ret_word.name=echo_num2str(prod.left);
    ret_word.set_left_able(false);
    return ret_word;
}
//2 <程序>-><外部声明><程序>
tree_node_t guid_func_2(guid_func_args_t args){
    production_t prod = get_prod_by_idx(args.get_pord_idx());
    Symble_block_stack_t &symble_controller = *Symble_block_stack_t::get_shared_impl();
    tree_node_t arg0=args.get_arg(0);
    tree_node_t arg1=args.get_arg(1);
    tree_node_t ret_word;
    /*
     enum type_t{
     UNDEFINED,
     VAR,
     ADDRESS_SIGN,
     CODE_BLOCK,
     STATIC_VALUE;
     };
     coor_t coor;
     value_t value;
     string code;
     type_t type=UNDEFINED;
     string name;
     */
    ret_word.coor=arg0.coor;
    ret_word.value = arg0.value;
    ret_word.code=arg0.code;
    ret_word.type=arg0.type;
    ret_word.name=echo_num2str(prod.left);
    ret_word.set_left_able(false);
    ret_word.code=arg0.code+arg1.code;
    return ret_word;
}

//4 <外部声明>-><函数定义>
tree_node_t guid_func_4(guid_func_args_t args){
    production_t prod = get_prod_by_idx(args.get_pord_idx());
    Symble_block_stack_t &symble_controller = *Symble_block_stack_t::get_shared_impl();
    tree_node_t arg0=args.get_arg(0);
    //    tree_node_t arg1=args.get_arg(1);
    tree_node_t ret_word;
    /*
     enum type_t{
     UNDEFINED,
     VAR,
     ADDRESS_SIGN,
     CODE_BLOCK,
     STATIC_VALUE;
     };
     coor_t coor;
     value_t value;
     string code;
     type_t type=UNDEFINED;
     string name;
     */
    ret_word.coor=arg0.coor;
    ret_word.value=arg0.value;
    ret_word.code=arg0.code;
    ret_word.type=arg0.type;
    ret_word.name=echo_num2str(prod.left);
    ret_word.set_left_able(false);
//    cout<<ret_word.code<<endl;
//    cout<<"括号："<<ret_word.value.int_value<<endl;
    return ret_word;
}
//5 <外部声明>-><变量声明或定义>';'
tree_node_t guid_func_5(guid_func_args_t args){
    production_t prod = get_prod_by_idx(args.get_pord_idx());
    Symble_block_stack_t &symble_controller = *Symble_block_stack_t::get_shared_impl();
    tree_node_t arg0=args.get_arg(1);
    //    tree_node_t arg1=args.get_arg(1);
    tree_node_t ret_word;
    /*
     enum type_t{
     UNDEFINED,
     VAR,
     ADDRESS_SIGN,
     CODE_BLOCK,
     STATIC_VALUE;
     };
     coor_t coor;
     value_t value;
     string code;
     type_t type=UNDEFINED;
     string name;
     */
    ret_word.coor=arg0.coor;
    ret_word.value=arg0.value;
    ret_word.code=arg0.code;
    ret_word.type=arg0.type;
    ret_word.name=echo_num2str(prod.left);
    ret_word.set_left_able(false);
    cout<<"算数：括号："<<ret_word.value.int_value<<endl;
    return ret_word;
}
//6 <struct定义>-><struct声明><程序块首部><变量声明或定义列表><程序块尾部>
tree_node_t guid_func_6(guid_func_args_t args){
    production_t prod = get_prod_by_idx(args.get_pord_idx());
    Symble_block_stack_t &symble_controller = *Symble_block_stack_t::get_shared_impl();
    tree_node_t arg0=args.get_arg(0);// <struct声明> 包含名字信息和struct_type_t
    tree_node_t arg1=args.get_arg(2);// <变量声明或定义列表>
    tree_node_t arg2=args.get_arg(3);// <程序块尾部>包含block_t，包含了变量声明信息
    //    tree_node_t arg1=args.get_arg(1);
    tree_node_t ret_word;
    /*
     enum type_t{
     UNDEFINED,
     VAR,
     ADDRESS_SIGN,
     CODE_BLOCK,
     STATIC_VALUE;
     };
     coor_t coor;
     value_t value;
     string code;
     type_t type=UNDEFINED;
     string name;
     */
    ret_word.coor=arg0.coor;
    ret_word.value=arg0.value;
    ret_word.code=arg0.code;
    ret_word.type=tree_node_t::VAR;
    ret_word.name=echo_num2str(prod.left);
    ret_word.set_left_able(arg0.get_left_albe());
    ret_word.code=arg0.code+arg1.code+arg2.code;
    if(arg0.value.block_value){
        cout<<"guid_func_6: arg0.value.block_value != NULL"<<endl;
        exit(-1);
    }
    if(arg2.value.block_value == NULL){
        cout<<"guid_func_6: arg2.value.block_value == NULL"<<endl;
        exit(-1);
    }
    string struct_name=arg0.value.str_value;
//    struct_type_node_t *node = new struct_type_node_t(struct_name);
//    struct_type_dic::get_shared_impl()->push_struct_type_node(node);
    struct_type_node_t *node =  struct_type_dic::get_shared_impl()->get_struct_type_node_by_name(struct_name);
    string struct_type_sign = symble_controller.get_symble_node_by_name(struct_name).data_type.get_type_sign();
    Symble_block_t *block=arg2.value.block_value;
    stack_random<Symble_block_t::var_size>var_lst = block->get_stack_var();
    for (int i=0; i<var_lst.size(); i++) {
        Symble_node_t snode = block->get_symble_by_name(var_lst.get_by_idx(i).name);
        string name=snode.get_true_name();
        data_type_t data_type = snode.data_type;
        if(data_type.get_type_sign()==struct_type_sign){
            Utils::Exit_normal::pt_error_source_code_lan(string("struct cannot contains itself: struct:")+struct_name+" var:"+name, ret_word.coor);
        }
        node->push_var(name, data_type);
    }
    cout<<struct_name<<endl;
    return ret_word;
}
// <struct声明>-><struct数据类型>
tree_node_t guid_func_6_1(guid_func_args_t args){
    production_t prod = get_prod_by_idx(args.get_pord_idx());
    Symble_block_stack_t &symble_controller =  *Symble_block_stack_t::get_shared_impl();
    tree_node_t arg0=args.get_arg(0);
    //    tree_node_t arg1=args.get_arg(1);
    tree_node_t ret_word;
    /*
     enum type_t{
     UNDEFINED,
     VAR,
     ADDRESS_SIGN,
     CODE_BLOCK,
     STATIC_VALUE;
     };
     coor_t coor;
     value_t value;
     string code;
     type_t type=UNDEFINED;
     string name;
     */
    ret_word.coor=arg0.coor;
    ret_word.value=arg0.value;
    ret_word.code=arg0.code;
    ret_word.type=arg0.type;
    ret_word.name=echo_num2str(prod.left);
    ret_word.set_left_able(false);
    if(symble_controller.count_symble_node_by_name(ret_word.value.str_value)){
        Utils::Exit_normal::pt_error_source_code_lan(string("guid_func_6_1:struct name has already exist."), ret_word.coor);
    }
    if(ret_word.value.symble_node_value!=NULL){
        cout<<"error:in line="<<__LINE__<<endl;
        exit(-1);
    }
    Symble_node_t symble_node(ret_word.value.str_value,DATA_TYPE_SYMBLE,ret_word.coor.y,ret_word.coor.x);
    data_type_t data_type;
    data_type.push(new struct_type_t(ret_word.value.str_value));
    symble_node.data_type=data_type;
    symble_controller.push_symble(symble_node);
    
    struct_type_node_t *node = new struct_type_node_t(arg0.value.str_value);
    struct_type_dic::get_shared_impl()->push_struct_type_node(node);
    return ret_word;

}
//7 <函数定义>-><函数定义头><程序块>
tree_node_t guid_func_7(guid_func_args_t args){
    production_t prod = get_prod_by_idx(args.get_pord_idx());
    Symble_block_stack_t &symble_controller = *Symble_block_stack_t::get_shared_impl();
    tree_node_t arg0=args.get_arg(0);
    tree_node_t arg1=args.get_arg(1);
//    tree_node_t arg1=args.get_arg(1);
    tree_node_t ret_word;
    /*
     enum type_t{
     UNDEFINED,
     VAR,
     ADDRESS_SIGN,
     CODE_BLOCK,
     STATIC_VALUE;
     };
     coor_t coor;
     value_t value;
     string code;
     type_t type=UNDEFINED;
     string name;
     */
    ret_word.coor=arg0.coor;
    ret_word.value=arg1.value;
  //  ret_word.code=arg1.code;
    ret_word.type=tree_node_t::VAR;
    ret_word.name=echo_num2str(prod.left);
    ret_word.set_left_able(false);
    string init_str="";
    stack_random<Symble_block_t::var_size>var_stack = arg1.value.block_value->get_stack_var()
                                    ,tmp_var_stack = arg1.value.block_value->get_stack_tmp_var();
    string var_stack_alloc="::var_stack_alloc_begin\n";
    for(int i=0;i<var_stack.size();i++){
        Symble_block_t::var_size tmp=var_stack.get_by_idx(i);
        if(tmp.is_arr){
            init_str=init_str+tmp.name+" = "+"%__ADDRESS_OF__ "+tmp.name+"\n";

        }
        var_stack_alloc+=tmp.name+" "+to_string(tmp.size+(tmp.is_arr?unit:0))+"\n";
    }
    var_stack_alloc+="::var_stack_alloc_end\n";
    var_stack_alloc+=init_str;
    
    string tmp_var_stack_alloc="::tmp_var_stack_alloc_begin\n";
    for(int i=0;i<tmp_var_stack.size();i++){
        tmp_var_stack_alloc+=tmp_var_stack.get_by_idx(i).name+" "+to_string(tmp_var_stack.get_by_idx(i).size)+"\n";
    }
    tmp_var_stack_alloc+="::tmp_var_stack_alloc_end\n";
    
    ret_word.code="\n"+arg0.value.str_value+":\n"
                +"::push_registers\n"
                +"::push_stack_var "+to_string(arg1.value.block_value->get_stack_var_size())+"\n"
                +var_stack_alloc
                +"::push_stack_tmp "+to_string(arg1.value.block_value->get_stack_tmp_var_size())+"\n"
                +"::push_stack_tmp_unfree "
                            +to_string(arg1.value.block_value->get_max_size_tmpvar_unfree())+"\n"
                +tmp_var_stack_alloc
                +arg1.code
                +arg1.value.block_value->get_name()+"_exit:\n"
    +symble_controller.get_return_sign()+":\n"
                +"::pop_stack_tmp_unfree "
                            +to_string(arg1.value.block_value->get_max_size_tmpvar_unfree())+"\n"
                +"::pop_stack_tmp "+to_string(arg1.value.block_value->get_stack_tmp_var_size())+"\n"
                +"::pop_stack_var "+to_string(arg1.value.block_value->get_stack_var_size())+"\n"
                +"::pop_registers\n"
                +"::return\n";
    cout<<arg1.code<<endl;
    Symble_node_t snode = symble_controller.get_symble_node_by_name(arg0.value.str_value);
    /**
     *  限定函数返回值只能是32位int
     */
    /*
     check ret data type if it is int type;
     */

    if(snode.data_type.top()->get_type_sign() != primary_type_int().get_type_sign()
       || snode.data_type.get_type_stack_size()!=2){
        Utils::Exit_normal::pt_error_source_code_lan("函数返回值只能为int类型", arg0.coor);
    }
    
    cout<<"识别到函数："<<arg0.value.str_value<<endl;
    return ret_word;
}
//7.1 <函数声明>-><变量声明或定义>
tree_node_t guid_func_7_1(guid_func_args_t args){
    production_t prod = get_prod_by_idx(args.get_pord_idx());
    Symble_block_stack_t &symble_controller = *Symble_block_stack_t::get_shared_impl();
    tree_node_t arg0=args.get_arg(0);
//    tree_node_t arg1=args.get_arg(1);
    tree_node_t ret_word;
    /*
     enum type_t{
     UNDEFINED,
     VAR,
     ADDRESS_SIGN,
     CODE_BLOCK,
     STATIC_VALUE;
     };
     coor_t coor;
     value_t value;
     string code;
     type_t type=UNDEFINED;
     string name;
     */
    ret_word.coor=arg0.coor;
    ret_word.value=arg0.value;
    ret_word.code=arg0.code;
    ret_word.type=tree_node_t::VAR;
    ret_word.name=echo_num2str(prod.left);
    ret_word.set_left_able(false);
    /*
     check data type if it is func ptr;
     */
    if(ret_word.value.symble_node_value==NULL){
        cout<<"error:guid:"<<__LINE__<<endl;
        exit(-1);
    }
    data_type_t data_type = ret_word.value.symble_node_value->data_type;
    if(data_type.is_func_ptr()==false){
        Utils::Exit_normal::pt_error_source_code_lan(string("function must be func ptr.")+"func name="+ret_word.value.str_value+
                                " type sign="+data_type.get_type_sign(), ret_word.coor);
    }
    
    cout<<"识别到函数声明："<<arg0.value.str_value<<endl;
    
    
    return ret_word;
}
//8 <变量声明或定义>-><广义数据类型><类型计算式>
tree_node_t guid_func_8(guid_func_args_t args){
    production_t prod = get_prod_by_idx(args.get_pord_idx());
    Symble_block_stack_t &symble_controller = *Symble_block_stack_t::get_shared_impl();
    tree_node_t arg0=args.get_arg(0);
    tree_node_t arg1=args.get_arg(1);
    tree_node_t ret_word;
    /*
     enum type_t{
     UNDEFINED,
     VAR,
     ADDRESS_SIGN,
     CODE_BLOCK,
     STATIC_VALUE;
     };
     coor_t coor;
     value_t value;
     string code;
     type_t type=UNDEFINED;
     string name;
     */
    ret_word.coor=arg0.coor;
    ret_word.value=arg1.value;
    ret_word.code=arg1.code;
    ret_word.type=tree_node_t::VAR;
    ret_word.name=echo_num2str(prod.left);
    ret_word.set_left_able(false);
    data_type_t &data_type = ret_word.value.symble_node_value->data_type;
    if(symble_controller.count_symble_node_by_name(arg0.value.str_value)){
        data_type_t new_data_type;
        new_data_type = symble_controller.get_symble_node_by_name(arg0.value.str_value).data_type;
        data_type.push(new_data_type.top());
    }
    else{
        Utils::Exit_normal::pt_error_source_code_lan(string("undefined data type:"+arg0.value.str_value), arg0.coor);
    }
    cout<<"识别到变量："<<ret_word.value.str_value<<endl;
    cout<<"类型签名："<<ret_word.value.symble_node_value->data_type.get_type_sign()<<endl;
    cout<<"sizeof="<<data_type.get_sizeof()<<endl;
    
    symble_controller.push_symble(*ret_word.value.symble_node_value);
    
    return ret_word;
}
//9 <程序块>-><程序块首部><语句列表><程序块尾部>
tree_node_t guid_func_9(guid_func_args_t args){
    production_t prod = get_prod_by_idx(args.get_pord_idx());
    Symble_block_stack_t &symble_controller = *Symble_block_stack_t::get_shared_impl();
    tree_node_t arg0=args.get_arg(0);
    tree_node_t arg1=args.get_arg(1);
    tree_node_t arg2=args.get_arg(2);
    //    tree_node_t arg1=args.get_arg(1);
    tree_node_t ret_word;
    /*
     enum type_t{
     UNDEFINED,
     VAR,
     ADDRESS_SIGN,
     CODE_BLOCK,
     STATIC_VALUE;
     };
     coor_t coor;
     value_t value;
     string code;
     type_t type=UNDEFINED;
     string name;
     */
    ret_word.coor=arg0.coor;
    ret_word.value=arg0.value;
    ret_word.code=arg0.code;
    ret_word.type=tree_node_t::VAR;
    ret_word.name=echo_num2str(prod.left);
    ret_word.set_left_able(false);
    ret_word.code=arg0.code+
                    arg1.code+arg2.code;
    if(arg0.value.block_value ){
        cout<<"guid_func_9: arg0.value.block_value != NULL"<<endl;
        exit(-1);
    }
    if(arg2.value.block_value == NULL){
        cout<<"guid_func_9: arg2.value.block_value == NULL"<<endl;
        exit(-1);
    }
    ret_word.value.block_value=arg2.value.block_value;
    arg2.value.block_value=NULL;
    
    return ret_word;
}
//10.1 <函数定义头>-><函数声明>
tree_node_t guid_func_10_1(guid_func_args_t args){
    production_t prod = get_prod_by_idx(args.get_pord_idx());
    Symble_block_stack_t &symble_controller = *Symble_block_stack_t::get_shared_impl();
    tree_node_t arg0=args.get_arg(0);
//    tree_node_t arg1=args.get_arg(1);
    tree_node_t ret_word;
    /*
     enum type_t{
     UNDEFINED,
     VAR,
     ADDRESS_SIGN,
     CODE_BLOCK,
     STATIC_VALUE;
     };
     coor_t coor;
     value_t value;
     string code;
     type_t type=UNDEFINED;
     string name;
     */
    ret_word.coor=arg0.coor;
    ret_word.value=arg0.value;
    ret_word.code=arg0.code;
    ret_word.type=tree_node_t::VAR;
    ret_word.name=echo_num2str(prod.left);
    ret_word.set_left_able(false);
    ret_word.code=arg0.code;
    func_type_t &func_type = *static_cast<func_type_t*>(ret_word.value.symble_node_value->data_type.get_head());
    if(func_type.get_arg_num()){
        /**
         *  only for one argument
         */
        func_arg_t argument = func_type.get_func_arg_by_idx(0);
        symble_controller.set_arg(argument.get_arg_name(), argument.get_arg_data_type());
    }
    else{
        symble_controller.clear_arg();
    }
    
    
    return ret_word;
}

//10 <程序块>-><程序块首部><程序块尾部>
tree_node_t guid_func_10(guid_func_args_t args){
    production_t prod = get_prod_by_idx(args.get_pord_idx());
    Symble_block_stack_t &symble_controller = *Symble_block_stack_t::get_shared_impl();
    tree_node_t arg0=args.get_arg(0);
    tree_node_t arg1=args.get_arg(1);
    //    tree_node_t arg1=args.get_arg(1);
    tree_node_t ret_word;
    /*
     enum type_t{
     UNDEFINED,
     VAR,
     ADDRESS_SIGN,
     CODE_BLOCK,
     STATIC_VALUE;
     };
     coor_t coor;
     value_t value;
     string code;
     type_t type=UNDEFINED;
     string name;
     */
    ret_word.coor=arg0.coor;
    ret_word.value=arg0.value;
    ret_word.code=arg0.code;
    ret_word.type=tree_node_t::VAR;
    ret_word.name=echo_num2str(prod.left);
    ret_word.set_left_able(arg0.get_left_albe());
    ret_word.code=arg0.code+arg1.code;
    if(arg0.value.block_value ){
        cout<<"guid_func_10: arg0.value.block_value != NULL"<<endl;
        exit(-1);
    }
    if(arg1.value.block_value == NULL){
        cout<<"guid_func_10: arg1.value.block_value == NULL"<<endl;
        exit(-1);
    }
    Symble_block_t *block_value = new Symble_block_t();
    *block_value = *arg1.value.block_value;
    ret_word.value.block_value=block_value;
    return ret_word;
}
//11 <程序块首部>->'{'
tree_node_t guid_func_11(guid_func_args_t args){
    production_t prod = get_prod_by_idx(args.get_pord_idx());
    Symble_block_stack_t &symble_controller = *Symble_block_stack_t::get_shared_impl();
    tree_node_t arg0=args.get_arg(0);
    //    tree_node_t arg1=args.get_arg(1);
    tree_node_t ret_word;
    /*
     enum type_t{
     UNDEFINED,
     VAR,
     ADDRESS_SIGN,
     CODE_BLOCK,
     STATIC_VALUE;
     };
     coor_t coor;
     value_t value;
     string code;
     type_t type=UNDEFINED;
     string name;
     */
    ret_word.coor=arg0.coor;
    ret_word.value=arg0.value;
    ret_word.code=arg0.code;
    ret_word.type=tree_node_t::VAR;
    ret_word.name=echo_num2str(prod.left);
    ret_word.set_left_able(false);
    ret_word.value.int_value=0;
    symble_controller.push();
//    cout<<symble_controller.count_symble_node_by_name("aaa")<<endl;
    cout<<"controller.push()"<<endl;
    /*
     判断是否存在函数参数，若存在则向新的block中注册函数参数。返回插入新参数的中间代码
     */
    
    if(symble_controller.count_arg()){
        //only for one argument
        string name;
        Symble_node_t snode(symble_controller.top().get_name()+symble_controller.get_arg_name(),VAR_SYMBLE,arg0.coor.y,arg0.coor.x);
        snode.data_type=symble_controller.get_arg_data_type();
        snode.set_block_name(symble_controller.top().get_name());
        symble_controller.push_symble(snode);
        ret_word.code+=string("::get_func_arg ")+snode.get_name()+"\n";
    }
    symble_controller.clear_arg();
    return ret_word;
}
//12 <程序块尾部>->'}'
tree_node_t guid_func_12(guid_func_args_t args){
    production_t prod = get_prod_by_idx(args.get_pord_idx());
    Symble_block_stack_t &symble_controller = *Symble_block_stack_t::get_shared_impl();
    tree_node_t arg0=args.get_arg(0);
    //    tree_node_t arg1=args.get_arg(1);
    tree_node_t ret_word;
    /*
     enum type_t{
     UNDEFINED,
     VAR,
     ADDRESS_SIGN,
     CODE_BLOCK,
     STATIC_VALUE;
     };
     coor_t coor;
     value_t value;
     string code;
     type_t type=UNDEFINED;
     string name;
     */
    ret_word.coor=arg0.coor;
    ret_word.value=arg0.value;
    ret_word.code=arg0.code;
    ret_word.type=tree_node_t::VAR;
    ret_word.name=echo_num2str(prod.left);
    ret_word.set_left_able(arg0.get_left_albe());
    ret_word.code=string("");
    string pop_name;
    Symble_block_t *block = new Symble_block_t();
    *block=symble_controller.top();
    pop_name=symble_controller.pop();
    cout<<"controller.pop()"<<endl;
    cout<<symble_controller.top().get_csi()<<endl;
    cout<<"层次pop="<<pop_name<<endl;
    if(ret_word.value.block_value){
        cout<<"error :12 <程序块尾部>->'}' :block_value!=NULL"<<endl;
        exit(-1);
    }
    ret_word.value.block_value=block;
    return ret_word;
}
//13 <语句列表>-><语句>
tree_node_t guid_func_13(guid_func_args_t args){
    production_t prod = get_prod_by_idx(args.get_pord_idx());
    Symble_block_stack_t &symble_controller = *Symble_block_stack_t::get_shared_impl();
    tree_node_t arg0=args.get_arg(0);
    //    tree_node_t arg1=args.get_arg(1);
    tree_node_t ret_word;
    /*
     enum type_t{
     UNDEFINED,
     VAR,
     ADDRESS_SIGN,
     CODE_BLOCK,
     STATIC_VALUE;
     };
     coor_t coor;
     value_t value;
     string code;
     type_t type=UNDEFINED;
     string name;
     */
    ret_word.coor=arg0.coor;
    ret_word.value=arg0.value;
    ret_word.code=arg0.code;
    ret_word.type=arg0.type;
    ret_word.name=echo_num2str(prod.left);
    ret_word.set_left_able(arg0.get_left_albe());
    
    string free_code="";
    stack_random<string>tmp_lst = symble_controller.top().get_tmp_var_free_able();
    if (tmp_lst.size()) {
        free_code="::free_tmp_var ";
        for (int i=0;i<tmp_lst.size(); i++) {
            free_code+=tmp_lst.get_by_idx(i)+" ";
        }
        symble_controller.top().free_tmp_var_all();
        free_code+="\n";
    }
    ret_word.code+=free_code;

    return ret_word;
}
//14 <语句列表>-><语句列表><语句>
tree_node_t guid_func_14(guid_func_args_t args){
    production_t prod = get_prod_by_idx(args.get_pord_idx());
    Symble_block_stack_t &symble_controller = *Symble_block_stack_t::get_shared_impl();
    tree_node_t arg0=args.get_arg(0);
    tree_node_t arg1=args.get_arg(1);
    tree_node_t ret_word;
    /*
     enum type_t{
     UNDEFINED,
     VAR,
     ADDRESS_SIGN,
     CODE_BLOCK,
     STATIC_VALUE;
     };
     coor_t coor;
     value_t value;
     string code;
     type_t type=UNDEFINED;
     string name;
     */
    ret_word.coor=arg0.coor;
    ret_word.value=arg0.value;
    //ret_word.code=arg0.code;
    ret_word.type=tree_node_t::VAR;
    ret_word.name=echo_num2str(prod.left);
    ret_word.code=arg0.code+arg1.code;
    ret_word.set_left_able(arg0.get_left_albe());
    
    string free_code="";
    stack_random<string>tmp_lst = symble_controller.top().get_tmp_var_free_able();
    if (tmp_lst.size()) {
        free_code="::free_tmp_var ";
        for (int i=0;i<tmp_lst.size(); i++) {
            free_code+=tmp_lst.get_by_idx(i)+" ";
        }
        symble_controller.top().free_tmp_var_all();
        free_code+="\n";
    }
    ret_word.code+=free_code;
    return ret_word;
}
// <struct数据类型>->'struct'[关键字]
tree_node_t guid_func_data_type_struct(guid_func_args_t args){
    production_t prod = get_prod_by_idx(args.get_pord_idx());
    Symble_block_stack_t &symble_controller = *Symble_block_stack_t::get_shared_impl();
    tree_node_t arg0=args.get_arg(1);
    tree_node_t ret_word;
    /*
     enum type_t{
     UNDEFINED,
     VAR,
     ADDRESS_SIGN,
     CODE_BLOCK,
     STATIC_VALUE;
     };
     coor_t coor;
     value_t value;
     string code;
     type_t type=UNDEFINED;
     string name;
     */
    ret_word.coor=arg0.coor;
    ret_word.value=arg0.value;
    ret_word.type=tree_node_t::VAR;
    ret_word.name=echo_num2str(prod.left);
    ret_word.code=arg0.code;
    ret_word.set_left_able(false);
    return ret_word;
}
//17 <广义数据类型>-><struct数据类型>
tree_node_t guid_func_17(guid_func_args_t args){
    production_t prod = get_prod_by_idx(args.get_pord_idx());
    Symble_block_stack_t &symble_controller = *Symble_block_stack_t::get_shared_impl();
    tree_node_t arg0=args.get_arg(0);
    tree_node_t ret_word;
    /*
     enum type_t{
     UNDEFINED,
     VAR,
     ADDRESS_SIGN,
     CODE_BLOCK,
     STATIC_VALUE;
     };
     coor_t coor;
     value_t value;
     string code;
     type_t type=UNDEFINED;
     string name;
     */
    ret_word.coor=arg0.coor;
    ret_word.value=arg0.value;
    ret_word.type=tree_node_t::VAR;
    ret_word.name=echo_num2str(prod.left);
    ret_word.code=arg0.code;
    ret_word.set_left_able(false);
    if(!symble_controller.count_symble_node_by_true_name(arg0.value.str_value)){
        Utils::Exit_normal::pt_error_source_code_lan(
                    string("数据类型不存在:")+arg0.value.str_value, arg0.coor);
    }
    return ret_word;
}
//17.1 <参数列表>-><广义数据类型><类型计算式>
tree_node_t guid_func_17_1(guid_func_args_t args){
    production_t prod = get_prod_by_idx(args.get_pord_idx());
    Symble_block_stack_t &symble_controller = *Symble_block_stack_t::get_shared_impl();
    tree_node_t arg0=args.get_arg(0);
    tree_node_t arg1=args.get_arg(1);
    tree_node_t ret_word;
    /*
     enum type_t{
     UNDEFINED,
     VAR,
     ADDRESS_SIGN,
     CODE_BLOCK,
     STATIC_VALUE;
     };
     coor_t coor;
     value_t value;
     string code;
     type_t type=UNDEFINED;
     string name;
     */
    ret_word.coor=arg0.coor;
    ret_word.value=arg0.value;
    ret_word.type=tree_node_t::VAR;
    ret_word.name=echo_num2str(prod.left);
    ret_word.code=arg0.code;
    ret_word.set_left_able(false);
    if(!symble_controller.count_symble_node_by_true_name(arg0.value.str_value)){
        Utils::Exit_normal::pt_error_source_code_lan(
                                                     string("数据类型不存在:")+arg0.value.str_value, arg0.coor);
    }
    Symble_node_t &snode = *(ret_word.value.symble_node_value = new Symble_node_t(""));
    if(symble_controller.count_symble_node_by_name(arg0.value.str_value)==0){
        Utils::Exit_normal::pt_error_source_code_lan(string("cannot find data_type:")+arg0.value.str_value, ret_word.coor);
    }
    func_type_t *func_type_ptr=new func_type_t();
    data_type_t new_data_type,data_type;
    data_type = arg1.value.symble_node_value->data_type;
    new_data_type = symble_controller.get_symble_node_by_name(arg0.value.str_value).data_type;
    data_type.push(new_data_type.top());
    func_type_ptr->push_func_arg(arg1.value.str_value,data_type);
    
    snode.data_type.push(func_type_ptr);
    
    return ret_word;
}
//19 <类型计算式>->'*'<类型计算式>
tree_node_t guid_func_19(guid_func_args_t args){
    production_t prod = get_prod_by_idx(args.get_pord_idx());
    Symble_block_stack_t &symble_controller = *Symble_block_stack_t::get_shared_impl();
    tree_node_t arg0=args.get_arg(1);
//    tree_node_t arg1=args.get_arg(2);
    tree_node_t ret_word;
    /*
     enum type_t{
     UNDEFINED,
     VAR,
     ADDRESS_SIGN,
     CODE_BLOCK,
     STATIC_VALUE;
     };
     coor_t coor;
     value_t value;
     string code;
     type_t type=UNDEFINED;
     string name;
     */
    ret_word.coor=arg0.coor;
    ret_word.value=arg0.value;
    ret_word.code=arg0.code;
    ret_word.type=tree_node_t::VAR;
    ret_word.name=echo_num2str(prod.left);
    ret_word.set_left_able(arg0.get_left_albe());
    data_type_t &data_type = ret_word.value.symble_node_value->data_type;
    
    primary_type_pointer *type_ptr = new primary_type_pointer();
    data_type.push(type_ptr);

    return ret_word;
}
//20 <类型计算式>-><中括号运算>
tree_node_t guid_func_20(guid_func_args_t args){
    production_t prod = get_prod_by_idx(args.get_pord_idx());
    Symble_block_stack_t &symble_controller = *Symble_block_stack_t::get_shared_impl();
    tree_node_t arg0=args.get_arg(0);
    //    tree_node_t arg1=args.get_arg(1);
    tree_node_t ret_word;
    /*
     enum type_t{
     UNDEFINED,
     VAR,
     ADDRESS_SIGN,
     CODE_BLOCK,
     STATIC_VALUE;
     };
     coor_t coor;
     value_t value;
     string code;
     type_t type=UNDEFINED;
     string name;
     */
    
    ret_word.coor=arg0.coor;
    ret_word.value=arg0.value;
    ret_word.code=arg0.code;
    ret_word.type=arg0.type;
    ret_word.name=echo_num2str(prod.left);
    ret_word.set_left_able(arg0.get_left_albe());
    return ret_word;
}

//21 <中括号运算>-><中括号运算>'['<值>']'
tree_node_t guid_func_21(guid_func_args_t args){
    production_t prod = get_prod_by_idx(args.get_pord_idx());
    Symble_block_stack_t &symble_controller = *Symble_block_stack_t::get_shared_impl();
    tree_node_t arg0=args.get_arg(0);
    tree_node_t arg1=args.get_arg(2);
    tree_node_t ret_word;
    /*
     enum type_t{
     UNDEFINED,
     VAR,
     ADDRESS_SIGN,
     CODE_BLOCK,
     STATIC_VALUE;
     };
     coor_t coor;
     value_t value;
     string code;
     type_t type=UNDEFINED;
     string name;
     */
    ret_word.coor=arg0.coor;
    ret_word.value=arg0.value;
    ret_word.code=arg0.code;
    ret_word.type=tree_node_t::VAR;
    ret_word.name=echo_num2str(prod.left);
    ret_word.set_left_able(arg0.get_left_albe());
    data_type_t &data_type = ret_word.value.symble_node_value->data_type;
    
    if(arg1.type!=tree_node_t::STATIC_VALUE || arg1.value.str_value.length()){
        Utils::Exit_normal::pt_error_source_code_lan(string("声明数组大小需要常数"), arg1.coor);
    }
    primary_type_array *type_ptr = new primary_type_array(arg1.value.int_value);
    data_type.push(type_ptr);
    return ret_word;
}
//25 <函数括号运算>-><括号运算>
tree_node_t guid_func_25(guid_func_args_t args){
    production_t prod = get_prod_by_idx(args.get_pord_idx());
    Symble_block_stack_t &symble_controller = *Symble_block_stack_t::get_shared_impl();
    tree_node_t arg0=args.get_arg(0);
    //    tree_node_t arg1=args.get_arg(1);
    tree_node_t ret_word;
    /*
     enum type_t{
     UNDEFINED,
     VAR,
     ADDRESS_SIGN,
     CODE_BLOCK,
     STATIC_VALUE;
     };
     coor_t coor;
     value_t value;
     string code;
     type_t type=UNDEFINED;
     string name;
     */
    
    ret_word.coor=arg0.coor;
    ret_word.value=arg0.value;
    ret_word.code=arg0.code;
    ret_word.type=arg0.type;
    ret_word.name=echo_num2str(prod.left);
    ret_word.set_left_able(arg0.get_left_albe());
    return ret_word;
}

//22 <中括号运算>-><函数括号运算>
tree_node_t guid_func_22(guid_func_args_t args){
    production_t prod = get_prod_by_idx(args.get_pord_idx());
    Symble_block_stack_t &symble_controller = *Symble_block_stack_t::get_shared_impl();
    tree_node_t arg0=args.get_arg(0);
    //    tree_node_t arg1=args.get_arg(1);
    tree_node_t ret_word;
    /*
     enum type_t{
     UNDEFINED,
     VAR,
     ADDRESS_SIGN,
     CODE_BLOCK,
     STATIC_VALUE;
     };
     coor_t coor;
     value_t value;
     string code;
     type_t type=UNDEFINED;
     string name;
     */
    
    ret_word.coor=arg0.coor;
    ret_word.value=arg0.value;
    ret_word.code=arg0.code;
    ret_word.type=arg0.type;
    ret_word.name=echo_num2str(prod.left);
    ret_word.set_left_able(arg0.get_left_albe());
    return ret_word;
}
//23 <值>-><值>'='<逻辑或表达式>
tree_node_t guid_func_23(guid_func_args_t args){
    production_t prod = get_prod_by_idx(args.get_pord_idx());
    Symble_block_stack_t &symble_controller = *Symble_block_stack_t::get_shared_impl();
    tree_node_t arg0=args.get_arg(0);
    tree_node_t arg1=args.get_arg(2);
    tree_node_t ret_word;
    /*
     enum type_t{
     UNDEFINED,
     VAR,
     ADDRESS_SIGN,
     CODE_BLOCK,
     STATIC_VALUE;
     };
     coor_t coor;
     value_t value;
     string code;
     type_t type=UNDEFINED;
     string name;
     */
    ret_word.coor=arg0.coor;
    ret_word.name=echo_num2str(prod.left);
    ret_word.set_left_able(arg0.get_left_albe());
    //    if(arg0.type==tree_node_t::STATIC_VALUE && arg1.type==tree_node_t::STATIC_VALUE){
    //        ret_word.type=arg0.type;
    //    }
    //    else{
    //        ret_word.type=tree_node_t::VAR;
    //    }
    ret_word.type=tree_node_t::VAR;
    Tmp_var_ctrl &var_ctrl = *Tmp_var_ctrl::get_shared_impl();
   // string new_var=var_ctrl.alloc_tmp_var();
    string a,b;
    data_type_t type0;
    data_type_t type1;
    if(arg0.type==tree_node_t::STATIC_VALUE){
        a=to_string(arg0.value.int_value);
        type0=arg0.value.symble_node_value->data_type;
    }
    else if(arg0.type==tree_node_t::VAR){
        a=arg0.value.str_value;
        if(!symble_controller.count_symble_node_by_name(arg0.value.str_value) || arg0.value.str_value.length()==0){
            Utils::Exit_normal::pt_error_source_code_lan(string("未定义的关键字:")+arg0.value.str_value, arg0.coor);
        }
        Symble_node_t snode=symble_controller.get_symble_node_by_name(arg0.value.str_value);
        if(snode.get_block_name().length()){
            if (!var_ctrl.is_tmp_var(a)) {
//                a=snode.get_block_name()+a;
            }
        }
        else{
            cout<<"error:prefix.length==0"<<endl;
            exit(-1);
        }
        type0=snode.data_type;
    }
    if(arg1.type==tree_node_t::STATIC_VALUE){
        b=to_string(arg1.value.int_value);
        type1=arg1.value.symble_node_value->data_type;
    }
    else if(arg1.type==tree_node_t::VAR){
        b=arg1.value.str_value;
        if(!symble_controller.count_symble_node_by_name(arg1.value.str_value)){
            Utils::Exit_normal::pt_error_source_code_lan(string("未定义的关键字:")+arg1.value.str_value, arg0.coor);
        }
        Symble_node_t snode=symble_controller.get_symble_node_by_name(arg1.value.str_value);
        if(snode.get_block_name().length()){
            if (!var_ctrl.is_tmp_var(b)) {
//                b=snode.get_block_name()+b;
            }
        }
        else{
            cout<<"error:prefix.length==0"<<endl;
            exit(-1);
        }
        type1=snode.data_type;
    }
    
    if(type0.is_const()){
        Utils::Exit_normal::pt_error_source_code_lan(string("赋值运算：左值为const，无法赋值:")+a, arg0.coor);

    }
    if(!arg0.get_left_albe()){
        Utils::Exit_normal::pt_error_source_code_lan(string("赋值运算：无法作为左值，无法赋值:")+a, arg0.coor);
        
    }
    if(type0.get_type_sign() != type1.get_type_sign()){
        if(type0.get_sizeof()==type1.get_sizeof() && type0.get_sizeof()==unit
           &&!type1.is_struct()&&!type0.is_struct()){
            //默认的强制类型转换
        }
        else{
            Utils::Exit_normal::pt_error_source_code_lan(string("赋值运算：类型不匹配:")
                                                         +type0.get_type_sign()+" 与 "+type1.get_type_sign(), arg0.coor);
        }
        
    }
    
    string new_code=a+" = "+b;
    ret_word.code=arg0.code+arg1.code+new_code+"\n";
//    ret_word.value.str_value=new_var;
//    ret_word.value.symble_node_value = new Symble_node_t(new_var,VAR_SYMBLE,arg0.coor.y,arg0.coor.x);
//    ret_word.value.symble_node_value = arg0.value.symble_node_value;
//    ret_word.value.symble_node_value->data_type=type0;
//    Symble_node_t symble_node(new_var,VAR_SYMBLE,ret_word.coor.y,ret_word.coor.x);
//    symble_node.data_type=type0;
//    symble_controller.push_symble(symble_node);
    
    cout<<"算数：赋值："<<new_code<<endl;
    ret_word.value.str_value=a;
    return ret_word;
}
//27 <括号运算>->[关键字]
tree_node_t guid_func_27(guid_func_args_t args){
    production_t prod = get_prod_by_idx(args.get_pord_idx());
    Symble_block_stack_t &symble_controller = *Symble_block_stack_t::get_shared_impl();
    tree_node_t arg0=args.get_arg(0);
    //    tree_node_t arg1=args.get_arg(1);
    tree_node_t ret_word;
    /*
     enum type_t{
     UNDEFINED,
     VAR,
     ADDRESS_SIGN,
     CODE_BLOCK,
     STATIC_VALUE;
     };
     coor_t coor;
     value_t value;
     string code;
     type_t type=UNDEFINED;
     string name;
     */
    
    ret_word.coor=arg0.coor;
    ret_word.value=arg0.value;
    ret_word.code=arg0.code;
    ret_word.type=tree_node_t::VAR;
    ret_word.name=echo_num2str(prod.left);
    string block_prefix="";
    if(symble_controller.size()>1)
        block_prefix=symble_controller.top().get_name();
    ret_word.value.str_value=block_prefix+string(PREFIX_VAR)+ret_word.value.str_value;
    if(symble_controller.top_count_symble_node_by_name(ret_word.value.str_value)){
        Utils::Exit_normal::pt_error_source_code_lan(string("标识符重复定义:")+ret_word.value.str_value,ret_word.coor);
    }

  //  ret_word.value.int_value=0;//无用，仅仅是让log输出时不打印乱七八糟的数字
    cout<<"识别到变量算子："<<arg0.value.str_value<<endl;
    Symble_node_t *symble_node_ptr = new Symble_node_t(ret_word.value.str_value,VAR_SYMBLE,ret_word.coor.y,ret_word.coor.x);
    symble_node_ptr->set_block_name(symble_controller.top().get_name());
    ret_word.value.symble_node_value= symble_node_ptr;
    return ret_word;
}
//28 <括号运算>->'('<类型计算式>')'
tree_node_t guid_func_28(guid_func_args_t args){
    production_t prod = get_prod_by_idx(args.get_pord_idx());
    Symble_block_stack_t &symble_controller = *Symble_block_stack_t::get_shared_impl();
    tree_node_t arg0=args.get_arg(1);
    tree_node_t ret_word;
    /*
     enum type_t{
     UNDEFINED,
     VAR,
     ADDRESS_SIGN,
     CODE_BLOCK,
     STATIC_VALUE;
     };
     coor_t coor;
     value_t value;
     string code;
     type_t type=UNDEFINED;
     string name;
     */
    ret_word.coor=arg0.coor;
    ret_word.value=arg0.value;
    ret_word.code=arg0.code;
    ret_word.type=arg0.type;
    ret_word.name=echo_num2str(prod.left);
    ret_word.set_left_able(arg0.get_left_albe());
    Symble_block_t top=symble_controller.top();
    return ret_word;
}

//31 <语句>-><程序块>
tree_node_t guid_func_31(guid_func_args_t args){
    production_t prod = get_prod_by_idx(args.get_pord_idx());
    Symble_block_stack_t &symble_controller = *Symble_block_stack_t::get_shared_impl();
    tree_node_t arg0=args.get_arg(0);
    tree_node_t ret_word;
    /*
     enum type_t{
     UNDEFINED,
     VAR,
     ADDRESS_SIGN,
     CODE_BLOCK,
     STATIC_VALUE;
     };
     coor_t coor;
     value_t value;
     string code;
     type_t type=UNDEFINED;
     string name;
     */
    
    ret_word.coor=arg0.coor;
    ret_word.value=arg0.value;
    ret_word.code=arg0.code;
    ret_word.type=tree_node_t::VAR;
    ret_word.name=echo_num2str(prod.left);
    ret_word.set_left_able(false);
    return ret_word;
}
//34 <语句>-><值>';'
tree_node_t guid_func_34(guid_func_args_t args){
    production_t prod = get_prod_by_idx(args.get_pord_idx());
    Symble_block_stack_t &symble_controller = *Symble_block_stack_t::get_shared_impl();
    tree_node_t arg0=args.get_arg(0);
    //    tree_node_t arg1=args.get_arg(1);
    tree_node_t ret_word;
    /*
     enum type_t{
     UNDEFINED,
     VAR,
     ADDRESS_SIGN,
     CODE_BLOCK,
     STATIC_VALUE;
     };
     coor_t coor;
     value_t value;
     string code;
     type_t type=UNDEFINED;
     string name;
     */
    
    ret_word.coor=arg0.coor;
    ret_word.value=arg0.value;
    ret_word.code=arg0.code;
    ret_word.type=tree_node_t::VAR;
    ret_word.name=echo_num2str(prod.left);
    ret_word.set_left_able(false);
    return ret_word;
}
//48 <加法表达式>-><加法表达式>'+'<强制类型转换表达式>
tree_node_t guid_func_48(guid_func_args_t args){
    production_t prod = get_prod_by_idx(args.get_pord_idx());
    Symble_block_stack_t &symble_controller = *Symble_block_stack_t::get_shared_impl();
    tree_node_t arg0=args.get_arg(0);
    tree_node_t arg1=args.get_arg(2);
    tree_node_t ret_word;
    /*
     enum type_t{
     UNDEFINED,
     VAR,
     ADDRESS_SIGN,
     CODE_BLOCK,
     STATIC_VALUE;
     };
     coor_t coor;
     value_t value;
     string code;
     type_t type=UNDEFINED;
     string name;
     */
    ret_word.coor=arg0.coor;
    ret_word.name=echo_num2str(prod.left);
    //    if(arg0.type==tree_node_t::STATIC_VALUE && arg1.type==tree_node_t::STATIC_VALUE){
    //        ret_word.type=arg0.type;
    //    }
    //    else{
    //        ret_word.type=tree_node_t::VAR;
    //    }
    ret_word.type=tree_node_t::VAR;
    ret_word.set_left_able(false);
    Tmp_var_ctrl &var_ctrl = *Tmp_var_ctrl::get_shared_impl();
    string a,b;
    data_type_t type0;
    data_type_t type1;
    if(arg0.type==tree_node_t::STATIC_VALUE){
        a=to_string(arg0.value.int_value);
        type0=arg0.value.symble_node_value->data_type;
    }
    else if(arg0.type==tree_node_t::VAR){
        a=arg0.value.str_value;
        if(!symble_controller.count_symble_node_by_name(arg0.value.str_value)){
            Utils::Exit_normal::pt_error_source_code_lan(string("未定义的关键字:")+arg0.value.str_value, arg0.coor);
        }
        Symble_node_t snode=symble_controller.get_symble_node_by_name(arg0.value.str_value);
        if(snode.get_block_name().length())
        {
            if(!var_ctrl.is_tmp_var(a)){
              //  a=snode.get_block_name()+a;
            }
        }
            
        else{
            cout<<"error:prefix.length==0"<<endl;
            exit(-1);
        }
        type0=snode.data_type;
    }
    if(arg1.type==tree_node_t::STATIC_VALUE){
        b=to_string(arg1.value.int_value);
        type1=arg1.value.symble_node_value->data_type;
    }
    else if(arg1.type==tree_node_t::VAR){
        b=arg1.value.str_value;
        if(!symble_controller.count_symble_node_by_name(arg1.value.str_value)){
            Utils::Exit_normal::pt_error_source_code_lan(string("未定义的关键字:")+arg1.value.str_value, arg0.coor);
        }
        Symble_node_t snode=symble_controller.get_symble_node_by_name(arg1.value.str_value);
        if(snode.get_block_name().length())
        {
            if(!var_ctrl.is_tmp_var(b)){
              //  b=snode.get_block_name()+b;
            }
        }
        else{
            cout<<"error:prefix.length==0"<<endl;
            exit(-1);
        }
        type1=snode.data_type;
    }
    data_type_t int_type;
    string new_code="";
    int_type.push(new primary_type_int());
    if(type0.is_struct()||type1.is_struct()){// type0 and type1 all must be judged.
        Utils::Exit_normal::pt_error_source_code_lan(string("struct cannot be added"), ret_word.coor);
    }
    else if((type0.is_ptr_normal() && type1.get_type_sign() == int_type.get_type_sign())||(type0.get_type_sign()==int_type.get_type_sign() && type1.is_ptr_normal()))
    {    //指针的默认类型转换
        if(type0.is_ptr_normal()){
            //a为指针，给b乘上单元大小再加到a上
            string offset_tmp_var = var_ctrl.alloc_tmp_var();
            new_code=new_code
                    +offset_tmp_var+" = "+b+" * "+to_string(type0.get_ptr_offset())+"\n";
            b=offset_tmp_var;//a为指针，b为偏移
            //注册offset_tmp_var
            Symble_node_t symble_node(offset_tmp_var,VAR_SYMBLE,ret_word.coor.y,ret_word.coor.x);
            symble_node.data_type=int_type;
            symble_node.set_block_name(symble_controller.top().get_name());
            symble_controller.push_symble(symble_node);
            
        }
        else{   //type1.is_ptr_normal();
            //b为指针，给a乘上单元大小再加到b上
            string offset_tmp_var = var_ctrl.alloc_tmp_var();
            new_code=new_code
            +offset_tmp_var+" = "+a+" * "+to_string(type1.get_ptr_offset())+"\n";
            a=offset_tmp_var;//b为指针，a为偏移
            //注册offset_tmp_var
            Symble_node_t symble_node(offset_tmp_var,VAR_SYMBLE,ret_word.coor.y,ret_word.coor.x);
            symble_node.data_type=int_type;
            symble_node.set_block_name(symble_controller.top().get_name());
            symble_controller.push_symble(symble_node);
            type0=type1;
            
        }
    }
    else if(type0.get_type_sign() != type1.get_type_sign()){
        Utils::Exit_normal::pt_error_source_code_lan(string("加法：类型不匹配:")+type0.get_type_sign()+" 与 "+type1.get_type_sign(), arg0.coor);
    }
    string new_var=var_ctrl.alloc_tmp_var();

    new_code=new_code+new_var+" = "+ a + " + "+b;
    ret_word.code=arg0.code+arg1.code+new_code+"\n";
    ret_word.value.str_value=new_var;
    ret_word.value.symble_node_value = new Symble_node_t(new_var,VAR_SYMBLE,arg0.coor.y,arg0.coor.x);
    ret_word.value.symble_node_value->data_type=type0;
    //注册new_var
    Symble_node_t symble_node(new_var,VAR_SYMBLE,ret_word.coor.y,ret_word.coor.x);
    symble_node.data_type=type0;
    symble_node.set_block_name(symble_controller.top().get_name());
    symble_controller.push_symble(symble_node);
    
    cout<<"算数：加法："<<new_code<<endl;
    ret_word.value.symble_node_value->data_type=type0;
    return ret_word;
}
//49 <加法表达式>-><加法表达式>'-'<强制类型转换表达式>
tree_node_t guid_func_49(guid_func_args_t args){
    production_t prod = get_prod_by_idx(args.get_pord_idx());
    Symble_block_stack_t &symble_controller = *Symble_block_stack_t::get_shared_impl();
    tree_node_t arg0=args.get_arg(0);
    tree_node_t arg1=args.get_arg(2);
    tree_node_t ret_word;
    /*
     enum type_t{
     UNDEFINED,
     VAR,
     ADDRESS_SIGN,
     CODE_BLOCK,
     STATIC_VALUE;
     };
     coor_t coor;
     value_t value;
     string code;
     type_t type=UNDEFINED;
     string name;
     */
    ret_word.coor=arg0.coor;
    ret_word.name=echo_num2str(prod.left);
    //    if(arg0.type==tree_node_t::STATIC_VALUE && arg1.type==tree_node_t::STATIC_VALUE){
    //        ret_word.type=arg0.type;
    //    }
    //    else{
    //        ret_word.type=tree_node_t::VAR;
    //    }
    ret_word.type=tree_node_t::VAR;
    ret_word.set_left_able(false);
    Tmp_var_ctrl &var_ctrl = *Tmp_var_ctrl::get_shared_impl();
    string a,b;
    data_type_t type0;
    data_type_t type1;
    if(arg0.type==tree_node_t::STATIC_VALUE){
        a=to_string(arg0.value.int_value);
        type0=arg0.value.symble_node_value->data_type;
    }
    else if(arg0.type==tree_node_t::VAR){
        a=arg0.value.str_value;
        if(!symble_controller.count_symble_node_by_name(arg0.value.str_value)){
            Utils::Exit_normal::pt_error_source_code_lan(string("未定义的关键字:")+arg0.value.str_value, arg0.coor);
        }
        Symble_node_t snode=symble_controller.get_symble_node_by_name(arg0.value.str_value);
        if(snode.get_block_name().length())
        {
            if(!var_ctrl.is_tmp_var(a)){
                //  a=snode.get_block_name()+a;
            }
        }
        
        else{
            cout<<"error:prefix.length==0"<<endl;
            exit(-1);
        }
        type0=snode.data_type;
    }
    if(arg1.type==tree_node_t::STATIC_VALUE){
        b=to_string(arg1.value.int_value);
        type1=arg1.value.symble_node_value->data_type;
    }
    else if(arg1.type==tree_node_t::VAR){
        b=arg1.value.str_value;
        if(!symble_controller.count_symble_node_by_name(arg1.value.str_value)){
            Utils::Exit_normal::pt_error_source_code_lan(string("未定义的关键字:")+arg1.value.str_value, arg0.coor);
        }
        Symble_node_t snode=symble_controller.get_symble_node_by_name(arg1.value.str_value);
        if(snode.get_block_name().length())
        {
            if(!var_ctrl.is_tmp_var(b)){
                //  b=snode.get_block_name()+b;
            }
        }
        else{
            cout<<"error:prefix.length==0"<<endl;
            exit(-1);
        }
        type1=snode.data_type;
    }
    data_type_t int_type;
    string new_code="";
    int_type.push(new primary_type_int());
    if(type0.is_struct()||type1.is_struct()){// type0 and type1 all must be judged.
        Utils::Exit_normal::pt_error_source_code_lan(string("struct cannot be subed"), ret_word.coor);
    }
    else if((type0.is_ptr_normal() && type1.get_type_sign() == int_type.get_type_sign())||(type0.get_type_sign()==int_type.get_type_sign() && type1.is_ptr_normal()))
    {    //指针的默认类型转换
        if(type0.is_ptr_normal()){
            //a为指针，给b乘上单元大小再加到a上
            string offset_tmp_var = var_ctrl.alloc_tmp_var();
            new_code=new_code
            +offset_tmp_var+" = "+b+" * "+to_string(type0.get_ptr_offset())+"\n";
            b=offset_tmp_var;//a为指针，b为偏移
            //注册offset_tmp_var
            Symble_node_t symble_node(offset_tmp_var,VAR_SYMBLE,ret_word.coor.y,ret_word.coor.x);
            symble_node.data_type=int_type;
            symble_node.set_block_name(symble_controller.top().get_name());
            symble_controller.push_symble(symble_node);
            
        }
        else{   //type1.is_ptr_normal();
            //b为指针，给a乘上单元大小再加到b上
            string offset_tmp_var = var_ctrl.alloc_tmp_var();
            new_code=new_code
            +offset_tmp_var+" = "+a+" * "+to_string(type1.get_ptr_offset())+"\n";
            a=offset_tmp_var;//b为指针，a为偏移
            //注册offset_tmp_var
            Symble_node_t symble_node(offset_tmp_var,VAR_SYMBLE,ret_word.coor.y,ret_word.coor.x);
            symble_node.data_type=int_type;
            symble_node.set_block_name(symble_controller.top().get_name());
            symble_controller.push_symble(symble_node);
            type0=type1;
            
        }
    }
    else if(type0.get_type_sign() != type1.get_type_sign()){
        Utils::Exit_normal::pt_error_source_code_lan(string("减法：类型不匹配:")+type0.get_type_sign()+" 与 "+type1.get_type_sign(), arg0.coor);
    }
    string new_var=var_ctrl.alloc_tmp_var();
    
    new_code=new_code+new_var+" = "+ a + " - "+b;
    ret_word.code=arg0.code+arg1.code+new_code+"\n";
    ret_word.value.str_value=new_var;
    ret_word.value.symble_node_value = new Symble_node_t(new_var,VAR_SYMBLE,arg0.coor.y,arg0.coor.x);
    ret_word.value.symble_node_value->data_type=type0;
    //注册new_var
    Symble_node_t symble_node(new_var,VAR_SYMBLE,ret_word.coor.y,ret_word.coor.x);
    symble_node.data_type=type0;
    symble_node.set_block_name(symble_controller.top().get_name());
    symble_controller.push_symble(symble_node);
    
    ret_word.value.symble_node_value->data_type=type0;
    return ret_word;
}
//50 <比较表达式>-><比较表达式>'<'<加法表达式>
tree_node_t guid_func_50(guid_func_args_t args){
    production_t prod = get_prod_by_idx(args.get_pord_idx());
    Symble_block_stack_t &symble_controller = *Symble_block_stack_t::get_shared_impl();
    tree_node_t arg0=args.get_arg(0);
    tree_node_t arg1=args.get_arg(2);
    tree_node_t ret_word;
    /*
     enum type_t{
     UNDEFINED,
     VAR,
     ADDRESS_SIGN,
     CODE_BLOCK,
     STATIC_VALUE;
     };
     coor_t coor;
     value_t value;
     string code;
     type_t type=UNDEFINED;
     string name;
     */
    ret_word.coor=arg0.coor;
    ret_word.name=echo_num2str(prod.left);
    //    if(arg0.type==tree_node_t::STATIC_VALUE && arg1.type==tree_node_t::STATIC_VALUE){
    //        ret_word.type=arg0.type;
    //    }
    //    else{
    //        ret_word.type=tree_node_t::VAR;
    //    }
    ret_word.type=tree_node_t::VAR;
    ret_word.set_left_able(false);

    Tmp_var_ctrl &var_ctrl = *Tmp_var_ctrl::get_shared_impl();
    string new_var=var_ctrl.alloc_tmp_var();
    string a,b;
    data_type_t type0;
    data_type_t type1;
    if(arg0.type==tree_node_t::STATIC_VALUE){
        a=to_string(arg0.value.int_value);
        type0=arg0.value.symble_node_value->data_type;
    }
    else if(arg0.type==tree_node_t::VAR){
        a=arg0.value.str_value;
        if(!symble_controller.count_symble_node_by_name(arg0.value.str_value)){
            Utils::Exit_normal::pt_error_source_code_lan(string("未定义的关键字:")+arg0.value.str_value, arg0.coor);
        }
        Symble_node_t snode=symble_controller.get_symble_node_by_name(arg0.value.str_value);
        if(snode.get_block_name().length()){
            if(!var_ctrl.is_tmp_var(a)){
                // a=snode.get_block_name()+a;
            }
            
        }
        else{
            cout<<"error:prefix.length==0"<<endl;
            exit(-1);
        }
        type0=snode.data_type;
    }
    else{
        Utils::Exit_normal::pt_error_source_code_lan("比较表达式错误，类型错误", arg0.coor);
    }
    if(arg1.type==tree_node_t::STATIC_VALUE){
        b=to_string(arg1.value.int_value);
        type1=arg1.value.symble_node_value->data_type;
    }
    else if(arg1.type==tree_node_t::VAR){
        b=arg1.value.str_value;
        if(!symble_controller.count_symble_node_by_name(arg1.value.str_value)){
            Utils::Exit_normal::pt_error_source_code_lan(string("未定义的关键字:")+arg1.value.str_value, arg0.coor);
        }
        Symble_node_t snode=symble_controller.get_symble_node_by_name(arg1.value.str_value);
        if(snode.get_block_name().length()){
            if(!var_ctrl.is_tmp_var(b)){
              //  b=snode.get_block_name()+b;
            }
        }
        else{
            cout<<"error:prefix.length==0"<<endl;
            exit(-1);
        }
        type1=snode.data_type;
    }
    else{
        Utils::Exit_normal::pt_error_source_code_lan("比较表达式错误，类型错误", arg0.coor);
    }
    
    if(type0.get_type_sign() != type1.get_type_sign()){
        Utils::Exit_normal::pt_error_source_code_lan(string("小于运算符：类型不匹配:")+type0.get_type_sign()+" 与 "+type1.get_type_sign(), arg0.coor);
    }
    else if(type0.is_struct()){
        Utils::Exit_normal::pt_error_source_code_lan(string("逻辑运算符不能作用于结构体"), ret_word.coor);
    }
    string new_code=new_var+" = "+ a + " < "+b;
    ret_word.code=arg0.code+arg1.code+new_code+"\n";
    ret_word.value.str_value=new_var;
    ret_word.value.symble_node_value = new Symble_node_t(new_var,VAR_SYMBLE,arg0.coor.y,arg0.coor.x);
    ret_word.value.symble_node_value->data_type=type0;
    Symble_node_t symble_node(new_var,VAR_SYMBLE,ret_word.coor.y,ret_word.coor.x);
    symble_node.data_type=type0;
    symble_node.set_block_name(symble_controller.top().get_name());
    symble_controller.push_symble(symble_node);
    
    cout<<"算数：小于运算："<<new_code<<endl;
    return ret_word;
}
//59 <比较表达式>-><比较表达式>'=='<加法表达式>
tree_node_t guid_func_equal_to(guid_func_args_t args){
    production_t prod = get_prod_by_idx(args.get_pord_idx());
    Symble_block_stack_t &symble_controller = *Symble_block_stack_t::get_shared_impl();
    tree_node_t arg0=args.get_arg(0);
    tree_node_t arg1=args.get_arg(2);
    tree_node_t ret_word;

    ret_word.coor=arg0.coor;
    ret_word.name=echo_num2str(prod.left);
    //    if(arg0.type==tree_node_t::STATIC_VALUE && arg1.type==tree_node_t::STATIC_VALUE){
    //        ret_word.type=arg0.type;
    //    }
    //    else{
    //        ret_word.type=tree_node_t::VAR;
    //    }
    ret_word.type=tree_node_t::VAR;
    ret_word.set_left_able(false);
    
    Tmp_var_ctrl &var_ctrl = *Tmp_var_ctrl::get_shared_impl();
    string new_var=var_ctrl.alloc_tmp_var();
    string a,b;
    data_type_t type0;
    data_type_t type1;
    if(arg0.type==tree_node_t::STATIC_VALUE){
        a=to_string(arg0.value.int_value);
        type0=arg0.value.symble_node_value->data_type;
    }
    else if(arg0.type==tree_node_t::VAR){
        a=arg0.value.str_value;
        if(!symble_controller.count_symble_node_by_name(arg0.value.str_value)){
            Utils::Exit_normal::pt_error_source_code_lan(string("未定义的关键字:")+arg0.value.str_value, arg0.coor);
        }
        Symble_node_t snode=symble_controller.get_symble_node_by_name(arg0.value.str_value);
        if(snode.get_block_name().length()){
            if(!var_ctrl.is_tmp_var(a)){
                // a=snode.get_block_name()+a;
            }
            
        }
        else{
            cout<<"error:prefix.length==0"<<endl;
            exit(-1);
        }
        type0=snode.data_type;
    }
    else{
        Utils::Exit_normal::pt_error_source_code_lan("判等表达式错误，类型错误", arg0.coor);
    }
    if(arg1.type==tree_node_t::STATIC_VALUE){
        b=to_string(arg1.value.int_value);
        type1=arg1.value.symble_node_value->data_type;
    }
    else if(arg1.type==tree_node_t::VAR){
        b=arg1.value.str_value;
        if(!symble_controller.count_symble_node_by_name(arg1.value.str_value)){
            Utils::Exit_normal::pt_error_source_code_lan(string("未定义的关键字:")+arg1.value.str_value, arg0.coor);
        }
        Symble_node_t snode=symble_controller.get_symble_node_by_name(arg1.value.str_value);
        if(snode.get_block_name().length()){
            if(!var_ctrl.is_tmp_var(b)){
                //  b=snode.get_block_name()+b;
            }
        }
        else{
            cout<<"error:prefix.length==0"<<endl;
            exit(-1);
        }
        type1=snode.data_type;
    }
    else{
        Utils::Exit_normal::pt_error_source_code_lan("判等表达式错误，类型错误", arg0.coor);
    }
    
    if(type0.get_type_sign() != type1.get_type_sign()){
        Utils::Exit_normal::pt_error_source_code_lan(string("判等：类型不匹配:")+type0.get_type_sign()+" 与 "+type1.get_type_sign(), arg0.coor);
    }
    else if(type0.is_struct()){
        Utils::Exit_normal::pt_error_source_code_lan(string("逻辑运算符不能作用于结构体"), ret_word.coor);
    }
    string new_code=new_var+" = "+ a + " == "+b;
    ret_word.code=arg0.code+arg1.code+new_code+"\n";
    ret_word.value.str_value=new_var;
    ret_word.value.symble_node_value = new Symble_node_t(new_var,VAR_SYMBLE,arg0.coor.y,arg0.coor.x);
    ret_word.value.symble_node_value->data_type=type0;
    Symble_node_t symble_node(new_var,VAR_SYMBLE,ret_word.coor.y,ret_word.coor.x);
    symble_node.data_type=type0;
    symble_node.set_block_name(symble_controller.top().get_name());
    symble_controller.push_symble(symble_node);
    
    cout<<"算数：==运算："<<new_code<<endl;
    return ret_word;
}
// <逻辑且表达式>-><逻辑且表达式>'&&'<比较表达式>
tree_node_t guid_func_logic_and(guid_func_args_t args){
    production_t prod = get_prod_by_idx(args.get_pord_idx());
    Symble_block_stack_t &symble_controller = *Symble_block_stack_t::get_shared_impl();
    tree_node_t arg0=args.get_arg(0);
    tree_node_t arg1=args.get_arg(2);
    tree_node_t ret_word;
    string op("&&");
    ret_word.coor=arg0.coor;
    ret_word.name=echo_num2str(prod.left);
    //    if(arg0.type==tree_node_t::STATIC_VALUE && arg1.type==tree_node_t::STATIC_VALUE){
    //        ret_word.type=arg0.type;
    //    }
    //    else{
    //        ret_word.type=tree_node_t::VAR;
    //    }
    ret_word.type=tree_node_t::VAR;
    ret_word.set_left_able(false);
    
    Tmp_var_ctrl &var_ctrl = *Tmp_var_ctrl::get_shared_impl();
    string new_var=var_ctrl.alloc_tmp_var();
    string a,b;
    data_type_t type0;
    data_type_t type1;
    if(arg0.type==tree_node_t::STATIC_VALUE){
        a=to_string(arg0.value.int_value);
        type0=arg0.value.symble_node_value->data_type;
    }
    else if(arg0.type==tree_node_t::VAR){
        a=arg0.value.str_value;
        if(!symble_controller.count_symble_node_by_name(arg0.value.str_value)){
            Utils::Exit_normal::pt_error_source_code_lan(string("未定义的关键字:")+arg0.value.str_value, arg0.coor);
        }
        Symble_node_t snode=symble_controller.get_symble_node_by_name(arg0.value.str_value);
        if(snode.get_block_name().length()){
            if(!var_ctrl.is_tmp_var(a)){
                // a=snode.get_block_name()+a;
            }
            
        }
        else{
            cout<<"error:prefix.length==0"<<endl;
            exit(-1);
        }
        type0=snode.data_type;
    }
    else{
        Utils::Exit_normal::pt_error_source_code_lan(op+"表达式错误，类型错误", arg0.coor);
    }
    if(arg1.type==tree_node_t::STATIC_VALUE){
        b=to_string(arg1.value.int_value);
        type1=arg1.value.symble_node_value->data_type;
    }
    else if(arg1.type==tree_node_t::VAR){
        b=arg1.value.str_value;
        if(!symble_controller.count_symble_node_by_name(arg1.value.str_value)){
            Utils::Exit_normal::pt_error_source_code_lan(string("未定义的关键字:")+arg1.value.str_value, arg0.coor);
        }
        Symble_node_t snode=symble_controller.get_symble_node_by_name(arg1.value.str_value);
        if(snode.get_block_name().length()){
            if(!var_ctrl.is_tmp_var(b)){
                //  b=snode.get_block_name()+b;
            }
        }
        else{
            cout<<"error:prefix.length==0"<<endl;
            exit(-1);
        }
        type1=snode.data_type;
    }
    else{
        Utils::Exit_normal::pt_error_source_code_lan(op+"表达式错误，类型错误", arg0.coor);
    }
    
    if(type0.get_type_sign() != type1.get_type_sign()){
        Utils::Exit_normal::pt_error_source_code_lan(op+string("：类型不匹配:")+type0.get_type_sign()+" 与 "+type1.get_type_sign(), arg0.coor);
    }
    else if(type0.is_struct()){
        Utils::Exit_normal::pt_error_source_code_lan(string("逻辑运算符不能作用于结构体"), ret_word.coor);
    }
    string new_code=new_var+" = "+ a + " "+op+" "+b;
    ret_word.code=arg0.code+arg1.code+new_code+"\n";
    ret_word.value.str_value=new_var;
    ret_word.value.symble_node_value = new Symble_node_t(new_var,VAR_SYMBLE,arg0.coor.y,arg0.coor.x);
    ret_word.value.symble_node_value->data_type=type0;
    Symble_node_t symble_node(new_var,VAR_SYMBLE,ret_word.coor.y,ret_word.coor.x);
    symble_node.data_type=type0;
    symble_node.set_block_name(symble_controller.top().get_name());
    symble_controller.push_symble(symble_node);
    
//    cout<<"算数：&&运算："<<new_code<<endl;
    return ret_word;
}
// <逻辑或表达式>-><逻辑或表达式>'||'<逻辑且表达式>
tree_node_t guid_func_logic_or(guid_func_args_t args){
    production_t prod = get_prod_by_idx(args.get_pord_idx());
    Symble_block_stack_t &symble_controller = *Symble_block_stack_t::get_shared_impl();
    tree_node_t arg0=args.get_arg(0);
    tree_node_t arg1=args.get_arg(2);
    tree_node_t ret_word;
    string op("||");
    ret_word.coor=arg0.coor;
    ret_word.name=echo_num2str(prod.left);
    //    if(arg0.type==tree_node_t::STATIC_VALUE && arg1.type==tree_node_t::STATIC_VALUE){
    //        ret_word.type=arg0.type;
    //    }
    //    else{
    //        ret_word.type=tree_node_t::VAR;
    //    }
    ret_word.type=tree_node_t::VAR;
    ret_word.set_left_able(false);
    
    Tmp_var_ctrl &var_ctrl = *Tmp_var_ctrl::get_shared_impl();
    string new_var=var_ctrl.alloc_tmp_var();
    string a,b;
    data_type_t type0;
    data_type_t type1;
    if(arg0.type==tree_node_t::STATIC_VALUE){
        a=to_string(arg0.value.int_value);
        type0=arg0.value.symble_node_value->data_type;
    }
    else if(arg0.type==tree_node_t::VAR){
        a=arg0.value.str_value;
        if(!symble_controller.count_symble_node_by_name(arg0.value.str_value)){
            Utils::Exit_normal::pt_error_source_code_lan(string("未定义的关键字:")+arg0.value.str_value, arg0.coor);
        }
        Symble_node_t snode=symble_controller.get_symble_node_by_name(arg0.value.str_value);
        if(snode.get_block_name().length()){
            if(!var_ctrl.is_tmp_var(a)){
                // a=snode.get_block_name()+a;
            }
            
        }
        else{
            cout<<"error:prefix.length==0"<<endl;
            exit(-1);
        }
        type0=snode.data_type;
    }
    else{
        Utils::Exit_normal::pt_error_source_code_lan(op+"表达式错误，类型错误", arg0.coor);
    }
    if(arg1.type==tree_node_t::STATIC_VALUE){
        b=to_string(arg1.value.int_value);
        type1=arg1.value.symble_node_value->data_type;
    }
    else if(arg1.type==tree_node_t::VAR){
        b=arg1.value.str_value;
        if(!symble_controller.count_symble_node_by_name(arg1.value.str_value)){
            Utils::Exit_normal::pt_error_source_code_lan(string("未定义的关键字:")+arg1.value.str_value, arg0.coor);
        }
        Symble_node_t snode=symble_controller.get_symble_node_by_name(arg1.value.str_value);
        if(snode.get_block_name().length()){
            if(!var_ctrl.is_tmp_var(b)){
                //  b=snode.get_block_name()+b;
            }
        }
        else{
            cout<<"error:prefix.length==0"<<endl;
            exit(-1);
        }
        type1=snode.data_type;
    }
    else{
        Utils::Exit_normal::pt_error_source_code_lan(op+"表达式错误，类型错误", arg0.coor);
    }
    
    if(type0.get_type_sign() != type1.get_type_sign()){
        Utils::Exit_normal::pt_error_source_code_lan(op+string("：类型不匹配:")+type0.get_type_sign()+" 与 "+type1.get_type_sign(), arg0.coor);
    }
    else if(type0.is_struct()){
        Utils::Exit_normal::pt_error_source_code_lan(string("逻辑运算符不能作用于结构体"), ret_word.coor);
    }
    string new_code=new_var+" = "+ a + " "+op+" "+b;
    ret_word.code=arg0.code+arg1.code+new_code+"\n";
    ret_word.value.str_value=new_var;
    ret_word.value.symble_node_value = new Symble_node_t(new_var,VAR_SYMBLE,arg0.coor.y,arg0.coor.x);
    ret_word.value.symble_node_value->data_type=type0;
    Symble_node_t symble_node(new_var,VAR_SYMBLE,ret_word.coor.y,ret_word.coor.x);
    symble_node.data_type=type0;
    symble_node.set_block_name(symble_controller.top().get_name());
    symble_controller.push_symble(symble_node);
    
    //    cout<<"算数：&&运算："<<new_code<<endl;
    return ret_word;
}
//061 <下标表达式>-><下标表达式>'['<值>']'
tree_node_t guid_func_061(guid_func_args_t args){
    production_t prod = get_prod_by_idx(args.get_pord_idx());
    Symble_block_stack_t &symble_controller = *Symble_block_stack_t::get_shared_impl();
    tree_node_t arg0=args.get_arg(0);
    tree_node_t arg1=args.get_arg(2);
    tree_node_t ret_word;
    /*
     enum type_t{
     UNDEFINED,
     VAR,
     ADDRESS_SIGN,
     CODE_BLOCK,
     STATIC_VALUE;
     };
     coor_t coor;
     value_t value;
     string code;
     type_t type=UNDEFINED;
     string name;
     */

    ret_word.coor=arg0.coor;
    ret_word.name=echo_num2str(prod.left);
    //    if(arg0.type==tree_node_t::STATIC_VALUE && arg1.type==tree_node_t::STATIC_VALUE){
    //        ret_word.type=arg0.type;
    //    }
    //    else{
    //        ret_word.type=tree_node_t::VAR;
    //    }
    ret_word.type=tree_node_t::VAR;
    ret_word.set_left_able(true);
    Tmp_var_ctrl &var_ctrl = *Tmp_var_ctrl::get_shared_impl();
    string a,b;
    data_type_t type0;
    data_type_t type1;
    if(arg0.type==tree_node_t::STATIC_VALUE){
        a=to_string(arg0.value.int_value);
        type0=arg0.value.symble_node_value->data_type;
    }
    else if(arg0.type==tree_node_t::VAR){
        a=arg0.value.str_value;
        if(!symble_controller.count_symble_node_by_name(arg0.value.str_value)){
            Utils::Exit_normal::pt_error_source_code_lan(string("未定义的关键字:")+arg0.value.str_value, arg0.coor);
        }
        Symble_node_t snode=symble_controller.get_symble_node_by_name(arg0.value.str_value);
        if(snode.get_block_name().length())
        {
            if(!var_ctrl.is_tmp_var(a)){
                //  a=snode.get_block_name()+a;
            }
        }
        
        else{
            cout<<"error:prefix.length==0"<<endl;
            exit(-1);
        }
        type0=snode.data_type;
    }
    if(arg1.type==tree_node_t::STATIC_VALUE){
        b=to_string(arg1.value.int_value);
        type1=arg1.value.symble_node_value->data_type;
    }
    else if(arg1.type==tree_node_t::VAR){
        b=arg1.value.str_value;
        if(!symble_controller.count_symble_node_by_name(arg1.value.str_value)){
            Utils::Exit_normal::pt_error_source_code_lan(string("未定义的关键字:")+arg1.value.str_value, arg0.coor);
        }
        Symble_node_t snode=symble_controller.get_symble_node_by_name(arg1.value.str_value);
        if(snode.get_block_name().length())
        {
            if(!var_ctrl.is_tmp_var(b)){
                //  b=snode.get_block_name()+b;
            }
        }
        else{
            cout<<"error:prefix.length==0"<<endl;
            exit(-1);
        }
        type1=snode.data_type;
    }
    data_type_t int_type;
    string new_code="";
    int_type.push(new primary_type_int());
    //在这里进行下标运算的类型判断，跟加法一样，相当于把a[b]等价于了a+b
    if((type0.is_ptr_normal() && type1.get_type_sign() == int_type.get_type_sign())||(type0.get_type_sign()==int_type.get_type_sign() && type1.is_ptr_normal()))
    {    //指针的默认类型转换
        if(type0.is_ptr_normal()){
            //a为指针，给b乘上单元大小再加到a上
            string offset_tmp_var = var_ctrl.alloc_tmp_var();
            int offset =type0.get_ptr_offset();
            new_code=new_code
            +offset_tmp_var+" = "+b+" * "+to_string(offset)+"\n";
            b=offset_tmp_var;//a为指针，b为偏移
            //注册offset_tmp_var
            Symble_node_t symble_node(offset_tmp_var,VAR_SYMBLE,ret_word.coor.y,ret_word.coor.x);
            symble_node.data_type=int_type;
            symble_node.set_block_name(symble_controller.top().get_name());
            symble_controller.push_symble(symble_node);
            
        }
        else{   //type1.is_ptr_normal();
            //b为指针，给a乘上单元大小再加到b上
            string offset_tmp_var = var_ctrl.alloc_tmp_var();
            new_code=new_code
            +offset_tmp_var+" = "+a+" * "+to_string(type1.get_ptr_offset())+"\n";
            a=offset_tmp_var;//b为指针，a为偏移
            //注册offset_tmp_var
            Symble_node_t symble_node(offset_tmp_var,VAR_SYMBLE,ret_word.coor.y,ret_word.coor.x);
            symble_node.data_type=int_type;
            symble_node.set_block_name(symble_controller.top().get_name());
            symble_controller.push_symble(symble_node);
            type0=type1;
            
        }
    }
    else{
        Utils::Exit_normal::pt_error_source_code_lan("下标运算：类型错误", arg0.coor);
    }
    //    string new_code=new_var+" = "+a+" %__MEMORY_OF__ "+ to_string(type0.get_sizeof());
    
    
    string new_var=var_ctrl.alloc_tmp_var();
    new_code=new_code+new_var+" = "+ a + " + "+b+"\n";//这一句是计算p+offset
    //注册new_var
    {
        Symble_node_t symble_node(new_var,VAR_SYMBLE,ret_word.coor.y,ret_word.coor.x);
        symble_node.data_type=int_type;
        symble_node.set_block_name(symble_controller.top().get_name());
        symble_controller.push_symble(symble_node);
    }
    /*
     *  此时，new_var是保存p+offset的指针值。需要给new_var解引用
     */
    type0.pop_head();
    string new_var1=var_ctrl.alloc_tmp_var();
    new_code=new_code + new_var1+" = "+new_var+" %__MEMORY_OF__ "+ to_string(type0.get_sizeof())+"\n";
    
    ret_word.code=arg0.code+arg1.code+new_code+"\n";
    ret_word.value.str_value=new_var1;
    ret_word.value.symble_node_value = new Symble_node_t(new_var,VAR_SYMBLE,arg0.coor.y,arg0.coor.x);
    ret_word.value.symble_node_value->data_type=type0;
    
    //注册new_var1
    {
        Symble_node_t symble_node(new_var1,VAR_SYMBLE,ret_word.coor.y,ret_word.coor.x);
        symble_node.data_type=type0;
        symble_node.set_block_name(symble_controller.top().get_name());
        symble_controller.push_symble(symble_node);
    }
    
    ret_word.value.symble_node_value->data_type=type0;
    return ret_word;
}
//62 <元素>->[常量]
tree_node_t guid_func_62(guid_func_args_t args){
    production_t prod = get_prod_by_idx(args.get_pord_idx());
    Symble_block_stack_t &symble_controller = *Symble_block_stack_t::get_shared_impl();
    tree_node_t arg0=args.get_arg(0);
    //    tree_node_t arg1=args.get_arg(1);
    tree_node_t ret_word;
    /*
     enum type_t{
     UNDEFINED,
     VAR,
     ADDRESS_SIGN,
     CODE_BLOCK,
     STATIC_VALUE;
     };
     coor_t coor;
     value_t value;
     string code;
     type_t type=UNDEFINED;
     string name;
     */
    
    if(arg0.value.str_value.length()){
        Symble_node_t node = symble_controller.get_global_str_name(arg0.value.str_value);
        ret_word.coor=arg0.coor;
        ret_word.value=arg0.value;
        ret_word.code=arg0.code;
        ret_word.type=tree_node_t::VAR;
        ret_word.name=node.get_name();
        cout<<"识别到字符串："<<node.get_name()<<"="<< ret_word.value.str_value<<endl;
        /**
         *  字符串类型为 int*
         */
        ret_word.value.symble_node_value = new Symble_node_t("");
        data_type_t &data_type_ptr = ret_word.value.symble_node_value->data_type;
        data_type_ptr =data_type_t();
        data_type_ptr.push(new primary_type_pointer());
        data_type_ptr.push(new primary_type_int());
    }
    else{
        ret_word.coor=arg0.coor;
        ret_word.value=arg0.value;
        ret_word.code=arg0.code;
        ret_word.type=tree_node_t::STATIC_VALUE;
        ret_word.name=echo_num2str(prod.left);
        cout<<"识别到常量："<<ret_word.value.int_value<<endl;
        /**
         *  常数默认类型为 int
         */
        ret_word.value.symble_node_value = new Symble_node_t("");
        data_type_t &data_type_ptr = ret_word.value.symble_node_value->data_type;
        data_type_ptr =data_type_t();
        data_type_ptr.push(new primary_type_int());
    }
   
    return ret_word;
}

//61 <元素>->[关键字]
tree_node_t guid_func_61(guid_func_args_t args){
    production_t prod = get_prod_by_idx(args.get_pord_idx());
    Symble_block_stack_t &symble_controller = *Symble_block_stack_t::get_shared_impl();
    tree_node_t arg0=args.get_arg(0);
    //    tree_node_t arg1=args.get_arg(1);
    tree_node_t ret_word;
    /*
     enum type_t{
     UNDEFINED,
     VAR,
     ADDRESS_SIGN,
     CODE_BLOCK,
     STATIC_VALUE;
     };
     coor_t coor;
     value_t value;
     string code;
     type_t type=UNDEFINED;
     string name;
     */
    ret_word.coor=arg0.coor;
    ret_word.value=arg0.value;
    ret_word.code=arg0.code;
    ret_word.type=tree_node_t::VAR;
    ret_word.name=echo_num2str(prod.left);
    
    ret_word.set_left_able(true);
    if(ret_word.value.symble_node_value){
        cout<<"error: ret_word.value.symble_node_value!=NULL"<<endl;
        exit(-1);
    }
    ret_word.value.str_value=string(PREFIX_VAR)+ret_word.value.str_value;
    if(symble_controller.count_symble_node_by_true_name(ret_word.value.str_value)){
        ret_word.value.symble_node_value = new Symble_node_t(ret_word.value.str_value,VAR_SYMBLE,ret_word.coor.y,ret_word.coor.x);
        *ret_word.value.symble_node_value = symble_controller.get_symble_node_by_true_name(ret_word.value.str_value);
    }
    else{
        Utils::Exit_normal::pt_error_source_code_lan(string("关键字未定义:"+ret_word.value.str_value), ret_word.coor);
    }
    
    ret_word.value.str_value=ret_word.value.symble_node_value->get_name();
    return ret_word;
}
// <函数括号运算>-><函数括号运算>'('')'
tree_node_t guid_func_26(guid_func_args_t args){
    production_t prod = get_prod_by_idx(args.get_pord_idx());
    Symble_block_stack_t &symble_controller = *Symble_block_stack_t::get_shared_impl();
    tree_node_t arg0=args.get_arg(0);
    //    tree_node_t arg1=args.get_arg(1);
    tree_node_t ret_word;
    /*
     enum type_t{
     UNDEFINED,
     VAR,
     ADDRESS_SIGN,
     CODE_BLOCK,
     STATIC_VALUE;
     };
     coor_t coor;
     value_t value;
     string code;
     type_t type=UNDEFINED;
     string name;
     */
    
    ret_word.coor=arg0.coor;
    ret_word.value=arg0.value;
    ret_word.code=arg0.code;
    ret_word.type=tree_node_t::VAR;
    ret_word.name=echo_num2str(prod.left);
    ret_word.set_left_able(arg0.get_left_albe());
    if(symble_controller.top_count_symble_node_by_name(ret_word.value.str_value)){
        Utils::Exit_normal::pt_error_source_code_lan(string("标识符重复定义:")+ret_word.value.str_value,ret_word.coor);
    }
    cout<<"识别到函数括号："<<arg0.value.str_value<<endl;
    Symble_node_t &snode=*ret_word.value.symble_node_value;
    data_type_t &data_type=snode.data_type;
    data_type.push(new func_type_t());
    
    return ret_word;
}
// <函数括号运算>-><函数括号运算>'('<参数列表>')'
tree_node_t guid_func_27_1(guid_func_args_t args){
    production_t prod = get_prod_by_idx(args.get_pord_idx());
    Symble_block_stack_t &symble_controller = *Symble_block_stack_t::get_shared_impl();
    tree_node_t arg0=args.get_arg(0);
    tree_node_t arg1=args.get_arg(2);
    tree_node_t ret_word;
    /*
     enum type_t{
     UNDEFINED,
     VAR,
     ADDRESS_SIGN,
     CODE_BLOCK,
     STATIC_VALUE;
     };
     coor_t coor;
     value_t value;
     string code;
     type_t type=UNDEFINED;
     string name;
     */
    
    ret_word.coor=arg0.coor;
    ret_word.value=arg0.value;
    ret_word.code=arg0.code;
    ret_word.type=tree_node_t::VAR;
    ret_word.name=echo_num2str(prod.left);
    ret_word.set_left_able(arg0.get_left_albe());
    if(symble_controller.top_count_symble_node_by_name(ret_word.value.str_value)){
        Utils::Exit_normal::pt_error_source_code_lan(string("标识符重复定义:")+ret_word.value.str_value,ret_word.coor);
    }
    Symble_node_t &snode=*ret_word.value.symble_node_value;
    data_type_t &data_type=snode.data_type;
    func_type_t *ptr = static_cast<func_type_t*>(arg1.value.symble_node_value->data_type.top()->copy());
    data_type.push(ptr);
    
    return ret_word;
}
// copy
tree_node_t guid_func_copy(guid_func_args_t args){
    production_t prod = get_prod_by_idx(args.get_pord_idx());
    Symble_block_stack_t &symble_controller = *Symble_block_stack_t::get_shared_impl();
    tree_node_t arg0=args.get_arg(0);
    //    tree_node_t arg1=args.get_arg(1);
    tree_node_t ret_word;
    /*
     enum type_t{
     UNDEFINED,
     VAR,
     ADDRESS_SIGN,
     CODE_BLOCK,
     STATIC_VALUE;
     };
     coor_t coor;
     value_t value;
     string code;
     type_t type=UNDEFINED;
     string name;
     */
    ret_word.coor=arg0.coor;
    ret_word.value=arg0.value;
    ret_word.code=arg0.code;
    ret_word.type=arg0.type;
    ret_word.name=echo_num2str(prod.left);
    ret_word.set_left_able(arg0.get_left_albe());
    return ret_word;
}
// copy1
tree_node_t guid_func_copy1(guid_func_args_t args){
    production_t prod = get_prod_by_idx(args.get_pord_idx());
    Symble_block_stack_t &symble_controller = *Symble_block_stack_t::get_shared_impl();
    tree_node_t arg0=args.get_arg(1);
    //    tree_node_t arg1=args.get_arg(1);
    tree_node_t ret_word;
    /*
     enum type_t{
     UNDEFINED,
     VAR,
     ADDRESS_SIGN,
     CODE_BLOCK,
     STATIC_VALUE;
     };
     coor_t coor;
     value_t value;
     string code;
     type_t type=UNDEFINED;
     string name;
     */
    ret_word.coor=arg0.coor;
    ret_word.value=arg0.value;
    ret_word.code=arg0.code;
    ret_word.type=arg0.type;
    ret_word.name=echo_num2str(prod.left);
    ret_word.set_left_able(arg0.get_left_albe());
    Symble_block_t top=symble_controller.top();
    return ret_word;
}
tree_node_t guid_func_copy_var(guid_func_args_t args){
    production_t prod = get_prod_by_idx(args.get_pord_idx());
    Symble_block_stack_t &symble_controller = *Symble_block_stack_t::get_shared_impl();
    tree_node_t arg0=args.get_arg(0);
    //    tree_node_t arg1=args.get_arg(1);
    tree_node_t ret_word;
    /*
     enum type_t{
     UNDEFINED,
     VAR,
     ADDRESS_SIGN,
     CODE_BLOCK,
     STATIC_VALUE;
     };
     coor_t coor;
     value_t value;
     string code;
     type_t type=UNDEFINED;
     string name;
     */
    ret_word.coor=arg0.coor;
    ret_word.value=arg0.value;
    ret_word.code=arg0.code;
    ret_word.type=tree_node_t::VAR;
    ret_word.name=echo_num2str(prod.left);
    ret_word.set_left_able(arg0.get_left_albe());
    if(!symble_controller.count_symble_node_by_name(ret_word.value.str_value)){
        Utils::Exit_normal::pt_error_source_code_lan(string("unknowned data type:")+ret_word.value.str_value, ret_word.coor);
    }
    return ret_word;
}
// <函数括号表达式>-><函数括号表达式>'('')'
tree_node_t guid_func_58(guid_func_args_t args){
    production_t prod = get_prod_by_idx(args.get_pord_idx());
    Symble_block_stack_t &symble_controller = *Symble_block_stack_t::get_shared_impl();
    tree_node_t arg0=args.get_arg(0);
    //    tree_node_t arg1=args.get_arg(1);
    tree_node_t ret_word;
    /*
     enum type_t{
     UNDEFINED,
     VAR,
     ADDRESS_SIGN,
     CODE_BLOCK,
     STATIC_VALUE;
     };
     coor_t coor;
     value_t value;
     string code;
     type_t type=UNDEFINED;
     string name;
     */
    ret_word.coor=arg0.coor;
    ret_word.value=arg0.value;
    ret_word.code=arg0.code;
    ret_word.type=arg0.type;
    ret_word.name=echo_num2str(prod.left);
    ret_word.set_left_able(false);
    
    ret_word.type=tree_node_t::VAR;

    string a,b;
    if(arg0.type!=tree_node_t::VAR){
        Utils::Exit_normal::pt_error_source_code_lan("函数调用语法错误", arg0.coor);
    }
    Symble_node_t snode("");
    if(symble_controller.count_symble_node_by_name(arg0.value.str_value)){
        snode=symble_controller.get_symble_node_by_name(arg0.value.str_value);
    }
    else{
        Utils::Exit_normal::pt_error_source_code_lan(string("变量不存在:")+arg0.value.str_value, arg0.coor);
    }
    cout<<snode.data_type.get_type_sign()<<endl;
    if(!snode.data_type.is_func_ptr()){
        Utils::Exit_normal::pt_error_source_code_lan(string("非法的函数调用：")+arg0.value.str_value, arg0.coor);
    }
    
    Tmp_var_ctrl &var_ctrl = *Tmp_var_ctrl::get_shared_impl();
    string new_var=var_ctrl.alloc_tmp_var();
    Symble_node_t symble_node(new_var,VAR_SYMBLE,ret_word.coor.y,ret_word.coor.x);
    symble_node.set_block_name(symble_controller.top().get_name());
    data_type_t new_dt;
    new_dt.push(snode.data_type.top()->copy());
    symble_node.data_type=new_dt;
    symble_controller.push_symble(symble_node);
    
    
    string new_code="call ";
    new_code+=arg0.value.str_value;
    ret_word.code=arg0.code+new_code+"\n";
    ret_word.code+=new_var + " = %__FUNC_RET__\n";
    ret_word.value.str_value=new_var;
    ret_word.value.symble_node_value = symble_node.copy();
    cout<<"函数调用："<<new_code<<endl;
    
    
   // ret_word.code = ret_word.code+"call "+ret_word.name+"\n";
    return ret_word;
    
    
    
    
}
// <函数括号表达式>-><函数括号表达式>'('<实参列表>')'
tree_node_t guid_func_58_1(guid_func_args_t args){
    production_t prod = get_prod_by_idx(args.get_pord_idx());
    Symble_block_stack_t &symble_controller = *Symble_block_stack_t::get_shared_impl();
    tree_node_t arg0=args.get_arg(0);
    tree_node_t arg1=args.get_arg(2);
    tree_node_t ret_word;
    /*
     enum type_t{
     UNDEFINED,
     VAR,
     ADDRESS_SIGN,
     CODE_BLOCK,
     STATIC_VALUE;
     };
     coor_t coor;
     value_t value;
     string code;
     type_t type=UNDEFINED;
     string name;
     */
    ret_word.coor=arg0.coor;
    ret_word.value=arg0.value;
    ret_word.code=arg0.code+arg1.code;
    ret_word.type=arg0.type;
    ret_word.name=echo_num2str(prod.left);
    ret_word.set_left_able(false);
    
    ret_word.type=tree_node_t::VAR;
    
    string a,b;
    if(arg0.type!=tree_node_t::VAR){
        Utils::Exit_normal::pt_error_source_code_lan("函数调用语法错误", arg0.coor);
    }
    Symble_node_t snode("");
    if(symble_controller.count_symble_node_by_name(arg0.value.str_value)){
        snode=symble_controller.get_symble_node_by_name(arg0.value.str_value);
    }
    else{
        Utils::Exit_normal::pt_error_source_code_lan(string("变量不存在:")+arg0.value.str_value, arg0.coor);
    }
    cout<<snode.data_type.get_type_sign()<<endl;
    if(!snode.data_type.is_func_ptr()){
        Utils::Exit_normal::pt_error_source_code_lan(string("非法的函数调用：")+arg0.value.str_value, arg0.coor);
    }
    
    Tmp_var_ctrl &var_ctrl = *Tmp_var_ctrl::get_shared_impl();
    string new_var=var_ctrl.alloc_tmp_var();
    Symble_node_t symble_node(new_var,VAR_SYMBLE,ret_word.coor.y,ret_word.coor.x);
    symble_node.set_block_name(symble_controller.top().get_name());
    data_type_t new_dt;
    new_dt.push(snode.data_type.top()->copy());
    symble_node.data_type=new_dt;
    symble_controller.push_symble(symble_node);
    
    
    string new_code="call ";
    new_code+=arg0.value.str_value;
    ret_word.code+=new_code+"\n";
    ret_word.code+=new_var + " = %__FUNC_RET__\n";
    ret_word.value.str_value=new_var;
    ret_word.value.symble_node_value = symble_node.copy();
    cout<<"函数调用："<<new_code<<endl;
    
    
    // ret_word.code = ret_word.code+"call "+ret_word.name+"\n";
    return ret_word;
    
    
    
    
}
//42 <返回语句>->'return'<值>
tree_node_t guid_func_42(guid_func_args_t args){
    production_t prod = get_prod_by_idx(args.get_pord_idx());
    Symble_block_stack_t &symble_controller = *Symble_block_stack_t::get_shared_impl();
    tree_node_t arg0=args.get_arg(1);
    //    tree_node_t arg1=args.get_arg(1);
    tree_node_t ret_word;
    /*
     enum type_t{
     UNDEFINED,
     VAR,
     ADDRESS_SIGN,
     CODE_BLOCK,
     STATIC_VALUE;
     };
     coor_t coor;
     value_t value;
     string code;
     type_t type=UNDEFINED;
     string name;
     */
    ret_word.coor=arg0.coor;
    ret_word.value=arg0.value;
    ret_word.code=arg0.code;
    ret_word.type=tree_node_t::CODE_BLOCK;
    ret_word.name=echo_num2str(prod.left);
    ret_word.set_left_able(false);
    string ret_code="";
    string ret_str;
    if(ret_word.value.str_value.length()){
        ret_str=ret_word.value.str_value;
        if(symble_controller.count_symble_node_by_true_name(ret_str)){
            Symble_node_t snode_ret = symble_controller.get_symble_node_by_true_name(ret_str);
            data_type_t int_type;
            int_type.push(new primary_type_int());
            if(!(int_type==snode_ret.data_type)){
                Utils::Exit_normal::pt_error_source_code_lan(string("返回类型错误:")+ret_str+" 类型："+snode_ret.data_type.get_type_sign()+" 需要返回的类型："+int_type.get_type_sign(), ret_word.coor);
            }
        }
    }
    else {
        //int 类型，无需校验
        ret_str=to_string(ret_word.value.int_value);
    }
    string free_code = "";
    stack_random<string>tmp_lst = symble_controller.top().get_tmp_var_free_able();
    if(tmp_lst.size()){
        free_code="::free_tmp_var ";
        for(int i=0;i<tmp_lst.size();i++){
            free_code+= tmp_lst.get_by_idx(i)+" ";
        }
        free_code+="\n";
    }
    symble_controller.top().free_tmp_var_all();
    if(symble_controller.return_enable()){
        ret_code=ret_code
            +"%__FUNC_RET__ = "+ret_str+"\n"
            +free_code
            +"goto "+symble_controller.get_return_sign()+"\n"
            ;
    }
    else{
        Utils::Exit_normal::pt_error_source_code_lan(string("return只能在函数内使用"), arg0.coor);
    }
    ret_word.code+=ret_code;
    return ret_word;
}

/**
 while_continue:
 if condition ::free_code yes: goto then
 goto whlie_break;
 then:
 ...
 goto while_continue;
 while_break:
 
 */
//44 <循环头部>->'while''('<值>')'
tree_node_t guid_func_while_head(guid_func_args_t args){
    production_t prod = get_prod_by_idx(args.get_pord_idx());
    Symble_block_stack_t &symble_controller = *Symble_block_stack_t::get_shared_impl();
    tree_node_t arg0=args.get_arg(2);
    //    tree_node_t arg1=args.get_arg(4);
    tree_node_t ret_word;
    /*
     enum type_t{
     UNDEFINED,
     VAR,
     ADDRESS_SIGN,
     CODE_BLOCK,
     STATIC_VALUE;
     };
     coor_t coor;
     value_t value;
     string code;
     type_t type=UNDEFINED;
     string name;
     */
    ret_word.coor=arg0.coor;
    ret_word.name=echo_num2str(prod.left);
    ret_word.set_left_able(false);
    //    if(arg0.type==tree_node_t::STATIC_VALUE && arg1.type==tree_node_t::STATIC_VALUE){
    //        ret_word.type=arg0.type;
    //    }
    //    else{
    //        ret_word.type=tree_node_t::VAR;
    //    }
    ret_word.type=arg0.type;
    string a;
    data_type_t type0;
    ret_word.value=arg0.value;
//    if(arg0.type==tree_node_t::STATIC_VALUE){
//        type0=arg0.value.symble_node_value->data_type;
//    }
//    else if(arg0.type==tree_node_t::VAR){
//        if(!symble_controller.count_symble_node_by_name(arg0.value.str_value)){
//            Utils::Exit_normal::pt_error_source_code_lan(string("未定义的关键字:")+arg0.value.str_value, arg0.coor);
//        }
//        Symble_node_t snode=symble_controller.get_symble_node_by_name(arg0.value.str_value);
//        if(snode.get_block_name().length()){
//            
//        }
//        //            a=snode.get_block_name()+a;
//        else{
//            cout<<"error:prefix.length==0"<<endl;
//            exit(-1);
//        }
//        type0=snode.data_type;
//    }
    ret_word.code=arg0.code;
    symble_controller.push_loop();
    
    return ret_word;
}

// <循环语句>-><循环头部><程序块>;
tree_node_t guid_func_while(guid_func_args_t args){
    production_t prod = get_prod_by_idx(args.get_pord_idx());
    Symble_block_stack_t &symble_controller = *Symble_block_stack_t::get_shared_impl();
    tree_node_t arg0=args.get_arg(0);
    tree_node_t arg1=args.get_arg(1);
    tree_node_t ret_word;
    /*
     enum type_t{
     UNDEFINED,
     VAR,
     ADDRESS_SIGN,
     CODE_BLOCK,
     STATIC_VALUE;
     };
     coor_t coor;
     value_t value;
     string code;
     type_t type=UNDEFINED;
     string name;
     */
    ret_word.set_left_able(false);
    ret_word.coor=arg0.coor;
    ret_word.name=echo_num2str(prod.left);
    //    if(arg0.type==tree_node_t::STATIC_VALUE && arg1.type==tree_node_t::STATIC_VALUE){
    //        ret_word.type=arg0.type;
    //    }
    //    else{
    //        ret_word.type=tree_node_t::VAR;
    //    }
    ret_word.type=arg0.type;
    string a;
    data_type_t type0;
    ret_word.value=arg0.value;
    if(arg0.type==tree_node_t::STATIC_VALUE){
    
        type0=arg0.value.symble_node_value->data_type;
    }
    else if(arg0.type==tree_node_t::VAR){
        if(!symble_controller.count_symble_node_by_name(arg0.value.str_value)){
            Utils::Exit_normal::pt_error_source_code_lan(string("未定义的关键字:")+arg0.value.str_value, arg0.coor);
        }
        Symble_node_t snode=symble_controller.get_symble_node_by_name(arg0.value.str_value);
        if(snode.get_block_name().length()){
            
        }
//            a=snode.get_block_name()+a;
        else{
            cout<<"error:prefix.length==0"<<endl;
            exit(-1);
        }
        type0=snode.data_type;
    }

    
    string new_code="";
    /**
         while_continue:
         code_of_condition
        if condition ::free_code yes: goto while_then
         goto whlie_break;
         while_then:
         ...
         goto while_continue;
         while_break:
     */
    string condition;
    if(arg0.type == tree_node_t::VAR){
        condition=arg0.value.str_value;
    }
    else if(arg0.type == tree_node_t::STATIC_VALUE){
        condition=to_string(arg0.value.int_value);
    }
    string free_code = "";
    stack_random<string>tmp_lst = symble_controller.top().get_tmp_var_free_able();
    if(tmp_lst.size()){
        free_code="::free_tmp_var ";
        for(int i=0;i<tmp_lst.size();i++){
            free_code+= tmp_lst.get_by_idx(i)+" ";
        }
        free_code+="\n";
    }
    symble_controller.top().free_tmp_var_all();
    
    new_code=symble_controller.get_continue_sign()+":\n"
            +arg0.code+"if "+condition+"\n"
            +free_code+
            +"goto "+symble_controller.get_while_then_sign()+"\n"
            ;
    new_code+="goto "+symble_controller.get_break_sign()+"\n";
    new_code+=symble_controller.get_while_then_sign()+":\n";
    new_code+=arg1.code;
    new_code+="goto "+symble_controller.get_continue_sign()+"\n";
    new_code+=symble_controller.get_break_sign()+":\n";
    ret_word.code=new_code;
    symble_controller.pop_loop();
   // ret_word.code+="//while end\n";
    return ret_word;
}
// <if头部>->'if''('<值>')'
tree_node_t guid_func_if_head(guid_func_args_t args){
    production_t prod = get_prod_by_idx(args.get_pord_idx());
    Symble_block_stack_t &symble_controller = *Symble_block_stack_t::get_shared_impl();
    tree_node_t arg0=args.get_arg(2);
    
    //    tree_node_t arg1=args.get_arg(4);
    tree_node_t ret_word;
    ret_word.coor=arg0.coor;
    ret_word.name=echo_num2str(prod.left);
    ret_word.set_left_able(false);
    ret_word.type=arg0.type;
    string a;
    data_type_t type0;
    ret_word.value=arg0.value;
    
    ret_word.code=arg0.code;
    symble_controller.push_if();
    
    return ret_word;
}
// 46 <if语句>-><if头部><程序块>'else'<语句>
tree_node_t guid_func_if_else(guid_func_args_t args){
    production_t prod = get_prod_by_idx(args.get_pord_idx());
    Symble_block_stack_t &symble_controller = *Symble_block_stack_t::get_shared_impl();
    tree_node_t arg0=args.get_arg(0);
    tree_node_t arg1=args.get_arg(1);
    tree_node_t arg2=args.get_arg(3);
    string then_code=arg1.code;
    string else_code=arg2.code;
    tree_node_t ret_word;
    ret_word.set_left_able(false);
    ret_word.coor=arg0.coor;
    ret_word.name=echo_num2str(prod.left);
    ret_word.type=arg0.type;
    data_type_t type0;
    string condition;
    ret_word.value=arg0.value;
    if(arg0.type==tree_node_t::STATIC_VALUE){
        type0=arg0.value.symble_node_value->data_type;
        condition=to_string(arg0.value.int_value);
        
    }
    else if(arg0.type==tree_node_t::VAR){
        condition=arg0.value.str_value;
        if(!symble_controller.count_symble_node_by_name(arg0.value.str_value)){
            Utils::Exit_normal::pt_error_source_code_lan(string("未定义的关键字:")+arg0.value.str_value, arg0.coor);
        }
        Symble_node_t snode=symble_controller.get_symble_node_by_name(arg0.value.str_value);
        if(snode.get_block_name().length()){
            
        }
        //            a=snode.get_block_name()+a;
        else{
            cout<<"error:prefix.length==0"<<endl;
            exit(-1);
        }
        type0=snode.data_type;
    }
    
    stack_random<string>tmp_lst = symble_controller.top().get_tmp_var_free_able();
    string free_code="";
    if(tmp_lst.size()){
        free_code="::free_tmp_var ";
        for(int i=0;i<tmp_lst.size();i++){
            free_code+= tmp_lst.get_by_idx(i)+" ";
        }
        free_code+="\n";
    }
    symble_controller.top().free_tmp_var_all();
    string new_code="";
    // no else
    //    if condition ? yes:goto then
    //        goto endif
    //        then:
    //        ...else code...
    //    endif:
    new_code=new_code
        +arg0.code
        +"if "+condition +"\n"
        +free_code
        +"goto "+symble_controller.get_then_sign()+"\n"
        +symble_controller.get_else_sign()+":\n"
        +else_code
        +"goto "+symble_controller.get_endif_sign()+"\n"
        +symble_controller.get_then_sign()+":\n"
        +then_code
        +symble_controller.get_endif_sign()+":\n"
        ;
    ret_word.code+=new_code;
    return ret_word;
}
// 47 <if语句>-><if头部><语句>
tree_node_t guid_func_47(guid_func_args_t args){
    production_t prod = get_prod_by_idx(args.get_pord_idx());
    Symble_block_stack_t &symble_controller = *Symble_block_stack_t::get_shared_impl();
    tree_node_t arg0=args.get_arg(0);
    tree_node_t arg1=args.get_arg(1);
    tree_node_t ret_word;
    /*
     enum type_t{
     UNDEFINED,
     VAR,
     ADDRESS_SIGN,
     CODE_BLOCK,
     STATIC_VALUE;
     };
     coor_t coor;
     value_t value;
     string code;
     type_t type=UNDEFINED;
     string name;
     */
    ret_word.set_left_able(false);
    ret_word.coor=arg0.coor;
    ret_word.name=echo_num2str(prod.left);
    //    if(arg0.type==tree_node_t::STATIC_VALUE && arg1.type==tree_node_t::STATIC_VALUE){
    //        ret_word.type=arg0.type;
    //    }
    //    else{
    //        ret_word.type=tree_node_t::VAR;
    //    }
    ret_word.type=arg0.type;
    data_type_t type0;
    string condition;
    ret_word.value=arg0.value;
    if(arg0.type==tree_node_t::STATIC_VALUE){
        type0=arg0.value.symble_node_value->data_type;
        condition=to_string(arg0.value.int_value);

    }
    else if(arg0.type==tree_node_t::VAR){
        condition=arg0.value.str_value;
        if(!symble_controller.count_symble_node_by_name(arg0.value.str_value)){
            Utils::Exit_normal::pt_error_source_code_lan(string("未定义的关键字:")+arg0.value.str_value, arg0.coor);
        }
        Symble_node_t snode=symble_controller.get_symble_node_by_name(arg0.value.str_value);
        if(snode.get_block_name().length()){
            
        }
        //            a=snode.get_block_name()+a;
        else{
            cout<<"error:prefix.length==0"<<endl;
            exit(-1);
        }
        type0=snode.data_type;
    }
    
    stack_random<string>tmp_lst = symble_controller.top().get_tmp_var_free_able();
    string free_code="";
    if(tmp_lst.size()){
        free_code="::free_tmp_var ";
        for(int i=0;i<tmp_lst.size();i++){
            free_code+= tmp_lst.get_by_idx(i)+" ";
        }
        free_code+="\n";
    }
    symble_controller.top().free_tmp_var_all();
    string new_code="";
    // no else
//    if condition ? yes:goto then
//        goto endif
//        then:
//        ...else code...
//    endif:
    new_code=new_code
            +arg0.code
            +"if "+condition +"\n"
            +free_code
            +"goto "+symble_controller.get_then_sign()+"\n"
            +"goto "+symble_controller.get_endif_sign()+"\n"
            +symble_controller.get_then_sign()+":\n"
            +arg1.code
            +symble_controller.get_endif_sign()+":\n"
            ;
    ret_word.code+=new_code;
    return ret_word;
}
// <语句>->'continue'';'
tree_node_t guid_func_continue(guid_func_args_t args){
    production_t prod = get_prod_by_idx(args.get_pord_idx());
    Symble_block_stack_t &symble_controller = *Symble_block_stack_t::get_shared_impl();
    tree_node_t arg0=args.get_arg(0);
    //    tree_node_t arg1=args.get_arg(1);
    tree_node_t ret_word;
    /*
     enum type_t{
     UNDEFINED,
     VAR,
     ADDRESS_SIGN,
     CODE_BLOCK,
     STATIC_VALUE;
     };
     coor_t coor;
     value_t value;
     string code;
     type_t type=UNDEFINED;
     string name;
     */
    ret_word.coor=arg0.coor;
    ret_word.value=arg0.value;
    ret_word.code=arg0.code;
    ret_word.type=arg0.type;
    ret_word.name=echo_num2str(prod.left);
    ret_word.set_left_able(arg0.get_left_albe());
    string free_code = "";
    stack_random<string>tmp_lst = symble_controller.top().get_tmp_var_free_able();
    if(tmp_lst.size()){
        free_code="::free_tmp_var ";
        for(int i=0;i<tmp_lst.size();i++){
            free_code+= tmp_lst.get_by_idx(i)+" ";
        }
        free_code+="\n";
    }
    symble_controller.top().free_tmp_var_all();
    if(symble_controller.continue_enable()){
        
        ret_word.code+=free_code+"goto "+symble_controller.get_continue_sign()+"\n";
    }
    else{
        Utils::Exit_normal::pt_error_source_code_lan(string("continue invalid"), ret_word.coor);
    }
    return ret_word;
}
// <语句>->'break'';'
tree_node_t guid_func_break(guid_func_args_t args){
    production_t prod = get_prod_by_idx(args.get_pord_idx());
    Symble_block_stack_t &symble_controller = *Symble_block_stack_t::get_shared_impl();
    tree_node_t arg0=args.get_arg(0);
    //    tree_node_t arg1=args.get_arg(1);
    tree_node_t ret_word;
    /*
     enum type_t{
     UNDEFINED,
     VAR,
     ADDRESS_SIGN,
     CODE_BLOCK,
     STATIC_VALUE;
     };
     coor_t coor;
     value_t value;
     string code;
     type_t type=UNDEFINED;
     string name;
     */
    ret_word.coor=arg0.coor;
    ret_word.value=arg0.value;
    ret_word.code=arg0.code;
    ret_word.type=arg0.type;
    ret_word.name=echo_num2str(prod.left);
    ret_word.set_left_able(arg0.get_left_albe());
    string free_code = "";
    stack_random<string>tmp_lst = symble_controller.top().get_tmp_var_free_able();
    if(tmp_lst.size()){
        free_code="::free_tmp_var ";
        for(int i=0;i<tmp_lst.size();i++){
            free_code+= tmp_lst.get_by_idx(i)+" ";
        }
        free_code+="\n";
    }
    symble_controller.top().free_tmp_var_all();
    if(symble_controller.break_enable()){
        
        ret_word.code+=free_code+"goto "+symble_controller.get_break_sign()+"\n";
    }
    else{
        Utils::Exit_normal::pt_error_source_code_lan(string("break invalid"), ret_word.coor);
    }
    return ret_word;
}
//50.1 <实参列表>-><值>
tree_node_t guid_func_50_1(guid_func_args_t args){
    production_t prod = get_prod_by_idx(args.get_pord_idx());
    Symble_block_stack_t &symble_controller = *Symble_block_stack_t::get_shared_impl();
    tree_node_t arg0=args.get_arg(0);
    //    tree_node_t arg1=args.get_arg(1);
    tree_node_t ret_word;
    /*
     enum type_t{
     UNDEFINED,
     VAR,
     ADDRESS_SIGN,
     CODE_BLOCK,
     STATIC_VALUE;
     };
     coor_t coor;
     value_t value;
     string code;
     type_t type=UNDEFINED;
     string name;
     */
    ret_word.coor=arg0.coor;
    ret_word.value=arg0.value;
    ret_word.code=arg0.code;
    ret_word.type=arg0.type;
    ret_word.name=echo_num2str(prod.left);
    ret_word.set_left_able(arg0.get_left_albe());
    string a;
    if(arg0.value.str_value.length()){
        a=arg0.value.str_value;
    }
    else{
        a=to_string(arg0.value.int_value);
    }
    string code;
    code=string("::set_func_arg ")+a+"\n";
    ret_word.code+=code;
    return ret_word;
}
//58 <解引用表达式>->'*'<解引用表达式>
tree_node_t guid_func_58x(guid_func_args_t args){
    production_t prod = get_prod_by_idx(args.get_pord_idx());
    Symble_block_stack_t &symble_controller = *Symble_block_stack_t::get_shared_impl();
    tree_node_t arg0=args.get_arg(1);
    tree_node_t ret_word;
    /*
     enum type_t{
     UNDEFINED,
     VAR,
     ADDRESS_SIGN,
     CODE_BLOCK,
     STATIC_VALUE;
     };
     coor_t coor;
     value_t value;
     string code;
     type_t type=UNDEFINED;
     string name;
     */
    ret_word.coor=arg0.coor;
    ret_word.name=echo_num2str(prod.left);
    //    if(arg0.type==tree_node_t::STATIC_VALUE && arg1.type==tree_node_t::STATIC_VALUE){
    //        ret_word.type=arg0.type;
    //    }
    //    else{
    //        ret_word.type=tree_node_t::VAR;
    //    }
    ret_word.type=tree_node_t::VAR;
    ret_word.set_left_able(true);
    Tmp_var_ctrl &var_ctrl = *Tmp_var_ctrl::get_shared_impl();
    string new_var=var_ctrl.alloc_tmp_var();
    string a;
    data_type_t type0;
    if(arg0.type==tree_node_t::STATIC_VALUE){
        Utils::Exit_normal::pt_error_source_code_lan("无法对常量解引用", arg0.coor);
    }
    else if(arg0.type==tree_node_t::VAR){
        a=arg0.value.str_value;
        if(!symble_controller.count_symble_node_by_name(arg0.value.str_value)){
            Utils::Exit_normal::pt_error_source_code_lan(string("未定义的关键字:")+arg0.value.str_value, arg0.coor);
        }
        Symble_node_t snode=symble_controller.get_symble_node_by_name(arg0.value.str_value);
        if(snode.get_block_name().length()){
            
        }
//            a=snode.get_block_name()+a;
        else{
            cout<<"error:prefix.length==0"<<endl;
            exit(-1);
        }
        type0=snode.data_type;
    }
    else{
        Utils::Exit_normal::pt_error_source_code_lan("解引用对象必须为指针变量", arg0.coor);
    }
    if(!type0.is_ptr_normal()){
        Utils::Exit_normal::pt_error_source_code_lan("只能对指针解引用", arg0.coor);
    }
//    general_data_type_t *top_type_ptr=type0.top();
//    type0.pop();
//    type0.pop();
//    type0.push(top_type_ptr);
    
    type0.pop_head();
    
    string new_code=new_var+" = "+a+" %__MEMORY_OF__ "+ to_string(type0.get_sizeof());
    ret_word.code=arg0.code+new_code+"\n";
    ret_word.value.str_value=new_var;
    ret_word.value.symble_node_value = new Symble_node_t(new_var,VAR_SYMBLE,arg0.coor.y,arg0.coor.x);
    ret_word.value.symble_node_value->data_type=type0;
    Symble_node_t symble_node(new_var,VAR_SYMBLE,ret_word.coor.y,ret_word.coor.x);
    symble_node.data_type=type0;
    symble_node.set_block_name(symble_controller.top().get_name());
    symble_controller.push_symble(symble_node);
    return ret_word;
}

//60 <取地址表达式>->'&'<取地址表达式>
tree_node_t guid_func_60(guid_func_args_t args){
    production_t prod = get_prod_by_idx(args.get_pord_idx());
    Symble_block_stack_t &symble_controller = *Symble_block_stack_t::get_shared_impl();
    tree_node_t arg0=args.get_arg(1);
    tree_node_t ret_word;
    /*
     enum type_t{
     UNDEFINED,
     VAR,
     ADDRESS_SIGN,
     CODE_BLOCK,
     STATIC_VALUE;
     };
     coor_t coor;
     value_t value;
     string code;
     type_t type=UNDEFINED;
     string name;
     */
    ret_word.coor=arg0.coor;
    ret_word.name=echo_num2str(prod.left);
    //    if(arg0.type==tree_node_t::STATIC_VALUE && arg1.type==tree_node_t::STATIC_VALUE){
    //        ret_word.type=arg0.type;
    //    }
    //    else{
    //        ret_word.type=tree_node_t::VAR;
    //    }
    ret_word.type=tree_node_t::VAR;
    ret_word.set_left_able(false);
    Tmp_var_ctrl &var_ctrl = *Tmp_var_ctrl::get_shared_impl();
    string new_var=var_ctrl.alloc_tmp_var();
    string a;
    data_type_t type0;
    if(arg0.type==tree_node_t::STATIC_VALUE){
        Utils::Exit_normal::pt_error_source_code_lan("无法对常量取地址", arg0.coor);
    }
    else if(arg0.type==tree_node_t::VAR){
        a=arg0.value.str_value;
        if(var_ctrl.is_tmp_var(a) && arg0.get_left_albe()==false){
            Utils::Exit_normal::pt_error_source_code_lan("取地址对象不能当做左值", arg0.coor);
        }
        if(!symble_controller.count_symble_node_by_name(arg0.value.str_value)){
            Utils::Exit_normal::pt_error_source_code_lan(string("未定义的关键字:")+arg0.value.str_value, arg0.coor);
        }
        Symble_node_t snode=symble_controller.get_symble_node_by_name(arg0.value.str_value);
        if(snode.get_block_name().length()){
            
        }
//            a=snode.get_block_name()+a;
        else{
            cout<<"error:prefix.length==0"<<endl;
            exit(-1);
        }
        type0=snode.data_type;
    }
//    general_data_type_t *top_type_ptr=type0.top();
//    type0.pop();
//    type0.push(new primary_type_pointer());
//    type0.push(top_type_ptr);

    type0.push_head(new primary_type_pointer);
    
    string new_code=new_var+" = %__ADDRESS_OF__ "+ a;
    ret_word.code=arg0.code+new_code+"\n";
    ret_word.value.str_value=new_var;
    ret_word.value.symble_node_value = new Symble_node_t(new_var,VAR_SYMBLE,arg0.coor.y,arg0.coor.x);
    ret_word.value.symble_node_value->data_type=type0;
    Symble_node_t symble_node(new_var,VAR_SYMBLE,ret_word.coor.y,ret_word.coor.x);
    symble_node.data_type=type0;
    symble_node.set_block_name(symble_controller.top().get_name());
    symble_controller.push_symble(symble_node);
    return ret_word;
}
// 70 <成员选择表达式>-><成员选择表达式>'.'[关键字]
tree_node_t guid_func_70(guid_func_args_t args){
    production_t prod = get_prod_by_idx(args.get_pord_idx());
    Symble_block_stack_t &symble_controller = *Symble_block_stack_t::get_shared_impl();
    tree_node_t arg0=args.get_arg(0);
    tree_node_t arg1=args.get_arg(2);
    tree_node_t ret_word;
    /*
     enum type_t{
     UNDEFINED,
     VAR,
     ADDRESS_SIGN,
     CODE_BLOCK,
     STATIC_VALUE;
     };
     coor_t coor;
     value_t value;
     string code;
     type_t type=UNDEFINED;
     string name;
     */
    ret_word.coor=arg0.coor;
    ret_word.name=echo_num2str(prod.left);
    //    if(arg0.type==tree_node_t::STATIC_VALUE && arg1.type==tree_node_t::STATIC_VALUE){
    //        ret_word.type=arg0.type;
    //    }
    //    else{
    //        ret_word.type=tree_node_t::VAR;
    //    }
    ret_word.type=tree_node_t::VAR;
    ret_word.set_left_able(true);
    Tmp_var_ctrl &var_ctrl = *Tmp_var_ctrl::get_shared_impl();
    string a,b;
    data_type_t type0;
    data_type_t type1;
    if(arg0.type==tree_node_t::VAR){
        a=arg0.value.str_value;
        if(!symble_controller.count_symble_node_by_name(arg0.value.str_value)){
            Utils::Exit_normal::pt_error_source_code_lan(string("未定义的关键字:")+arg0.value.str_value, arg0.coor);
        }
        Symble_node_t snode=symble_controller.get_symble_node_by_name(arg0.value.str_value);
        if(snode.get_block_name().length())
        {
            if(!var_ctrl.is_tmp_var(a)){
                //  a=snode.get_block_name()+a;
            }
        }
        
        else{
            cout<<"error:prefix.length==0"<<endl;
            exit(-1);
        }
        type0=snode.data_type;
    }
    else{
        cout<<"error:guid:"<<__LINE__<<endl;
        exit(-1);
    }
    
    data_type_t int_type;
    int_type.push(new primary_type_int());
    string new_code="";
    if(!type0.is_struct()){
        Utils::Exit_normal::pt_error_source_code_lan(string("struct is needed in the left of point operator:\ntype_sign:")
                                                     +type0.get_type_sign()+ " left:"+a+" right:"+b
                                                     , arg0.coor);
    }
    string var_name = PREFIX_VAR + arg1.value.str_value;
    
    struct_type_t *type_struct = static_cast<struct_type_t*>(type0.get_head());
    
    if(!type_struct->count_var_name(var_name)){
        Utils::Exit_normal::pt_error_source_code_lan(string("var name not found in struct:")+
                                                     type0.get_type_sign()+" impl:"+a+"."+var_name, arg1.coor);
    }
    type1 = type_struct->get_var_data_type_by_name(var_name);
    int offset = type_struct->get_var_offset_by_name(var_name);
    int var_size = type1.get_sizeof();
    /**
     *  struct_name(a) is the pointer that points the head of struct.  ptr+offset = address of var
     *  a is struct_name
     *  0.  T1 = %__ADDRESS_OF__ a
     *  1.  T1 = T1 + offset
     *  2.  T2 = T1 %__MEMORY_OF__ var_sizeof （Except arrays while array is: T2 = T1)
     *  3.  ans(ret_word) = T2
     */
    string tmp_var_address = var_ctrl.alloc_tmp_var();

    string tmp_left_value = var_ctrl.alloc_tmp_var();
    
    {
        new_code=new_code
                +tmp_var_address + " = "+ " %__ADDRESS_OF__ " +a+"\n"
                +tmp_var_address + " = "+ tmp_var_address +" + " + to_string(offset)+"\n";
        if(!type1.is_array()){
            new_code=new_code
                +tmp_left_value + " = " + tmp_var_address + " %__MEMORY_OF__ " + to_string(var_size)+"\n";
        }
        else{
            new_code=new_code
            +tmp_left_value + " = " +tmp_var_address+"\n";
        }
        
        
        {
            //regist tmp_var_address:
            Symble_node_t symble_node(tmp_var_address,VAR_SYMBLE,ret_word.coor.y,ret_word.coor.x);
            symble_node.data_type=int_type; // address
            symble_node.set_block_name(symble_controller.top().get_name());
            symble_controller.push_symble(symble_node);
        }
        {
            //regist tmp_left_value
            Symble_node_t symble_node(tmp_left_value,VAR_SYMBLE,ret_word.coor.y,ret_word.coor.x);
            symble_node.data_type=type_struct->get_var_data_type_by_name(var_name) ; // struct.var
            symble_node.set_block_name(symble_controller.top().get_name());
            symble_controller.push_symble(symble_node);
        }
        
    }
    ret_word.code=arg0.code+arg1.code+new_code;
    ret_word.value.str_value=tmp_left_value;
    ret_word.value.symble_node_value = new Symble_node_t(tmp_left_value,VAR_SYMBLE,arg0.coor.y,arg0.coor.x);
    ret_word.value.symble_node_value->data_type=type1;
    
    return ret_word;
}
void registe_primary_data_type(){
    Symble_block_stack_t &symble_controller = *Symble_block_stack_t::get_shared_impl();
    Symble_node_t symble_node("int",DATA_TYPE_SYMBLE,0,0);
    data_type_t data_type;
    data_type.push(new primary_type_int());
    symble_node.data_type=data_type;
    symble_controller.push_symble(symble_node);
}
void set_guid_func(){
    Guider &guider = *Guider::get_shared_impl();
    guider.resize(get_num_of_prod());
    
    guider.regist_guid_func_by_name("S'-><程序>"           ,guid_func_0);
    guider.regist_guid_func_by_name("<程序>-><外部声明>"    ,guid_func_1);
    guider.regist_guid_func_by_name("<程序>-><外部声明><程序>"          ,guid_func_2);
    guider.regist_guid_func_by_name("<外部声明>-><struct定义>';'"    ,guid_func_copy);
    guider.regist_guid_func_by_name("<外部声明>-><函数定义>"          ,guid_func_4);
    guider.regist_guid_func_by_name("<外部声明>-><变量声明或定义>';'"    , guid_func_5);
    guider.regist_guid_func_by_name("<struct定义>-><struct声明><程序块首部><变量声明或定义列表><程序块尾部>", guid_func_6);
    guider.regist_guid_func_by_name("<struct声明>-><struct数据类型>", guid_func_6_1);
    
    guider.regist_guid_func_by_name("<函数定义>-><函数定义头><程序块>"      , guid_func_7);
    guider.regist_guid_func_by_name("<变量声明或定义>-><广义数据类型><类型计算式>"   ,guid_func_8);
    guider.regist_guid_func_by_name("<程序块首部>->'{'"   ,guid_func_11);
    
    guider.regist_guid_func_by_name("<类型计算式>-><中括号运算>"   ,guid_func_20);
    guider.regist_guid_func_by_name("<中括号运算>-><函数括号运算>"   ,guid_func_22);

    guider.regist_guid_func_by_name("<函数括号运算>-><括号运算>"   ,guid_func_25);
    guider.regist_guid_func_by_name("<函数括号运算>-><函数括号运算>'('')'"   ,guid_func_26);
    guider.regist_guid_func_by_name("<括号运算>->[关键字]"   ,guid_func_27);
    
    guider.regist_guid_func_by_name("<程序块尾部>->'}'"   ,guid_func_12);
    guider.regist_guid_func_by_name("<程序块>-><程序块首部><程序块尾部>"   ,guid_func_10);
    
    
    
    guider.regist_guid_func_by_name("<元素>->[关键字]"   ,guid_func_61);
    guider.regist_guid_func_by_name("<括号表达式>-><元素>",guid_func_copy);
    guider.regist_guid_func_by_name("<函数括号表达式>-><括号表达式>",guid_func_copy);
    
    guider.regist_guid_func_by_name("<函数括号表达式>-><函数括号表达式>'('')'"   ,guid_func_58);
    
//    guider.regist_guid_func_by_name("<下标表达式>-><函数括号表达式>"   ,guid_func_copy);
//    guider.regist_guid_func_by_name("<取地址表达式>-><成员选择表达式>"   ,guid_func_copy);
    guider.regist_guid_func_by_name("<解引用表达式>-><成员选择表达式>"   ,guid_func_copy);
    /**
     *  表达式文法越靠下，就越接近变量。下标运算符要在取成员运算符下面。否则co[0].a就不成立，需要额外加括号为(co[0]).a
     *  上例同级的表达式应当定义到一行里（一个分号行内）; 如加减法需要在同一行里。[]和.可以设计到同一行里。
     */
    guider.regist_guid_func_by_name("<成员选择表达式>-><函数括号表达式>"   ,guid_func_copy);
    guider.regist_guid_func_by_name("<强制类型转换表达式>-><解引用表达式>"   ,guid_func_copy);
    guider.regist_guid_func_by_name("<加法表达式>-><强制类型转换表达式>"   ,guid_func_copy);
    guider.regist_guid_func_by_name("<比较表达式>-><加法表达式>"   ,guid_func_copy);
    guider.regist_guid_func_by_name("<逻辑且表达式>-><比较表达式>"   ,guid_func_copy);
    guider.regist_guid_func_by_name("<逻辑或表达式>-><逻辑且表达式>"   ,guid_func_copy);
    guider.regist_guid_func_by_name("<值>-><逻辑或表达式>"   ,guid_func_copy);
    guider.regist_guid_func_by_name("<语句>-><值>';'"   ,guid_func_34);
    guider.regist_guid_func_by_name("<语句列表>-><语句>"   ,guid_func_13);
    guider.regist_guid_func_by_name("<程序块>-><程序块首部><语句列表><程序块尾部>"   ,guid_func_9);
    guider.regist_guid_func_by_name("<语句>-><变量声明或定义>';'"   ,guid_func_copy);
    guider.regist_guid_func_by_name("<语句列表>-><语句列表><语句>"   ,guid_func_14);
    guider.regist_guid_func_by_name("<加法表达式>-><加法表达式>'+'<强制类型转换表达式>",guid_func_48);
    guider.regist_guid_func_by_name("<加法表达式>-><加法表达式>'-'<强制类型转换表达式>",guid_func_49);
    guider.regist_guid_func_by_name("<语句>-><程序块>"   ,guid_func_31);
    guider.regist_guid_func_by_name("<元素>->[常量]"   ,guid_func_62);
    guider.regist_guid_func_by_name("<值>-><值>'='<逻辑或表达式>"   ,guid_func_23);
    guider.regist_guid_func_by_name("<比较表达式>-><比较表达式>'<'<加法表达式>"   ,guid_func_50);
    guider.regist_guid_func_by_name("<循环头部>->'while''('<值>')'"   ,guid_func_while_head);
    guider.regist_guid_func_by_name("<循环语句>-><循环头部><程序块>"   ,guid_func_while);
    guider.regist_guid_func_by_name("<语句>-><循环语句>"   ,guid_func_copy);
    guider.regist_guid_func_by_name("<类型计算式>->'*'<类型计算式>"   ,guid_func_19);
    guider.regist_guid_func_by_name("<解引用表达式>->'&'<解引用表达式>"   ,guid_func_60);
    guider.regist_guid_func_by_name("<解引用表达式>->'*'<解引用表达式>"   ,guid_func_58x);
    guider.regist_guid_func_by_name("<返回语句>->'return'<值>"   ,guid_func_42);
    guider.regist_guid_func_by_name("<语句>-><返回语句>';'"   ,guid_func_copy);
    guider.regist_guid_func_by_name("<括号运算>->'('<类型计算式>')'"   ,guid_func_28);
    guider.regist_guid_func_by_name("<中括号运算>-><中括号运算>'['<值>']'"   ,guid_func_21);
    guider.regist_guid_func_by_name("<括号表达式>->'('<值>')'"   ,guid_func_copy1);
    guider.regist_guid_func_by_name("<成员选择表达式>-><成员选择表达式>'['<值>']'"   ,guid_func_061);
    guider.regist_guid_func_by_name("<广义数据类型>->[数据类型]"   ,guid_func_copy_var);
    guider.regist_guid_func_by_name("<广义数据类型>-><struct数据类型>"   ,guid_func_17);
    guider.regist_guid_func_by_name("<变量声明或定义列表>-><变量声明或定义>';'"   ,guid_func_copy);
    guider.regist_guid_func_by_name("<成员选择表达式>-><成员选择表达式>'.'[关键字]"   ,guid_func_70);
    guider.regist_guid_func_by_name("<struct数据类型>->'struct'[关键字]"   ,guid_func_copy1);
//    guider.regist_guid_func_by_name("<struct声明>-><广义数据类型>"   ,guid_func_copy);
    guider.regist_guid_func_by_name("<变量声明或定义列表>-><变量声明或定义>';'<变量声明或定义列表>"   ,guid_func_copy);
    guider.regist_guid_func_by_name("<函数声明>-><变量声明或定义>"   ,guid_func_7_1);
    guider.regist_guid_func_by_name("<参数列表>-><广义数据类型><类型计算式>"   ,guid_func_17_1);
    guider.regist_guid_func_by_name("<函数括号运算>-><函数括号运算>'('<参数列表>')'"   ,guid_func_27_1);
    guider.regist_guid_func_by_name("<实参列表>-><值>"   ,guid_func_50_1);
    guider.regist_guid_func_by_name("<函数括号表达式>-><函数括号表达式>'('<实参列表>')'"   ,guid_func_58_1);
    guider.regist_guid_func_by_name("<函数定义头>-><函数声明>"   ,guid_func_10_1);
    guider.regist_guid_func_by_name("<if语句>-><if头部><语句>"   ,guid_func_47);
    guider.regist_guid_func_by_name("<if头部>->'if''('<值>')'"   ,guid_func_if_head);
    guider.regist_guid_func_by_name("<语句>-><if语句>"   ,guid_func_copy);
    guider.regist_guid_func_by_name("<if语句>-><if头部><程序块>'else'<语句>"   ,guid_func_if_else);
    guider.regist_guid_func_by_name("<语句>->'continue'';'"   ,guid_func_continue);
    guider.regist_guid_func_by_name("<语句>->'break'';'"   ,guid_func_break);
    guider.regist_guid_func_by_name("<比较表达式>-><比较表达式>'=='<加法表达式>"   ,guid_func_equal_to);
    guider.regist_guid_func_by_name("<逻辑且表达式>-><逻辑且表达式>'&&'<比较表达式>"   ,guid_func_logic_and);
    guider.regist_guid_func_by_name("<逻辑或表达式>-><逻辑或表达式>'||'<逻辑且表达式>"   ,guid_func_logic_or);
    guider.regist_guid_func_by_name("S'-><程序>"   ,guid_func_0);
    guider.regist_guid_func_by_name("S'-><程序>"   ,guid_func_0);
    guider.regist_guid_func_by_name("S'-><程序>"   ,guid_func_0);
    guider.regist_guid_func_by_name("S'-><程序>"   ,guid_func_0);
    guider.regist_guid_func_by_name("S'-><程序>"   ,guid_func_0);
    guider.regist_guid_func_by_name("S'-><程序>"   ,guid_func_0);
    guider.regist_guid_func_by_name("S'-><程序>"   ,guid_func_0);
    guider.regist_guid_func_by_name("S'-><程序>"   ,guid_func_0);
    guider.regist_guid_func_by_name("S'-><程序>"   ,guid_func_0);
    guider.regist_guid_func_by_name("S'-><程序>"   ,guid_func_0);
    guider.regist_guid_func_by_name("S'-><程序>"   ,guid_func_0);
    guider.regist_guid_func_by_name("S'-><程序>"   ,guid_func_0);
    guider.regist_guid_func_by_name("S'-><程序>"   ,guid_func_0);
    guider.regist_guid_func_by_name("S'-><程序>"   ,guid_func_0);
    guider.regist_guid_func_by_name("S'-><程序>"   ,guid_func_0);
    guider.regist_guid_func_by_name("S'-><程序>"   ,guid_func_0);

}
