#ifndef _STATPRINTF_H
#define _STATPRINTF_H
#include "bool.h"
#include "zlib.h"
#include "stdio.h"
class CStatPrintf {
public:
CStatPrintf(char * name,int lstr,char *mode);
~CStatPrintf();
void ChangeLenStr(int lstr);
BOOL Valid();
void TextErr(char *t);
int StatPrintf(const char *format,...);
private:
	int lll;
	char *pstr;
	int lenstr;
	FILE * ff;
	gzFile ffgz;
	BOOL valid;
	char texterr[80];
};
#endif
