//
//  syntax_analyze.h
//  test
//
//  Created by dawn on 2019/11/29.
//  Copyright © 2019 baotong.wbt. All rights reserved.
//

#ifndef syntax_analyze_matrix_h
#define syntax_analyze_matrix_h
#define ACTION_S 0
#define ACTION_R 1
#define ACTION_ACC 2
#include "macro.h"

#include <iostream>
#include "lexical_analyze.h"
using namespace std;
//将Vn,Vt转为int
int echo_str2num(string str);
string echo_num2str(int num);
struct production_t{
    production_t(){
        detail.clear();
    }
    int val;
    int left;
    vector<int>detail;//产生式右边
    void show();
};
production_t get_prod_by_idx(int idx);
int get_num_of_prod();
struct map_cell_t{
    int action=-1;
    int go_to;//移进：状态转移，归约：产生式id
    void show(){
        if(action==ACTION_R){
            cout<<"R";
            cout<<go_to;
        }
        else if(action==ACTION_S){
            cout<<"S";
            cout<<go_to-1;
        }
        else{
            cout<<"error";
        }
    }
};
int get_syntax_matrix(vector<map<int,map_cell_t>> &matrix,const char *path);
#endif /* syntax_analyze_h */
