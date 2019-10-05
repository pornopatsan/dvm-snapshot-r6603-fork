#define _STATFILE_
#include "treeinter.h"
#include <stdio.h>
#include <string.h>
#include "dvmh_stat.h"

// @todo! ПРОВЕРИТЬ БЕЗ GZ+

extern short reverse,szsh,szd,szv,szl,torightto,torightfrom;
// list of intervals for each processor
CTreeInter::CTreeInter(gzFile stream,unsigned long lint,char *pbuff,
					   unsigned int n,unsigned long qint,short maxn,
					   char * ppn,double proct,
					   int iIM,int jIM,short sore,
					   unsigned char *pbuffer,
					   dvmh_stat_header *dvmhStatHeader)
// stream-file descriptor pointer,
//lint- information length in bytes,
// pbuff - beginning of the buffer at the collection stage,
//n - processor number,
//qint - number of intervals
//maxn - maximal nesting level
// ppn - processor name
// proct - processor time
//iIm- 0/1 sign of summing on index i
//jIM-0/1 sign of summing on index j
//sore - sign of summing or elements print
//pbuffer - file gz+,data have been read
{
	valid=TRUE;
	nproc=n;
	qinter=qint;
	maxnlev=maxn;
	curninter=1;
	pt=NULL;
	pprocname=NULL;
	sign_buffer=NULL;
	unsigned char *buffer;

	// -- dvmh
	this->dvmhStatHeader = dvmhStatHeader;
	// --

	if (ppn!=NULL) {// processor name
		if (dvmhDebug) { printf("   Process name: %s\n", ppn); fflush(stdout); }
		pprocname=new char[strlen(ppn)+1];
		if (pprocname==NULL) throw("Out of memory\n");
		strcpy(pprocname,ppn);
	}

	proctime=proct;
	// dynamically allocate memory for intervals of struct tinter_ch
	if (pbuffer==NULL) { //data had not been read
		buffer=new unsigned char[lint];
		if (buffer==NULL) throw("Out of memory\n");
		sign_buffer=buffer;
		long l=gztell(stream);
		// read interval information from file
		int s=gzread(stream,buffer,lint);
		if ((unsigned long)s!=lint) {
			valid=FALSE;
			sprintf(texterr,"Can't read intervals from file, addr=%ld, length=%ld\n",
					l,lint);
			delete []sign_buffer;
			sign_buffer=NULL;
			return;
		}
	} else buffer=pbuffer;

	unsigned char *pch=buffer;
	pinter *pi=NULL;

	// allocate memory for intervals of struct ttree
	pt=new ptree[qinter];
	if (pt==NULL) throw("Out of memory\n");
	ident id;
	// calculate size of interval without name of DVM-programm

	unsigned long offsetDvmhStat = QI_SHORT*szsh+QI_LONG*szl+QI_VOID*szv+QI_DOUBLE*szd; // -- dvmh
//	int lintone = QI_SHORT*szsh+QI_LONG*szl+QI_VOID*szv+QI_DOUBLE*szd + this->dvmhStatHeader->sizeIntervalConstPart; // -- dvmh
	int commonLengthForInterval=QI_SHORT*szsh+QI_LONG*szl+QI_VOID*szv+QI_DOUBLE*szd + this->dvmhStatHeader->sizeIntervalConstPart; // -- dvmh
	int extraLengthForThreads = 0;

	s_GRPTIMES times[StatGrpCount][StatGrpCount];
	int a=MAKEDOUBLE(pi,times[0],nenter,QI_SHORT,QI_LONG,QI_VOID);
	a=MAKELONG(pi,nline,nline,QI_SHORT);

	for (unsigned long ll=0;ll<qinter;ll++) {
		int lt=0;
		// copy time characteristics from file
		for (int i=0;i<StatGrpCount;i++) {
			for (int j=0;j<StatGrpCount;j++) {
				times[i][j].CallCount=0.0; times[i][j].ProductTime=0.0; times[i][j].LostTime=0.0;
				CPYMEM(times[i][j].CallCount,
					   pch+MAKEDOUBLE(pi,times[lt],nenter,QI_SHORT,QI_LONG,QI_VOID),szd);
				CPYMEM(times[i][j].ProductTime,
					   pch+MAKEDOUBLE(pi,times[lt+1],nenter,QI_SHORT,QI_LONG,QI_VOID),szd);
				CPYMEM(times[i][j].LostTime,
					   pch+MAKEDOUBLE(pi,times[lt+2],nenter,QI_SHORT,QI_LONG,QI_VOID),szd);
				lt=lt+3;
			}
		}
		// add information to interval matrix
		s_SendRecvTimes addinfo;
		addinfo.SendCallTime=0.0;
		addinfo.MinSendCallTime=0.0;
		addinfo.MaxSendCallTime=0.0;
		addinfo.SendCallCount=0;
		addinfo.RecvCallTime=0.0;
		addinfo.MinRecvCallTime=0.0;
		addinfo.MaxRecvCallTime=0.0;
		addinfo.RecvCallCount=0;
		CPYMEM(addinfo.SendCallTime,pch+MAKEDOUBLE(pi,SendCallTime,nenter,QI_SHORT,QI_LONG,QI_VOID),szd);
		CPYMEM(addinfo.MinSendCallTime,pch+MAKEDOUBLE(pi,MinSendCallTime,nenter,QI_SHORT,QI_LONG,QI_VOID),szd);
		CPYMEM(addinfo.MaxSendCallTime,pch+MAKEDOUBLE(pi,MaxSendCallTime,nenter,QI_SHORT,QI_LONG,QI_VOID),szd);
		CPYMEM(addinfo.SendCallCount,pch+MAKELONG(pi,SendCallCount,nline,QI_SHORT),szl);
		CPYMEM(addinfo.RecvCallTime,pch+MAKEDOUBLE(pi,RecvCallTime,nenter,QI_SHORT,QI_LONG,QI_VOID),szd);
		CPYMEM(addinfo.MinRecvCallTime,pch+MAKEDOUBLE(pi,MinRecvCallTime,nenter,QI_SHORT,QI_LONG,QI_VOID),szd);
		CPYMEM(addinfo.MaxRecvCallTime,pch+MAKEDOUBLE(pi,MaxRecvCallTime,nenter,QI_SHORT,QI_LONG,QI_VOID),szd);
		CPYMEM(addinfo.RecvCallCount,pch+MAKELONG(pi,RecvCallCount,nline,QI_SHORT),szl);

		// -- dvmh
		dvmh_stat_interval *dvmhStatInterval = this->readDvmhStat(pch + offsetDvmhStat);
		extraLengthForThreads = dvmhStatHeader->threadsAmount * sizeof(dvmh_stat_interval_thread);
		// --
		int allLengthForInterval = commonLengthForInterval + extraLengthForThreads;

		
	//	id.pname=(char *)(pch + lintone);
		id.pname=(char *)(pch + allLengthForInterval);
		// copy identifier information
		id.nline=0; id.nline_end=0; id.proc=0;id.nenter=0; id.expr=0;id.nlev=0;
		CPYMEM(id.nline,pch+MAKELONG(pi,nline,nline,QI_SHORT),szl);
		CPYMEM(id.nline_end,pch+MAKELONG(pi,nline_end,nline,QI_SHORT),szl);
		CPYMEM(id.proc,pch+MAKELONG(pi,qproc,nline,QI_SHORT),szl);
		CPYMEM(id.nlev,pch+MAKESHORT(pi,nlev,nlev),szsh);
		CPYMEM(id.expr,pch+MAKELONG(pi,expr,nline,QI_SHORT),szl);
		CPYMEM(id.nenter,pch+MAKEDOUBLE(pi,nenter,nenter,QI_SHORT,QI_LONG,QI_VOID),szd);
		short sh=0;
		CPYMEM(sh,pch+MAKESHORT(pi,type,nlev),szsh);
		id.t=(typefrag)sh;
		unsigned char *pptr=NULL;
		unsigned long l0=0;
		// copy referenses on up, down and next intervals
		CPYMEM(pptr,pch+MAKEVOID(pi,up,up,QI_SHORT,QI_LONG),szv);
		if (pptr==NULL) {
			memcpy(&pt[ll].up,&l0,sizeof(l0));
		} else {
			long l=(char*)pptr-pbuff;
			pptr=buffer+l;
			pt[ll].up=0;
			CPYMEM(pt[ll].up,pptr+MAKELONG(pi,ninter,nline,QI_SHORT),szl);
		}
		pptr=NULL;
		CPYMEM(pptr,pch+MAKEVOID(pi,down,up,QI_SHORT,QI_LONG),szv);
		if (pptr==NULL) {memcpy(&pt[ll].down,&l0,sizeof(l0));
		} else {
			long l=(char*)pptr-pbuff;
			pptr=buffer+l;
			pt[ll].down=0;
			CPYMEM(pt[ll].down,pptr+MAKELONG(pi,ninter,nline,QI_SHORT),szl);
		}
		pptr=NULL;
		CPYMEM(pptr,pch+MAKEVOID(pi,next,up,QI_SHORT,QI_LONG),szv);
		if (pptr==NULL) {memcpy(&pt[ll].next,&l0,sizeof(l0));
		} else {
			long l=(char *)pptr-pbuff;
			pptr=buffer+l;
			pt[ll].next=0;
			CPYMEM(pt[ll].next,pptr+MAKELONG(pi,ninter,nline,QI_SHORT),szl);
		}
		// time characteristics for each interval
		pt[ll].pint=new CInter(times,addinfo,id,ll+1,iIM,jIM,sore, dvmhStatInterval);
		if (pt[ll].pint==NULL) throw("Out of memory\n");

	//	pch = pch + lintone + 1 + strlen((char*)(pch + lintone));
		pch = pch + allLengthForInterval + 1 + strlen((char*)(pch + allLengthForInterval));
	}
	if (sign_buffer!=NULL) {delete []sign_buffer; sign_buffer=NULL;}
	return;
}

unsigned long CTreeInter::memAlign(unsigned long pointer, const unsigned short align, const unsigned long size) {
	unsigned long p = pointer;
	if (align - p % align < size)
		while (p % align) p++;
	//printf("          memAlign %u -> %u (%u) rest %u\n", pointer, p, size, align - p % align);
	return p;
}

dvmh_stat_interval *CTreeInter::readDvmhStat(unsigned char * const buffer) {
	dvmh_stat_interval            *dvmhStatInterval;
	dvmh_stat_interval_gpu        *dvmhStatGpu;
	dvmh_stat_interval_gpu_metric *dvmhStatMetric;


	unsigned long dvmhStatShift = 0;
	unsigned long t, t2; /*@todo del */
	int dvmh_i, dvmh_j;

	dvmhStatInterval = new dvmh_stat_interval();
	if (!dvmhStatInterval) throw("Out of memory\n");

	// Инициализируем аккумулируемые времена
	dvmhStatInterval->allGPUTimeProductive = 0.0f;
	dvmhStatInterval->allGPUTimeLost       = 0.0f;
	dvmhStatInterval->allThreadsUserTime   = 0.0f;
	dvmhStatInterval->allThreadsSystemTime = 0.0f;

	CPYMEM(dvmhStatInterval->mask, buffer + dvmhStatShift, szl);
	if (dvmhDebug) { printf("Mask: %lu (%u)\n", dvmhStatInterval->mask, dvmhStatShift); fflush(stdout); }
	dvmhStatShift += szl;

	dvmhStatInterval->threads = new dvmh_stat_interval_thread[dvmhStatHeader->threadsAmount];
        if(!(dvmhStatInterval->threads)) throw ("Out of memory\n");

	// Читаем информацию по каждому GPU
	for (dvmh_i = 0; dvmh_i < DVMH_STAT_MAX_GPU_CNT; ++dvmh_i) {
		dvmhStatGpu = & (dvmhStatInterval->gpu[dvmh_i]);
		if (dvmhDebug) { printf("   GPU: %lu (%u)\n", dvmh_i, dvmhStatShift); fflush(stdout); }
		t2 = dvmhStatShift;

		// Инициализируем аккумулируемые времена GPU
		dvmhStatGpu->timeProductive = 0.0f;
		dvmhStatGpu->timeLost       = 0.0f;

		if (dvmhDebug) { printf("        gpuTimeProductive: %.4f\n", dvmhStatGpu->timeProductive); fflush(stdout); }
		if (dvmhDebug) { printf("        gpuTimeLost      : %.4f\n", dvmhStatGpu->timeLost); fflush(stdout); }

		// Copy metrics
		for (dvmh_j = 0; dvmh_j < DVMH_STAT_METRIC_CNT; ++dvmh_j) {
			t = dvmhStatShift;
			dvmhStatMetric = & (dvmhStatGpu->metrics[dvmh_j]);
			if (dvmhDebug) { printf("      Metric: %d (%u)\n", dvmh_j, dvmhStatShift); fflush(stdout); }

			CPYMEM(dvmhStatMetric->isReduced, buffer+dvmhStatShift, szsh);
			if (dvmhDebug) { printf("        isReduced     : %d (%u)\n", dvmhStatMetric->isReduced,dvmhStatShift); fflush(stdout); }
			dvmhStatShift += szsh;

			CPYMEM(dvmhStatMetric->hasOwnMeasures, buffer+dvmhStatShift, szsh);
			if (dvmhDebug) { printf("        hasOwnMeasures: %d (%u)\n", dvmhStatMetric->hasOwnMeasures,dvmhStatShift); fflush(stdout); }
			dvmhStatShift += szsh;

			CPYMEM(dvmhStatMetric->countMeasures, buffer+dvmhStatShift, szl);
			if (dvmhDebug) { printf("        countMeasures : %d (%u)\n", dvmhStatMetric->countMeasures,dvmhStatShift); fflush(stdout); }
			dvmhStatShift += szl;

			CPYMEM(dvmhStatMetric->timeProductive, buffer+dvmhStatShift, szd);
			if (dvmhDebug) { printf("        timeProductive: %.4f (%u)\n", dvmhStatMetric->timeProductive,dvmhStatShift); fflush(stdout); }
			dvmhStatShift += szd;

			CPYMEM(dvmhStatMetric->timeLost, buffer+dvmhStatShift, szd);
			if (dvmhDebug) { printf("        timeLost      : %.4f (%u)\n", dvmhStatMetric->timeLost,dvmhStatShift); fflush(stdout); }
			dvmhStatShift += szd;

			CPYMEM(dvmhStatMetric->min, buffer+dvmhStatShift, szd);
			if (dvmhDebug) { printf("        min           : %.4f (%u)\n", dvmhStatMetric->min,dvmhStatShift); fflush(stdout); }
			dvmhStatShift += szd;

			CPYMEM(dvmhStatMetric->mean, buffer+dvmhStatShift, szd);
			if (dvmhDebug) { printf("        mean          : %.4f (%u)\n", dvmhStatMetric->mean,dvmhStatShift); fflush(stdout); }
			dvmhStatShift += szd;

			CPYMEM(dvmhStatMetric->max, buffer+dvmhStatShift, szd);
			if (dvmhDebug) { printf("        max           : %.4f (%u)\n", dvmhStatMetric->max,dvmhStatShift); fflush(stdout); }
			dvmhStatShift += szd;

			CPYMEM(dvmhStatMetric->sum, buffer+dvmhStatShift, szd);
			if (dvmhDebug) { printf("        sum           : %.4f (%u)\n", dvmhStatMetric->sum,dvmhStatShift); fflush(stdout); }
			dvmhStatShift += szd;

            #if DVMH_EXTENDED_STAT == 1
            CPYMEM(dvmhStatMetric->q1, buffer+dvmhStatShift, szd);
			if (dvmhDebug) { printf("        q1            : %.4f (%u)\n", dvmhStatMetric->q1,dvmhStatShift); fflush(stdout); }
            dvmhStatShift += szd;

            CPYMEM(dvmhStatMetric->median, buffer+dvmhStatShift, szd);
			if (dvmhDebug) { printf("        median        : %.4f (%u)\n", dvmhStatMetric->median,dvmhStatShift); fflush(stdout); }
            dvmhStatShift += szd;

			CPYMEM(dvmhStatMetric->q3, buffer+dvmhStatShift, szd);
			if (dvmhDebug) { printf("        q3            : %.4f (%u)\n", dvmhStatMetric->q3,dvmhStatShift); fflush(stdout); }
            dvmhStatShift += szd;
            #endif

			if (dvmhDebug) { printf("        size          : %u\n", dvmhStatShift-t); fflush(stdout); }

			dvmhStatGpu->timeProductive += dvmhStatMetric->timeProductive > 0 ? dvmhStatMetric->timeProductive : 0.0;
			dvmhStatGpu->timeLost       += dvmhStatMetric->timeLost       > 0 ? dvmhStatMetric->timeLost       : 0.0;
		}

		if (dvmhDebug) { printf("     timeProductive: %.4f\n", dvmhStatGpu->timeProductive); fflush(stdout); }
		if (dvmhDebug) { printf("     timeLost      : %.4f\n", dvmhStatGpu->timeLost); fflush(stdout); }
		if (dvmhDebug) { printf("     size          : %d\n", dvmhStatShift-t2); fflush(stdout); }

		dvmhStatInterval->allGPUTimeProductive += dvmhStatGpu->timeProductive > 0 ? dvmhStatGpu->timeProductive : 0.0;
		dvmhStatInterval->allGPUTimeLost       += dvmhStatGpu->timeLost       > 0 ? dvmhStatGpu->timeLost       : 0.0;
	}

	if (dvmhDebug) { printf("allGPUTimeProductive: %.4f\n", dvmhStatInterval->allGPUTimeProductive); fflush(stdout); }
	if (dvmhDebug) { printf("allGPUTimeLost      : %.4f\n", dvmhStatInterval->allGPUTimeLost); fflush(stdout); }

	dvmhStatInterval->threadsUsed = false;
	if (dvmhDebug) { printf("Threads:\n"); fflush(stdout); }
	for(unsigned long i = 0; i < dvmhStatHeader->threadsAmount; ++i)
	{
		dvmh_stat_interval_thread* dvmhStatThread = &(dvmhStatInterval->threads[i]);
		CPYMEM(dvmhStatThread->user_time, buffer+dvmhStatShift, szd);
		if (dvmhDebug) { printf("    [%d] User time   : %.8f\n", i, dvmhStatThread->user_time); fflush(stdout); }
		dvmhStatShift += szd;
		CPYMEM(dvmhStatThread->system_time, buffer+dvmhStatShift, szd);
		if (dvmhDebug) { printf("    [%d] System time : %.8f\n", i, dvmhStatThread->system_time); fflush(stdout); }
		dvmhStatShift += szd;

		dvmhStatInterval->allThreadsUserTime   += dvmhStatThread->user_time   > 0 ? dvmhStatThread->user_time   : 0.0;
		dvmhStatInterval->allThreadsSystemTime += dvmhStatThread->system_time > 0 ? dvmhStatThread->system_time : 0.0;
	}
	if (dvmhDebug) { printf("allThreadsUserTime  : %.4f\n", dvmhStatInterval->allThreadsUserTime); fflush(stdout); }
	if (dvmhDebug) { printf("allThreadsSystemTime: %.4f\n", dvmhStatInterval->allThreadsSystemTime); fflush(stdout); }

	if(dvmhStatInterval->allThreadsUserTime > 0 || dvmhStatInterval->allThreadsSystemTime > 0)
		dvmhStatInterval->threadsUsed = true;

	return dvmhStatInterval;
}

//----------------------------------------
//return result of constructor execution
BOOL CTreeInter::Valid()
{
	return(valid);
}
//-------------------------------------------
// error message
void CTreeInter::TextErr(char *p)
{
	strcpy(p,texterr);
}

//-------------------------------------------------
//set current interval at the first interval
void CTreeInter::BeginInter(void)
{
	for (unsigned long i=0;i<qinter;i++) {
		pt[i].sign=0;
	}
	curninter=1;
	return;
}
//--------------------------------------------------
//read identifier information of current interval
void CTreeInter::NextInter(ident **id)
{
	*id=NULL;
	for (unsigned long i=curninter;i<=qinter;i++) {
		if (pt[i-1].sign==0) {
			pt[i-1].sign=1;
			curninter=i;
			CInter *p=pt[i-1].pint;
			p->ReadIdent(id);
			return;
		}
	}
	return;

}
//------------------------------------------------
// return pointer to interval with the same identifier information
// set current interval
CInter *CTreeInter::FindInter(ident *id)
//id - identifier information
{
	unsigned long n;
	ident *idcur;
	pt[curninter-1].pint->ReadIdent(&idcur);
	if (id==idcur) return(pt[curninter-1].pint); //the same processor
	if (id->nlev==idcur->nlev) { // the same level
		n=pt[curninter-1].up;
		if (n>0) n=pt[n-1].down;
		else n=curninter;// first interval
		while(n>0) {
			if (pt[n-1].sign==0 && pt[n-1].pint->CompIdent(id)==1) {
				pt[n-1].sign=1;
				curninter=n;
				return(pt[n-1].pint);
			}
			n=pt[n-1].next;
		}
		return(NULL);
	}
	// need level > current level
	n=curninter;
	if (id->nlev>idcur->nlev) {
		// find need down level
		while (id->nlev>idcur->nlev) {
			n=pt[n-1].down;
			if (n==0) return(NULL);
			pt[n-1].pint->ReadIdent(&idcur);
		}
		// find need interval on finded level
		while(n>0) {
			if (pt[n-1].sign==0 && pt[n-1].pint->CompIdent(id)==1) {
				pt[n-1].sign=1;
				curninter=n;
				return(pt[n-1].pint);
			}
			n=pt[n-1].next;
		}
		return(NULL);
	} else {
		// find need up level
		while (id->nlev<idcur->nlev) {
			n=pt[n-1].up;
			if (n==0) return(NULL);
			pt[n-1].pint->ReadIdent(&idcur);
		}
		unsigned long n1=n;
		n=pt[n-1].up;
		if (n>0) n=pt[n-1].down;else n=n1;
		while(n>0) {
			if (pt[n-1].sign==0 && pt[n-1].pint->CompIdent(id)==1) {
				pt[n-1].sign=1;
				curninter=n;
				return(pt[n-1].pint);
			}
			n=pt[n-1].next;
		}
	}
	return(NULL);
}
//--------------------------------------------------
//sum time characteristics
void CTreeInter::SumLevel(void)
{
	for (short i=maxnlev;i>0;i--) {
		for (unsigned long j=0;j<qinter;j++) {
			ident *id;
			pt[j].pint->ReadIdent(&id);
			if (id->nlev==i) {
				// psum - up level
				unsigned long up=pt[j].up;
				CInter *psum=pt[up-1].pint;
				pt[j].pint->SumInter(psum);
			}
		}
	}
	pt[0].pint->SumInter(NULL);
}
//---------------------------------------------------
//processor time
void CTreeInter::ReadProcTime(double &time)
{
	time=proctime;
}
//---------------------------------------------------
//processor name
void CTreeInter::ReadProcName(char **name)
{
	*name=pprocname;
}
//--------------------------------------------------
// deallocate memory for tree interval
CTreeInter::~CTreeInter()
{
	if (pprocname!=NULL) delete []pprocname;
	if (sign_buffer!=NULL) delete []sign_buffer;
	if (pt==NULL) return;
	for (unsigned long i=0;i<qinter;i++) {
		if (pt[i].pint!=NULL) pt[i].pint->~CInter();
		pt[i].pint=NULL;
	}
	delete []pt;
}
