//
//  utils.hpp
//  test
//
//  Created by dawn on 2019/12/22.
//  Copyright © 2019 chuyi. All rights reserved.
//

#ifndef utils_h
#define utils_h

#include "macro.h"
using namespace std;
struct coor_t{
    int y;
    int x;
};
class Utils{
//static string error_info;
public:
    /**
     * 正常退出：源程序错误，编译器工作正常
     */
    class Exit_normal{
    public:
        /**
         * 打印语法错误，并exit
         */
        static void pt_error_source_code_lan(string error_info,coor_t coor){
            cout<<"语法错误:"<<error_info<<endl;
            if(coor.y>=0 && coor.x>=0)
                cout<<"行："<<coor.y<<" 列："<<coor.x<<endl;
            exit(0);
        }
    };

    static int up_word_alignment(int ori_size){
        if(ori_size%4){
            return (ori_size-ori_size%4)+1;
        }
        return ori_size;
    }
};
template <typename Type>
class stack_random{
    vector<Type>vec;
    int idx=0;
public:
    void erase_by_idx(int aim){
        if(aim==idx-1){
            pop();
            return;
        }
        if(aim>=idx){
            cout<<"stack_radom:erase_by_idx"<<endl;
            exit(-1);
        }
        for(int i=aim+1;i<idx;i++){
            Type t=vec[i];
            vec[i]=vec[i-1];
            vec[i-1]=t;
        }
        pop();
    }
    void clear(){
        vec.clear();
        idx=0;
    }
    int size(){
        return idx;
    }
    void push(Type node){
        if(idx==vec.size()){
            vec.push_back(node);
        }
        else{
            vec[idx]=node;
        }
        idx++;
    }
    void pop(){
        idx--;
    }
    Type& top(){
        if(idx>vec.size() || idx<=0){
            cout<<"stack_radom:top():数组越界"<<endl;
            cout<<idx<<endl;
            exit(-1);
        }
        return vec[idx-1];
    }
    Type& get_by_idx(int idx){
        if(idx>=size()){
            cout<<"error:stack_random:get_by_idx:idx>=size"<<endl;
            cout<<"idx="<<idx<<" size="<<size()<<endl;
            exit(-1);
        }
        return vec[idx];
    }
    /**
     * 将另一个stack_random插入到后面
     */
    Type& push_stack_back(stack_random<Type>&back){
        for(int i=0; i<back.size(); i++) {
            this->push(back.get_by_idx(i));
        }
        return *this;
    }
    /**
     * 删除0下标元素，复杂度为O（n）。
     */
    void pop_head(){
        for(int i=1;i<size();i++){
            Type t=vec[i];
            vec[i]=vec[i-1];
            vec[i-1]=t;
        }
        pop();
        
    }
    /**
     * 在头部插入一个元素。复杂度为O（n）。
     */
    void push_head(Type node){
        push(node);
        for(int i=size()-1;i>=1;i--){
            Type t=vec[i];
            vec[i]=vec[i-1];
            vec[i-1]=t;
        }
    }
};
#endif /* utils_hpp */
