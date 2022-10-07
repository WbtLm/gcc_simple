
/*
 2019年4月9日18:49:42
 */
#include<iostream>
#include<vector>
#include<map>
#include<set>
#include<cstring>
#include "lexical_analyze.h"
#define pi 3
using namespace std;
typedef int (*Fc)(void*);
char const *C_TXT="c.txt";
string errifo;    //errno
int ip_lexical;    //记录当前扫描行数
Fc echo[128]={0};//从字符到处理函数的映射
set<string>st;//判标识符
map<string,string>macro_s;
string in;    //输入字符流
int book[128];//其他字符
int special[128];//不允许替换的字符
inline bool isal(char ch)    //下划线也算成字母
{
    if((ch>='a' && ch<='z') || (ch>='A' && ch<='Z') || ch=='_')
    {
        return 1;
    }
    return 0;
}
int idx;//base

inline bool isdigital(char ch)    //判数字
{
    if(ch>='0' && ch<='9')
    {
        return 1;
    }
    return 0;
}
inline bool isspace(char ch)//空格，回车，终结符-1也算
{
    if(ch==' ' || ch=='\n' || ch=='\r' || ch=='\t' || ch==-1)
        return 1;
    return 0;
}
inline bool iselse(char ch)//是不是其他合法字符 (包含终结符)，若不合法则报错
{
    if(isspace(ch))    //要在book前面
        return 1;    //是空白符
    if(book[0])
    {
        if(book[ch])
            return 1;
        else return 0;
    }
    
    if(ch<=5)    //是非法字符  这个要在isspace后面
    {
        cout<<"error0 line="<<ip_lexical<<endl;
        return 1;
    }
    if(isal(ch) || isdigital(ch))    //是字母，数字
        return 0;
    return 1;
}



vector<word_node_t>vec;    //元组集
char ori[]={
    'a',    //字母
    '0',    //数字
    '/',    //special
    's',    //空白，包括tab，space，回车， 其他字符也认为是空白
    'e',    //error 不能识别的字符
    'o',    //其他字符 除了空白符
    '"',    //special
    '\'',    //special
    '#',    //special
    '+',    //special
    '-',    //special
    '=',    //special
    '!',    //special
    '>',    //special
    '<',    //special
    '*',    //special
    '%',    //special
    '^',    //special
    '|',    //special
    '&',    //special
    '(',
    ')',
    '{',
    '}',
    '[',
    ']',
    '.',
    '~',
    ';',
    ',',
    ':',
    '\\',
    '?'
    //    'a',
    //    '0',
};
int fa(void *arg)    //字母，下划线
{
    string &s= *(string*)arg;
    //cout<<s<<endl;
    int p=1;
    char ch;
    char con='S';
    while(1){
        ch=s[p++];
        switch(con)
        {
            case 'S':
                if(isal(ch))
                {
                    con='S';
                }
                else if(isdigital(ch))
                {
                    con='A';
                }
                else if(iselse(ch))
                {
                    con='3';
                    p--;
                    vec.push_back(word_node_t(s.substr(0,p),3,ip_lexical,idx));
                    return p;
                }
                else
                {
                    return -1;
                }
                break;
            case 'A':
                if(iselse(ch))
                {
                    con='1';
                    p--;
                    vec.push_back(word_node_t(s.substr(0,p),1,ip_lexical,idx));
                    return p;
                }
                else if(isdigital(ch) || isal(ch))
                {
                    con='A';
                }
                break;
        }
    }
}
int fn(void *arg)    //数字
{
    string &s= *(string*)arg;
    
    int p=1;
    char ch;
    char con='S';
    
    while(1)
    {
        ch=s[p++];
        switch(con)
        {
            case 'S':
                if(isdigital(ch))
                {
                    con='S';
                }
                else
                {
                    con='2';
                    p--;
                    vec.push_back(word_node_t(s.substr(0,p),2,ip_lexical,idx));
                    return p;
                }
                break;
        }
    }
}

int fs(void *arg)    //空白符
{
    string &s= *(string*)arg;
    if(s[0]=='\n')
    {
        ip_lexical++;
        idx=-1;
    }
    return 1;
}

int f1(void *arg)//        '/',
{
    string &s= *(string*)arg;
    int p=1;
    char ch;
    char con='S';
    int tip=ip_lexical;
    if(s[1]=='=')//    /=
    {
        vec.push_back(word_node_t(s.substr(0,2),5,tip,idx));
        return 2;
    }
    while(1)
    {
        ch=s[p++];
        if(ch=='\n')
        {
            ip_lexical++;
            idx=0;
        }
        switch(con)
        {
            case 'S':
                if(ch=='/')
                {
                    con='A';
                }
                else if(ch=='*')
                {
                    con='B';
                }
                else // no if
                {
                    con='Z';//5
                    p--;
                    vec.push_back(word_node_t(s.substr(0,p),5,tip,idx));
                    return p;
                }
                break;
            case 'A':
                if(ch=='\n')
                {
                    con='E';
                    p--;
                    ip_lexical--;//尾部的回车不算进去。要把多扣掉的补回来 ，下一个字符是'\n' ，所以不用还原idx
                    vec.push_back(word_node_t(s.substr(0,p),0,tip,idx));
                    return p;
                }
                else //任意字符，，no if
                {
                    con='A';
                }
                break;
            case 'B':
                if(ch=='*')
                {
                    con='C';
                }
                else
                {
                    con='B';
                    if(p+1==(int)s.length())
                    {
                        errifo="注释符不匹配";
                        return -1;
                    }
                }
                break;
            case 'C':
                if(ch=='/')
                {
                    vec.push_back(word_node_t(s.substr(0,p),0,tip,idx));
                    return p;
                }
                else//no if
                {
                    con='B';
                    if(p+1==(int)s.length())
                    {
                        errifo="注释符不匹配";
                        return -1;
                    }
                }
                break;
        }
    }
}

int ferr(void *arg)
{
    string &s= *(string*)arg;
    cout<<"不能识别字符-->"<<s[0]<<"<-- 行="<<ip_lexical<<" idx="<<idx<<endl;
    cout<<s[0]<<s[1]<<endl;
    return -1;
}

int fo(void *arg)//        'other',单个字符
{
    string &s= *(string*)arg;
    vec.push_back(word_node_t(s.substr(0,1),4,ip_lexical,idx+1));
    return 1;
}

int f3(void *arg)//        '双引号',
{
    string &s= *(string*)arg;
    int p=1;
    int flag=0;//转义
    while((p<(int)s.length() && s[p]!='"') || flag)
    {
        flag=0;
        if(s[p]=='\\' && flag==0)
        {
            flag=1;
        }
        p++;
    }
    if(p==(int)s.length())
    {
        errifo="双引号不匹配";
        return -1;
    }
    p++;
    vec.push_back(word_node_t(s.substr(0,p),7,ip_lexical,idx));
    return p;
}

int f4(void *arg) // 单引号
{
    string &s= *(string*)arg;
    int p=1;
    int flag=0;//转义
    while(p<(int)s.length() && s[p]!='\'' || flag)
    {
        if(s[p]=='\\' && flag==0)
        {
            flag=1;
        }
        else
            flag=0;
        p++;
    }
    if(p==(int)s.length())
    {
        errifo="单引号不匹配";
        return -1;
    }
    p++;
    vec.push_back(word_node_t(s.substr(0,p),7,ip_lexical,idx));
    return p;
}

int fj(void *arg) //井号
{
    string &s= *(string*)arg;
    int p=1;
    for(;p<(int)s.length();p++)
    {
        //宏只有注释能影响 ，为了简单我们不允许宏这一行加注释，否则认为是宏的一部分
        if(s[p]=='\n')
            break;
    }
    vec.push_back(word_node_t(s.substr(0,p),6,ip_lexical,idx));
    return p;
}


int f5(void *arg)//        '+',
{
    string &s= *(string*)arg;
    int p=1;
    if(s[1]=='=')//+=
    {
        vec.push_back(word_node_t(s.substr(0,2),5,ip_lexical,idx));
        return 2;
    }
    while(p<(int)s.length() && s[p]=='+')
        p++;
    switch(p)
    {
        default:
            if((p&1)==0)
            {
                for(int i=0;i<p;i+=2)
                {
                    vec.push_back(word_node_t(s.substr(i,2),5,ip_lexical,idx+i*2));
                }
                return p;
            }
            errifo="'+'不匹配";
            return -1;
        case 1:
            vec.push_back(word_node_t(s.substr(0,1),5,ip_lexical,idx));
            return 1;
        case 2:
            vec.push_back(word_node_t(s.substr(0,2),5,ip_lexical,idx));
            return 2;
        case 3:
            vec.push_back(word_node_t(s.substr(0,2),5,ip_lexical,idx));
            vec.push_back(word_node_t(s.substr(2,1),5,ip_lexical,idx+1));
            return 3;
        case 5:
            vec.push_back(word_node_t(s.substr(0,2),5,ip_lexical,idx));
            vec.push_back(word_node_t(s.substr(2,1),5,ip_lexical,idx+2));
            vec.push_back(word_node_t(s.substr(3,2),5,ip_lexical,idx+3));
            return 5;
            
    }
    
}

int f6(void *arg)//        '-',
{
    string &s= *(string*)arg;
    int p=1;
    
    if(s[1]=='>' || s[1]=='=')
    {
        vec.push_back(word_node_t(s.substr(0,2),5,ip_lexical,idx));
        return 2;
    }
    while(p<(int)s.length() && s[p]=='-')
        p++;
    switch(p)
    {
        default:
            if((p&1)==0)
            {
                for(int i=0;i<p;i+=2)
                {
                    vec.push_back(word_node_t(s.substr(i,2),5,ip_lexical,idx+i*2));
                }
                return p;
            }
            errifo="'-'不匹配";
            return -1;
        case 1:
            vec.push_back(word_node_t(s.substr(0,1),5,ip_lexical,idx));
            return 1;
        case 2:
            vec.push_back(word_node_t(s.substr(0,2),5,ip_lexical,idx));
            return 2;
        case 3:
            vec.push_back(word_node_t(s.substr(0,2),5,ip_lexical,idx));
            vec.push_back(word_node_t(s.substr(2,1),5,ip_lexical,idx+1));
            return 3;
        case 5:
            vec.push_back(word_node_t(s.substr(0,2),5,ip_lexical,idx));
            vec.push_back(word_node_t(s.substr(2,1),5,ip_lexical,idx+2));
            vec.push_back(word_node_t(s.substr(3,2),5,ip_lexical,idx+3));
            return 5;
            
    }
    
}

int f7(void *arg)//        '=',
{
    string &s= *(string*)arg;
    int p=1;
    if(s[1]=='=')
    {
        vec.push_back(word_node_t(s.substr(0,2),5,ip_lexical,idx));
        return 2;
    }
    vec.push_back(word_node_t(s.substr(0,1),5,ip_lexical,idx));
    return 1;
}

int f8(void *arg)//        '!',
{
    string &s= *(string*)arg;
    if(s[1]=='=')
    {
        vec.push_back(word_node_t(s.substr(0,2),5,ip_lexical,idx));
        return 2;
    }
    vec.push_back(word_node_t(s.substr(0,1),5,ip_lexical,idx));
    return 1;
}
int f9(void *arg)//        '>',
{
    string &s= *(string*)arg;
    if(s[1]=='=' || s[1]=='>') // >=  >>
    {
        vec.push_back(word_node_t(s.substr(0,2),5,ip_lexical,idx));
        return 2;
    }
    vec.push_back(word_node_t(s.substr(0,1),5,ip_lexical,idx));
    return 1;
}
int f10(void *arg)//        '<',
{
    string &s= *(string*)arg;
    if(s[1]=='=' || s[1]=='<') // >= ,  >>
    {
        vec.push_back(word_node_t(s.substr(0,2),5,ip_lexical,idx));
        return 2;
    }
    vec.push_back(word_node_t(s.substr(0,1),5,ip_lexical,idx));
    return 1;
}
int f11(void *arg)//        '*',
{
    string &s= *(string*)arg;
    if(s[1]=='=') // *=
    {
        vec.push_back(word_node_t(s.substr(0,2),5,ip_lexical,idx));
        return 2;
    }
    vec.push_back(word_node_t(s.substr(0,1),5,ip_lexical,idx));
    return 1;
}
int f12(void *arg)//        '%',
{
    string &s= *(string*)arg;
    if(s[1]=='=') // >= ,  >>
    {
        vec.push_back(word_node_t(s.substr(0,2),5,ip_lexical,idx));
        return 2;
    }
    vec.push_back(word_node_t(s.substr(0,1),5,ip_lexical,idx));
    return 1;
}

int f13(void *arg)//        '^',
{
    string &s= *(string*)arg;
    if(s[1]=='=') // ^=
    {
        vec.push_back(word_node_t(s.substr(0,2),5,ip_lexical,idx));
        return 2;
    }
    vec.push_back(word_node_t(s.substr(0,1),5,ip_lexical,idx));
    return 1;
}
int f14(void *arg)//        '|',
{
    string &s= *(string*)arg;
    if(s[1]=='|' || s[1]=='=' ) // ^=
    {
        vec.push_back(word_node_t(s.substr(0,2),5,ip_lexical,idx));
        return 2;
    }
    vec.push_back(word_node_t(s.substr(0,1),5,ip_lexical,idx));
    return 1;
}
int f15(void *arg)//        '&',
{
    string &s= *(string*)arg;
    if(s[1]=='&' || s[1]=='=' ) // ^=
    {
        vec.push_back(word_node_t(s.substr(0,2),5,ip_lexical,idx));
        return 2;
    }
    vec.push_back(word_node_t(s.substr(0,1),5,ip_lexical,idx));
    return 1;
}

/*
 返回值为已经吃掉的字符串长度
 */
Fc func[128]={
    fa,    //'字母'
    fn,    //数字
    f1,    //注释
    fs,    //空白
    ferr,//非法字符
    fo,    //other除去space
    f3,//双引号
    f4,//单引号(
    fj,//井号
    f5,//+
    f6,//-
    f7,//=
    f8,//!
    f9,//>
    f10,//<
    f11,//*
    f12,//%
    f13,//^
    f14,//|
    f15,//&
    0
};
void init(const char*input_path)
{
    macro_s.clear();
    cout<<__LINE__<<endl;
    special['"']=1;//f
    special['/']=1;    //finished
    special['\'']=1;//单引号
    special['#']=1;//finished
    special['+']=1;
    special['-']=1;
    special['=']=1;
    special['!']=1;
    special['>']=1;
    special['<']=1;
    special['*']=1;
    special['%']=1;
    special['^']=1;
    special['|']=1;
    special['&']=1;
    //    special['\\']=1;
    for(int i=0;i<(int)sizeof(ori);i++)
    {
        echo[ori[i]]=func[i];
        if(iselse(ori[i]))
            book[ori[i]]=1;
    }
    book[0]=1;
        cout<<__LINE__<<endl;

    FILE *fp=fopen(input_path,"rt");
    if(fp==NULL)
    {
        cout<<"r.in:fopen err"<<endl;
        exit(0);
    }
    char tmp[1024];
        cout<<__LINE__<<endl;

    while(fgets(tmp,1024,fp))
    {
        in+=tmp;
    }
    fclose(fp);
    
    fp=fopen(C_TXT,"rt");
    if(fp==NULL)
    {
        cout<<"c.txt:fopen err"<<endl;
        exit(0);
    }
    st.clear();
        cout<<__LINE__<<endl;

    while(fscanf(fp,"%s",tmp)!=EOF)
    {
        st.insert(string(tmp));
    }
}


int get_word(vector<word_node_t>&word_lst,const char*input_path)
{
    //  freopen("w.out","wt",stdout);
    
    init(input_path);
    iselse('(');
    idx=0;
    int ret;
    cout<<"已读到源文件r.in" <<endl;

    in+='\n';
    in+=-1;
    while(in.length())
    {
        char ch=in[0];
        if(ch==-1)
        {
            break;
        }
        if(isal(ch))    //是字母
        {
            ch='a';
        }
        else if(isdigital(ch))
        {
            ch='0'; //是数字
        }
        else if(isspace(ch))
        {
            ch='s';  //是空白符
        }
        else if(special[ch]==0 && iselse(ch) && !isspace(ch))
        {
            ch='o';
        }
        if(echo[ch]==NULL)
        {
            ret=(*echo['e'])(&in);
            cout<<"find an illegal charactor-->"<<in[0]<<"<--in idx="<<idx<<endl;
        }
        else
            ret=(*echo[ch])(&in);
        if(ret<0)
        {
            cout<<"error1 line="<<ip_lexical<<" idx="<<idx<<" ifo:"<<errifo<<endl;
            break;
        }
        idx+=ret;
        //        cout<<in.substr(0,ret);
        in=in.substr(ret,in.length()-ret);
        
        //    cout<<in<<endl;
    }
    
    cout<<"==============================="<<endl;
    cout<<"size="<<vec.size()<<endl;
    cout<<"ip="<<ip_lexical<<endl;
    word_node_t terminal("[#]",8,0,0);
    vec.push_back(terminal);
    for(int i=0;i<(int)vec.size();i++)
    {
        if(vec[i].type==0)//注释
        {
            continue;
        }
        cout<<"==============================="<<endl;
        vec[i].show();
    }
    word_lst=vec;
    return 1;
}

word_node_t::word_node_t(string ss,int t,int l,int i){
    idx=i;
    s=ss;
    type=t;
    line=l;
    if(type==3)//保留字，也叫关键字
    {
        if(is_reserved(s)==0)
        {
            type=1;    //标识符
        }
    }
    name="";
    word_type=UNDEFINED_TYPE_WORD;
    string ori_lst[4]={
        "while",
        "if",
        "else",
        "struct",
    };
    bool flag=0;

    switch(type){
        case 0: //制导翻译
            name=s;
            break;
        case 3:
            flag=0;
            for(int i=0;i<4;i++){
                if(ori_lst[i]==s){
                    name="'"+s+"'";
                    flag=1;
                    break;
                }
            }
            if(flag){
                break;
            }
            if(s=="int"||s=="char"||s=="void"){
                word_type=DATA_TYPE_WORD;
                name="[数据类型]";
                str_val=s;
                break;
            }
            //no break;
        case 1:
            word_type = KEYWORD_WORD;
            
            if(s=="return"){
                name="'return'";
            }
            else if(s=="break"){
                name="'break'";
            }
            else if(s=="continue"){
                name="'continue'";
            }
            else{
                name="[关键字]";
            }
            str_val=s;
            break;
        case 2:
            word_type = STATIC_VALUE_WORD;
            name="[常量]";
            int_val=atoi(s.c_str());
            break;
        case 4:case 5:
            word_type = TERMINAL_WORD;
            name="'"+s+"'";
            break;
        case 7:
            word_type=STATIC_VALUE_WORD;
            name="[常量]";
            if(s[0]=='\''){
                
                if(s.length()==3){
                    int_val=s[1];
                }
                else if(s.length()==4){
                    show();
                    cout<<"词法分析器暂不支持单引号内转义字符,词法分析程序："<<__LINE__<<endl;
                    exit(-1);
                }
                else{
                    cout<<"词法错误：单引号内符号不合法"<<endl;
                    show();
                    exit(-1);
                }
            }
            else if(s[0]=='"'){
                if(s.length()<2){
                    cout<<"error:双引号错误"<<endl;
                    show();
                    exit(-1);
                }
                str_val=s.substr(1,s.length()-2);
            }
            else{
                cout<<"error:wrong type and text"<<endl;
                show();
                exit(-1);
            }
            break;
        case 8:
            word_type=STATIC_VALUE_WORD;
            name="[#]";
            break;
        case 6:
        default:
            word_type = UNDEFINED_TYPE_WORD;
            cout<<"UNDEFINED_TYPE:"<<type<<endl;
            show();
            exit(-1);
            break;
    }
}

bool is_reserved(string s){
    return st.count(s);
}
