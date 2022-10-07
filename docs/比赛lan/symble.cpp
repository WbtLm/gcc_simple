//
//  symble.cpp
//  test
//
//  Created by dawn on 2019/12/7.
//  Copyright © 2019 chuyi. All rights reserved.
//

#include "symble.h"

Symble_node_t::Symble_node_t(string name){
    this->name=name;
    data_type_t undefined_type;
    data_type = undefined_type;
}
Symble_node_t::Symble_node_t(string _name,symble_type_t _type,int _line_pos,int _idx_pos){
    name=_name;
    type=_type;
    coor.y=_line_pos;
    coor.x=_idx_pos;
    switch(_type){
        case VAR_SYMBLE:
//            name="_var_"+name;
            break;
        case VALUE_SYMBLE:
//            name="_val_"+name;
            is_const=true;
            break;
        case DATA_TYPE_SYMBLE:
            cout<<"数据类型类型"<<endl;
//            name="_dtp_"+name;
            break;
        default:
            cout<<"未定义类型"<<endl;
            break;
    }
}
void Symble_block_t::init()
{
    snode_lst.clear();
    echo_name2idx.clear();
   // name=get_new_block_name();
}
/**
 * 前缀有
 * PREFIX_系列
 #define PREFIX_VAR "__var_"
 #define PREFIX_CODE_ADDRESS "__sign_"
 #define PREFIX_STRING "__string_"
 #define PREFIX_CODE_BLOCK "__block_"
 #define PREFIX_TMP_VAR "__T_"
 */
string Symble_node_t::get_true_name(){
    string prefix[5];
    prefix[0]=PREFIX_VAR;
    prefix[1]=PREFIX_CODE_ADDRESS;
    prefix[2]=PREFIX_STRING;
    prefix[3]=PREFIX_CODE_BLOCK;
    prefix[4]=PREFIX_TMP_VAR;
    for (int i=0; i<5; i++) {
        if(name.length()>=prefix[i].length() && name.substr(0,prefix[i].length())==prefix[i]){
            if(prefix[i]==PREFIX_CODE_BLOCK)
            {
                for(int j=prefix[i].length();j<name.length();j++){
                    if(name[j]=='_'){
                        return name.substr(j,name.length()-j);
                    }
                }
                cout<<"error:Symble_node_t::get_true_name"<<__LINE__<<endl;
                exit(-1);
            }
            else
                return name.substr(prefix[i].length(),name.length()-prefix[i].length());
        }
    }
    return name;
}

void Symble_node_t::set_const(bool _is_const){
    is_const=_is_const;
}
void Symble_node_t::set_data_type(string _name,string str){
    cout<<"void Symble_node_t::set_data_type(string _name,string str):代码没有编写"<<endl;
    exit(-1);
}
void Symble_node_t::error_pt(string msg){   //="wrong"
    cout<<"error:"<<msg<<endl;
    show();
}
void Symble_node_t::show(){
    cout<<"name="<<name<<" val="<<val;
    cout<<" line="<<coor.y<<" idx_pos="<<coor.x<<endl;
    show_type();
}
void Symble_node_t::show_type(){
    switch(type){
        case VAR_SYMBLE:
            cout<<"变量类型"<<endl;
            break;
        case VALUE_SYMBLE:
            cout<<"数值类型"<<endl;
            break;
        case DATA_TYPE_SYMBLE:
            cout<<"数据类型类型"<<endl;
            break;
        default:
            cout<<"未定义类型"<<endl;
            break;
    }
}
int Symble_block_t::count_symble_by_true_name(string name){
    return (int)echo_name2idx.count(name);
}
Symble_node_t Symble_block_t::get_symble_by_name(string name){
    if(echo_name2idx.count(name)==0){
        cout<<"符号"<<name<<"不存在"<<endl;
        exit(-1);
    }
    return snode_lst.get_by_idx(echo_name2idx[name]);
}

void Symble_block_t::error_pt(string msg){  //="wrong"
    cout<<"symble_lst error:"<<msg<<endl;
}
void Symble_block_t::push(Symble_node_t symble){
    if(echo_name2idx.count(symble.get_name())){
        error_pt("symbol name重复");
        cout<<">old symble:"<<endl;
        get_symble_by_name(symble.get_name()).show();
        cout<<">new symble:"<<endl;
        symble.show();
        exit(-1);
    }
    Tmp_var_ctrl &var_ctrl=*Tmp_var_ctrl::get_shared_impl();
    if (var_ctrl.is_tmp_var(symble.get_name())) {
        tmp_var_free_able.push(symble.get_name());
        size_tmpvar_unfree+=symble.data_type.get_sizeof();
        max_size_tmpvar_unfree=max(max_size_tmpvar_unfree,max_size_tmpvar_unfree);
    }
    echo_name2idx[symble.get_name()]=snode_lst.size();
    snode_lst.push(symble);
}


Symble_block_stack_t* Symble_block_stack_t::shared_impl=NULL;
int Symble_block_t::block_conter = 0;
