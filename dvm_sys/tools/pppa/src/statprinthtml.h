#ifndef _STATPRINTHTML_H
#define _STATPRINTHTML_H
#include "bool.h"
#include <fstream>

class CStatPrintHtml {
public:
CStatPrintHtml(const char * name);
~CStatPrintHtml();
BOOL Valid();
void TextErr(char *t);
template <typename T>
int StatPrint(T x,...);
int StartTable();
int EndTable();
private:
    int table; //сейчас идет запись в таблицу
	std::fstream fs;
	BOOL valid;
	char texterr[80];
};
#endif
