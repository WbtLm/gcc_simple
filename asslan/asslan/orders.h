int order_addi(int arg,char*args[]);
int order_addu(int arg,char*args[]);
int order_and(int arg,char*args[]);
int order_andi(int arg,char*args[]);
int order_beq(int arg,char*args[]);
int order_bgtz(int arg,char*args[]);
int order_bne(int arg,char*args[]);
int order_j(int arg,char*args[]);
int order_jal(int arg,char*args[]);
int order_jalr(int arg,char*args[]);
int order_jr(int arg,char*args[]);
int order_lui(int arg,char*args[]);
int order_lw(int arg,char*args[]);
int order_mfhi(int arg,char*args[]);
int order_mflo(int arg,char*args[]);
int order_mthi(int arg,char*args[]);
int order_mtlo(int arg,char*args[]);
int order_mult(int arg,char*args[]);
int order_multu(int arg,char*args[]);
int order_nor(int arg,char*args[]);
int order_or(int arg,char*args[]);
int order_ori(int arg,char*args[]);
int order_sll(int arg,char*args[]);
int order_slt(int arg,char*args[]);
int order_sltu(int arg,char*args[]);
int order_sra(int arg,char*args[]);
int order_srl(int arg,char*args[]);
int order_sub(int arg,char*args[]);
int order_subu(int arg,char*args[]);
int order_sw(int arg,char*args[]);
int order_xor(int arg,char*args[]);
int order_xori(int arg,char*args[]);

int order_add(int arg,char*args[]);
#include<map>
#include<string>
#include"head.h"
using namespace std;
typedef int(*order_func_t)(int,char*[]);
class order_guidence{
    string error_info="";
	map<string,order_func_t>func_mp;
    order_guidence()=default;
	static order_guidence *shared_impl;
public:
    bool is_error(){
        return error_info.length()!=0;
    }
    string get_error_info(){
        return error_info;
    }
	void regist_order(string order_name,order_func_t order_func){
		if(func_mp.count(order_name)){
			cout<<"error:order_name has already exist"<<endl;
			cout<<order_name<<endl;
			exit(-1); 
		}
		func_mp[order_name]=order_func;
	}
	order_func_t get_order_func(string order_name){
        error_info = "";
		if(!func_mp.count(order_name)){
			cout<<"error:order_name is not exist"<<endl;
			cout<<order_name<<endl;
            error_info="error:order_name is not exist";
            return NULL;
		}
		return func_mp[order_name];
	}
	static order_guidence* get_shared_impl(){
		if(shared_impl==NULL){
			return shared_impl = new order_guidence();
            
		}
		return shared_impl;
	}
};
order_guidence *order_guidence::shared_impl=NULL;
void order_regist(){
	order_guidence &impl = *order_guidence::get_shared_impl();
	impl.regist_order("addi",order_addi);
	impl.regist_order("addu",order_addu);
	impl.regist_order("and",order_and);
	impl.regist_order("andi",order_andi);
	impl.regist_order("beq",order_beq);
	impl.regist_order("bgtz",order_bgtz);
	impl.regist_order("bne",order_bne);
	impl.regist_order("j",order_j);
	impl.regist_order("jal",order_jal);
	impl.regist_order("jalr",order_jalr);
	impl.regist_order("jr",order_jr);
	impl.regist_order("lui",order_lui);
	impl.regist_order("lw",order_lw);
	impl.regist_order("mfhi",order_mfhi);
	impl.regist_order("mflo",order_mflo);
	impl.regist_order("mthi",order_mthi);
	impl.regist_order("mtlo",order_mtlo);
	impl.regist_order("mult",order_mult);
    impl.regist_order("nor",order_nor);
    impl.regist_order("or",order_or);
    impl.regist_order("ori",order_ori);
    impl.regist_order("sll",order_sll);
    impl.regist_order("slt",order_slt);
    impl.regist_order("sltu",order_sltu);
    impl.regist_order("sra",order_sra);
    impl.regist_order("srl",order_srl);
    impl.regist_order("sub",order_sub);
    impl.regist_order("subu",order_subu);
    impl.regist_order("sw",order_sw);
    impl.regist_order("xor",order_xor);
    impl.regist_order("xori",order_xori);
    impl.regist_order("add",order_add);


}
/*

*/



