#include "interface.h"

int PPPA_GetStat(short *spath, short *&result)
{
	int retSize = -1;
	try
	{	
		char * cpath;
		int len;
		std::string resVal = "";
		statistics.clear();
		convertShortToChar(spath, len, cpath);
		statistics.init(cpath);
		statistics.to_string(resVal);
		copyStringToShort(result, resVal);
		retSize = (int)resVal.size() + 1;
	}
	catch (...)
	{
		retSize = -1;
	}
	printf("PPPA: return from DLL\n");
	return retSize;
}

int PPPA_GetCompStat(short *spath1, short * spath2, short *&result1, short *&result2)
{
	int retSize = -1;
	try
	{
		char * cpath1, *cpath2;
		int len1, len2;
		std::string resVal1 = "", resVal2="";
		comp1.clear();
		comp2.clear();
		cut_comp1.clear();
		cut_comp2.clear();
		convertShortToChar(spath1, len1, cpath1);
		convertShortToChar(spath2, len2, cpath2);
		comp1.init(cpath1);
		comp2.init(cpath2);
		stat_intersect(comp1, comp2, cut_comp1, cut_comp2);
		cut_comp1.to_string(resVal1);
		cut_comp2.to_string(resVal2);
		copyStringToShort(result1, resVal1);
		copyStringToShort(result2, resVal2);
		retSize = (int)resVal1.size() + 1;
	}
	catch (...)
	{
		retSize = -1;
	}
	printf("PPPA: return from DLL\n");
	return retSize;
}