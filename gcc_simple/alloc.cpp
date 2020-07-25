//
//  alloc.cpp
//  test
//
//  Created by dawn on 2019/12/14.
//  Copyright © 2019 chuyi. All rights reserved.
//

#include "alloc.h"

Tmp_var_ctrl* Tmp_var_ctrl::shared_impl=NULL;
string Tmp_var_ctrl::alloc_tmp_var(){
    if(free_num==0){
        string str=prefix+to_string(total_num);
        total_num++;
        using_var.insert(str);
        free_var.push_back("error");
        return str;
    }
    free_num--;
    return free_var[free_num];
}
string get_reg_name(reg_t reg){
    string ret="r";
    return ret+to_string((int)reg);
}
