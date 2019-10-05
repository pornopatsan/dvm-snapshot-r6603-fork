#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <fstream>
#include <vector>
#include <cstring>
#include <cstdlib>
#include <algorithm>

using namespace std;

struct LoopInfo
{
    char *version_of_sm;
    char *function_name;
    char *reg_info;
    char *cmem;
    char *cmem_all;
    int stack_frame;
    int spill_stores;
    int spill_loads;
    int used_reds;
    int line_number;
    int variant;
    int index_type;
    const char *deps;
    const char *loop;
    
    void correct_sm();
    void correct_name();
    void correct_regs();
    void correct_cmem();
};

struct comparator
{
    bool operator() (LoopInfo i, LoopInfo j)
    {
        if (i.line_number == j.line_number)
            return (i.variant < j.variant);
        else
            return (i.line_number < j.line_number);     
    }
} comp;

void LoopInfo::correct_sm()
{   
    int k = 0;
    for(int i = strlen(version_of_sm) - 2; version_of_sm[i] != '\'' ; i--, k++)
    {
        version_of_sm[4 - k] = version_of_sm[i];
    }
    version_of_sm[k] = '\0';
}

void LoopInfo::correct_name()
{       
    char *ck = strstr(function_name, "_cuda_kernel");
    if(ck)
    {
        if (strstr(function_name, "loop_"))
            loop = "Loop ";
        else if (strstr(function_name, "sequence_"))
            loop = "Sequence of statements ";
        else
            loop = "Unknown statements ";

        char *tmpstr = new char[2];
        tmpstr[1] = '\0';

        int k = 1;
        for (int i = -1; ck[i] != '_'; --i)
        {
            tmpstr[0] = ck[i];
            line_number += k * atoi(tmpstr);
            k *= 10;
        }

        char *ck = strstr(function_name, "_case");
        if (ck)
        {
            k = 1;
            for (int i = -1; ck[i] != '_'; --i)
            {
                tmpstr[0] = ck[i];
                variant += k * atoi(tmpstr);
                k *= 10;
            }
            if (variant == 1)
                deps = "dependency";
            else
                deps = "dependencies";

            for (int i = 1, k = 2; ; i++, k *= 2)
            {
                if (variant + 1 == k)
                {
                    variant = i;
                    break;
                }
            }
        }

        ck = strstr(function_name, "_kernel");
        index_type = 0;
        if (ck)
        {
            char *ckI = strstr(function_name, "_int");
            char *ckL = strstr(function_name, "_long");
            char *ckLL = strstr(function_name, "_llong");
                        
            if (ckI)
                index_type = 1;
            if (ckL)
                index_type = 2;
            if (ckLL)
                index_type = 3;
        }

        delete [] tmpstr;
    }
    else
        function_name[0] = '\0';
}

void LoopInfo::correct_regs()
{
    char *tmp = new char[128];
    int idc = 0;
    for(int i = 0; ; i++)
    {
        if(isdigit(reg_info[i]))
        {           
            int p = 0;
            for( ; reg_info[i] != ' '; ++i,++p)
            {
                tmp[p] = reg_info[i];
            }
            tmp[p] = '\0';
            if(idc == 0)
                stack_frame = atoi(tmp);
            else if(idc == 1)
                spill_stores = atoi(tmp);
            else if(idc == 2)
            {
                spill_loads = atoi(tmp);
                break;
            }
            idc++;
        }
    }
    
    int i = 0;
    int flag = 1;
    for( ;cmem[i] != ','; i++)
    {
        if(isdigit(cmem[i]) && flag == 1)
        {           
            int p = 0;
            for( ; cmem[i] != ' '; ++i,++p)
            {
                tmp[p] = cmem[i];
            }
            tmp[p] = '\0';
            used_reds = atoi(tmp);
            flag = 0;
        }       
    }
    delete []tmp;
    tmp = new char[strlen(cmem)];

    int idx = 0;
    for(size_t k = i + 1; k < strlen(cmem); ++k, ++idx)
    {
        tmp[idx] = cmem[k];
    }
    tmp[idx] = '\0';
    strcpy(cmem, tmp);
    delete []tmp;
}

void LoopInfo::correct_cmem()
{
    cmem_all = new char[strlen(cmem) + 256];
    cmem_all[0] = '\0';
    char *tmp = new char[16];

    int k = 0;
    for(size_t i = 0; i < strlen(cmem); ++i, ++k)
    {
        if(cmem[i] == 'c' || cmem[i] == 'l' || cmem[i] == 's' || cmem[i] == 'g')
        {
            if(i + 3 < strlen(cmem))    
            {
                if(cmem[i+1] == 'm' && cmem[i+2] == 'e' && cmem[i+3] == 'm')
                {
                    cmem_all[k] = '\0';
                    if(cmem[i] == 'c')
                    {
                        strcat(cmem_all, "of constant memory ");
                        k += strlen("of constant memory ");
                    }
                    else if(cmem[i] == 'l')
                    {
                        strcat(cmem_all, "of local memory ");
                        k += strlen("of local memory ");
                    }
                    else if(cmem[i] == 's')
                    {
                        strcat(cmem_all, "of shared memory ");
                        k += strlen("of shared memory ");
                    }
                    else if(cmem[i] == 'g')
                    {
                        strcat(cmem_all, "of global memory ");
                        k += strlen("of global memory ");
                    }
                    i += 4;
                    if(cmem[i] == '[')
                    {
                        i++;
                        int p = 0;
                        for( ;cmem[i] != ']'; ++i)
                        {
                            tmp[p] = cmem[i];
                            p++;
                        }
                        tmp[p] = '\0';
                        strcat(cmem_all, "in bank ");
                        strcat(cmem_all, tmp);
                        k += strlen(tmp) + strlen("in bank ");
                    }

                    if (cmem[i] == ',')
                    {
                        strcat(cmem_all, ",");
                        k++;
                    }
                    
                    k = k - 1;
                    
                }
                else 
                    cmem_all[k] = cmem[i];
            }
            else 
                cmem_all[k] = cmem[i];
        }
        else
            cmem_all[k] = cmem[i];
    }
    cmem_all[k] = '\0';
    delete []tmp;
}

int count_of_arch(vector<LoopInfo> &kernels)
{
    int count = 1;
    char *fver = kernels[0].version_of_sm;
    for(size_t i = 1; i < kernels.size() - 1; ++i)
    {
        if(strcmp(fver, kernels[i].version_of_sm) != 0)
            count ++;
    }
    return count;
}


int main(int argc, char** argv)
{   
    //ifstream cin(argv[1]); // for debuging

    vector<LoopInfo> allKernels;
    vector<char*> allStr;
    char *first_line = new char[1024];
    bool firstLine = false;
    int flag_sm = 1;    
    
    while(!cin.eof())
    {
        char *buf = new char[1024];
        cin.getline(buf, 4096);
        if(buf[strlen(buf) - 1] == '\r')
        {
            buf[strlen(buf) - 1] = '\0';
        }
        if(strlen(buf) != 0)
            allStr.push_back(buf);
    }
    /*cout << "debug " << allStr.size() << endl;
    for(int i = 0; i < allStr.size(); ++i)
        cout << allStr[i] << endl;*/
    int startIdx = 0;
    if(allStr.size() == 0)
    {
        cout << "0 kernels for parallel loop compiled\n";
        return 1;
    }

    if(strstr(allStr[0], "Compiling") == NULL)
    {
        first_line = allStr[0];
        for(size_t i = 0; i < strlen(first_line) - 16; ++i)
            first_line[i] = first_line[i + 16];
        first_line[strlen(first_line) - 16] = '\0';
        startIdx++;
        firstLine = true;
    }

    for(size_t i = startIdx; i < allStr.size(); )
    {
        bool fullSize = true;

        if(strstr(allStr[i], "Compiling") == NULL)
        {
            i++;
            continue;
        }
        else if(!strstr(allStr[i], "loop_") && !strstr(allStr[i], "sequence_"))
        {
            i++;
            continue;
        }

        LoopInfo tmp;
        char *buf = new char[1024];

        buf = allStr[i];
        tmp.version_of_sm = new char[strlen(buf) + 1];
        tmp.version_of_sm[0] = '\0';
        strcat(tmp.version_of_sm, buf);     
        i++;

        buf = allStr[i];
        if (strstr(buf, "Function properties") == NULL)
            fullSize = false;

        if (fullSize)
        {
            tmp.function_name = new char[strlen(buf) + 1];
            tmp.function_name[0] = '\0';
            strcat(tmp.function_name, buf);
            i++;
        }
        else
        {
            tmp.function_name = NULL;
            int count = 1;
            int i1 = 0;
            int i_ = -1;
            char *tbuf = NULL;
            char *pbuf = allStr[i - 1];

            while (pbuf[i1] != '\'')
                i1++;
            i_ = i1 + 1;
            i1++;
            while (pbuf[i1] != '\'')
            {
                count++;
                i1++;
            }

            tbuf = new char[count];         
            tmp.function_name = new char[count + 1 + strlen("Function properties for ")];
            tbuf[count - 1] = tmp.function_name[0] = '\0';
            strcat(tmp.function_name, "Function properties for ");
            i1 = i_;
            i_ = 0;
            while (pbuf[i1] != '\'')
            {
                tbuf[i_] = pbuf[i1];
                i_++;
                i1++;
            }
            strcat(tmp.function_name, tbuf);
            delete[] tbuf;
        }

        if (fullSize)
        {
            buf = allStr[i];
            tmp.reg_info = new char[strlen(buf) + 1];
            tmp.reg_info[0] = '\0';
            strcat(tmp.reg_info, buf);
            i++;
        }
        else
            tmp.reg_info = (char*)"0 bytes stack frame, 0 bytes spill stores, 0 bytes spill loads";
                
        buf = allStr[i];
        tmp.cmem = new char[strlen(buf) + 1];
        tmp.cmem[0] = '\0';
        strcat(tmp.cmem, buf);
        i++;

        tmp.stack_frame = 0;
        tmp.spill_loads = 0;
        tmp.spill_stores = 0;
        tmp.used_reds = 0;
        tmp.line_number = 0;
        tmp.variant = 0;

        allKernels.push_back(tmp);
    }
    
    if(allKernels.size() == 0)
    {
        delete[]first_line;
        allStr.clear();
        allKernels.clear();
        cout << "0 kernels for parallel loop compiled\n";
        return 1;
    }

    if(firstLine)
    {
        LoopInfo tmp;
        allKernels.push_back(tmp);
        allKernels[allKernels.size() - 1].cmem = first_line;
        allKernels[allKernels.size() - 1].correct_cmem();
    }
    else
    {
        LoopInfo tmp;
        allKernels.push_back(tmp);
    }
    
    for(size_t i = 0; i < allKernels.size() - 1; ++i)
    {
        if (allKernels[i].version_of_sm)
            allKernels[i].correct_sm();
        if (allKernels[i].function_name)
            allKernels[i].correct_name();
        if (allKernels[i].reg_info)
            allKernels[i].correct_regs();
        if (allKernels[i].cmem)
            allKernels[i].correct_cmem();
    }
    sort(allKernels.begin(), allKernels.begin() + allKernels.size() - 1, comp);
    flag_sm = count_of_arch(allKernels);
    if(argc == 1)
        cout << "     Information of CUDA Ptx assembler for compiled module 'unknown':" << endl;
    else
        cout << "     Information of CUDA Ptx assembler for compiled module '" << argv[1] << "':" << endl;
    if(flag_sm == 1)
        cout << "Compiled all kernels for " << allKernels[0].version_of_sm << " architecture" << endl;
    if(firstLine)
        cout << "Used " << allKernels[allKernels.size() - 1].cmem_all << endl;
    //cout << endl;

    const char *rt_TYPE[] = { "0", "int", "long", "long long" };
    for(size_t i = 0; i < allKernels.size() - 1; ++i)
    {       
        if(allKernels[i].line_number == 0)
            continue;
        if(i != 0)
            cout << endl;
        if(allKernels[i].variant == 0)
            cout << allKernels[i].loop << "on line " << allKernels[i].line_number << ":" << endl;
        else
            cout << allKernels[i].loop << "on line " << allKernels[i].line_number << " (with " << allKernels[i].variant << " " << allKernels[i].deps << "):" << endl;
        if(flag_sm != 1)
            cout << "  " << "Compiling for " << allKernels[i].version_of_sm << " architecture" << endl;
        cout << "  " << "Used " << allKernels[i].used_reds << " registers" << endl;
        if(allKernels[i].spill_stores != 0)
            cout << "  " << "Used " << allKernels[i].spill_stores << " bytes spill stores (" << allKernels[i].spill_stores / 4 << " 4-byte words)"  << endl;
        if(allKernels[i].spill_loads != 0)
            cout << "  " << "Used " << allKernels[i].spill_loads << " bytes spill loads (" << allKernels[i].spill_loads / 4 << " 4-byte words)"  << endl;
        if(allKernels[i].stack_frame != 0)
            cout << "  " << "Used " << allKernels[i].stack_frame << " bytes stack frames" << endl;
        cout << "  " << "Used" << allKernels[i].cmem_all << endl;  
        if (allKernels[i].index_type != 0)
            cout << "  " << "Used type '" << rt_TYPE[allKernels[i].index_type] << "' for indexing" << endl;
    }

    delete[]first_line; 
    allStr.clear();
    allKernels.clear();
    //getchar(); // for debuging
    return 0;
}