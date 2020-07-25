//
//  guidance.hpp
//  test
//
//  Created by dawn on 2019/12/6.
//  Copyright © 2019 chuyi. All rights reserved.
//

#ifndef guidance_h
#define guidance_h
#include "syntax_analyze_matrix.h"
#include "symble.h"
#include "tree_node.h"
#include "macro.h"
typedef vector<map<int,map_cell_t>> matrix_t;
int slr1_ctrl(matrix_t matrix,stack<tree_node_t> word_stack);

class guid_func_args_t{
    vector<tree_node_t>sign_lst;
    int prod_idx;
public:
    int size(){
        return sign_lst.size();
    }
    guid_func_args_t(int _prod_idx){
        prod_idx=_prod_idx;
    }
    void push_back(tree_node_t sign){
        sign_lst.push_back(sign);
    }
    int get_pord_idx(){
        return prod_idx;
    }
    tree_node_t get_arg(int idx){
        if(idx>=sign_lst.size()){
            cout<<"error:tree_node_t get_arg(int idx):idx out of bound"<<endl;
            exit(-1);
        }
        return sign_lst[sign_lst.size()-idx-1];
    }
};
typedef tree_node_t (*guid_func_ptr_t)(guid_func_args_t);
class Guider{//单例
    vector<guid_func_ptr_t>func_lst;
    Guider(){
        prod_func_name2idx.clear();
    }
    Guider& operator = (const Guider& guider); // =deleted
    static Guider *share_impl;
    map<string,int>prod_func_name2idx;
    string get_prod_name(int prod_idx){
        production_t prod = get_prod_by_idx(prod_idx);
        string name="";
        name+= echo_num2str(prod.left)+"->";
        for(int i=0;i<(int)prod.detail.size();i++){
            name+=echo_num2str(prod.detail[i]);
        }
        return name;
    }
public:
    static Guider* get_shared_impl(){
        if(share_impl){
            return share_impl;
        }
        return share_impl = new Guider();
    }
    tree_node_t call_guid_func(guid_func_args_t &args){
        if(func_lst[args.get_pord_idx()]==NULL){
            cout<<"function == null"<<endl;
            cout<<args.get_pord_idx()<<endl;
            exit(-1);
        }
        cout<<"call func:"<<args.get_pord_idx()<<endl;
        return func_lst[args.get_pord_idx()](args);
    }
    void set_guid_func_by_idx(int idx,guid_func_ptr_t func){
        if(func_lst.size()-1<idx){
            resize(idx+1);
        }
        func_lst[idx]=func;
    }
    void regist_guid_func_by_name(string name,guid_func_ptr_t func){
        if(prod_func_name2idx.size()==0){
            int num=get_num_of_prod();
            for(int i=0;i<num;i++){
                prod_func_name2idx[get_prod_name(i)]=i;
            }
        }
        if(!prod_func_name2idx.count(name)){
            cout<<"regist_guid_func_by_name failed: name is not exist:"<<endl;
            cout<<name<<endl;
            exit(-1);
        }
        set_guid_func_by_idx(prod_func_name2idx[name], func);
    }
    void resize(int size){
        func_lst.resize(size);
    }
};
void set_guid_func();
/**
 * 使用registe_primary_data_type()函数来向符号表最底层注册基本数据类型int
 */
void registe_primary_data_type();
string get_middle_code();
#endif /* guidance_hpp */
