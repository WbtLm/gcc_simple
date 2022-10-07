//
//  basic_data_type.h
//  test
//
//  Created by dawn on 2019/12/22.
//  Copyright © 2019 chuyi. All rights reserved.
//

#ifndef data_type_h
#define data_type_h

#include "macro.h"

class general_data_type_t{
public:
    /**
     * 返回类型的签名
     */
    virtual string get_type_sign()=0;
    /**
     * 返回这种类型变量占用的内存大小，单位：字节,32位字内存对齐
     */
    virtual int get_sizeof()=0;
    /**
     *  是否是函数指针
     */
    virtual bool is_func_ptr()=0;
    /**
     *  是否是普通指针（函数指针为false,数组名为true）
     */
    virtual bool is_ptr_normal()=0;
    /**
     *  是否const
     */
    virtual bool is_const()=0;
    /**
     *  是否defined
     */
    virtual bool is_defined()=0;
    /**
     *  将类复制一份
     */
    virtual general_data_type_t* copy()=0;
    /**
     *  是否是数组维度（指针）
     */
    virtual bool is_array()=0;
    /**
     *  是否是结构体
     */
    virtual bool is_struct(){
        return false;
    }
};
/**
 *  仅用于初始化，不能被使用
 */
class primary_type_undefined:public general_data_type_t{
public:
    /**
     * 返回类型的签名
     */
    string get_type_sign(){
        cout<<"undefined data type::get_type_sign()"<<endl;
        exit(-1);
        return string("()");
    }
    /**
     * 返回这种类型变量占用的内存大小，单位：字节,32位字内存对齐
     */
    int get_sizeof(){
        cout<<"undefined data type::get_sizeof()"<<endl;
        exit(-1);
        return 4;
    }
    /**
     *  是否是函数指针
     */
    bool is_func_ptr(){
        cout<<"undefined data type::is_func_ptr()"<<endl;
        exit(-1);
        return false;
    }
    /**
     *  是否是普通指针（函数指针为false）
     */
    bool is_ptr_normal(){
        cout<<"undefined data type::is_ptr_normal()"<<endl;
        exit(-1);
        return false;
    }
    /**
     *  是否const
     */
    bool is_const(){
        cout<<"undefined data type::is_const()"<<endl;
        exit(-1);
        return false;
    }
    /**
     *  是否defined
     */
    bool is_defined(){
        return false;
    }
    /**
     *  将类复制一份
     */
    general_data_type_t* copy(){
        return new primary_type_undefined();
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
class primary_type_int:public general_data_type_t{
public:
    /**
     * 返回类型的签名
     */
    string get_type_sign(){
        return string("int");
    }
    /**
     * 返回这种类型变量占用的内存大小，单位：字节,32位字内存对齐
     */
    int get_sizeof(){
        return 4;
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
    general_data_type_t* copy(){
        return new primary_type_int();
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
class primary_type_pointer:public general_data_type_t{
public:
    /**
     * 返回类型的签名
     */
    string get_type_sign(){
        return string("*");
    }
    /**
     * 返回这种类型变量占用的内存大小，单位：字节,32位字内存对齐
     */
    int get_sizeof(){
        return 0;
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
        return true;
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
        return new primary_type_pointer();
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

class primary_type_array:public general_data_type_t{
    int size=0;
public:
    primary_type_array()=default;
    primary_type_array(int _size){
        set_array_size(_size);
    }
    primary_type_array& set_array_size(int _size){
        size=_size;
        return *this;
    }
    int get_array_size(){
        return size;
    }
    /**
     * 返回类型的签名
     */
    string get_type_sign(){
        string sign="[";
        sign+=to_string(size);
        return sign+"]";
    }
    /**
     * 返回这种类型变量占用的内存大小，单位：字节
     */
    int get_sizeof(){
        return 0;
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
        return true;
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
    primary_type_array* copy(){
        primary_type_array *ret=new primary_type_array();
        *ret=*this;
        return ret;
    }
    /**
     *  是否是数组维度（指针）
     */
    bool is_array(){
        return true;
    }
    /**
     *  是否是结构体
     */
    bool is_struct(){
        return false;
    }
};

/**
 *      如何计算sizeof，也就是如何确定变量最终属性。从类型签名开始讲起：
 *      签名顺序为识别顺序，如变量：int (*a[3])();的类型签名大概为：[3]，*，(func)，int。
 *      变量最终属性，如变量否为指针是由倒数第二个属性决定的，本例子由(func)决定了变量a为函数指针。
 *      变量是否为数组是由签名前缀决定的，如上例子中由[3]决定了变量a为一位数组长度为3，再如int (*b[3][4])();签名大概为[3],[4],*,(func),int，前缀[3][4]决定了是3*4的二维数组
 *
 *      sizeof=单元个数 * 单元大小。单元个数是由数组决定的。单元大小是由变量最终属性决定的
 *      变量最终属性：最终属性（如指针），决定了该变量单元大小（若不是数组的话单元大小实际上就是变量实际占用的空间大小）。如int (*a[3])()，变量a最终为函数指针。
 */
class data_type_t{
    stack_random<general_data_type_t*>type_stack;
    bool book_const=false;
public:
    int get_type_stack_size(){
        return type_stack.size();
    }
    general_data_type_t* get_head(){
        if(type_stack.size()){
            return type_stack.get_by_idx(0);
        }
        cout<<"data_type_t:get_head:type_stack.size()==0"<<endl;
        exit(-1);
    }
    void push_head(general_data_type_t *ptr){
        type_stack.push_head(ptr);
    }
    void push(general_data_type_t *ptr){
        type_stack.push(ptr);
    }
    general_data_type_t* top(){
        return type_stack.top();
    }
    void pop_head(){
        type_stack.pop_head();
    }
    void pop(){
        type_stack.pop();
    }
    void set_const(bool is_const){
        book_const=is_const;
    }
    /**
     * 签名规则：(type_stack[top].sign/type_stack[top-1].sign/...type_stack[0].sign)
     */
    string get_type_sign(){
        string sign="";
        for(int i=0;i<type_stack.size();i++){
            sign+=type_stack.get_by_idx(i)->get_type_sign();
            if(i!=type_stack.size()-1){
                sign+="/";
            }
        }
        return sign+"";
    }
private:
    /**
     *  获取单元个数。如数组单元个数为各维度大小的乘积。若非数组变量则get_cell_num()==1。
     */
    int get_cell_num(){
        int cnt=1;
        if(type_stack.size()==0){
            cout<<"error:get_cell_num():type_stack.size()==0"<<endl;
            exit(-1);
        }
        for(int i=0;i<type_stack.size();i++){
            if(type_stack.get_by_idx(i)->is_array()){
                primary_type_array *arr = static_cast<primary_type_array*>(type_stack.get_by_idx(i));
                cnt*=arr->get_array_size();
            }
            else{
                break;
            }
        }
        return cnt;
    }
    /**
     * 获取每个单元的大小。如数组每个格子的大小。若非数组(或者数组cell个数为1)，则get_cell_size() == get_sizeof()。
     */
    int get_cell_size(){
        if(type_stack.size()==0){
            cout<<"error:get_cell_size():type_stack.size()==0"<<endl;
            exit(-1);
        }
        int i=0;
        for (i=0; i<type_stack.size(); i++) {
            if(!type_stack.get_by_idx(i)->is_array())
                break;
        }
        int cell_size=type_stack.get_by_idx(i)->get_sizeof();
        if(type_stack.get_by_idx(i)->is_func_ptr() || type_stack.get_by_idx(i)->is_ptr_normal()){
            cell_size=4;
        }
        return Utils::up_word_alignment(cell_size);
    }
public:
    /**
     *  对于数组和指针类型，获取最外层维度加法时的偏移量（单位：字节）。
     *  例1：如数组 int co[5][3][4]，最外层维度为[5]，偏移量为3*4*sizeof(int)。
     *  例2：如指针 int **p，最外层维度为*，偏移量为sizeof(int*)。
     *  也就是(如例1)：offset(co)=sizeof(co)/get_array_size(最内层维度[5]) = 5*3*4*sizeof(int)/(5);
     */
    int get_ptr_offset(){
        int offset=4;
        if(is_array()){
            int cell_num_inside;
            if(!type_stack.size()){
                cout<<"error:get_ptr_offset:type_stack.size()==0"<<endl;
                exit(-1);
            }
            primary_type_array *arr = static_cast<primary_type_array*>(type_stack.get_by_idx(0));
            cell_num_inside=arr->get_array_size();
            offset=get_sizeof()/cell_num_inside;
        }
        else if(is_ptr_normal()){   //若是普通指针，则offset决定于[1~top-1]决定的大小，具体请见get_cell_size的计算方式。
            /*
             type_stack.size()一定>2
             */
            general_data_type_t* ptr = type_stack.get_by_idx(1);
            int cell_size=ptr->get_sizeof();
            if(ptr->is_func_ptr() || ptr->is_ptr_normal()){
                cell_size=4;
            }
            if(!cell_size){
                cout<<"error:get_ptr_offset():if-is_ptr_normal:cell_size==0"<<endl;
                exit(-1);
            }
            offset = Utils::up_word_alignment(cell_size);
        }
        else{
            /*
             不是数组，不是指针，则此函数无意义。认为是非法调用
             */
            cout<<"error:get_ptr_offset():非数组，非普通指针，非法的函数调用"<<endl;
            exit(-1);
        }
        
        return offset;
    }
    /**
     * 返回这种类型变量占用的内存大小，单位：字节
     */
    int get_sizeof(){
        int size=0;
        size=get_cell_num()*get_cell_size();
        if(size==0){
            cout<<"error:type.get_sizeof():size==0"<<endl;
            exit(-1);
        }
        return Utils::up_word_alignment(size);
    }
    /**
     *  是否是函数指针
     */
    bool is_func_ptr(){
        if(type_stack.size()==0){
            cout<<"error:type empty:"<<endl;
            exit(-1);
        }
        if(type_stack.size()<2){
            return false;
        }
        general_data_type_t *head_ptr=type_stack.get_by_idx(0);
        return head_ptr->is_func_ptr();
    }
    /**
     *  是否是普通指针（函数指针为false）
     *  决定于签名最外侧是否为*
     *  如 int *p[2];签名为 [2]/ * /int，决定于[2]，为数组指针（可能数组指针也算普通指针）。
     */
    bool is_ptr_normal(){
        if(type_stack.size()<2)
            return false;
        return type_stack.get_by_idx(0)->is_ptr_normal();
    }
    /**
     *  是否const
     */
    bool is_const(){
        return book_const;
    }
    /**
     *  是否defined
     */
    bool is_defined(){
        if(type_stack.size() && type_stack.get_by_idx(0)->is_defined())
            return true;
        return false;
    }
    bool operator==(data_type_t &t){
        return get_type_sign() == t.get_type_sign();
    }
    /**
     *  将类复制一份
     */
    data_type_t* copy(){
        data_type_t* ret=new data_type_t();
        ret->book_const=book_const;
        for(int i=0;i<type_stack.size();i++){
            ret->type_stack.push(type_stack.get_by_idx(i)->copy());
        }
        return ret;
    }
    virtual ~data_type_t(){
        //stack_random<general_data_type_t*>type_stack;有内存泄漏不知道如何解决,真的需要增加智能指针？
    }
    /**
     *  是否是数组维度（指针）
     */
    bool is_array(){
        if(type_stack.size() && type_stack.get_by_idx(0)->is_array()){
            primary_type_array *arr = static_cast<primary_type_array*>(type_stack.get_by_idx(0));
            if(arr->get_array_size()<=0){
                cout<<"is_array():array_size <=0 "<<endl;
                cout<<"array_size="<<arr->get_array_size();
                exit(-1);
            }
            return true;
        }
        return false;
    }
    /**
      *  是否是结构体
      */
    bool is_struct(){
        if(type_stack.size()){
            return type_stack.get_by_idx(0)->is_struct();
        }
        return false;
    }
};

#endif
