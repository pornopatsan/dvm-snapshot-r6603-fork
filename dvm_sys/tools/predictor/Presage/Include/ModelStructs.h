#ifndef _MODELSTRUCTS_H
#define _MODELSTRUCTS_H

#include "Vm.h"
#include "AMView.h"
#include "DArray.h"
#include "BGroup.h"
#include "RedGroup.h"
#include "RedVar.h"
#include "ParLoop.h"
#include "RemAccessBuf.h"


/* MPS/AM/AMView structures */

struct _PSInfo  {
	static int	count;
	long		ID;
    VM*			VM_Obj;

	int size() { return count; } 
	int operator ++() { return ++count; }
	int operator --() { return --count; }
};

struct _AMInfo {
	static int count;
	long ID;
	long PS_ID;

	int size() { return count; }
	int operator ++() { return ++count; }
	int operator --() { return --count; }
};

struct _AMViewInfo {
	static int count;
	long ID;
    long AM_ID;
    AMView* AMView_Obj;

	int size() { return count; } 
	int operator ++() { return ++count; }
	int operator --() { return --count; }
};

struct _DArrayInfo  {
	static int count;
	long ID;					// ArrayHeader
    long AlignType;
    DArray* DArray_Obj;

	int size() { return count; } 
	int operator ++() { return ++count; }
	int operator --() { return --count; }
};

struct _DArrayFlag {
	static int count;
	long		ID;
	double* ProcessTimeStamp;
	double	time_start, 
					time_end;

	int size() { return count; } 
	int operator ++() { return ++count; }
	int operator --() { return --count; }
};

struct _RedVarInfo {
	static int count;
	long ID;
    RedVar* RedVar_Obj;

	int size() { return count; } 
	int operator ++() { return ++count; }
	int operator --() { return --count; }
};

struct _RedGrpInfo {
	static int count;
	long ID;
    RedGroup* RedGroup_Obj;

	int size() { return count; } 
	int operator ++() { return ++count; }
	int operator --() { return --count; }
};

struct _ShdGrpInfo {
	static int count;
	long ID;
    BoundGroup* BoundGroup_Obj;
	double * ProcessTimeStamp;

	int size() { return count; } 
	int operator ++() { return ++count; }
	int operator --() { return --count; }
};

struct _ReductInfo {
	long ID;
    double time_start, time_end;
	static int count;

	int size() { return count; } 
	int operator ++() { return ++count; }
	int operator --() { return --count; }
};

struct _ShadowInfo {
	static int count;
    long ID;
    double time_start, time_end;

	int size() { return count; } 
	int operator ++() { return ++count; }
	int operator --() { return --count; }
};

struct _ParLoopInfo {
	static int		count;
    long			ID;
    long			Rank;
    long			AlignType;
	bool			exfrst;
	long			exfrst_SGR;
	bool			imlast;
	long			imlast_SGR;
	bool			across;
	long			across_SGR;
	int				PatternType;
	long			PatternID;
	//====
	int	type_size;
	bool Invers[10];
	_ShdGrpInfo* SGnew;
	_ShdGrpInfo* SG;
	//=***

	std::vector<long>	AxisArray;
    ParLoop*		ParLoop_Obj;

	int				size() { return count; } 
	int				operator ++() { return ++count; }
	int				operator --() { return --count; }
};

struct _RemAccessInfo {
	static int		count;
    long			ID;
	RemAccessBuf*	RemAccess_Obj; // этот параметр не используется
	double* StartRemoteTimes;
	double* EndRemoteTimes;

	int				size() { return count; } 
	int				operator ++() { return ++count; }
	int				operator --() { return --count; }
};


#endif 
