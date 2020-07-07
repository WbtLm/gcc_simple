//
//  hhh.hpp
//  asslan
//
//  Created by dawn on 2020/2/25.
//  Copyright © 2020 chuyi. All rights reserved.
//

#ifndef hhh_hpp
#define hhh_hpp

#include <iostream>
#include <stack>
#include <iostream>
#include <vector>
#include <map>
#include <set>
#include <cstring>
#include <sstream>
#define op 31,26
#define rs 25,21
#define rt 20,16
#define rd 15,11
#define shamt 10,6
#define func 5,0
#define imm 15,0
#define address 25,0
#define offset 15,0
using namespace std;
typedef long long ll;
typedef unsigned long long ull;
//by Õı±¶ÕÆ (≥ıÒ¥)
#define err "error";
void debug(char ch[]);
template<typename T>T shl(T a,int time);
void st(int l,int r,ll val);
bool isdigital(char ch);
bool isalpha(char ch);
char tolow(char ch);
bool ish(char ch);
ll numofh(char ch);
ll numofb(char const *s);
ll numofstr(char const str[]);
void check();
char * rmsp(char s[]); //…æ≥˝Õ∑≤øø’◊÷∑˚
extern ull code;
#endif /* hhh_hpp */
