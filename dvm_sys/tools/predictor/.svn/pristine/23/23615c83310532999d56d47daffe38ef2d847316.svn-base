#ifndef _CallInfo_H
#define _CallInfo_H

#include <vector>

#include "Event.h"

struct TraceCall {
    Event func_id;
    int source_line;
    char* source_file;
    int call_info_count;
    char** call_info; // pointer to lines with input function params
    int ret_info_count;
    char** ret_info;  // pointer to lines with output function params

	TraceCall(Event func_id, int source_line, char* source_file, int call_info_count,
		char** call_info, int ret_info_count, char** ret_info);
};


// Common CallInfo structures

struct IDOnlyInfo {
   long ID;
};

// Interval structures

struct binter_Info {
    long	line;
    char *	file;
	long	index;

	~binter_Info() { delete file; }
};

typedef struct IDOnlyInfo  einter_Info;

// Message sending structures

typedef struct IDOnlyInfo  rtl_BarrierInfo;

struct rtl_BcastInfo {
    long	Count; 
	long	Size;
};

// MPS/AM/AMView structures 

typedef struct IDOnlyInfo  CreateVMSInfo;
typedef struct IDOnlyInfo  getam_Info;

struct crtps_Info {
	long				PSRef;			// ID
	long				PSRefParent;
	std::vector<long>	InitIndexArray;
	std::vector<long>	LastIndexArray;	 
	long				StaticSign;
};

struct getps_Info {
	long				PSRef;
	long				AMRef;
};

struct psview_Info {
	long				PSRef;			// ID
	long				PSRefParent;
	long				Rank;
	std::vector<long>	SizeArray;
	long				StaticSign;
};

struct setelw_Info {
	long				PSRef;			// 16
	long				AMViewRef;		// 16
	long				AddrNumber;		// 10
	std::vector<long>	WeightNumber;
	// length = sun i = [0,AddrNumber-1] WeightNumber[i]  
	std::vector<double>	LoadWeight;
};

typedef struct IDOnlyInfo  delps_Info;

struct getamr_Info {
	long				AMRef;			// ID
	long				AMViewRef;
	std::vector<long>	IndexArray;		
};

struct getamv_Info {
	long				ArrayHeader;	// ArrayHeader
	long				AMViewRef;
};

struct mapam_Info {
	long				AMRef;	
	long				PSRef;
};

typedef struct IDOnlyInfo  runam_Info;

struct crtamv_Info {
    long				ID;				// AMViewRef
    long				AM_ID;			// AMRef
	long				StaticSign;		// StaticSign
	std::vector<long>	SizeArray;		// Rank + SizeArray
};

typedef struct IDOnlyInfo  delamv_Info;

//====
struct blkdiv_Info {
	long				ID;				// AMViewRef
	std::vector<long>	AMVAxisDiv;		// AMVAxisDiv[]
};
//=***

struct distr_Info {
	long				ID;				// AMViewRef
	long				PSRef;			// PSRef
	std::vector<long>	AxisArray;		// AxisArray[]
	std::vector<long>	DistrParamArray;// DistrParamArray[]
};

struct redis_Info {
	long				ID;				// AMViewRef
	long				AID;			// ArrayHeader
	long				PSRef;			// PSRef
	std::vector<long>	AxisArray;		// AxisArray[]
	std::vector<long>	DistrParamArray;// DistrParamArray[]
    long				NewSign;
};

// DArray structures

struct crtda_Info {
    long				ArrayHandlePtr;	// ArrayHandlePtr
	long				ArrayHeader;	// ArrayHeader
    long				TypeSize;
	long				StaticSign;		//
	long				ReDistrSign;	//
	std::vector<long>	SizeArray;
	std::vector<long>	LowShdWidthArray;
	std::vector<long>	HiShdWidthArray;
};

struct align_Info
{
	long				ArrayHeader;	// ArrayHeader
	long				ArrayHandlePtr;	// ArrayHandlePtr
	long				PatternRefPtr;	// PatternRefPtr
	long				PatternRef;		// PatternRef
	int					PatternType;	// AMView = 1, DisArray = 2
    std::vector<long>	AxisArray;
	std::vector<long>	CoeffArray;
	std::vector<long>	ConstArray;
};

typedef struct IDOnlyInfo  delda_Info;

struct realn_Info {
	long				ArrayHandlePtr;	// ArrayHandlePtr
	long				ArrayHeader;	// ArrayHeader
	long				PatternRefPtr;	// PatternRefPtr
	long				PatternRef;		// PatternRef
	int					PatternType;	// AMView = 1, DisArray = 2
    std::vector<long>	AxisArray;
    std::vector<long>	CoeffArray;
    std::vector<long>	ConstArray;
    long				NewSign;
};

struct arrcpy_Info {

	long				FromBufferPtr;
	long				FromArrayHeader;
	long				FromArrayHandlePtr;
    std::vector<long>	FromInitIndexArray;
	std::vector<long>	FromLastIndexArray;
	std::vector<long>	FromStepArray;

	long				ToBufferPtr;
	long				ToArrayHeader;
	long				ToArrayHandlePtr;
    std::vector<long>	ToInitIndexArray;
	std::vector<long>	ToLastIndexArray;
	std::vector<long>	ToStepArray;

    long				CopyRegim;
	long				CopyFlagPtr;
};

struct waitcp_Info {
	long CopyFlagPtr;
};

// ParLoop structures

struct crtpl_Info {
    long	ID;
    long	Rank;
};

struct mappl_Info {
    long				LoopRef;		//ID; 
	long				PatternRefPtr;
	long				PatternRef;
	int					PatternType;	// AMView = 1, DisArray = 2
    std::vector<long>	AxisArray;
    std::vector<long>	CoeffArray;
    std::vector<long>	ConstArray;
    std::vector<long>	InInitIndexArray;
    std::vector<long>	InLastIndexArray;
    std::vector<long>	InStepArray;
};

typedef struct IDOnlyInfo  dopl_Info;
typedef struct IDOnlyInfo  endpl_Info;

//grig
struct  dopl_full_Info
{
  long ID;
  std::vector<long>   Dim;
  std::vector<long>   Step;
  std::vector<long>   Lower;
  std::vector<long>   Upper;
	long ReturnVar; //====//
};
//\grig

// Reduction structures

typedef struct IDOnlyInfo  crtrg_Info;

struct crtred_Info {
	long	ID;
    long	RedArrayType; 
	long	RedArrayLength; 
	long	LocElmLength;
};

struct insred_Info {
	long	RG_ID; 
	long	RV_ID;
};

typedef struct IDOnlyInfo  delrg_Info;
typedef struct IDOnlyInfo  delred_Info;
typedef struct IDOnlyInfo  strtrd_Info;
typedef struct IDOnlyInfo  waitrd_Info;

// Shadow structures

struct crtshg_Info {
	long StaticSign;
	long ShadowGroupRef;
};

struct inssh_Info {
	Event				func;				// function ID
    long				ShadowGroupRef;		// SHG_ID;
	long				ArrayHeader;		//
	long				ArrayHandlePtr;		// DA_ID;
    long				FullShdSign;		// only for inssh_, incsh_
	long				MaxShdCount;		// only for insshd_, incshd
	std::vector<long>	ShdSignArray;		// for insshd_, incshd_

    std::vector<long>	LowShdWidthArray;
	std::vector<long>	HiShdWidthArray;

    std::vector<long>	InitDimIndex;
	std::vector<long>	LastDimIndex;

    std::vector<long>	InitLowShdIndex;
	std::vector<long>	LastLowShdIndex;

    std::vector<long>	InitHiShdIndex;
	std::vector<long>	LastHiShdIndex;
};

struct exfrst_Info{
	long			ID;					// LoopRef
	long			ShadowGroupRef;
};

struct imlast_Info{
	long			ID;					// LoopRef
	long			ShadowGroupRef;
};

struct across_Info {
	long	AcrossType; 
	long	OldShadowGroupRef; 
	long	NewShadowGroupRef; 
	double	PipeLinePar;

	long	CondPipeLine;
	long	ErrPipeLine;
	long	PipeLinePLAxis;
};

/*
AcrossType=1; 
OldShadowGroupRef=9b7ac0;
NewShadowGroupRef=9b77c0;
PipeLinePar=0.000000;
CondPipeLine=0 
ErrPipeLine=60
*/

typedef struct IDOnlyInfo  delshg_Info;
typedef struct IDOnlyInfo  strtsh_Info;
typedef struct IDOnlyInfo  waitsh_Info;
typedef struct IDOnlyInfo  sendsh_Info;
typedef struct IDOnlyInfo  recvsh_Info;

// Regular access to remote data

struct crtrbl_Info {
	long	RemArrayHeader;
	long	BufferHeader;
	long	StaticSign;
	long	LoopRef;
	std::vector<long>	AxisArray;  
	std::vector<long>	CoeffArray;  
	std::vector<long>	ConstArray;  
};

struct crtrbp_Info {
	long	ID;			//	BufferHeader;
	long	RemArrayHeader;
	long	StaticSign;
	long	PSRef;
	long	IsLocal;
	std::vector<long>	CoordArray;  
};

struct loadrb_Info {
	long	ID;			//	BufferHeader;
	long	RenewSign;
};

typedef struct IDOnlyInfo  waitrb_Info;

struct srmem_Info {
	long				MemoryCount;
	std::vector<long>	LengthArray;  
};

// Root info
struct root_Info {
     long VProcCount;
     long VPSRank;
     std::vector<long>	VPSSize;
};

extern bool GetCallParams(TraceCall &trc_call, void*& call_params);

#endif 
