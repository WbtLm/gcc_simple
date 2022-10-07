//
//  symble.h
//  test
//
//  Created by dawn on 2019/12/6.
//  Copyright © 2019 chuyi. All rights reserved.
//

#ifndef symble_h
#define symble_h
#include "macro.h"
#include "data_type.h"
#include "alloc.h"

enum symble_type_t{
    UNDEFINED_SYMBLE_TYPE,
    VAR_SYMBLE,
    VALUE_SYMBLE,
    DATA_TYPE_SYMBLE,
    MIDDLE_TYPE_SYMBLE,
};
typedef unsigned int uint;

/**
 *
 */
class Symble_node_t{    //符号
    string name;
    string block_name;
    
public:
    void set_block_name(string bname){
        block_name=bname;
    }
    string get_block_name(){
        return block_name;
    }
    /**
     * name为关键字，若symble为静态字符串则需要向全局层次注册字符串，并置name为对应**的静态变量名。注册时symble此name填写分配到的名字，置str_val为对应字符串
     */
    uint val;
    string str_val;
    symble_type_t type=UNDEFINED_SYMBLE_TYPE;
    data_type_t data_type;
    coor_t coor;
    bool is_const=false;
    Symble_node_t()=delete;
//    Symble_node_t(const Symble_node_t& copy){
//        val=copy.val;
//        str_val=copy.str_val;
//        type=copy.type;
//        data_type=copy.data_type;
//        coor=copy.coor;
//    }
    Symble_node_t(string name);
    /**
     * 若是注册字符串，还需置str_val
     */
    void set_type(symble_type_t _type){
        type = _type;
    }
    Symble_node_t(string _name,symble_type_t _type,int _line_pos,int _idx_pos);
    string get_true_name();
    string get_name(){
        return name;
    }
    void set_int_val(uint _val);
    void set_const(bool _is_const);
    void set_data_type(string _name,string str);
    void error_pt(string msg="wrong");
    void show();
    void show_type();
    Symble_node_t* copy(){
        Symble_node_t *ret=new Symble_node_t("");
        *ret=*this;
        return ret;
    }
};
class Symble_block_t{   //符号表块
    string name;
    vector<string>counter_stack_info;//csi: counter stack info
    string csi_string;
    stack_random<Symble_node_t>snode_lst;
    map<string,int>echo_name2idx;
    void init();
    void set_csi_string(){
        csi_string="";
        for(int i=0;i<(int)counter_stack_info.size();i++){
            csi_string+="/"+counter_stack_info[i];
        }
    }
    static int block_conter;

    /**
     *  支持tmp var释放的冗余信息(名字)。记录block中没有被释放的所有tmp var
     *
     */
    stack_random<string>tmp_var_free_able;
    int size_tmpvar_unfree=0;
    int max_size_tmpvar_unfree=0;
public:
    struct var_size{
        string name;
        int size;
        bool is_arr=false;
        bool is_struct=false;
        
        var_size(string _name,int _size,bool _is_arr=false,bool _is_struct=false){
            name=_name;
            size=_size;
            is_arr=_is_arr;
            is_struct=_is_struct;
        }
    };
    int size(){
        return snode_lst.size();
    }
    /**
     *  将新block内部符号合并到本block后面，间接合并临时变量
     */
    Symble_block_t& push_block_back(Symble_block_t &block){
        for(int i=0;i<block.size();i++){
            this->push(block.get_snode_by_idx(i));
        }
        return *this;
    }
    /**
     * 获取所有的需要释放的tmp var，用于归约为<语句>时释放临时变量。
     */
    stack_random<string>get_tmp_var_free_able(){
        return tmp_var_free_able;
    }
    /**
     * tmp var已被释放完毕，调用此函数清除相应的记录
     */
    void free_tmp_var_all(){
        tmp_var_free_able.clear();
        size_tmpvar_unfree=0;
    }
    /**
     * 获取临时变量需要的栈空间（合理释放后）
     */
    int get_max_size_tmpvar_unfree(){
        return max_size_tmpvar_unfree;
    }
    /**
     * 如名
     */
    Symble_node_t& get_snode_by_idx(int idx){
        return snode_lst.get_by_idx(idx);
    }
    /**
     * 获取所有的变量，临时变量除外
     */
    stack_random<var_size>get_stack_var(){
        stack_random<var_size>ret;
        Tmp_var_ctrl &var_ctrl=*Tmp_var_ctrl::get_shared_impl();

        for (int i=0;i<snode_lst.size(); i++) {
            Symble_node_t &snode=snode_lst.get_by_idx(i);
            if(!var_ctrl.is_tmp_var(snode.get_name())){
                ret.push(var_size(snode.get_name()
                        ,snode.data_type.get_sizeof()+(snode.data_type.is_array()?unit:0)
                        ,snode.data_type.is_array()
                        ,snode.data_type.is_struct())
                         );
                cout<<snode.data_type.get_type_sign()<<endl;;
            }
        }
        return ret;
    }
    /**
     * 获取所有的临时变量
     */
    stack_random<var_size>get_stack_tmp_var(){
        stack_random<var_size>ret;
        for (int i=0;i<snode_lst.size(); i++) {
            Symble_node_t &snode=snode_lst.get_by_idx(i);
            Tmp_var_ctrl &var_ctrl=*Tmp_var_ctrl::get_shared_impl();
            if(var_ctrl.is_tmp_var(snode.get_name())){
                ret.push(var_size(snode.get_name(),snode.data_type.get_sizeof()));
            }
        }
        return ret;
    }
    /**
     * 不包括临时变量
     */
    int get_stack_var_size(){
        int ret=0;
        for(int i=0;i<snode_lst.size();i++){
            Tmp_var_ctrl &var_ctrl=*Tmp_var_ctrl::get_shared_impl();
            if(!var_ctrl.is_tmp_var(snode_lst.get_by_idx(i).get_name())){
                data_type_t dt=snode_lst.get_by_idx(i).data_type;
                ret+=dt.get_sizeof()+(dt.is_array()?unit:0);
            }
        }
        return Utils::up_word_alignment(ret);
    }
    /**
     * 临时变量总大小（无释放）
     */
    int get_stack_tmp_var_size(){
        int ret=0;
        for(int i=0;i<snode_lst.size();i++){
            Tmp_var_ctrl &var_ctrl=*Tmp_var_ctrl::get_shared_impl();
            if(var_ctrl.is_tmp_var(snode_lst.get_by_idx(i).get_name())){
                ret+=snode_lst.get_by_idx(i).data_type.get_sizeof();
            }
        }
        return Utils::up_word_alignment(ret);
    }
    static string get_new_block_name(){
        return string(PREFIX_CODE_BLOCK)+to_string(block_conter++);
    }
//    Symble_block_t()
    void set_name(string _name){
        name = _name;
    }
    string get_name(){
        return name;
    }
    void set_csi(vector<string>csi,int size){
        counter_stack_info.clear();
        for(int i=0;i<size;i++){
            counter_stack_info.push_back(csi[i]);
        }
        set_csi_string();
    }
    string get_csi(){
        return csi_string;
    }
    Symble_block_t(){
        init();
    }
    int count_symble_by_true_name(string name);
    Symble_node_t get_symble_by_name(string name);
    void error_pt(string msg="wrong");
    void push(Symble_node_t symble);
    
    void show(){
        for(int i=0;i<snode_lst.size();i++){
            cout<<snode_lst.get_by_idx(i).get_true_name()<<endl;
        }
    }
};
class Symble_block_stack_t{ //单例  全局符号表栈
    vector<string>csi_stack;
    int csi_size=0;
    stack_random<Symble_block_t>symble_stack;
    static Symble_block_stack_t *shared_impl;
    Symble_block_stack_t(){
        this->push("__main_stack");
    }
    Symble_block_stack_t& operator=(Symble_block_stack_t& arg)=delete;
    string func_name;
    int new_name=0;
    map<string,string>str_name_map;
    /**
     * 记录下循环名，以便于获取continue、break等的信息
     */
    stack_random<string>loop_stack;
    /**
     * 记录下if名，以便于分支控制
     */
    stack_random<string>if_stack;
    /**
     *  参数控制。only for one argument
     */
    data_type_t arg_data_type;
    string arg_name;    //无前缀名字。因为setter不知道block_name
    
public:
    /*
      arg ctrl
     */
    void set_arg(string name,data_type_t data_type){
        arg_name=name;
        arg_data_type=data_type;
    }
    void clear_arg(){
        arg_name="";
    }
    int count_arg(){
        return arg_name.length()!=0;
    }
    string get_arg_name(){
        return arg_name;
    }
    data_type_t get_arg_data_type(){
        return arg_data_type;
    }
    //arg end
    
    enum name_type_t{
        UNDEFINED,
        VAR,
        ADDRESS_SIGN,
        CODE_BLOCK,
//        STATIC_VALUE,
    };
    int size(){
        return symble_stack.size();
    }
    static Symble_block_stack_t* get_shared_impl(){
        if(shared_impl)
            return shared_impl;
        return shared_impl = new Symble_block_stack_t();
    }
    int get_csi_size(){
        return csi_size;
    }
    /**
     * 在全局中查询某个 名字 是否存在
     */
    int count_symble_node_by_name(string name){
        if(name.length()==0){
            return true;//string("")
        }
        int cnt=0;
        for(int i=symble_stack.size()-1;i>=0;i--){
            Symble_block_t block=symble_stack.get_by_idx(i);
            if (block.count_symble_by_true_name(name)) {
                cnt++;
            }
        }
        return cnt;
    }
    /**
     * 顶层层次中查询某个 名字 是否存在
     */
    int top_count_symble_node_by_name(string name){
        int cnt=0;
        Symble_block_t block=symble_stack.get_by_idx(symble_stack.size()-1);
        if (block.count_symble_by_true_name(name)) {
            cnt++;
        }
        return cnt;
    }
    /**
     * 获取名字对应的信息
     */
    Symble_node_t get_symble_node_by_name(string name){
        
        for(int i=symble_stack.size()-1;i>=0;i--){
            Symble_block_t block=symble_stack.get_by_idx(i);
            if (block.count_symble_by_true_name(name)) {
                return block.get_symble_by_name(name);
            }
        }
        cout<<"error:Symble_block_stack_t.get_symble_by_name:name not found:";
        cout<<name<<endl;
        exit(-1);
        return Symble_node_t("");
        
    }
    /**
     * 为名字加前缀后再进行查询，优先查询无前缀名字，若不存在则枚举前缀再次查询
     */
    bool count_symble_node_by_true_name(string name){
        if(count_symble_node_by_name(name)){
            return true;
        }
        for(int i=symble_stack.size()-1;i>=0;i--){
            Symble_block_t block = symble_stack.get_by_idx(i);
            string new_name = block.get_name()+name;
            if(count_symble_node_by_name(new_name)){
                return true;
            }
        }
        return false;
    }
    /**
     * 为名字加前缀后再进行查询，优先查询无前缀名字，若不存在则枚举前缀再次查询
     */
    Symble_node_t get_symble_node_by_true_name(string name){
        if(count_symble_node_by_name(name)){
            return get_symble_node_by_name(name);
        }
        if(count_symble_node_by_true_name(name)){
            for(int i=symble_stack.size()-1;i>=0;i--){
                Symble_block_t block = symble_stack.get_by_idx(i);
                string new_name = block.get_name()+name;
                if(count_symble_node_by_name(new_name)){
                    return block.get_symble_by_name(new_name);
                }
            }
        }
        
        cout<<"error:Symble_block_stack_t.get_symble_by_true_name:block_name+true_name not found:";
        cout<<name<<endl;
        exit(-1);
        return Symble_node_t("");
    }
    /**
     * 获取一个新的全局不重复的名字
     */
    string get_new_name(string prefix){
      
        if(prefix==PREFIX_VAR){
            
        }
        else if(prefix==PREFIX_CODE_ADDRESS){
            
        }
        else if(prefix==PREFIX_STRING){
            
        }
        else{
            cout<<"new_name:错误的prefix"<<endl;
            exit(-1);
        }
        string ret=prefix;
        char tmp[1024];
        sprintf(tmp,"%d", new_name);
        string t(tmp);
        new_name++;
        return ret+t;
        //return ret+symble_stack.top().get_csi()+t;
    }
    /**
     * 栈的层次结构push一层，表示一个新的块层次。
     * 1.name 为新层次起个名字
     */
    void push(string name="default_block_name"){
        Symble_block_t new_block;
        new_block.set_name(Symble_block_t::get_new_block_name());
        symble_stack.push(new_block);
        if(csi_stack.size()<=csi_size){
            csi_stack.push_back(name);
        }
        else{
            csi_stack[csi_size]=name;
        }
        /*
         若csi_size==1,则说明是新函数，自动注册函数信息，以便于为return语句提供函数签名等信息
         */
        if (csi_size==1) {
            func_name=get_new_name(PREFIX_CODE_ADDRESS)+"_func";
        }
        csi_size++;
        cout<<"csi: size="<<get_csi_size()<<endl;
        top().show();
        
    }
    Symble_block_t& top(){
        return symble_stack.top();
    }
    /**
     *  栈层次结构出栈，删除一个层次。
     *  return 出栈层次的名字
     */
    string pop(){
        //维护函数名
        if(csi_size==1){
            func_name="";
        }
        top().show();
        symble_stack.pop();
        csi_size--;
        cout<<"csi: size="<<get_csi_size()<<endl;
        return csi_stack[csi_size];
    }
    /**
     * 向符号表栈顶的层次中提交一个新的符号信息。内部若发现名字重复会exit，故需要手动提前查询top_count_symble_node_by_name(string name)方法防止相同命名空间的名字重复，并进行错误提示。
     */
    void push_symble(Symble_node_t symble_node){
        if(top_count_symble_node_by_name(symble_node.get_name())){
            cout<<"error:push_symble.top_count_symble_node_by_name(symble_node.name)==true."<<endl;
            cout<<"new symble_node="<<endl;
            symble_node.show();
            cout<<endl;
            cout<<"old symble_node="<<endl;
            get_symble_node_by_name(symble_node.get_name()).show();
            cout<<"exit due to error."<<endl;
            exit(-1);
        }
        Symble_block_t block = symble_stack.top();
        block.push(symble_node);
        block.set_csi(csi_stack, csi_size);
        symble_stack.pop();
        symble_stack.push(block);
    }
    /**
     *  获取一个字符串对应的变量，若字符串还没有注册会自动注册。
     */
    Symble_node_t get_global_str_name(string str){
        string name;
        Symble_block_t block = symble_stack.get_by_idx(0);
        if(str_name_map.count(str)){
            name = str_name_map[str];
        }
        else{
            name=get_new_name(PREFIX_STRING);
            str_name_map[str]=name;
            Symble_node_t new_symble(name,VAR_SYMBLE,0,0);
            new_symble.set_const(true);
            new_symble.str_val=str;
            block.push(new_symble);
        }
        return get_symble_node_by_name(name);
    }
    /**
     * 判断是否可以使用return语句
     */
    bool return_enable(){
        return func_name.length()!=0;
    }
    /**
     *  获取当前函数的return标号,调用前要确定是否可以return，否则会exit(-1)
     */
    string get_return_sign(){
        if(!return_enable()){
            cout<<"error:get_return_sign_of_func:return unable"<<endl;
            exit(-1);
        }
        return func_name+"_return";
    }
    /*
         while_continue:
        code_of_condition
         if condition yes: goto then
         goto whlie_break;
         while_then:
         ...
         goto while_continue;
         while_break:
     */
    bool continue_enable(){
        return loop_stack.size()!=0;
    }
    bool break_enable(){
        return continue_enable();
    }
    string get_while_then_sign(){
        if(!continue_enable()){
            cout<<"error:get_while_then_sign:while_then unable"<<endl;
            exit(-1);
        }
        return loop_stack.top()+"_while_then";
    }
    string get_continue_sign(){
        if(!continue_enable()){
            cout<<"error:get_continue_sign:continue unable"<<endl;
            exit(-1);
        }
        return loop_stack.top()+"_continue";
    }
    string get_break_sign(){
        if(!break_enable()){
            cout<<"error:get_break_sign:break unable"<<endl;
            exit(-1);
        }
        return loop_stack.top()+"_break";
    }
    void push_loop(){
        loop_stack.push(get_new_name(PREFIX_CODE_ADDRESS)+"_loop");
    }
    void pop_loop(){
        if(!loop_stack.size()){
            cout<<"error:pop_loop:loop_stack.size==0"<<endl;
            exit(-1);
        }
        loop_stack.pop();
    }
    /**
    if condition ? yes:goto then
     else:
        ...then code...
     goto endif
     then:
        ...else code...
     endif:
     */

    string get_then_sign(){
        if (!if_stack.size()) {
            cout<<"error:get_then_sign:if_stack.size()==0"<<endl;
            exit(-1);
        }
        return if_stack.top()+"_then";
    }
    string get_else_sign(){
        if (!if_stack.size()) {
            cout<<"error:get_else_sign:if_stack.size()==0"<<endl;
            exit(-1);
        }
        return if_stack.top()+"_else";
    }
    string get_endif_sign(){
        if (!if_stack.size()) {
            cout<<"error:get_endif_sign:if_stack.size()==0"<<endl;
            exit(-1);
        }
        return if_stack.top()+"_endif";
    }
    void push_if(){
        if_stack.push(get_new_name(PREFIX_CODE_ADDRESS)+"_if");
    }
    void pop_if(){
        if(!if_stack.size()){
            cout<<"error:pop_if:if_stack.size==0"<<endl;
            exit(-1);
        }
        if_stack.pop();
    }
};
#endif /* symble_h */
