//
//  back_var_ctrl.cpp
//  test
//
//  Created by dawn on 2020/1/9.
//  Copyright © 2020 chuyi. All rights reserved.
//

#include "back_var_ctrl.h"
back_var_ctrl *back_var_ctrl::back_var_ctrl_impl = NULL;
#include "macro_inst.h"





void var_node_t::get_left_addr(reg_t reg_dest){
    if(book_left_able==0){
        cout<<"book_left_able == false"<<endl;
        exit(0);
    }
    if(book_left_var){
        move_var_to_reg(left_ptr_var, reg_dest);
    }
    else{
        ins_move_imm_to_reg(reg_dest, left_addr);
    }
}
