#include "statlist.h"


CStat comp1, comp2, statistics, cut_comp1, cut_comp2;

static int strLen(const short *shString)
{
	int t = 0;
	if (shString == NULL)
		return 0;
	while (shString[t] != '|')
		t++;
	return t;
}

static void convertShortToChar(const short *shString, int &strL, char *&chString)
{
	strL = strLen(shString);
	chString = new char[strL + 1];
	for (int i = 0; i < strL; ++i)
		chString[i] = (char)shString[i];
	chString[strL] = '\0';
}

static void copyStringToShort(short *&result, const std::string &resVal)
{
	result = new short[resVal.size() + 1];
	for (unsigned int i = 0; i < resVal.size(); ++i)
		result[i] = resVal[i];
	result[resVal.size()] = (short)'\0';
}

extern "C" { __declspec(dllexport) int PPPA_GetStat(short *spath, short *&result); }
extern "C" { __declspec(dllexport) int PPPA_GetCompStat(short *spath1, short *spath2,short *&result1, short*&result2); }
