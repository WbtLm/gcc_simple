//
//  head.cpp
//  asslan
//
//  Created by dawn on 2020/2/25.
//  Copyright © 2020 chuyi. All rights reserved.
//
#include "head.h"
ull code = 0;
char const *errCode = NULL;


void debug(char ch[])
{
    printf("%s\n",ch);
}

template<typename T>
T shl(T a,int time)
{
    for(int i=0;i<time;i++)
    {
        a<<=1;
        a&=~1;
    }
    return a;
}
void st(int l,int r,ll val)
{
    int len = (l-r+1);
    for(int i=l+1;i<=31;i++)
    {
        val &= ~(1<<i);
    }
    val=shl(val,r);
    //    int b=1;
    //    b<<=r;
    //    for(int i=0;i<s;i++)
    //    {
    //        code&=~b;
    //        b<<=1;
    //    }
    code|=val;
}
bool isdigital(char ch)
{
    if(ch>='0' && ch<='9')
        return 1;
    return 0;
}
bool isalpha(char ch)
{
    if(ch>='a'&& ch<='z')
        return 1;
    if(ch>='A' && ch<='Z')
        return 1;
    return 0;
}
char tolow(char ch)
{
    if(ch>='a'&& ch<='z')
        return ch;
    return ch+('a'-'A');
}

bool ish(char ch)
{
    if(isdigital(ch))
        return 1;
    if(tolow(ch)>='a' && tolow(ch)<='f')
        return 1;
    return 0;
}
ll numofh(char ch)
{
    ll ret=0;
    if(!ish(ch))
    {
        errCode = err;
        return -1;
    }
    if(isdigital(ch))
    {
        ret=ch-'0';
    }
    else
    {
        ret=tolow(ch)-'a'+10;
    }
    errCode=NULL;
    return ret;
}
ll numofb(char const *s)
{
    int len=strlen(s);
    ll ret=0;
    for(int i=0;i<len;i++)
    {
        if(i==len-1 && s[i]=='b')
            break;
        if(s[i]!='0'&&s[i]!='1')
        {
            errCode=err;
            return -1;
        }
        ret<<=1;
        if(s[i]=='1')
        {
            ret|=1;
        }
    }
    errCode=NULL;
    return ret;
}
ll numofstr(char const str[])
{
    ll retNum=0;
    string s(str);
    ll ret=0;
    bool h=0;
    int sign=1;
    for(int i=0;i<(int)s.length();i++)
    {
        
        if(tolow(s[(int)s.length()-1]) == 'd' && i==(int)s.length()-1)
        {
            s[(int)s.length()-1]=0;
            break;
        }
        if(i==0)
        {
            if(s[0]=='-')
            {
                sign=-1;
                continue;
            }
        }
        if(isdigital(s[i]))
        {
            ;
        }
        else
        {
            h=1;
            break;
        }
    }
    
    if(h)
    {
        if(tolow(s[s.length()-1])!='h')
        {
            errCode=err;
            return -1;
        }
        for(int i=0;i<(int)s.length()-1;i++)
        {
            if(ish(s[i]))
            {
                retNum*=16;
                retNum += numofh(s[i]);
            }
            else
            {
                errCode=err;
                return -1;
            }
        }
    }
    else
    {
        retNum = atoi(str);
    }
    errCode=NULL;
    return retNum;
}
void check()
{
    if(errCode!=NULL)
    {
        cout<<"failed:"<<errCode<<endl;
        exit(1);
    }
}
char * rmsp(char s[]) //…æ≥˝Õ∑≤øø’◊÷∑˚
{
    int len=strlen(s);
    int i;
    for(i=0;i<len;i++)
    {
        if(s[i]=='\t'||s[i]==' '||s[i]=='\n'||s[i]=='\r')
        {
            
        }
        else
        {
            break;
        }
    }
    for(int j=0;i<=len;j++,i++)
    {
        s[j]=s[i];
    }
    return s;
}
