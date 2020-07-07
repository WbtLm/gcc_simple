//
//  struct_type_dic.h
//  gcc_simple
//
//  Created by dawn on 2020/3/24.
//  Copyright © 2020 chuyi. All rights reserved.
//

#ifndef struct_type_dic_h
#define struct_type_dic_h

#include "macro.h"
#include "data_type.h"

class struct_type_node_t:general_data_type_t{
    stack_random<string>var_name_lst;
    stack_random<data_type_t>data_type_lst;
    stack_random<int>offset_lst;
    int pos=0;
    string struct_name;
    //优化复杂度
    string last_name="-";
    int last_idx=-1;
public:
    struct_type_node_t()=default;
    struct_type_node_t(string name){
        set_struct_name(name);
    }
    string get_struct_name(){
        return struct_name;
    }
    void set_struct_name(string _struct_name){
        struct_name=_struct_name;
    }
    bool count_var_name(string name){
        for (int i=0; i<var_name_lst.size(); i++) {
            if(var_name_lst.get_by_idx(i)==name){
                return true;
            }
        }
        return false;
    }
    /**
     * 增加一条成员变量
     */
    void push_var(string var_name,data_type_t data_type){
        if(count_var_name(var_name)){
            cout<<"struct_type_node_t::push_var::count_var_name(var_name)!=false"<<endl;
            cout<<var_name<<endl;
            exit(-1);
        }
        var_name_lst.push(var_name);
        data_type_lst.push(data_type);
        offset_lst.push(pos);
        pos+=data_type.get_sizeof();
    }
    /**
     *  读取一条成员变量偏移
     */
    int get_offset_by_name(string var_name){
        if(var_name==last_name){
            return offset_lst.get_by_idx(last_idx);
        }
        for(int i=0;i<var_name_lst.size();i++){
            if(var_name_lst.get_by_idx(i)==var_name){
                return offset_lst.get_by_idx(i);
            }
        }
        cout<<"struct_type_node_t:get_offset_by_name:var_name not found"<<endl;
        cout<<var_name<<endl;
        exit(-1);
    }
    /**
     * 
     */
    data_type_t get_data_type_by_name(string var_name){
        if(var_name==last_name){
            return data_type_lst.get_by_idx(last_idx);
        }
        for(int i=0;i<var_name_lst.size();i++){
            if(var_name_lst.get_by_idx(i)==var_name){
                return data_type_lst.get_by_idx(i);
            }
        }
        cout<<"struct_type_node_t:get_data_type_by_name:var_name not found"<<endl;
        exit(-1);
    }
    /**
     * 返回类型的签名
     */
    string get_type_sign(){
        string sign="(struct:";
        sign+=struct_name;
        sign+=")";
        return sign;
    }
    /**
     * 返回这种类型变量占用的内存大小，单位：字节,32位字内存对齐
     */
    int get_sizeof(){
        return pos;
    }
    /**
     *  是否是函数指针
     */
    bool is_func_ptr(){
        return false;
    }
    /**
     *  是否是普通指针（函数指针为false,数组名为true）
     */
    bool is_ptr_normal(){
        return false;
    }
    /**
     *  是否const
     */
    bool is_const(){
        return false;
    }
    /**
     *  是否defined
     */
    bool is_defined(){
        return true;
    }
    /**
     *  将类复制一份
     */
    general_data_type_t* copy(){
        general_data_type_t *copy = new struct_type_node_t();
        *copy=*this;
        return copy;
    }
    /**
     *  是否是数组维度（指针）
     */
    bool is_array(){
        return false;
    }
    /**
     *  是否是结构体
     */
    bool is_struct(){
        return true;
    }
    
    
};
class struct_type_dic{
    static struct_type_dic *impl;
    struct_type_dic()=default;
    map<string,struct_type_node_t*>dictionary;
public:
    static struct_type_dic *get_shared_impl(){
        if(impl==NULL){
            return impl=new struct_type_dic();
        }
        return impl;
    }
    void push_struct_type_node(struct_type_node_t *node){
        if(dictionary.count(node->get_struct_name())){
            cout<<"struct_type_dic:push_struct_type_node:dic.count(name)!=0"<<endl;
            exit(-1);
        }
        dictionary[node->get_struct_name()]=node;
    }
    bool count_struct_type_node_by_name(string struct_name){
        return dictionary.count(struct_name);
    }
    struct_type_node_t* get_struct_type_node_by_name(string struct_name){
        if(!count_struct_type_node_by_name(struct_name)){
            cout<<"struct_type_dic:get_struct_type_node_by_name:dic.count==false"<<endl;
            cout<<struct_name<<endl;
            exit(-1);
        }
        return dictionary[struct_name];
    }
};

class struct_type_t:public general_data_type_t{
    string struct_name="";
    struct_type_dic &dic = *struct_type_dic::get_shared_impl();
public:
    struct_type_t()=default;
    struct_type_t(string _struct_name){
        struct_name=_struct_name;
    }
    void set_struct_name(string struct_name){
        this->struct_name=struct_name;
    }
    string get_struct_name(){
        return struct_name;
    }
    
    /**
     *
     */
    bool count_var_name(string var_name){
        if(struct_name.length()==0){
            cout<<"struct_type_t:count_var_name:struct_name.length()==0"<<endl;
            exit(-1);
        }
        return dic.get_struct_type_node_by_name(struct_name)->count_var_name(var_name);
    }
    void push_var(string var_name,data_type_t data_type){
        return dic.get_struct_type_node_by_name(struct_name)->push_var(var_name, data_type);
    }
    data_type_t get_var_data_type_by_name(string var_name){
        return dic.get_struct_type_node_by_name(struct_name)->get_data_type_by_name(var_name);
    }
    int get_var_offset_by_name(string var_name){
        return dic.get_struct_type_node_by_name(struct_name)->get_offset_by_name(var_name);
    }
    
    string get_type_sign(){
        return dic.get_struct_type_node_by_name(struct_name)->get_type_sign();
    }
    /**
     * 返回这种类型变量占用的内存大小，单位：字节
     */
    int get_sizeof(){
        return dic.get_struct_type_node_by_name(struct_name)->get_sizeof();
    }
    /**
     *  是否是函数指针
     */
    bool is_func_ptr(){
        return false;
    }
    /**
     *  是否是普通指针（函数指针为false）
     */
    bool is_ptr_normal(){
        return false;
    }
    /**
     *  是否const
     */
    bool is_const(){
        return false;
    }
    /**
     *  是否defined
     */
    bool is_defined(){
        return true;
    }
    /**
     *  将类复制一份
     */
    struct_type_t* copy(){
        return new struct_type_t(get_struct_name());
    }
    /**
     *  是否是数组维度（指针）
     */
    bool is_array(){
        return false;
    }
    bool is_struct(){
        return true;
    }
};

#endif /* struct_type_dic_h */
