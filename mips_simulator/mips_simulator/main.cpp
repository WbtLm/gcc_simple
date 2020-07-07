/**
实现的指令： 
lui	1
ori	1
addi	1
or	1
jal	1
jalr	1
sw	1
lw	1
add	1
slt	1
jr	1
j	1
bne	1
*/ 
#include <iostream>
#include <stack>
#include <iostream>
#include <vector>
#include <map>
#include <set>
#include <cstring>
#include <sstream>
using namespace std;
typedef unsigned int uint;
void exit_error(int ret);
class Reg_ctrl{
	uint regs[32];
    uint hi;
    uint lo;
	Reg_ctrl(){
		regs[0]=0;
	}
	static Reg_ctrl* shared_impl;
public:
	static Reg_ctrl* get_shared_impl(){
		if(shared_impl){
			return shared_impl;
		}
		return shared_impl=new Reg_ctrl();
	}
	uint get_reg(int reg){
		return regs[reg];
	}
	void set_reg(int reg,uint val){
		if(reg==0){
			cout<<"reg_ctrl::set_reg:不能修改r_zero"<<endl;
			exit_error(0);
		}
		regs[reg]=val;
	}
    void set_hi(uint val){
        hi=val;
    }
    void set_lo(uint val){
        lo=val;
    }
    uint get_hi(){
        return hi;
    }
    uint get_lo(){
        return lo;
    }
};
Reg_ctrl* Reg_ctrl::shared_impl=NULL;
class Mem_ctrl{
	uint mem[1024 * 1024];//1M
	static Mem_ctrl* shared_impl;
	Mem_ctrl(){
		
	}
public:
	static Mem_ctrl* get_shared_impl(){
		if(shared_impl){
			return shared_impl;
		}
		return shared_impl=new Mem_ctrl();
	}
	uint get_mem(uint addr){
		if(addr==0){
			cout<<"Mem_ctrl:get_mem:addr == 0"<<endl;
			exit_error(0);
		}
		if(addr&0x3){
			cout<<"Mem_ctrl:get_mem:addr&0x3 != 0 "<<endl;
			cout<<addr<<endl;
			exit_error(0);
		}
        if(addr>=1024*1024) {
            cout<<"Mem_ctrl:get_memif(addr>:addr too large"<<endl;
            cout<<addr<<endl;
            exit_error(0);
        }
		return mem[addr>>2];
	}
	void set_mem(uint addr,uint val){
		if(addr==0){
			cout<<"Mem_ctrl:set_mem:addr == 0"<<endl;
			exit_error(0);
		}
		if(addr>=1024*1024) {
			cout<<"Mem_ctrl:set_mem:addr too large"<<endl;
			cout<<addr<<endl;
			exit_error(0);
		}
		mem[addr>>2]=val;
		
	}
};
Mem_ctrl* Mem_ctrl::shared_impl=NULL;
class Ins_node{
	uint ins;
	string ins_str;
	uint idx=0;
	string note;
public:
	void set_note(string str){
		note=str;
	}
	string get_str(){
		return to_string(idx)+" "+note;
	}
	Ins_node(){
		ins_str="";
		ins=0;
	}
	void set_ins(uint idx,uint _ins){
		this->idx=idx;
		ins=_ins;
		for(int i=31;i>=0;i--){
			ins_str +=string("") + to_string(!!(ins&(1<<i)));
		}
	}
	Ins_node(uint idx,uint _ins):Ins_node(){
		set_ins(idx,_ins);
	}
	string get_ins_range(int r,int l){
		int length=r-l+1;
		int left=31-r;
		return ins_str.substr(left,length);
	}
};

class Ins_ctrl{
	 
	Ins_node ins_lst[1024*1024];
	string src_lst[1024*1024];
	static Ins_ctrl* shared_impl;
	Ins_ctrl(){
		
	}
	string mcode;
	int pos=0;
	string get_next(int &pos){
	    if(mcode.length() && mcode[mcode.length()-1]!='\n'){
	        mcode=mcode+"\n";
	    }
	    if(pos>=(int)mcode.length()){
	        cout<<"pos越界"<<endl;
	        exit_error(-1);
	    }
	    string ret;
	    while (1) {
	        ret="";
	        int i;
	        for(i=pos;i<(int)mcode.length();i++){
	            if(mcode[i]=='\n'){
	                ret = mcode.substr(pos,i-pos);
	                break;
	            }
	        }
	        pos=i+1;
	        if(ret.length()){
	            break;
	        }
	    }
	    for(int i=(int)ret.length()-1;i>=0;i--){
	        if(ret[i]!=' ' && ret[i]!='\t'){
	            ret=ret.substr(0,i+1);
	            break;
	        }
	    }
	    return ret;
	}
public:
	string get_src(uint idx){
		if(idx&0x3){
			cout<<"Ins_ctrl:get_src:idx&0x3 != 0"<<endl;
			cout<<idx<<endl;
			exit_error(0); 
		}
		return src_lst[idx>>2];
	}
	static Ins_ctrl* get_shared_impl(){
		if(shared_impl){
			return shared_impl;
		}
		return shared_impl=new Ins_ctrl();
	}
	int init_with_ins_str(string ins_str){
		mcode=ins_str; 
		pos=0;
		uint idx=0;
		while(pos<(int)ins_str.length()){
			string line=get_next(pos);
			src_lst[idx]=line;
			string note = line;
            if(line.length()>2 && line.substr(0,2)=="//"){
                
                continue;
            }
			//消去32'h 
			if(line.substr(0,4)=="32'h"){
				line=line.substr(4,line.length()-4);
			}
			//剪掉分号以后的东西 
			for(int i=0;i<(int)line.length();i++){
				if(line[i]==';'){
					line=line.substr(0,i);
					break;
				}
			}
			//十六进制转二进制
			uint code;
			sscanf(line.c_str(),"%x",&code);
			
			if(line.length()==0){
				cout<<"code.len==0"<<endl;
				cout<<note<<endl;
				exit_error(0);
			}
			cout<<code<<endl;
			ins_lst[idx].set_ins(idx,code);
			idx++;
		} 
		return idx;
	}
	Ins_node get_ins_by_idx(uint idx){
		if(idx&0x3){
			cout<<"Ins_ctrl:get_ins_by_idx:mem idx&0x3 != 0"<<endl;
			cout<<idx<<endl;
			exit_error(0); 
		}
		return ins_lst[idx>>2];
	}
	
}; 
Ins_ctrl* Ins_ctrl::shared_impl=NULL; 
int strtoi(string s){
	int ret=0;
	for(int i=0;i<(int)s.length();i++){
		ret<<=1;
		if(s[i]=='1'){
			ret|=1;
		}
	}
	return ret;
}
#define _op_ 31,26
#define _rs_ 25,21
#define _rt_ 20,16
#define _rd_ 15,11
#define _shamt_ 10,6
#define _func_ 5,0
#define _imm_ 15,0
#define _address_ 25,0
#define _offset_ 15,0
uint pc=0;
Reg_ctrl &reg_ctrl=*Reg_ctrl::get_shared_impl();
Mem_ctrl &mem_ctrl=*Mem_ctrl::get_shared_impl();
Ins_ctrl &ins_ctrl=*Ins_ctrl::get_shared_impl();
uint sign_ex(uint &val_16){
	val_16&=0xffff;
	if((val_16>>15)&1){
		return val_16|=~0xffff;
	}
	else{
		return val_16;
	}
} 
void show();
int run_R(Ins_node ins){
	string ins_func = ins.get_ins_range(_func_);
	int rs = strtoi(ins.get_ins_range(_rs_));
	int rt = strtoi(ins.get_ins_range(_rt_));
	int rd = strtoi(ins.get_ins_range(_rd_));
	uint a=reg_ctrl.get_reg(rs);
	uint b=reg_ctrl.get_reg(rt);
	if(ins_func == "100101"){ //or
		uint val= a | b;
		reg_ctrl.set_reg(rd,val);
	}
	else if(ins_func == "100000"){ //add
		uint val= a + b;
		reg_ctrl.set_reg(rd,val);	
	}
	else if(ins_func == "001000"){ //jr
		pc=a;
	}
    else if(ins_func == "101010"){ //slt
        int inta,intb;
        inta=a;
        intb=b;
        if(inta<intb){
            reg_ctrl.set_reg(rd,1);
        }
        else{
            reg_ctrl.set_reg(rd,0);
        }
    }
    else if(ins_func == "101011"){ //sltu
        if(a<b){
            reg_ctrl.set_reg(rd,1);
        }
        else{
            reg_ctrl.set_reg(rd,0);
        }
    }
	else if(ins_func == "001001"){ //jalr
		//rs为跳转地址reg_addr，rd保存断点 
		reg_ctrl.set_reg(rd,pc);
		pc=reg_ctrl.get_reg(rs);
	}
    else if(ins_func == "011000"){ //mult
        unsigned long long va=a;
        unsigned long long vb=b;
        unsigned long long ans;
        ans=va*vb;
        uint lo=ans&0xffffffff;
        uint hi=ans>>16;
        reg_ctrl.set_lo(lo);
        reg_ctrl.set_hi(hi);
    }
    else if(ins_func == "011000"){ //mult
        unsigned long long va=a;
        unsigned long long vb=b;
        unsigned long long ans;
        ans=va*vb;
        uint lo=ans&0xffffffff;
        uint hi=ans>>16;
        reg_ctrl.set_lo(lo);
        reg_ctrl.set_hi(hi);
    }
    else if(ins_func == "010010"){ //mflo
        uint lo = reg_ctrl.get_lo();
        reg_ctrl.set_reg(rd, lo);
    }
    else if(ins_func == "100010"){ //sub
        uint val= a - b;
        reg_ctrl.set_reg(rd,val);
    }
	else{
        cout<<"ins_R:ins_func not found"<<endl;
		cout<<ins.get_str()<<endl;
        cout<<ins_func<<endl;
		exit_error(0);
	}
    return 0;
}
int run(Ins_node ins){
	string ins_op = ins.get_ins_range(_op_);
	uint imm = strtoi(ins.get_ins_range(_imm_));
	imm&=0xffff;
	int rs = strtoi(ins.get_ins_range(_rs_));
	int rt = strtoi(ins.get_ins_range(_rt_));
	int addr=strtoi(ins.get_ins_range(_address_));
	
	uint vala=reg_ctrl.get_reg(rs);
	uint valb=reg_ctrl.get_reg(rt); 
	if(ins.get_ins_range(_op_) == "000000"){
		return run_R(ins);
	}
	else if(ins_op == "001111"){ //lui
		imm<<=16;
		imm&=~0xffff;
		reg_ctrl.set_reg(rt,imm);
	}
	else if(ins_op == "001101"){ //ori
		
		uint val = reg_ctrl.get_reg(rs) | imm;
		reg_ctrl.set_reg(rt,val);
	}
	else if(ins_op == "001000"){ //addi
		uint val = reg_ctrl.get_reg(rs)+sign_ex(imm);
		reg_ctrl.set_reg(rt,val);
	}
	else if(ins_op == "000011"){ //jal
		reg_ctrl.set_reg(31,pc);
		uint val=pc&~0x0fffffff;
		addr<<=2;
		addr&=0x0fffffff;
		addr|=val;
		pc=addr;
	}
	else if(ins_op == "000101"){ //bne 
		if(vala != valb){
			imm<<=2;
			imm=sign_ex(imm);
			pc=imm;
		}
	} 
	else if(ins_op == "000010"){ //j
		uint val=pc&~0x0fffffff;
		addr<<=2;
		addr&=0x0fffffff;
		addr|=val;
		pc=addr;
	}
	else if(ins_op == "100011"){ //lw
		uint mem_addr = vala+sign_ex(imm);
		uint val = mem_ctrl.get_mem(mem_addr);
		reg_ctrl.set_reg(rt,val); 
	}
	else if(ins_op == "101011"){ //sw
		uint mem_addr = vala+sign_ex(imm);
		mem_ctrl.set_mem(mem_addr,valb);
	}
	else{
		cout<<"未定义指令"<<endl;
		cout<<ins.get_str()<<endl; 
		exit_error(0);
	}
    return 0;
}
bool is_num(string str){
	for(int i=0;i<(int)str.length();i++){
		if(!(str[i]>='0' && str[i]<='9')){
			return false;
		}
	}
	return true;
}
bool reg_show_book[32]={0};
vector<string>mem_name;
vector<int>mem_addr;
vector<string>mem_name_s;
vector<int>mem_addr_s;
int debug(){	
	cout<<"====>";
	char order[1024];
	string s;
	gets(order);
	cout<<endl;
	int len=strlen(order);
	s=order;
	stringstream in(order);
	string fst,snd,trd;
	in>>fst;
	if(len==0){
		return 1;
	}
    if(fst=="bc"){
        int num;
        if(in>>snd){
            if(is_num(snd)){
                num=atoi(snd.c_str());
                cout<<num<<endl;
            }
        }
        return 0;
    }
	if(fst=="sr"){
		for(int i=0;i<32;i++){
			printf("r%2d %d\n",i,reg_ctrl.get_reg(i));
		}
		cout<<endl;
		return 0;
	}
	if(fst=="r"){
        int num=1;
        if(in>>snd){
            if(is_num(snd)){
                num=atoi(snd.c_str());
            }
        }
		return num;
	}
	if(fst=="rr"){
		return 100;
	}
	if(fst=="rrr"){
		return 1000;
	}
	if(fst=="c"){
		return -1; 
	} 
	if(fst=="exit"){
		exit(0);
	}
	if(fst=="pr"){
		while(in>>snd){
			if(is_num(snd)){
				int num=atoi(snd.c_str());
				if(num>=0 && num<=31){
					reg_show_book[num]=!reg_show_book[num];
                    cout<<"success"<<endl;
				}
                else{
                    cout<<"reg invalid"<<endl;
                }
			}
            else{
                cout<<"snd is not num"<<endl;
            }
		}
		return 0;
	}
	if(fst=="sh"){
		show();
		return 0;
	}
	if(fst=="pm"){
		while(in>>snd>>trd){
			
			if(is_num(snd)){
				int num=atoi(snd.c_str());
				if(num>0){
					mem_addr.push_back(num);
					mem_name.push_back(trd); 
					cout<<"success"<<endl;
				}
			}
		}
		return 0;
	}
    if(fst=="ps"){
        while(in>>snd>>trd){
            
            if(is_num(snd)){
                int num=atoi(snd.c_str());
                if(num>=0){
                    mem_addr_s.push_back(num);
                    mem_name_s.push_back(trd);
                    cout<<"success"<<endl;
                }
            }
        }
        return 0;
    }
    
	cout<<"wrong order"<<endl;
	return 0;
}
void exit_error(int ret){
	while(1){
		debug();
	}
} 
void show(){
    
	for(int i=0;i<32;i++){
		if(reg_show_book[i])
			printf("r%2d\t%u\n",i,reg_ctrl.get_reg(i));
	}
	for(int i=0;i<(int)mem_addr.size();i++){
		printf("<%d>\t%s\t%d\n",mem_addr[i],mem_name[i].c_str(),mem_ctrl.get_mem(mem_addr[i]));
	}
    int addr;
    for(int i=0;i<(int)mem_addr_s.size();i++){
        addr =-mem_addr_s[i]+reg_ctrl.get_reg(30);
        printf("<%d>\t%s\t%d\n",addr,mem_name_s[i].c_str(),mem_ctrl.get_mem(addr));
    }
    
}
int main(){
	string code,tmp;
	char c;
	FILE* fp = fopen("/Users/dawn/f/lan/run.c","rt");
	if(fp==NULL){
		cout<<"fp==null"<<endl;
		return 0;
	}
	while((c=fgetc(fp))!=EOF){
		code+=c;
	}
	cout<<"========="<<endl;
	uint ins_num=ins_ctrl.init_with_ins_str(code);
	pc=0;
	//r_sp = 1023;
	//r_fp = 1024
#define TREM_ADDR 0x00fffff
	reg_ctrl.set_reg(29,1023*4);	//r_sp
	reg_ctrl.set_reg(30,1024*4);	//r_fp
	reg_ctrl.set_reg(31,TREM_ADDR);//r_ra==trem_addr
    cout<<"trem addr(pc)="<<TREM_ADDR<<endl;
	int cnt=0;
	while(1){
		if(pc/4>=ins_num){
			cout<<"pc out of range"<<endl;
			cout<<pc<<endl;
			break;
		}
		Ins_node ins = ins_ctrl.get_ins_by_idx(pc); 
		cout<<ins.get_str()<<" //"<<ins_ctrl.get_src(pc)<<endl;
		while(cnt==0){
			
			cnt=debug();
		}
		cnt--;
		pc+=4;
		run(ins);show();
		if(pc>=0x00fffff){
			cout<<"end"<<endl;
			break;
		}
		
	}
    if(pc!=TREM_ADDR){
        cout<<"error:pc != TREM_ADDR "<<TREM_ADDR<<endl;
    }
    else{
        cout<<"======================="<<endl;
        cout<<"success:pc == TREM_ADDR "<<endl;
        cout<<"exit with return code:"<<reg_ctrl.get_reg(2)<<endl;//r_ret_1
        cout<<"======================="<<endl;
    }
	while(1){
		printf("pc=%u\n",pc);
		debug();
	}

} 





