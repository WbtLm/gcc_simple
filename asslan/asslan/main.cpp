//
//  main.cpp
//  asslan
//
//  Created by dawn on 2020/2/25.
//  Copyright © 2020 chuyi. All rights reserved.
//

#include <iostream>
#include "head.h"
#include "assemblylan.h"
int main(int argc, const char * argv[]) {
    if(argc!=1){
        cout<<"arg_num!=2"<<endl;
        return -1;
    }
    int ret=assemblylan("/Users/dawn/f/lan/code.c");
    if(!ret){
        cout<<"exit due to error"<<endl;
        exit(-1);
    }
    return 0;
}
