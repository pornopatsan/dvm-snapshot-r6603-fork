#include <iostream>
using namespace std;
#include "types.h"
#include "trace.h"

const int BUFFER_SIZE = 1024;

void findMatches(const CLevelList& vDefLevel, ifstream& in)
{
    char szBuff[BUFFER_SIZE + 1];

    CLevelList vCurrLevel;
    bool bHeader = true;
    for (long lLine = 1; !in.eof(); ++lLine)
    {
        in.getline(szBuff, BUFFER_SIZE);
        string strCurrent = removeSpaces(szBuff);
        if (strCurrent.empty() || '#' == strCurrent[0])
            continue;

        int nKey;
        for (nKey = 0; nKey < N_UNKNOWN; ++nKey)
            if (0 == strCurrent.find(g_rgKeywords[nKey]))
                break;

        switch (nKey)
        {
            case N_END_HEADER :
                bHeader = false;
                break;

            case N_SLOOP:
            case N_PLOOP:
            case N_TASKREGION:
            {
                if (bHeader)
                    break;

                try
                {
                    vector<long> vNums = parseString(string(g_rgKeywords[nKey]) + ":#(", strCurrent);
                    vCurrLevel.push_back(CLevel(nKey == N_TASKREGION ? TASK_REGION : LOOP, vNums));
                    compare(vCurrLevel, vDefLevel, lLine);
                }
                catch (const string&)
                {
                    printBad(lLine);
                }

                break;
            }
            case N_ITERATION:
            {
                try
                {
                    vector<long> vNums = parseString(string(g_rgKeywords[nKey]) + ":#,(%)", strCurrent);

                    if (vNums.size() > 1)
		                        {
					                        if (ITERATION == vCurrLevel.back().m_eType)
								                        {
											                            //vCurrLevel.back().m_vIndexes.assign(vNums.begin() + 1, vNums.end());
														    		    vCurrLevel.back().m_vIndexes.erase(vCurrLevel.back().m_vIndexes.begin(), vCurrLevel.back().m_vIndexes.end());                                       
																                         vCurrLevel.back().m_vIndexes.insert(vCurrLevel.back().m_vIndexes.begin(),vNums.begin() + 1, vNums.end());
																			                         }
																						                         else
																									                         {
																												                             vCurrLevel.push_back(CLevel(ITERATION, vector<long>(vNums.begin() + 1, vNums.end())));
																															                             }
																																		                             compare(vCurrLevel, vDefLevel, lLine);
																																					                         }
																																								                     else
																																										                         {
																																													                         printBad(lLine);
																																																                     }
																																																		                     }
																																																				     
                catch (const string&)
                {
                    printBad(lLine);
                }

                break;
            }
            case N_END_LOOP:
            {
                if (bHeader)
                    break;

                try
                {
                    if (0 == vCurrLevel.size())
                        throwBadFormat();

                    vector<long> vNums = parseString(string(g_rgKeywords[nKey]) + ":#;", strCurrent);

                    // Remove last iteration
                    if (ITERATION == vCurrLevel.back().m_eType)
                        vCurrLevel.pop_back();

                    // Remove loop
                    if (0 != vCurrLevel.size() && ITERATION != vCurrLevel.back().m_eType)
                    {
                        if (vCurrLevel.back().m_vIndexes != vNums)
                            printBad(lLine);

                        vCurrLevel.pop_back();
                        if (0 == vCurrLevel.size())
                            compare(vCurrLevel, vDefLevel, lLine);
                    }
                    else
                    {
                        printBad(lLine);
                    }
                }
                catch (const string&)
                {
                    printBad(lLine);
                }
                break;
            }
            default :
                break;
        }
    }
}

void compare(const CLevelList& vLevel1, const CLevelList& vLevel2, long lLine)
{
    if (vLevel1 == vLevel2)
    {
        cout << "Line " << lLine << "\n";
    }
}

void printBad(long lLine)
{
    cout << "Bad trace format. Line " << lLine << "\n";
}

const char *g_rgKeywords[] =
{
    "FULL",
    "MODIFY",
    "MINIMAL",
    "NONE",
    "MODE",
    "EMPTYITER",
    "SL",
    "PL",
    "TR",
    "IT",
    "BW",
    "AW",
    "RV_BW",
    "RV_AW",
    "RV_RD",
    "RV",
    "RD",
    "SKP",
    "EL",
    "END_HEADER",
    "ARR",
    "MULTIDIMARR",
    "DEFARRSTEP",
    "DEFITERSTEP"
};
