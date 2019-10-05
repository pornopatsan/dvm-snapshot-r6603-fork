#include "types.h"
#include <stdlib.h>
#include <string.h>

const char s_cszSequentialBranch[] = "Sequential branch.";
const char s_cszTaskRegion[] = "TaskRegion";
const char s_cszTaskRegionPattern[] = "(No(#),Task(%))";
const char s_cszLoop[] = "Loop";
const char s_cszLoopPattern[] = "(No(#),Iter(%))";

CLevelList parse(const string& strContext) throw (string&)
{
    CLevelList vRes;

    string str = removeSpaces(strContext);

    if (0 == str.compare(s_cszSequentialBranch))
        return vRes;

    while (!str.empty() && '.' != str[0])
    {
        if (',' == str[0])
            str.erase(str.begin());

//        if (0 == str.compare(0, strlen(s_cszTaskRegion), s_cszTaskRegion))
//        if (0 == str.compare(s_cszTaskRegion)),0, strlen(s_cszTaskRegion)))
        if (0 == compareString(str,s_cszTaskRegion))
        {
            str.erase(0, strlen(s_cszTaskRegion));
            vector<long> vNums = parseString(s_cszTaskRegionPattern, str);
            if (vNums.size() < 2)
                throwBadFormat();

            vRes.push_back(CLevel(TASK_REGION, vector<long>(vNums.begin(), vNums.begin() + 1)));
            vRes.push_back(CLevel(ITERATION, vector<long>(vNums.begin() + 1, vNums.end())));
        }
//        else if (0 == str.compare(0, strlen(s_cszLoop), s_cszLoop))
//        else if (0 == str.compare(s_cszLoop),0, strlen(s_cszLoop)))
        else if (0 == compareString(str,s_cszLoop))
        {
			str.erase(0, strlen(s_cszLoop));
            vector<long> vNums = parseString(s_cszLoopPattern, str);
            if (vNums.size() < 2)
                throwBadFormat();

            vRes.push_back(CLevel(LOOP, vector<long>(vNums.begin(), vNums.begin() + 1)));
            vRes.push_back(CLevel(ITERATION, vector<long>(vNums.begin() + 1, vNums.end())));
        }
        else
            throwBadFormat();
    }

    return vRes;
}

vector<long> parseString(const string& strPattern, string& strContext) throw(string&)
{
    vector<long> vRes;

    string::iterator iContext = strContext.begin();
    string::const_iterator iPattern = strPattern.begin();
    while (iContext != strContext.end() && iPattern != strPattern.end())
    {
        switch (*iPattern)
        {
        case '#' : // Number
            {
                if (!isdigit(*iContext))
                    throwBadFormat();
                long lNum = atol(&(*iContext));
                while (isdigit(*iContext))
                    ++iContext;
                vRes.push_back(lNum);
                break;
            }
        case '%' : // Comma separated list of numbers
            {
                for (;;)
                {
                    if (!isdigit(*iContext))
                        throwBadFormat();
                    long lNum = atol(&(*iContext));
                    while (isdigit(*iContext))
                        ++iContext;
                    vRes.push_back(lNum);
                    if (',' != *iContext)
                        break;
                    ++iContext;
                }
                break;
            }
        default : // Other
            {
                if (*iPattern != *iContext)
                    throwBadFormat();
                ++iContext;
            }
        }

        ++iPattern;
    }
    if (iPattern != strPattern.end())
        throwBadFormat();

    strContext.erase(strContext.begin(), iContext);
    return vRes;
}

int compareString(const string& str,const char* templ)
{
    int size=strlen(templ);
	int j=0;
    for (string::const_iterator i = str.begin(); (j < size)&&(i != str.end()); ++i,++j)
    {
        if (templ[j] != *i ) return 1;
    }
return 0;
}

string removeSpaces(const string& str)
{
    string strRes;
    for (string::const_iterator i = str.begin(); i != str.end(); ++i)
    {
        if (' ' != *i && '\t' != *i && '\n' != *i && '"' != *i)
            strRes.append(1, *i);
    }
    return strRes;
}

void throwBadFormat() throw(string&)
{
    throw string("Bad format of context string.");
}
