#define _STATFILE_
#include <stdio.h>
#include <string.h>
#include "strall.h"
#include "potensyn.h"
short reverse,szsh,szd,szv,szl,torightto,torightfrom;
// Dynamically allocate array of synchronization times
CSynchro::CSynchro(gzFile stream,unsigned long lsyn,unsigned char *pbuff)
// stream - pointer to the file written during DVM-program execution
// lsyn - length of written information
{
	valid=TRUE;
	ps=NULL;
	pbuff_read=NULL;
	// dynamically allocate array of char from file (strall.h tsyn_ch)
	unsigned char *buffer;
	if (pbuff==NULL) {
		buffer=new unsigned char[lsyn];
		if (buffer==NULL) throw("Out of memory\n");
		pbuff_read=buffer;
		long l=gztell(stream);
	// read from file to the allocated buffer
		int s=gzread(stream,buffer,lsyn);
		if ((unsigned long)s!=lsyn) {
			valid=FALSE;
			sprintf(texterr,"Can't read synchronization times from file addr=%ld,length=%ld\n",
			l,lsyn);
			if (pbuff_read!=NULL) {
				delete []pbuff_read;
				pbuff_read=NULL;
			}
			return;
		}
	}else buffer=pbuff;
	// calculate size of struct tsyn_ch
	unsigned long lsynone=QSYN_SHORT*szsh+QSYN_LONG*szl+QSYN_VOID*szv+
		QSYN_DOUBLE*szd;
	//set pointer to the first synchronization time
	unsigned char *p=buffer+lsyn-lsynone;
	// number of synchronization times
 	qsyn=lsyn/lsynone;
	// allocate array of struct tsyn
	ps=new psyn[qsyn];
	if (ps==NULL) throw("Out of memory\n");
	psyn *psl=ps;
	unsigned long i;
	// copy values from struct of char to struct tsyn
	for (i=0;i<qsyn;i++) {
		psl->sh.nitem=0; psl->l.ninter=0; psl->v.ppgrp=NULL; psl->d.time=0.0;
		CPYMEM(psl->sh.nitem,p+MAKESHORT(ps,nitem,nitem),szsh);
		CPYMEM(psl->l.ninter,p+MAKELONG(ps,ninter,ninter,QSYN_SHORT),szl);
  		// ppgrp -reference to group, used only for compare values
		CPYMEM(psl->v.ppgrp,p+MAKEVOID(ps,ppgrp,ppgrp,QSYN_SHORT,QSYN_LONG),szv);
		CPYMEM(psl->d.time,p+MAKEDOUBLE(ps,time,time,QSYN_SHORT,QSYN_LONG,QSYN_VOID),szd);
		p=p-lsynone;
		psl++;
	}
	if (pbuff_read!=NULL) {
				delete []pbuff_read;
				pbuff_read=NULL;
			}
}
//------------------------------------------
// deallocate  struct tsyn and memory of group references
CSynchro::~CSynchro()
{
	if (ps==0) return;
	if (pbuff_read!=NULL) delete []pbuff_read;
	delete []ps;
}
//---------------------------------------------------------
//return result of constructor execution
int CSynchro::Valid()
{
	return(valid);
}
//-------------------------------------------
//error message
void CSynchro::TextErr(char *p)
{
	strcpy(p,texterr);
}
//-----------------------------------------------
// calculate number of times different types
BOOL CSynchro::Count(unsigned long n,short waserr )
// n - number of interval
// waserr - sign of error during accumulating times 
{
	if (n==0) return(0);
	//qoper - array of number of times
	for (int j=0;j<QCOLLECT+QCOLLECT;j++) {
		qoper[j]=0;
	}
	ncurr=0;
	// set current ninter
	ninter=n;
	psyn *pp=ps;
	// calculate number of times, nitem - type of time
	for (unsigned long i=0;i<qsyn;i++) {
		if (pp->l.ninter==n) {
			qoper[pp->sh.nitem-1]++;
		}
		pp++;
	}
	// veryfy number of calls and number of returns
	if (waserr!=0) return(0);
	for (int ak=0;ak<QCOLLECT;ak=ak+4) {
		// number of start calls and number of wait calls
		if (qoper[ak]!=qoper[ak+QCOLLECT] || qoper[ak+2]!=qoper[ak+2+QCOLLECT]) {
			valid=FALSE;
			sprintf(texterr,"Number of calls !=number of returns interval=%ld\n",n);
			return(1);
		}
	}
	return(0);
}
//-------------------------------------------
// return number of synchronization time, function call after Count()
int CSynchro::GetCount(typecollect nitem1)
// nitem1 - type of synchronyzation time
{
	short nitem=(short)nitem1;
	ncurr=0;
	first=1; // the first Find
	return (qoper[nitem-1]);
}
//--------------------------------------------
//return next synchronyzation time,call after Count()
double CSynchro::Find(typecollect nitem1)
// nitem1 - type of synchronyzation time
{
	short nitem=(short)nitem1;
	if (first!=1) ncurr++;
	first++;
	psyn *psl=ps+ncurr;
	// ninter set Count()
	for (unsigned long i=ncurr;i<qsyn;i++) {
		if (psl->sh.nitem==nitem && psl->l.ninter==ninter) {
			ncurr=i; // set current synchronization time
			return(psl->d.time);
		}
		psl++;
	}
	ncurr=0;
	return(0.0);
}
//-----------------------------------------------------
// return current synchronyzation time, call after Find()
double CSynchro::GetCurr(void)
{
	// ncurr set find
	if (ncurr>=qsyn) return(0.0);
	psyn *psl=ps+ncurr;
	return(psl->d.time);
}
//--------------------------------------------------------
//return nearest time from current
double CSynchro::FindNearest(typecollect nitem1)
// nitem1 - type of synchronyzation time
// for overlap, for call_wait_operation find ret_start_operation
{
	if (ncurr>=qsyn ) return(0.0);
	short nitem=(short)nitem1;
	psyn *psl=ps+ncurr;
	psyn *psl_curr=psl;
	for (unsigned long i=ncurr;;i--) {
		if (psl->sh.nitem==nitem) {
			if (psl->v.ppgrp==psl_curr->v.ppgrp) {
				return(psl->d.time);
			}
		}
		psl--;
		if (i==0) return(0.0);
	}
}
