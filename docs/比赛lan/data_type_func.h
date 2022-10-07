//
//  data_type_func.hpp
//  gcc_simple
//
//  Created by dawn on 2020/4/6.
//  Copyright © 2020 chuyi. All rights reserved.
//

#ifndef data_type_func_h
#define data_type_func_h

#include "macro.h"
#include "data_type.h"
class func_arg_t{
    data_type_t arg_data_type;
    string arg_name;//形参
public:
    func_arg_t()=default;
    func_arg_t(string name,data_type_t& data_type){
        set_arg_name(name);
        set_arg_data_type(data_type);
    }
    void set_arg_data_type(data_type_t &dt){
        if(dt.is_defined()==false){
            cout<<string("data_type undefined in class func_arg_t:")<<__LINE__<<endl;
            exit(-1);
        }
        arg_data_type=dt;
    }
    data_type_t get_arg_data_type(){
        return arg_data_type;
    }
    void set_arg_name(string &name){
        arg_name=name;
    }
    string get_arg_name(){
        return arg_name;
    }
};
class func_arg_lst_t{
    stack_random<func_arg_t>arg_lst;
public:
    void push_arg(func_arg_t &func_arg){
        arg_lst.push(func_arg);
    }
    func_arg_t get_func_arg_by_idx(int idx){
        if(idx>arg_lst.size()){
            cout<<"error:func_arg_lst_t:get_func_arg_by_idx:idx out of bound:size="<<arg_lst.size()<<" idx="<<idx<<endl;
            exit(-1);
        }
        return arg_lst.get_by_idx(idx);
    }
    int size(){
        return arg_lst.size();
    }
};
class func_type_t:public general_data_type_t{
    data_type_t ret_type;
    func_arg_lst_t arg_lst;
public:
    void push_func_arg(func_arg_t &func_arg){
        arg_lst.push_arg(func_arg);
    }
    void push_func_arg(string name,data_type_t data_type){
        func_arg_t arg(name,data_type);
        arg_lst.push_arg(arg);
    }
    void set_func_arg_lst(func_arg_lst_t lst){
        arg_lst=lst;
    }
    int get_arg_num(){
        return arg_lst.size();
    }
    func_arg_t get_func_arg_by_idx(int idx){
        return arg_lst.get_func_arg_by_idx(idx);
    }
    string get_type_sign(){
        string sign="(__func_";
        //   sign+=func_name;
        for(int i=0;i<(int)arg_lst.size();i++){
            sign+="/";
            sign+=
            arg_lst.get_func_arg_by_idx(i).get_arg_data_type()
            .get_type_sign();
        }
        return sign+")";
    }
    /**
     * 返回这种类型变量占用的内存大小，单位：字节
     */
    int get_sizeof(){
        return 4;
    }
    /**
     *  是否是函数指针
     */
    bool is_func_ptr(){
        return true;
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
        return true;
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
    func_type_t* copy(){
        func_type_t *ret=new func_type_t();
        *ret=*this;
        return ret;
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
        return false;
    }
};
#endif /* data_type_func_h */
