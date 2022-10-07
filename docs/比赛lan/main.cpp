//
//  main.cpp
//  test
//
//  Created by dawn on 2019/11/29.
//  Copyright © 2019 chuyi. All rights reserved.
//





#include "macro.h"
#include "lexical_analyze.h"
#include "syntax_analyze_matrix.h"
#include "guidance.h"
#include "tree_node.h"
//#include "background/background.h"
using namespace std;
void show_prod_lst(){
    cout<<"prod_lst:"<<endl;
    int size=get_num_of_prod();
    for(int i=0;i<size;i++){
        cout<<i<<' ';
        get_prod_by_idx(i).show();
    //    cout<<endl;
    }
}
int main()
{
//    tool();
    registe_primary_data_type();
    int ret;
    vector<word_node_t>word_lst;
    vector<map<int,map_cell_t>>matrix;
    ret=get_syntax_matrix(*&matrix,"lan.txt");
    if(ret<0){
        return -1;
    }
    ret=get_word(*&word_lst,"input.c");
    
    if(ret<0){
        return -1;
    }
    stack<tree_node_t>word_stack;
    //倒序入栈，接口转换
    for(int i=(int)word_lst.size()-1;i>=0;i--){
        tree_node_t tree_node;
        word_node_t word_node=word_lst[i];
        
        tree_node.coor.x=word_node.idx;
        tree_node.coor.y=word_node.line;
        tree_node.value.int_value=word_node.int_val;
        tree_node.value.str_value=word_node.str_val;
        
        tree_node.name=word_node.name;
        if(tree_node.name=="[关键字]"){
            cout<<"value"<<endl;
        }
        if(word_node.type==0){
            //注释
            continue;
        }
        word_stack.push(tree_node);
        
    }
    //show matrix
    /*
    for (int i=0; i<matrix.size(); i++) {
        cout<<"cond="<<i<<endl;
        for(auto it=matrix[i].begin();it!=matrix[i].end();++it){
            cout<<echo_num2str(it->first)<<":";
            it->second.show();
            cout<<endl;
        }
    }*/
    set_guid_func();//自动机前要设置制导函数
    show_prod_lst();
    //return 0;
    cout<<"自动机:"<<endl<<endl<<endl;
    //adapter

    ret=slr1_ctrl(matrix,word_stack);
    
//    cout<<"输入word为"<<endl;
//    while(word_stack.empty()==0){
//        word_stack.top().show_word();
//        cout<<endl;
//        word_stack.pop();
//    }
    
    if(ret<0){
        return -1;
    }
    else if(ret==1){
        cout<<"识别成功"<<endl;
    }
    else{
        cout<<"识别终止"<<endl;
    }
    //cout<<get_middle_code()<<endl;
    {
        FILE *out_fp = fopen("mid.c", "wt");
        if(out_fp==NULL){
            cout<<"error:mid.c out_fp == null"<<endl;
            return 0;
        }
        fprintf(out_fp, "%s",get_middle_code().c_str());
    }
    /*
    string term;
    term = background(get_middle_code());
    
    cout<<"=======middle====="<<endl;
    cout<<get_middle_code()<<endl;
    
    cout<<"========term======"<<endl;
    cout<<term<<endl;
    cout<<"========end======="<<endl;
    {
        FILE *out_fp = fopen("/Users/dawn/f/lan/code.c", "wt");
        if(out_fp==NULL){
            cout<<"error:code.txt out_fp == null"<<endl;
            return 0;
        }
        fprintf(out_fp, "%s",term.c_str());
    }
    cout<<get_debug_str()<<endl;
    */ 

}
int (*func())(){
    return 0;
}
