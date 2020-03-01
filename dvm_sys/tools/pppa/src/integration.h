//
// Created by Пенёк on 03/11/2019.
//

#ifndef DVM_SYS_INTEGRATION_H

extern "C"  {
     //name - stat file name, res - json string
    __declspec(dllexport) int read_stat_(char * name, char * &res);
};

extern "C"  {
//     st1, st2, res1, res2 - json strings
    __declspec(dllexport) int stat_intersect_(char * st1, char * st2, char * &res1, char * &res2);
};

#define DVM_SYS_INTEGRATION_H

#endif //DVM_SYS_INTEGRATION_H
