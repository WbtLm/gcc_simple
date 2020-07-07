//
//  main.cpp
//  test
//
//  Created by dawn on 2019/11/1.
//  Copyright © 2019 chuyi. All rights reserved.
//

#include<iostream>
#include<vector>
#include<map>
#include<set>
#include"syntax_analyze_matrix.h"
using namespace std;
string lan;
int *m_start_idx=NULL;
map<string,int>echoMap;//VN and VT -> int
int echo_str2num(string str){
    if(echoMap.count(str))
        return echoMap[str];
    return -1;
}
vector<string>num2str;
vector<string>m_num2str;//非终结符，从1开始

vector<set<int>>first_set_lst,follow_set_lst;
string echo_num2str(int num){
    if(num>=10000){
        if(num-10000>=num2str.size() || num-10000<0){
            cout<<"error:echo_num2str:num-10000>size || <0"<<endl;
            cout<<num<<endl;
            exit(-1);
        }
        return num2str[num-10000];
    }
    else{
        if(num>=m_num2str.size() || num<0){
            cout<<"error:echo_num2str:num>size||<0"<<endl;
            cout<<num<<endl;
            exit(-1);
        }
        return m_num2str[num];
    }
}

void production_t::show(){
    int sz=(int)detail.size();
    cout<<m_num2str[left]<<"->";
    for(int i=0;i<sz;i++){
        
        cout<<echo_num2str(detail[i]);
    }
    cout<<endl;
}
vector<production_t>production;
int get_num_of_prod(){
    return (int)production.size();
}

production_t get_prod_by_idx(int idx){
    if(idx>=production.size()){
        cout<<"get_prod_by_idx idx >= size"<<endl;
        cout<<idx<<endl;
        exit(-1);
    }
    return production[idx];
}
bool operator<(production_t a,production_t b){
    if(a.left==b.left){
        return a.val<b.val;
    }
    return a.left<b.left;
}
int prod_ip=1;//产生式编号
int ip=10001;//终结符从10001开始
int m_ip=1;//非终结符 最大9999
//>=10001的''符有ip-10001个
//>=1的'm'符有m_ip-1个
inline int get_num(){
    return ip-10001;
}

inline int get_m_num(){ //不包含初始符号G[S']:S'
    return m_ip-1;
}
inline int get_m_max_idx(){
    return get_m_num();
}
inline bool is_m(int v){
    return v<10000;
}
inline bool is_not_m(int v){
    return !is_m(v);
}
inline bool isN(int v){ //Vn 终结符
    return is_not_m(v);
}
void show_first(int val){
    cout<<echo_num2str(val)<<".first_set={"<<endl;
    for(auto it=first_set_lst[val].begin();it!=first_set_lst[val].end();++it){
        cout<<echo_num2str(*it)<<" , ";
    }
    cout<<endl<<"}"<<endl;
}
void show_follow(int val){
    cout<<echo_num2str(val)<<".follow_set={"<<endl;
    for(auto it=follow_set_lst[val].begin();it!=follow_set_lst[val].end();++it){
        cout<<echo_num2str(*it)<<" , ";
    }
    cout<<endl<<"}"<<endl;
}
void prod_show(production_t prod){
    int sz=(int)prod.detail.size();
    cout<<m_num2str[prod.left]<<"->";
    for(int i=0;i<sz;i++){
        cout<<echo_num2str(prod.detail[i]);
    }
}
int analize_lan_txt(){
    num2str.push_back("");
    m_num2str.push_back("S'");
    int id;
    int i=0;
    int maxlen=(int)lan.length();
    char con=0;
    int flag=1;
    int line_number=1;
    int pos_x=1;
    int startIdx=0;
    int prod_left=-1;
    int prod_con=0;
    production_t prod;
    prod.val=0;
    prod.left=0;
    prod.detail.push_back(1);
    production.push_back(prod);
    prod.detail.clear();
    while (i<maxlen) {
        for(flag=1;i<maxlen && flag;i++,pos_x++){
            switch (lan[i]) {
                
                case '-':
                    if(prod_con!=0){
                        cout<<i<<" line:"<<line_number<<" position:"<<pos_x<<endl<<"error:unexpected symbol "<<lan[i]<<endl<<"is ';' expected in the end of line "<<line_number-1<<"?"<<endl;
                        return -1;
                    }
                    //no break;
                case '<':
                case '\'':
                case '[':
                    startIdx=i;
                    con=lan[i];
                    flag=0;
                    break;
                case ' ':case '\t':
                    break;
                case '\n':
                    line_number++;
                    pos_x=1;
                    break;
                case '|':
                    if(prod_con!=1){
                        cout<<i<<" line:"<<line_number<<" position:"<<pos_x<<endl<<"error:unexpected symbol |"<<endl;
                        return -1;
                    }
                    if(i+1<lan.length() && lan[i+1]=='|'){
                        cout<<i<<" line:"<<line_number<<" position:"<<pos_x+1<<endl<<"error:unexpected symbol |"<<endl;
                        return -1;
                    }
                    prod.left=prod_left;
                    prod.val=prod_ip++;
                    production.push_back(prod);
                    prod.detail.clear();
                    break;
                case ';':
                    if(con){
                        cout<<i<<" line:"<<line_number<<" position:"<<pos_x<<endl<<"error3:unexpected: "<<lan[i]<<endl;
                        return -1;
                    }
                    if(prod_con!=1){
                        cout<<i<<" line:"<<line_number<<" position:"<<pos_x<<endl<<"warning:unexpected: "<<lan[i]<<endl;
                        break;
                    }
                    prod.left=prod_left;
                    prod.val=prod_ip++;
                    production.push_back(prod);
                    prod.detail.clear();
                    prod_con=0;
                    prod_left=-1;
                    break;
                default:
                    cout<<"line:"<<line_number<<" position:"<<pos_x<<endl<<"error2:wrong symbol:"<<lan[i]<<endl;
                    cout<<"info:"<<con<<endl;
                    return -1;
            }
        }
        for(flag=1;i<maxlen && flag;i++,pos_x++){
            char ch=lan[i];
            if(con=='\'' || con=='\"')
                if(ch!='\'' && ch!='\"')
                    ch=0;
            switch (ch) {
                case '>':
                    if(con!='<'&&con!='-'){
                        cout<<i<<" line:"<<line_number<<" position:"<<pos_x<<endl<<"error1:unexpected: "<<lan[i]<<endl;
                        cout<<"info:"<<con<<endl;
                        return -1;
                    }
                    flag=0;
                    if(con=='<'){
                        string str=lan.substr(startIdx,i-startIdx+1);
                        if(echoMap.count(str)==0){
                            id=m_ip;
                            echoMap[str]=m_ip++;
                            //                   cout<<"add: "<<str<<endl;
                            m_num2str.push_back(str);
                        }
                        else{
                            id=echoMap[str];
                        }
                        //===
                        if(prod_con==0){
                            prod_left=id;
                            prod.val=0;
                            prod.detail.clear();
                        }
                        else if(prod_con==1){
                            prod.detail.push_back(id);
                        }
                    }
                    else if(con=='-'){
                        prod_con=1;
                    }
                    con=0;
                    break;
                case '\'':
                    if(con!='\''){
                        cout<<"line:"<<line_number<<" position:"<<pos_x<<endl<<"error:unexpected :"<<lan[i]<<endl;
                        cout<<"info:"<<con<<endl;
                        
                        return -1;
                    }
                {
                    flag=0;
                    string str=lan.substr(startIdx,i-startIdx+1);
                    if(echoMap.count(str)==0){
                        id=ip;
                        echoMap[str]=ip++;
                        //                  cout<<"add "<<ip-1<<": "<<str<<endl;
                        num2str.push_back(str);
                    }
                    else{
                        id=echoMap[str];
                    }
                    if(prod_con==0){
                        cout<<i<<" line:"<<line_number<<" position:"<<pos_x-str.length()<<endl<<"error:unexpected NT in the left of prodution: "<<str<<endl;
                        return -1;
                    }
                    else if(prod_con==1){
                        prod.detail.push_back(id);
                    }
                    con=0;
                }
                    break;
                case ']':
                    if(con!='['){
                        cout<<"line:"<<line_number<<" position:"<<pos_x<<endl<<"error:wrong symbol:"<<lan[i]<<endl;
                        cout<<"info:"<<con<<endl;
                        return -1;
                    }
                {
                    flag=0;
                    string str=lan.substr(startIdx,i-startIdx+1);
                    if(echoMap.count(str)==0){
                        id=ip;
                        echoMap[str]=ip++;
                        //                   cout<<"add: "<<str<<endl;
                        num2str.push_back(str);
                    }
                    else{
                        id=echoMap[str];
                    }
                    if(prod_con==0){
                        cout<<i<<" line:"<<line_number<<" position:"<<pos_x-str.length()<<endl<<"error:unexpected NT in the left of prodution: "<<str<<endl;
                        return -1;
                    }
                    else if(prod_con==1){
                        prod.detail.push_back(id);
                    }
                    con=0;
                }
                    break;
                case ' ':case '\t':
                    break;
                case '\n':
                    line_number++;
                    pos_x=1;
                    break;
                    
                default:
                    
                    break;
            }
        }
        
    }
    if(con!=0 || prod_con!=0){
        cout<<"line:"<<line_number<<" position:"<<pos_x<<endl<<"error:Unexpected end"<<lan[i]<<endl;
        cout<<"info:"<<con<<endl;
        return -1;
    }
    return 0;
}
struct prod_externed_t{
    int val;    //项目对应的产生式
    int idx;    //点的位置
    bool operator!=(struct prod_externed_t a){
        if(a.idx==idx && a.val==val)
            return true;
        return false;
    }
    void show(){
        int sz=(int)production[val].detail.size();
        cout<<m_num2str[production[val].left]<<"->";
        for(int i=0;i<sz;i++){
            if(i==idx){
                cout<<'.';
            }
            cout<<echo_num2str(production[val].detail[i]);
        }
        if(idx==sz){
            cout<<'.';
        }
        cout<<endl;
    }
};
//bool operator==(struct prod_externed_t a, struct prod_externed_t b){
//    return !(a!=b);
//}
bool operator<(struct prod_externed_t a, struct prod_externed_t b){
    if(a.val==b.val){
        return a.idx<b.idx;
    }
    return a.val<b.val;
}
typedef vector<prod_externed_t> prod_set_t;//项目集
void prod_set_t_show(prod_set_t prod_set){
    int sz=(int)prod_set.size();
    for(int i=0;i<sz;i++){
        prod_set[i].show();
    }
}
//bool operator<(prod_set_t a,prod_set_t b){
//    if(a.size()==b.size()){
//        int sz=(int)a.size();
//        for(int i=0;i<sz;i++){
//            if(a[i]!=b[i]){
//                return a[i]<b[i];
//            }
//        }
//    }
//    return a.size()<b.size();
//}
map<prod_set_t,int>set2num;
struct edge_t{
    int l;
    int r;
    int val;
    int type;//0是归约，1是移进
    void show(){
        cout<<l<<" -> "<<r<<"  : "<<(type?"R":"S")<<" "<<echo_num2str(val)<<endl;
    }
};
vector<edge_t>edge_lst;
vector<prod_set_t>prod_set_lst;//项目集列表，BFS的队列

bool is_termianl_condition(prod_set_t st){
    for (int i=0; i<(int)st.size();i++) {
        production_t prod = production[st[i].val];
        if(prod.detail.size()!=st[i].idx){
            return false;
        }
    }
    return true;
}
/*
 求解DFA之前，必须保证已经求得follow集
 
 
 */

int create_prod_set_DFA(){

    int num;
    int m_num;
    m_num=get_m_num();
    num=get_num();
    prod_set_lst.clear();
    set2num.clear();
    edge_lst.clear();
    production_t prod;
    prod_set_t st;
    prod_set_t st_new;
    prod_externed_t ex;
    ex.val=0;
    ex.idx=0;
    st.push_back(ex);
    set<prod_externed_t>book;
    
    //BFS
    prod_set_lst.push_back(st);
    int head=0;
    int step=1;
    vector<prod_set_t>prod_set_lst_middle;
    vector<int>edge_middle;     //goto
    vector<int>action_middle;  //action
    prod_set_lst_middle.push_back(st);
    edge_middle.push_back(0);
    action_middle.push_back(ACTION_S);
    while(head<(int)prod_set_lst.size()){
        st=prod_set_lst[head];//选择新的项目集
#ifdef LOG_MATRIX_ENABLE
        cout<<"选择新项目集："<<head<<"."<<endl;
        prod_set_t_show(st);
#endif
        if(is_termianl_condition(st)){
#ifdef LOG_MATRIX_ENABLE
            cout<<"项目集已是终态"<<endl;
#endif
            step=0;
            head++;
            continue;
        }
#ifdef LOG_MATRIX_ENABLE
        cout<<endl;
#endif
        for(;step<2;step++){
            if(step==0){
                //产生新的项目集
                int edge_idx;
                edge_middle.clear();
                action_middle.clear();
                prod_set_lst_middle.clear();
                //>=10001的''符有ip-10001个
                //>=1的'm'符有m_ip-1个
                //先判断是否可归约
                int r_val=-1;
                for(int i=0;i<(int)st.size();i++){
                    prod_externed_t ex = st[i];
                    production_t prod = production[ex.val];
                    if(ex.idx == prod.detail.size()){
                        //归约
                        if(r_val==-1){
                            //无归约冲突
                            r_val=ex.val;
#ifdef LOG_MATRIX_ENABLE
                            cout<<"找到一处归约:"<<endl;
                            ex.show();
#endif
                        }
                        else{
                            cout<<"Error:归约-归约冲突，没有实现解决冲突的策略"<<endl;
                            ex.show();
                            return -1;
                        }
                    }
                    
                }
                for(int i=1;i<=m_num+num;i++){//枚举所有可能产生的边
                    //先枚举非终结符m_num个，再枚举终结符
                    if(i>m_num){
                        edge_idx=i-m_num+10000;//终结符
                    }
                    else{
                        edge_idx=i;
                    }
#ifdef LOG_MATRIX_ENABLE
                    cout<<"对于边："<<i<<" "<<edge_idx<<" "<<echo_num2str(edge_idx)<<endl;
#endif
         //           con=0;
                    st_new.clear();//新的项目集  //移进项
                    vector<int>r_new;      //归约项
                    r_new.clear();
                    for(int j=0;j<(int)st.size();j++){  //对于某个边，枚举项目集中所有的项目，看看有没有发生移进
                        ex=st[j];   //ex为项目，prod为对应的产生式，st为项目集，pos为点的位置，edge_idx为边
                        int pos=ex.idx;
                        prod=production[ex.val];
                        if(pos<prod.detail.size() && prod.detail[pos]==edge_idx){
                            //可以推出新的产生式，移进有效。
                            //判断是否与归约发生冲突
                            if(r_val!=-1){
#ifdef LOG_MATRIX_ENABLE
                                cout<<"hint:发生移进-归约冲突，尝试解决"<<endl;
                                production[r_val].show();
                                cout<<"----"<<endl;
                                ex.show();
#endif
                                int left=production[r_val].left;
                                if(follow_set_lst[left].count(edge_idx)){
                                    cout<<"无法解决冲突"<<endl;
                                    show_follow(left);
                                    return -1;
                                }
#ifdef LOG_MATRIX_ENABLE
                                cout<<"冲突可解决，抛弃归约选择移进"<<endl;
#endif
                            }
                            ex.idx++;
                            st_new.push_back(ex);   //移进不可能产生重复项，因为原项目集没有重复项。
#ifdef LOG_MATRIX_ENABLE
                            cout<<"移进后的项目为："<<endl;
                            ex.show();
                            cout<<endl;
#endif
                        }
                    }
//                    for(int j=0;j<(int)st.size();j++){  //对于某个边，枚举项目集中所有的项目，看看有没有发生归约
//                        //归约会和所有移进发生冲突
//                        if(pos==prod.detail.size()){
//                            //产生归约
//                            cout<<"归约项目为"<<endl;
//                            ex.show();
//                            r_new.push_back(const_reference __x)
//                            st_new.push_back(ex);
//
//                        }
//                    }
                    if(st_new.size())//若成立，表示对于当前字符所有的移进项目都可与归约项目区分开，归约不成立。
                    {
                        prod_set_lst_middle.push_back(st_new);
                        edge_middle.push_back(edge_idx);
                        action_middle.push_back(ACTION_S);
                    }
                    else if(r_val!=-1){//若无移进且归约有效
                        if(follow_set_lst[production[r_val].left].count(edge_idx)==0){//若当前符号不在follow(left)中，则不能归约
                            
                        }
                        else{
#ifdef LOG_MATRIX_ENABLE
                            cout<<"归约成立"<<endl;
#endif
                        }
                        
                    }
                }//end of for 移进项目已经寻找完毕
            }
            else{
#ifdef LOG_MATRIX_ENABLE
                cout<<"递归过程，原项目集有："<<prod_set_lst_middle.size()<<" == "<< edge_middle.size()<<endl;
#endif
                if(prod_set_lst_middle.size()==0){
                    continue;
                }
                for(int i=0;i<(int)prod_set_lst_middle.size();i++){
#ifdef LOG_MATRIX_ENABLE
                    cout<<echo_num2str(edge_middle[i])<<":"<<endl;
                    prod_set_t_show(prod_set_lst_middle[i]);
#endif
                }
#ifdef LOG_MATRIX_ENABLE
                cout<<endl;
#endif
                for(int j=0;j<(int)prod_set_lst_middle.size();j++){
                    st=prod_set_lst_middle[j];
                    int edge_val=edge_middle[j];
                    //迭代，BFS
#ifdef LOG_MATRIX_ENABLE
                    cout<<"迭代："<<endl;
                    prod_set_t_show(st);
                    cout<<endl;
#endif
                    book.clear();
                    for(int i=0;i<(int)st.size();i++){
                        book.insert(st[i]);
                    }
                    for(int i=0;i<(int)st.size();i++){
                        ex=st[i];
                        prod=production[ex.val];
                        if(ex.idx>=prod.detail.size()){
                            continue;
                        }
                        int left=prod.detail[ex.idx];//产生式左边为left的产生式对应的原始项目ex都需要加入项目集
                        
                        if(is_m(prod.detail[ex.idx])){//若产生式第idx个字符为非终结符（大写字母）
                            //将left对应的所有产生式的原始项目集都加入（要去重）
                            for(int k=m_start_idx[left];k<(int)production.size() && production[k].left==left;k++){
                                ex.val=k;
                                ex.idx=0;
                                //若没有就加入
                                if(book.count(ex)==0){
                                    st.push_back(ex);
#ifdef LOG_MATRIX_ENABLE
                                    cout<<"加入：";
                                    ex.show();
#endif
                                    book.insert(ex);
                                }
                            }
                            
                        }
                    }
                    int l,r;
                    l=head;
                    if(set2num.count(st) == 0){
                        r=(int)prod_set_lst.size();
                        set2num[st]=r;
                        
                        prod_set_lst.push_back(st);
#ifdef LOG_MATRIX_ENABLE
                        cout<<"新项目集不重复，加入列表中。项目集为："<<endl;
                        prod_set_t_show(st);
                        cout<<endl;
#endif
                    }
                    else{
#ifdef LOG_MATRIX_ENABLE
                        cout<<"项目集重复"<<endl;
                        cout<<set2num[st]<<endl;
                        prod_set_t_show(st);
                        cout<<"and"<<endl;
                        prod_set_t_show(prod_set_lst[set2num[st]]);
                 //       cout<<(prod_set_lst[set2num[st]] < st)<<endl;
#endif
                        r=set2num[st];
                    }
                    //构造边
                    edge_t edge;
                    edge.l=l;
                    edge.r=r;
                    edge.val=edge_val;
                    edge.type=action_middle[j];
#ifdef LOG_MATRIX_ENABLE
                    cout<<"得到一条边：";
                    edge.show();
#endif
                    edge_lst.push_back(edge);
                }
            }
        }//end for
        step=0;
        head++;
    }
    return (int)edge_lst.size();
}
/*
 求非终结符的first集和follow集，要求文法中没有 ε ,且文法没有扩展
 */
int find_First_and_Last_set(vector<production_t>&prod_lst){
    int flag=1;
    //求first
//    第一遍将形如E->a...(以终结符打头)的产生式中的a提取出来。
#ifdef LOG_MATRIX_ENABLE
    cout<<"求解first"<<endl;
#endif
    for(unsigned int i=0;i<production.size();i++){
        production_t &prod=production[i];
#ifdef LOG_MATRIX_ENABLE
        cout<<"对于；"<<endl;
        prod.show();
#endif
        if(is_not_m(prod.detail[0])){
#ifdef LOG_MATRIX_ENABLE
            cout<<echo_num2str(prod.detail[0])<<"加入"<<echo_num2str(prod.left)<<endl;
#endif
            first_set_lst[prod.left].insert(prod.detail[0]);
        }
#ifdef LOG_MATRIX_ENABLE
        cout<<endl;
#endif
    }
    //first 迭代
    flag=1;
    while (flag) {
        flag=0;
        for (unsigned int i=0; i<production.size(); i++) {
            int val=production[i].detail[0];
            int left=production[i].left;
            if(is_not_m(val)){
                continue;
            }
            for(set<int>::iterator it=first_set_lst[val].begin(); it!=first_set_lst[val].end();++it) {
                if(first_set_lst[left].count(*it)==0){
                    first_set_lst[left].insert(*it);
                    flag=1;
                }
            }
        }
    }
    //求follow
#ifdef LOG_MATRIX_ENABLE
    cout<<"求解follow"<<endl;
#endif
    flag=1;
    while (flag) {
        flag=0;
        for(unsigned int i=0;i<production.size();i++){
            //遍历产生式，对所有非终结符进行如下两个操作。
            /*
             1）若B → αAβ是G的产生式，则将FIRST(β) - ε 加入FOLLOW(A)
             2）若B → αA是G的产生式，则将FOLLOW(B) 加入到FOLLOW(A) 【因为把B用αA替换之后，B后面紧跟的字符就是A后面紧跟的字符】
             */
#ifdef LOG_MATRIX_ENABLE
            cout<<"对于产生式："<<endl;;
            production[i].show();
#endif
            for(unsigned int j=0;j<production[i].detail.size();j++){
                int v=production[i].detail[j];
                if(is_m(v)){
#ifdef LOG_MATRIX_ENABLE
                    cout<<"对于字符:"<<echo_num2str(v)<<endl;
#endif
                    if(j==production[i].detail.size()-1){
#ifdef LOG_MATRIX_ENABLE
                        cout<<"规则2"<<endl;
#endif
                        //2)
                        int left=production[i].left;
                        //将left.follow加入 v.follow 其中left一定是非终结符
                        for(set<int>::iterator it=follow_set_lst[left].begin();it!=follow_set_lst[left].end();++it){
                            if(follow_set_lst[v].count(*it)==0){
                                follow_set_lst[v].insert(*it);
                                flag=1;
#ifdef LOG_MATRIX_ENABLE
                                cout<<*it<<endl;
                                cout<<echo_num2str(*it)<<endl;
#endif
                            }
                        }
                    }
                    else{
#ifdef LOG_MATRIX_ENABLE
                        cout<<"规则1"<<endl;
#endif
                        //1）
                        int tmp=production[i].detail[j+1];
                        if(is_m(tmp)){
                            //将tmp.first加入v.follow
#ifdef LOG_MATRIX_ENABLE
                            cout<<"将"<<tmp<<".first加入"<<v<<".follow"<<endl;
#endif
                            for(set<int>::iterator it=first_set_lst[tmp].begin();it!=first_set_lst[tmp].end();++it){
                                if(follow_set_lst[v].count(*it)==0){
                                    follow_set_lst[v].insert(*it);
                                    flag=1;
#ifdef LOG_MATRIX_ENABLE
                                    cout<<*it<<endl;
                                    cout<<echo_num2str(*it)<<endl;
#endif
                                }
                                
                                
                            }
                        }
                        else{
                            //将tmp.first={tmp}加入v.follow
                            if(follow_set_lst[v].count(tmp)==0){
                                follow_set_lst[v].insert(tmp);
                                flag=1;
#ifdef LOG_MATRIX_ENABLE
                                cout<<tmp<<endl;
                                cout<<echo_num2str(tmp)<<endl;
#endif
                            }
                            
                        }
                    }
                }
            }
            
        }
    }
    return 0;
}
int check_m_defined(){
    bool *book = new bool[get_m_num()+1];
    for(int i=1;i<(int)production.size();i++){
        book[production[i].left]=true;
    }
    for(int i=1;i<=(int)get_m_num();i++){
        if(!book[i]){
            return -i;
        }
    }
    return 1;
}
//若可归约，则返回项目序号，否则返回-1
int find_R_left(int ex_set_idx,int idx_begin){
    prod_set_t ex_set=prod_set_lst[ex_set_idx];
    for(int i=idx_begin;i<(int)ex_set.size();i++){
        prod_externed_t ex=ex_set[i];
        if(ex.idx==production[ex.val].detail.size()){
            return i;
        }
    }
    return -1;
}
vector<int> find_acc(){
    //(3)若S‘->E. 属于 Si ,则置Action[Si,#]=acc
    vector<int>ret;
    for(int i=1;i<(int)prod_set_lst.size();i++){
        for(int j=0;j<(int)prod_set_lst[i].size();j++){
            prod_externed_t ex = prod_set_lst[i][j];
            if(ex.idx==production[ex.val].detail.size() && ex.val==0){
                ret.push_back(i);
                break;
            }
        }
    }
    return ret;
}

vector<map<int,map_cell_t>>find_matrix(){
    vector<map<int,map_cell_t>>ret;
    ret.clear();
    ret.resize(prod_set_lst.size()+1);
    ret[0].count(10005);
    map_cell_t cell;
    //所有的边都是移进项目
    int cnt=0;
    for(int i=1;i<(int)edge_lst.size();i++){
        edge_t edge=edge_lst[i];
        edge.show();
        /*
         int l;
         int r;
         int val;
         int type;//0是归约，1是移进
         */
        cell.action=ACTION_S;
        cell.go_to=edge.r;
        
        if(ret[edge.l].count(edge.val)){
            //理论上讲这里不会被执行到。
            cout<<"error in line:"<<__LINE__<<endl;
            exit(0);
        }
#ifdef LOG_MATRIX_ENABLE
        cout<<edge.l-1<<" , "<<echo_num2str(edge.val)<<"\t=\t";
        cell.show();
        cout<<endl;
#endif
        ret[edge.l][edge.val]=cell;
        cnt++;
    }
    //检查所有项目集的归约项目。
    for(int i=1;i<(int)prod_set_lst.size();i++){
        int ex_idx=-1;
        prod_set_t_show(prod_set_lst[i]);
        while(1){
            ex_idx=find_R_left(i,ex_idx+1);
            if(ex_idx<0){
                break;
            }
            prod_externed_t ex = prod_set_lst[i][ex_idx];
            int prod_idx=ex.val;
            production_t prod=production[prod_idx];
            int left=prod.left;
            //令map[i,left.follow] = {R,prod_idx},表示状态i遇到字符follow[it]时使用产生式prod归约
            for(auto it=follow_set_lst[left].begin();it!=follow_set_lst[left].end();++it){
                cell.action=ACTION_R;
                cell.go_to=prod_idx;
                ret[i][*it]=cell;
#ifdef LOG_MATRIX_ENABLE
                cout<<i<<" , "<<echo_num2str(*it)<<"\t=\t";
                cell.show();
                cout<<endl;
#endif
                cnt++;
            }
        }
    }
    vector<int> acc_vec=find_acc();
    for(int i=0;i<(int)acc_vec.size();i++){
        if(ret[acc_vec[i]].count(echo_str2num("[#]"))){
            cout<<"error:acc冲突"<<endl;
            exit(-1);
        }
        else{
            map_cell_t cell;
            cell.action=ACTION_ACC;
            cell.go_to=0;
            ret[acc_vec[i]][echo_str2num("[#]")]=cell;
            cout<<"acc: "<<acc_vec[i]<<endl;
        }
    }
    //vector<map<int,map_cell_t>>ret;
    cout<<"有"<<cnt<<"个cell"<<endl;
    return ret;
}

int get_syntax_matrix(vector<map<int,map_cell_t>> &matrix,const char *path){
    
    FILE *fp=fopen(path,"rt");
    if(fp==NULL){
        cout<<"fp==NULL"<<endl;
        return -1;
    }
    char tmp[128];
    lan="";
    while (fgets(tmp, 127, fp)) {
        lan+=tmp;
    }
    fclose(fp);
    cout<<"read "<<lan.length()<<"byte"<<endl;
    int ret;
    ret=analize_lan_txt();//解析文本，产生式离散化并生成产生式列表
    if(ret<0){
        return -1;
    }
    sort(production.begin()+1, production.end());
    for(int i=0;i<(int)production.size();i++){
        production[i].val=i;
    }
    
//    //显示产生式列表
#ifdef LOG_MATRIX_ENABLE
    for(int j=0;j<(int)production.size();j++){
        production_t &prod=production[j];
        cout<<prod.val<<" "<<m_num2str[prod.left]<<"->";
        for (int i=0; i<(int)prod.detail.size(); i++) {
            if(is_m(prod.detail[i])){
                cout<<m_num2str[prod.detail[i]];
            }
            else{
                cout<<num2str[prod.detail[i]-10000];
            }
        }
        cout<<endl;
    }
#endif    //由于[#]仅出现在求follow集的过程中，求DFA时需要从产生式中去掉[#]，故保存一下原始文法（字符集不需要），求完follow后替换回来
    vector<int>detail_tmp;
    {
        //添加额外的终结符[#]
        
        string str="[#]";
        echoMap[str]=ip++;
        num2str.push_back(str);
        m_num2str.push_back(str);
        
        //扩展文法，添加结束符[#]
        detail_tmp=production[0].detail;
        production[0].detail.push_back(ip-1);
    }

    //求解first和follow集,结果存放在vector<set<int>>first_set_lst,follow_set_lst;中
    first_set_lst.resize(get_m_num()+1);
    follow_set_lst.resize(get_m_num()+1);
    find_First_and_Last_set(production);
    //将产生式还原回来，因为项目集规范族不要[#]
    production[0].detail=detail_tmp;
    //显示
#ifdef LOG_MATRIX_ENABLE
    for(int i=0;i<(int)first_set_lst.size();i++){
        show_first(i);
    }
    for(int i=0;i<(int)follow_set_lst.size();i++){
        show_follow(i);
    }
#endif
    //产生项目集规范族
    //需要先对产生式列表进行排序，排序依据为左部(上面已完成)。然后对每个左部第一次出现的位置进行统计，保存在全局变量int *m_start_idx;里
    //左部标号范围为1~9999，真实数量和实际最大标号为m_ip-1，
    m_start_idx=new int[m_ip];  //原始指针，需手动释放
    memset(m_start_idx,0, m_ip*sizeof(int));
    //枚举所有的产生式，统计左部位置
    for(int i=1;i<(int)production.size();i++){
        if(m_start_idx[production[i].left]==0){
            m_start_idx[production[i].left]=i;
        }
    }
    
    ret=create_prod_set_DFA();
    delete[] m_start_idx;       //原始指针，手动释放
    if(ret<0){
        return -1;
    }
    cout<<"项目集个数："<<prod_set_lst.size()<< "个，";
#ifdef LOG_MATRIX_ENABLE
    cout<<"为："<<endl;
    for(int i=0;i<(int)prod_set_lst.size();i++){
        prod_set_t prod_set = prod_set_lst[i];
        cout<<i<<" |--------------------------------|"<<endl;
        prod_set_t_show(prod_set);
    }
#else
    cout<<endl;
#endif
#ifdef LOG_MATRIX_ENABLE
    cout<<"边为:"<<endl;
    
    for(int i=0;i<edge_lst.size();i++){
        edge_lst[i].show();
    }
#endif
    //检查文法完善性
    ret=check_m_defined();
    if(ret<=0){
        cout<<"文法不完善:存在未定义的非终结符："<<endl;
        cout<<echo_num2str(-ret)<<endl;
        return -1;
    }
#ifdef LOG_MATRIX_ENABLE
    cout<<"终结符有："<<endl;
    for(int i=10001;i<ip;i++){
        cout<<echo_num2str(i)<<endl;
    }
#endif
//    exit(0);
    cout<<"单向边个数为："<<edge_lst.size()<<endl;
    matrix = find_matrix();
    matrix[0].count(10005);
  //  cout<<"终结符有:"<<get_num()<<endl;
    cout<<"非终结符有:"<<get_m_num()<<endl;
    return 1;
}
//int get_lan_tree(vector<word_node_t>word_lst,vector<map<int,map_cell_t>>matrix){
//
//    return 0;
//}
//



