//
//  tree_node.hpp
//  test
//
//  Created by dawn on 2019/12/9.
//  Copyright © 2019 chuyi. All rights reserved.
//

#ifndef tree_node_h
#define tree_node_h

#include "macro.h"
#include "symble.h"
using namespace std;

class value_t{
public:
    string str_value;
    int int_value;
    Symble_block_t* block_value=NULL;
    Symble_node_t *symble_node_value=NULL;
    ~value_t(){
        if(symble_node_value){
           // delete symble_node_value;
        }
        if(block_value){
           // delete block_value;
        }
        
    }
    value_t()=default;
    value_t(const value_t& copy){
        str_value=copy.str_value;
        int_value=copy.int_value;
        if(copy.symble_node_value){
            symble_node_value=new Symble_node_t("");
            *symble_node_value=*copy.symble_node_value;
        }
        if(copy.block_value)
        {
            block_value = new Symble_block_t();
            *block_value=*copy.block_value;
        }
    }
};
class tree_node_t{
    bool left_able = false;
public:
    enum type_t{
        UNDEFINED,
        VAR,
        ADDRESS_SIGN,
        CODE_BLOCK,
        STATIC_VALUE,
    };
    
    void set_left_able(bool _left_able){
        left_able=_left_able;
    }
    bool get_left_albe(){
        return left_able;
    }
    coor_t coor;
    value_t value;
    string code;
    type_t type=UNDEFINED;
    string name;
    void show(){
        cout<<"name="<<name<<endl;
        cout<<"value:"<<endl;
        show_value();
        if(code.length()){
            cout<<"===code==="<<endl;
            cout<<code<<endl;
            cout<<"===code end==="<<endl;
        }
    }
    void show_value(){
        switch(type){
            case UNDEFINED:
                cout<<"tree_node_t.show()::type undefined"<<endl;
            //    exit(-1);
                break;
            case VAR:
                cout<<"VAR"<<endl;
                cout<<value.int_value<<endl;
                break;
            case ADDRESS_SIGN:
                cout<<"ADDRESS_SIGN"<<endl;
                cout<<value.str_value<<endl;
                break;
            case CODE_BLOCK:
                cout<<"CODE_BLOCK"<<endl;
                if(value.block_value){
                    value.block_value->show();
                }
                break;
            case STATIC_VALUE:
                cout<<"STATIC_VALUE"<<endl;
                cout<<value.int_value<<endl;
                break;
            default:
                cout<<"tree_node_t.show()::error:无法识别type"<<endl;
                exit(-1);
                break;
        }
    }
};

#endif /* tree_node_hpp */
