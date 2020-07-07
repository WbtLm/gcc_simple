//
//  lexical_analyze.h
//  test
//
//  Created by dawn on 2019/11/29.
//  Copyright © 2019 baotong.wbt. All rights reserved.
//

#ifndef lexical_analyze_h
#define lexical_analyze_h
#include "macro.h"
//s是保留字？
bool is_reserved(string s);
enum word_type_t{
    UNDEFINED_TYPE_WORD,     //未定义
    MIDDLE_WORD,             //VN
    STATIC_VALUE_WORD,       //[常量]
    TERMINAL_WORD,           //终结符
    DATA_TYPE_WORD,          //[数据类型]
    KEYWORD_WORD,            //[关键字]
};
struct word_node_t{    //元组
    //词法分析获取的信息
    string s;
    int type;
    int line;
    int idx;
    //adapter 转换的信息
    word_type_t word_type;
    string name;    //比如[关键字]、'*'等
    int int_val=0;       //int实数或char类型（保存在低8位）
    string str_val;     //字符串常量或变量名
    
    word_node_t()=delete;
    word_node_t(string ss,int type_num,int l,int i);//词法分析使用
    word_node_t(int word_type,int l,int i);
  //  {//语法分析器使用。
//        word_type=
//    }
    word_node_t(string name){
        this->name=name;
        word_type=MIDDLE_WORD;
    }
    void show_word(){
        cout<<"name="<<name<<endl;
        cout<<"int_val="<<int_val<<endl;
        cout<<"str_val="<<str_val<<endl;
    }
    void show(){
        cout<<s<<endl;
        cout<<"type="<<type;
        pt_type();
        cout<<endl;
        cout<<"line="<<line<<" idx="<<idx<<endl;
        cout<<"name="<<name<<endl;
        cout<<"int_val="<<int_val<<endl;
        cout<<"str_val="<<str_val<<endl;
    }
    void pt_type()
    {
        switch(type)
        {
            case 0:cout<<"注释";break;
            case 1:cout<<"标识符(变量，函数名，等)";break;
            case 2:cout<<"常数";break;
            case 3:cout<<"保留字(关键字)";break;
            case 4:cout<<"分隔符";break;
            case 5:cout<<"运算符";break;
            case 6:cout<<"编译命令";break;
            case 7:cout<<"常量";break;
            case 8:cout<<"结束符";break;
        }
    }
};

int get_word(vector<word_node_t>&word_lst,const char *input_path);

#endif /* lexical_analyze_h */
