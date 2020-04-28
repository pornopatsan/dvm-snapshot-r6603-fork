//
// Created by Пенёк on 03/11/2019.
//

#include "integration.h"
#include "statlist.h"
#include <cstdlib>
#include <iostream>

void _stat_to_char(CStat &stat, char * &res){
    json j;
//    std::cout << std::endl << std::endl;
//    auto cur = stat.inter_tree;
//    while (cur != NULL){
//        for (int i = 0; i < cur->id.nlev; ++i)
//            std::cout << "    ";
//        std::cout << cur->id.nlev << " " << ((cur->id.t == SEQ) ? "SEQ" : ((cur->id.t == PAR) ? "PAR" : "USER"));
//        std::cout << " " << ((cur->id.t == USER) ? cur->id.expr : NULL) << "  " << cur->id.nline << std::endl;
//        cur = cur->next;
//    }
    stat.to_json(j);
    std::string str = j.dump();
    res = (char*) malloc(sizeof(char) * (str.size() + 1));
    for (int i = 0; i < str.size(); ++i){
        res[i] = str[i];
    }
    res[str.size()] = '\0';
}

int read_stat_(char * name, char * &res){
    CStat stat;
    stat.init(name);
    if (!stat.isinitialized)
        return 1;
    _stat_to_char(stat, res);
    return 0;
}

int stat_intersect_(char * st1, char * st2, char * &res1, char * &res2){
    json j1 = json::parse(st1), j2 = json::parse(st2);
    CStat stat1(j1), stat2(j2), stat_res1, stat_res2;
    stat_intersect(stat1, stat2, stat_res1, stat_res2);
    std::cout << "stat_intersect OK " << std::endl;
    if (!stat1.isinitialized || !stat2.isinitialized || !stat_res1.isinitialized || !stat_res2.isinitialized){
        std::cout << ">>  Not inicialized\n\n";
        return 1;
    }
    _stat_to_char(stat_res1, res1);
    _stat_to_char(stat_res2, res2);
    return 0;
}