#define _STATFILE_
#include "inter.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#define CLEAR(A) memset(A, 0, sizeof A);

typedef s_GRPTIMES (*matrix) [StatGrpCount];

CInter::CInter(
	    matrix          	pt,
		s_SendRecvTimes 	ps,
		ident           	id,
		unsigned long   	nint,
		int             	iIM,
		int 				jIM,
		short 				sore,
	    dvmh_stat_interval *dvmhStatInterval
) { int i, j;

// ----------------------------------------------------------------------------------------
// -- Store DVMH statistics for the interval
// ----------------------------------------------------------------------------------------

	// store prepared DVMH statistics
	this->dvmhStatInterval = dvmhStatInterval;

// ----------------------------------------------------------------------------------------
// -- Store general information about the interval
// ----------------------------------------------------------------------------------------
	// set interval name (name of DVM programm)
	if (id.pname) {
		idint.pname = new char[strlen(id.pname) + 1];
        if (idint.pname == NULL) 
        	throw("Internal error: out of memory at %s, line %d \n", __FILE__, __LINE__);
        strcpy(idint.pname, id.pname);
	}
	else idint.pname = NULL;

	idint.nline     = id.nline;     // number of DVM-programm line
    idint.nline_end = id.nline_end; // number of end of DVM-programm line
	idint.nenter    = id.nenter;    // number of enters into the interval
	idint.expr      = id.expr;      // conditional expession
	idint.nlev      = id.nlev;      // number of interval level
	idint.t         = id.t;         // type of interval	
	idint.proc      = id.proc;      // number of processors§	
	ninter          = nint;			// inteval number

// ----------------------------------------------------------------------------------------
// -- Clean statistics storages
// ----------------------------------------------------------------------------------------

	CLEAR(mgen)
	CLEAR(mcom)
	CLEAR(mrcom)
	CLEAR(msyn)
	CLEAR(mvar)
	CLEAR(mcall)
	CLEAR(moverlap)
	CLEAR(lost)
	CLEAR(calls)
	CLEAR(prod)

// ----------------------------------------------------------------------------------------
// -- Aggregate statistics information
// ----------------------------------------------------------------------------------------

	// Execution characteristics on each processor
	for (i = 0; i < StatGrpCount; i++) {		
		mgen[SUMCOM]   += pt[i][MsgPasGrp].LostTime;
		mgen[SUMRCOM]  += pt[i][MsgPasGrp].ProductTime;
	
		// mgen[CPUUSR]   += pt[i][UserGrp].ProductTime;
		// mgen[INSUFUSR] += pt[i][UserGrp].LostTime;		
		// mgen[IOTIME]   += pt[i][IOGrp].ProductTime;	

		for (j = 0; j < StatGrpCount; j++) {

			if (i == UserGrp) mgen[CPUUSR]   += pt[UserGrp][j].ProductTime;
			if (i == UserGrp) mgen[INSUFUSR] += pt[UserGrp][j].LostTime;
			if (i == IOGrp)   mgen[IOTIME]   += pt[IOGrp][j].ProductTime;
			
			mgen[CPU]   += pt[i][j].ProductTime;	
			mgen[EXEC]  += pt[i][j].ProductTime + pt[i][j].LostTime;
			mgen[INSUF] += pt[i][j].LostTime;
		}
	}

	mgen[EXEC]  = mgen[EXEC]  - mgen[SUMRCOM];
	mgen[CPU]   = mgen[CPU]   - mgen[CPUUSR] - mgen[SUMRCOM] - mgen[IOTIME];
	mgen[INSUF] = mgen[INSUF] - mgen[INSUFUSR] - mgen[SUMCOM];
	
	if (mgen[CPU] < 0) mgen[CPU] = 0.0;

	//real synchronization,number of calls, communication
	// reduction
	mcom[RD]  = pt[WaitRedGrp][MsgPasGrp].LostTime + pt[StartRedGrp][MsgPasGrp].LostTime;
	mrcom[RD] = pt[WaitRedGrp][MsgPasGrp].ProductTime;
	mcall[RD] = pt[UserGrp][WaitRedGrp].CallCount;
	
	// shadow
	mcom[SH]  = pt[WaitShdGrp][MsgPasGrp].LostTime + pt[DoPLGrp][MsgPasGrp].LostTime + pt[StartShdGrp][MsgPasGrp].LostTime;
	mrcom[SH] = pt[WaitShdGrp][MsgPasGrp].ProductTime + pt[DoPLGrp][MsgPasGrp].ProductTime;
	mcall[SH] = pt[UserGrp][WaitShdGrp].CallCount;
	
	// remote access
	mcom[RA]  = pt[RemAccessGrp][MsgPasGrp].LostTime;
	mrcom[RA] = pt[RemAccessGrp][MsgPasGrp].ProductTime;
	mcall[RA] = pt[UserGrp][RemAccessGrp].CallCount;
	
	// redistribute
	mcom[RED]  = pt[ReDistrGrp][MsgPasGrp].LostTime;
	mrcom[RED] = pt[ReDistrGrp][MsgPasGrp].ProductTime;
	mcall[RED] = pt[UserGrp][ReDistrGrp].CallCount;
	
	// input/output
	mcom[IO]  = pt[IOGrp][MsgPasGrp].LostTime;
	mrcom[IO] = pt[IOGrp][MsgPasGrp].ProductTime;
	mcall[IO] = pt[UserGrp][IOGrp].CallCount;

	// add information
	SendCallTime    = ps.SendCallTime;
	MinSendCallTime = ps.MinSendCallTime;
	MaxSendCallTime = ps.MaxSendCallTime;
	SendCallCount   = ps.SendCallCount;
	RecvCallTime    = ps.RecvCallTime;
	MinRecvCallTime = ps.MinRecvCallTime;
	MaxRecvCallTime = ps.MaxRecvCallTime;
	RecvCallCount   = ps.RecvCallCount;
	mgen[START]     = SendCallTime + RecvCallTime;

	// -- FOR DEBUG !!! 
	if (iIM != 0) {
		for (i = 0; i < StatGrpCount; i++) {
			if (sore == 1) {//sum
				for (j = 0; j < StatGrpCount; j++) {
					//mgen[j] = mgen[j] + pt[i][k].ProductTime;
					lost[i]  = lost[i]  + pt[i][j].LostTime;
					prod[i]  = prod[i]  + pt[i][j].ProductTime;
					calls[i] = calls[i] + pt[i][j].CallCount;
				}
			} else {
				//mgen[j]=pt[iIM-1][i].ProductTime;
				lost[i]  = pt[iIM-1][i].LostTime;
				prod[i]  = pt[iIM-1][i].ProductTime;
				calls[i] = pt[iIM-1][i].CallCount;
			}
		}
	}
	if (jIM != 0) {
		for (i = 0; i < StatGrpCount; i++) {
			if (sore == 1) {
				for (j = 0; j < StatGrpCount; j++) {
					//mgen[j] = mgen[j] + pt[k][i].ProductTime;
					prod[i]  = prod[i]  + pt[j][i].ProductTime;
					lost[i]  = lost[i]  + pt[j][i].LostTime;
					calls[i] = calls[i] + pt[j][i].CallCount;
				}
			} else {
				//mgen[j] = pt[i][jIM-1].ProductTime;
				prod[i]  = pt[i][jIM-1].ProductTime;
				lost[i]  = pt[i][jIM-1].LostTime;
				calls[i] = pt[i][jIM-1].CallCount;
			}
		}
	}
}
//-------------------------------------------------
// deallocate memory for name of DVM-program
CInter::~CInter()
{
	if (idint.pname!=NULL) delete []idint.pname;
	delete[] this->dvmhStatInterval->threads;
	delete this->dvmhStatInterval;
}
//--------------------------------------------------
// addition execution time characteristics
void CInter::AddTime(typetime t2,double val)
//t2 - type of execution characteristics
// val - additional value
{
#ifdef _DEBUG
	if (t2<0 || t2>ITER) {
		printf("CInter AddTime incorrect typetime %d\n",t2);
		return;
	}
#endif
	if (t2 == DVMH_GPU_TIME_PRODUCTIVE)
		this->dvmhStatInterval->allGPUTimeProductive += val;
	else if (t2 == DVMH_GPU_TIME_LOST)
		this->dvmhStatInterval->allGPUTimeLost += val;
	else if (t2 == DVMH_THREADS_USER_TIME)
		this->dvmhStatInterval->allThreadsUserTime += val;
	else if (t2 == DVMH_THREADS_SYSTEM_TIME)
		this->dvmhStatInterval->allThreadsSystemTime += val;
	else
		mgen[t2] = mgen[t2] + val;

}
//--------------------------------------------------
//write new execution time characteristics
void CInter::WriteTime(typetime t2,double val)
//t2 - type of execution characteristics
// val - new value
{
#ifdef _DEBUG
	if (t2<0 || t2>ITER) {
		printf("CInter WriteTime incorrect typetime %d\n",t2);
		return;
	}
#endif
	if (t2 == DVMH_GPU_TIME_PRODUCTIVE)
		this->dvmhStatInterval->allGPUTimeProductive = val;
	else if (t2 == DVMH_GPU_TIME_LOST)
		this->dvmhStatInterval->allGPUTimeLost = val;
	else if (t2 == DVMH_THREADS_USER_TIME)
		this->dvmhStatInterval->allThreadsUserTime = val;
	else if (t2 == DVMH_THREADS_SYSTEM_TIME)
		this->dvmhStatInterval->allThreadsSystemTime = val;
	else
		mgen[t2] =  val;
}
//-------------------------------------------------
// read execution time characteristics
void CInter::ReadTime(typetime t2,double &val)
//t2 - type of execution characteristics
// val - answer
{
#ifdef _DEBUG
	if (t2<0 || t2>ITER) {
		printf("CInter ReadTime incorrect typetime %d\n",t2);
		return;
	}
#endif
	if (t2 == DVMH_GPU_TIME_PRODUCTIVE)
		val = this->dvmhStatInterval->allGPUTimeProductive;
	else if (t2 == DVMH_GPU_TIME_LOST)
		val = this->dvmhStatInterval->allGPUTimeLost;
	else if (t2 == DVMH_THREADS_USER_TIME)
		val = this->dvmhStatInterval->allThreadsUserTime;
	else if (t2 == DVMH_THREADS_SYSTEM_TIME)
		val = this->dvmhStatInterval->allThreadsSystemTime;
	else
		val = mgen[t2];
}
//--------------------------------------------------
// addition times of collective operations
void CInter::AddTime(typegrp t1,typecom t2,double val)
//t1 - type of communication operation
//t2 - type of collective operation
//val - additional value
{
#ifdef _DEBUG
	if (t2<0 || t2>RED) {
			printf("CInter AddTime incorrect typecom %d\n",t2);
		return;
	}
#endif
	switch (t1) {
		case COM:
			mcom[t2]=mcom[t2]+val;
			break;
		case RCOM:
			mrcom[t2]=mrcom[t2]+val;
			break;
		case SYN :
			msyn[t2]=msyn[t2]+val;
			break;
		case VAR:
			mvar[t2]=mvar[t2]+val;
			break;
		case CALL:
			mcall[t2]=mcall[t2]+val;
			break;
		case OVERLAP:
			moverlap[t2]=moverlap[t2]+val;
			break;
		default:
			printf("CInter WriteCom incorrect typegrp\n");
			break;
	}
}
//---------------------------------------------------
// read communication collective operations time
void CInter::ReadTime(typegrp t1,typecom t2,double &val)
//t1 - type of communication operation
//t2 - type of collective operation
//val - answer
{
#ifdef _DEBUG
	if (t2<0 || t2>RED) {
			printf("CInter ReadTime incorrect typecom %d\n",t2);
		return;
	}
#endif
	switch (t1) {
		case COM:
			val=mcom[t2];
			break;
		case RCOM:
			val=mrcom[t2];
			break;
		case SYN :
			val=msyn[t2];
			break;
		case VAR:
			val=mvar[t2];
			break;
		case CALL:
			val=mcall[t2];
			break;
		case OVERLAP:
			val=moverlap[t2];
			break;
		default:
			printf("CInter ReadTime incorrect typegrp\n");
			break;
	}
}
//---------------------------------------------------
// read time from interval matrix
void CInter::ReadTime(typetimeim t1,int t2,double &val)
//t1 - type of time (lost/number of calls)
//t2 - index
//val - answer

{
#ifdef _DEBUG
	if (t2<0 || t2>=StatGrpCount) {
		printf("CInter ReadTime incorrect 2 parameter %d\n",t2);
		return;
	}
#endif
	switch (t1) {
		case CALLSMT:
			val=calls[t2];
			break;
		case LOSTMT:
			val=lost[t2];
			break;
		case PRODMT:
			val=prod[t2];
			break;
		default:
			printf("CInter ReadTime incorrect type of im time\n");
			break;
	}
}
//-----------------------------------------------------
// compare identifier information on other processors
int CInter::CompIdent(ident *p)
//p - pointer identifire information
{
	if ((idint.pname==NULL || (strcmp(p->pname,idint.pname)==0)) && (p->nline==idint.nline) &&
	(p->nlev==idint.nlev) &&  (p->expr==idint.expr) && 	(p->t==idint.t)) {
		return(1);
	}
	return(0);
}
//------------------------------------------------------
// read identifier information
void CInter::ReadIdent(ident **p)
{
	*p=&idint;
}
//-----------------------------------------------------
// sum times characteristics upon levels
void CInter::SumInter(CInter *p)
{
	int i;
	for (i=0;i<=RED;i++) {
		mgen[SUMSYN]=mgen[SUMSYN]+msyn[i];
		mgen[SUMVAR]=mgen[SUMVAR]+mvar[i];
		mgen[SUMOVERLAP]=mgen[SUMOVERLAP]+moverlap[i];
	}
	mgen[PROC]=(double)idint.proc;
	if (idint.proc!=0) {
		mgen[LOST]=mgen[INSUF]+mgen[INSUFUSR]+mgen[IDLE]+mgen[SUMCOM];
	}
	if (p==NULL) return;
	for (i=0;i<=ITER;i++) {
		if (i<SUMSYN || i>SUMOVERLAP) p->mgen[i]=p->mgen[i]+mgen[i];
	}
	for (i=0;i<StatGrpCount;i++) {
		p->lost[i]=p->lost[i]+lost[i];
		p->prod[i]=p->prod[i]+prod[i];
		p->calls[i]=p->calls[i]+calls[i];
	}
	// add information
	p->SendCallTime=p->SendCallTime+SendCallTime;
	p->MinSendCallTime=p->MinSendCallTime+MinSendCallTime;
	p->MaxSendCallTime=p->MaxSendCallTime+MaxSendCallTime;
	p->SendCallCount=p->SendCallCount+SendCallCount;
	p->RecvCallTime=p->RecvCallTime+RecvCallTime;
	p->MinRecvCallTime=p->MinRecvCallTime+MinRecvCallTime;
	p->MaxRecvCallTime=p->MaxRecvCallTime+MaxRecvCallTime;
	p->RecvCallCount=p->RecvCallCount+RecvCallCount;

	// sum communication information
	for (i=0;i<=RED;i++) {
		p->mcom[i]=p->mcom[i]+mcom[i];
		p->mrcom[i]=p->mrcom[i]+mrcom[i];
		p->msyn[i]=p->msyn[i]+msyn[i];
		p->mvar[i]=p->mvar[i]+mvar[i];
		p->moverlap[i]=p->moverlap[i]+moverlap[i];
		p->mcall[i]=p->mcall[i]+mcall[i];
	}
}