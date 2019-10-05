#if !defined( _POTENSYN_H )
#define _POTENSYN_H
#include "zlib.h"
#include "bool.h"
#include "inter.h"
#include "treeinter.h"
struct syn_short{
	short nitem;
};
struct syn_long{
	unsigned long ninter;
};
struct syn_void{
	void* ppgrp;
};
struct syn_double{
	double time;
};
#define QSYN_SHORT sizeof(syn_short)/SZSH
#define QSYN_LONG sizeof(syn_long)/SZL
#define QSYN_VOID sizeof(syn_void)/SZV
#define QSYN_DOUBLE sizeof(syn_double)/SZD
typedef struct tsyn {
           syn_double d;
           syn_void v;
           syn_long l;
           syn_short sh;
}psyn;
class CSynchro {
public:
	CSynchro(gzFile stream,unsigned long l,unsigned char *pbuff);
	~CSynchro();
	BOOL Valid();
	void TextErr(char *t);
	BOOL Count(unsigned long nin,short waserr);
	int GetCount(typecollect nitem);
	double Find(typecollect nitem);
	double GetCurr(void);
	double FindNearest(typecollect nitem);
private:
	BOOL valid;
	char texterr[80];
	psyn *ps;
	unsigned long qsyn;
    unsigned long ninter;
	unsigned char *pbuff_read;
	int qoper[QCOLLECT+QCOLLECT];
	unsigned long ncurr;
	short first;
	int err;
};
#endif
