//
// Created by Пенёк on 03/11/2019.
//

#ifndef DVM_SYS_INTEGRATION_H

extern "C"  {
    __declspec(dllexport) int read_stat_(char * name, char * &res);
};

#define DVM_SYS_INTEGRATION_H

#endif //DVM_SYS_INTEGRATION_H
