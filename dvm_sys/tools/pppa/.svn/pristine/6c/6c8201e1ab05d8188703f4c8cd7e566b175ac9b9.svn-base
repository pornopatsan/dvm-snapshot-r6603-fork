#include "statprintf.h"
#include <string.h>
#include <stdarg.h>
CStatPrintf::CStatPrintf(char *name,int lstr,char *mode)
//name -file name
//lstr - string length for sprintf,used only for compressed file
// mode -file mode,"wb0"- not compress out file 
{
	valid=TRUE;
	ff=NULL;
	lenstr=lstr;
	pstr=new char[lstr];
	if (pstr==NULL) throw("Out of memory\n");
	if (strcmp(mode,"wb0")!=0)  {// compress file
		char *pname=new char[strlen(name)+4];
		if (pname==NULL) throw("Out of memory\n");
		strcpy(pname,name);
		strcat(pname,".gz");
		ffgz=gzopen(pname,mode);
		if (ffgz==NULL) {
			valid=FALSE;
			sprintf(texterr,"Can't open file %s\n",name);
			return;
		}
	} else {
		ff=fopen(name,"w");
		if (ff==NULL) {
			valid=FALSE;
			sprintf(texterr,"Can't open file %s\n",name);
			return;
		}
	}
	return;
}
//-------------------------------------------------
//return result of constructor execution
BOOL CStatPrintf::Valid()
{
	return(valid);
}
//-------------------------------------------
// error message
void CStatPrintf::TextErr(char *p)
{
	strcpy(p,texterr);
}
//------------------------------------------------
// change length of string, if it > lenstr
void CStatPrintf::ChangeLenStr(int lstr)
{
	if (lstr<=lenstr) return;
	char * ppstr=new char[lstr];
	if (ppstr==NULL) throw("Out of memory\n");
	delete []pstr;
	pstr=ppstr;
	lenstr=lstr;
	return;
}
//---------------------------------------------------------
int CStatPrintf::StatPrintf(const char *format,...)
	{
		va_list arglist;
		va_start(arglist,format);
		if (ff==NULL) { // compress file
			int len=vsprintf(pstr,format,arglist);
			if (len<=0) return 1;
			int ans=gzwrite(ffgz,pstr,unsigned(len));
			if (ans!=len) return 1;
		} else {
			vfprintf(ff,format,arglist);
		}
		va_end(arglist);
		return 0;
	}
//----------------------------------------------------------
CStatPrintf::~CStatPrintf()
{ 
	if (ff==NULL) {
		delete []pstr;
		gzclose(ffgz);
	} else fclose (ff);
	return;
}
