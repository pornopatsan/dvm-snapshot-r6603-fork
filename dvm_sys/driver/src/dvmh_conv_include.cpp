#define _CRT_SECURE_NO_WARNINGS
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <map>
#include <algorithm>
#include <string>
#include <vector>
#include <fstream>

static std::map<char, const char *> SpecialSymbols;
static int sizeOfProgName = 0;

static char *convertLine(const char *buf) {
    int count = 0;
    for (unsigned i = 0; i < strlen(buf); i++) {
        if (SpecialSymbols.find(buf[i]) != SpecialSymbols.end())
            count += strlen(SpecialSymbols[buf[i]]);
    }

    char *newBuf = new char[strlen(buf) + count + 1];

    unsigned k = 0;
    for (unsigned i = 0; i < strlen(buf); i++) {
        if (SpecialSymbols.find(buf[i]) != SpecialSymbols.end()) {
            const char *tmp = SpecialSymbols[buf[i]];
            for (unsigned k1 = 0; k1 < strlen(tmp); k1++, k++)
                newBuf[k] = tmp[k1];
        } else {
            newBuf[k] = buf[i];
            k++;
        }
    }
    newBuf[k] = '\0';

    return newBuf;
}

static bool isSpecLine(const char *buf) {
    bool retval = false;
    if (strstr(buf, "#pragma once") != NULL)
        retval = true;
    else if (strstr(buf, "#include") != NULL)
        retval = true;

    return retval;
}

static void eraseInfo(char *&convLine) {
    char *find = strstr(convLine, "dvm_gpu_");
    if (find != NULL) {
        memset(find, ' ', sizeof(char)* (8 + sizeOfProgName + 1)); // erase "dvm_gpu_<progName>_"
        find += (8 + sizeOfProgName + 1);
        int i = 0;
        while (find[i] != '(') {
            i++;
        }
        find[i - 1] = ' '; // erase last "_" in function name
    }
}

static char *correctName(char *convLine) {
    char *find = strstr(convLine, "void");
    std::string out = "";
    if (find != NULL) {
        find += 5; // skip "void "

        while (find[0] == ' ')
            find++;
        memset(find, ' ', sizeof(char)* (8 + sizeOfProgName + 1)); // erase "dvm_gpu_<progName>_"
        find += (8 + sizeOfProgName + 1);
        int i = 0;
        while (find[i] != '(') {
            out.push_back(find[i]);
            i++;
        }
        find[i - 1] = ' '; // erase last "_" in kernel name
    }

    const char *str = out.c_str();
    char *ret = new char[strlen(str) + 1];
    strcpy(ret, str);
    ret[strlen(str) - 1] = '\0'; // erase last "_"

    return ret;
}

static void correctBrackers(const char *convLine, int &balance) {
    int i = 0;
    while (convLine[i] != '\0') {
        if (convLine[i] == '{')
            balance++;
        else if (convLine[i] == '}')
            balance--;
        i++;
    }
}

int main(int argc, char ** argv)
{
    SpecialSymbols.insert(std::make_pair('\n', "\\n\"\n\""));
    SpecialSymbols.insert(std::make_pair('"', "\\\""));
    SpecialSymbols.insert(std::make_pair('\\', "\\\\"));
    int balance = 0;
    int kernel_body = 0;
    int device_body = 0;

    if(argc == 3) { // convert FTN_Cuda file
        char *progName = argv[2];
        sizeOfProgName = strstr(progName, ".DVMH") - progName;

        char *buf = new char[4096];
        std::vector<std::string> kernelNames;
        std::vector<std::string> toPrintKernels;
        std::vector<std::string> deviceFunctions;
        std::map<std::string, int> dictK;
        std::map<std::string, int> dictF;

        toPrintKernels.reserve(256);
        deviceFunctions.reserve(256);
        if(std::cin.good()) {
            while (!std::cin.eof()) {
                std::cin.getline(buf, 4096);
                if (isSpecLine(buf) == false) {
                    char *convLine = convertLine(buf);
                    correctBrackers(convLine, balance);
                    if (strstr(convLine, "__global__") != NULL) {
                        toPrintKernels.push_back("");
                        kernelNames.push_back("");
                        char *var = correctName(convLine);
                        dictK[var] = toPrintKernels.size() - 1;                    
                        kernelNames.back() = kernelNames.back() + "static const char *" + var + " = \n";
                        toPrintKernels.back() = toPrintKernels.back() + "\"" + convLine + "\\n\"\n";
                        delete []var;
                        kernel_body = 1;
                    } else if (strstr(convLine, "__device__") != NULL) {
                        deviceFunctions.push_back("");
                        char *var = correctName(convLine);
                        dictF[var] = deviceFunctions.size() - 1;
                        deviceFunctions.back() = deviceFunctions.back() + "\"" + convLine + "\\n\"\n";
                        delete[]var;
                        device_body = 1;
                    } else if (balance == 0 && kernel_body) {
                        eraseInfo(convLine);
                        toPrintKernels.back() = toPrintKernels.back() + "\"" + convLine + "\\n\"\n" + ";\n";
                        kernel_body = 0;
                    } else if (balance == 0 && device_body) {
                        eraseInfo(convLine);
                        deviceFunctions.back() = deviceFunctions.back() + "\"" + convLine + "\\n\"\n";
                        device_body = 0;
                    } else if (kernel_body) {
                        eraseInfo(convLine);
                        toPrintKernels.back() = toPrintKernels.back() + "\"" + convLine + "\\n\"\n";
                    } else if (device_body) {
                        eraseInfo(convLine);
                        deviceFunctions.back() = deviceFunctions.back() + "\"" + convLine + "\\n\"\n";
                    }
                    delete[] convLine;
                }
            }
            
            std::ifstream cuda_info(argv[1]);
            while (cuda_info.good() && !cuda_info.eof()) {
                cuda_info.getline(buf, 4096);
                int count = strlen(buf);
                if (strstr(buf, "//DVMH_CALLS") != NULL) {
                    char *find = strstr(buf, " ");
                    char nameOfKernel[512];
                    char nameOfFunc[512];

                    int i = 1;
                    while (find[i] != ':') {
                        nameOfKernel[i - 1] = find[i];
                        i++;
                    }
                    nameOfKernel[i - 1] = '\0';
                    i++;

                    while (find[i] == ' ') // skip spaces
                        i++;
                    int idxOfKernel = dictK[std::string(nameOfKernel)];

                    while (find[i] != '\0') {
                        int k = 0;
                        while (find[i] != ' ' && find[i] != '\0') {
                            nameOfFunc[k] = find[i];
                            i++;
                            k++;
                        }
                        nameOfFunc[k] = '\0';

                        while (find[i] == ' ') // skip spaces
                            i++;

                        int idxOfFunc = dictF[std::string(nameOfFunc)];
                        toPrintKernels[idxOfKernel] = deviceFunctions[idxOfFunc] + toPrintKernels[idxOfKernel];
                    }
                }
            }

            for (unsigned i = 0; i < toPrintKernels.size(); ++i) {
                std::cout << kernelNames[i] << toPrintKernels[i];
            }

            delete[] buf;
        }        
    } else if (argc == 2) { // convert to string variable
        std::cout << argv[1] << std::endl;
        if(std::cin.good()) {
            char *buf = new char[4096];
            while (!std::cin.eof()) {
                std::cin.getline(buf, 4096);
                if (isSpecLine(buf) == false) {
                    char *convLine = convertLine(buf);
                    std::cout << "\"" << convLine << "\\n\"\n";

                    delete[] convLine;
                }
            }
            delete[] buf;
        } else {
            std::cout << "\"\"" << std::endl;
        }
        
        std::cout << ";" << std::endl;
    }

    return 0;
}
