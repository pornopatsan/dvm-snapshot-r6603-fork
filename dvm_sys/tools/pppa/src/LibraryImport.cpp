//
// Created by Пенёк on 14.09.2020.
//

#include <jni.h>
#include <stdio.h>
#include <iostream>
#include <string>
#include "LibraryImport.h"
#include "statlist.h"

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

JNIEXPORT jstring JNICALL Java_LibraryImport_readStat(JNIEnv * env, jobject obj, jstring s)
{
//    printf("Hello, Java! I am C++!!! Hm\n");
    jboolean isCopy;
    char *path = (char *) (env)->GetStringUTFChars(s, &isCopy);
//    std::string string = std::string(path, strlen(path));
//    std::cout << string << "\n";
//    char msg[60] = "Puk puk";
//    std::string str = "Std::string";
//    jstring result = (env)->NewStringUTF(msg);

    CStat stat;
    stat.init(path);
    if (!stat.isinitialized)
        return NULL;
    char *res;
    _stat_to_char(stat, res);
    return (env)->NewStringUTF(res);
}