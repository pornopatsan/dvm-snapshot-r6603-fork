//
// Created by Пенёк on 03/11/2019.
//

#include "integration.h"
#include "statlist.h"
#include <cstdlib>
#include <iostream>

int read_stat_(char * name, char * &res){
    CStat stat;
    stat.init(name);
    std::string str;
    stat.to_string(str);
    res = (char*) malloc(sizeof(char) * (str.size() + 1));
    for (int i = 0; i < str.size(); ++i){
        res[i] = str[i];
    }
    res[str.size()] = '\0';
    return stat.nproc;
}