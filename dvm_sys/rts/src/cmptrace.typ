
#ifndef _CMPTRACE_TYP_
#define _CMPTRACE_TYP_
/********************/

#ifdef _UNIX_
    #include <signal.h>
#else
    #include <windows.h>
#endif

/*************************************************************************
* Trace record types:
* Before variable assignment:
*      BW: [<type>] <oper>; { <file>, <line> }
*  After variable assignment:
*      AW: [<type>] <oper> = <value>; { <file>, <line> }
* Variable reading:
*      RD: [<type>] <oper> = <value>; { <file>, <line> }
* Reduction result :
*      RV: [<type>] <value>; { <file>, <line> }
* Beginning of sequential loop :
*      SL: <no>(<parent>) [<rank>] = <level>, (iter), ...; { <file>, <line> }
* Beginning of parallel loop :
*      PL: <no>(<parent>) [<rank>]  = <level>, (iter), ...; { <file>, <line> }, <point_no>
* Task region
*      TR: <no>(<parent>) [<rank>]  = <level>, (iter), ...; { <file>, <line> }, <point_no>
* End of loop or task region:
*      EL: <no>; { <file>, <line> }, <point_no>
*          CS(<name>, <file>, <line>, <number>, <access>)="<value>"
*             ...
*          CS(<name>, <file>, <line>, <number>, <access>)="<value>"
*      or
*          CS(<name>, <file>, <line>, <number>, <access>) FAILED
* Beginning of iteration:
*      IT: <abs_no>, (iter)
* Skip operators:
*      SKP: { <file>, <line> }
* Chunk of iterations:
*      CHUNK:
************************************************************************/

typedef enum tag_TraceMode
{
    mode_CONFIG = 0,
    mode_WRITETRACE = 1,
    mode_CONFIG_WRITETRACE = 2,
    mode_COMPARETRACE = 3
}
enum_TraceMode;

typedef enum tag_TraceLevel
{
    level_DEFAULT = -1,
    level_NONE = 0,
    level_MINIMAL,
    level_MODIFY,
    level_FULL,
    level_CHECKSUM
}
enum_TraceLevel;

typedef enum tag_TraceType
{
    trc_ITER = 0,
    trc_STRUCTBEG,
    trc_STRUCTEND,
    trc_PREWRITEVAR,
    trc_POSTWRITEVAR,
    trc_READVAR,
    trc_REDUCTVAR,
    trc_SKIP,
    trc_CHUNK,
    trc_ERROR = -1
}
enum_TraceType;

typedef enum tag_MsgRecType
{
    msg_SEND = 0,
    msg_RECV,
    msg_ERR,
    msg_FIN
}
enum_MsgRecType;

typedef union tag_VALUE
{
    int    _int;
    long   _long;
	long long  _longlong;
    float  _float;
    double _double;
    float  _complex_float[2];
    double _complex_double[2];
}
VALUE;

typedef struct tag_CHECKSUM
{
    void    *pInfo;           /* pointer to the array debugger
                                 information */
    double   sum;             /* checksum of the array */
    DvmType     lArrNo;          /* array number in the tArrays table */
    byte     accType;         /* access type: 1-read, 2-write,
                                 3-read and write */
    byte     errCode;         /* 0 - impossible to calculate checksum
                                     for this array (no other info)
                                 1 - checksum is calculated
                                 2 - checksum is not calculated because of
                                     different local checksums of replicated arrays */
    void     *pAddr;          /* array address information for trapping purposes */
}
CHECKSUM;

typedef struct tag_SENDCHECKSUM
{
   char      arrInfo [MaxOperand + MaxSourceFileName + 12]; /* zero-terminating string array ID */
   byte      accType;         /* access type: 1-read, 2-write,
                                 3-read and write */
   byte      errCode;         /* 0 - impossible to calculate checksum
                                     for this array (no other info)
                                 1 - checksum is calculated
                                 2 - checksum is not calculated because of
                                     different local checksums of replicated arrays */
   double    sum;             /* checksum of the array */
}
SENDCHECKSUM;

typedef struct tag_ERR_INFO
{
   double firstHitRel, firstHitAbs;
   double maxRelAcc, maxRelAccAbs;
   double maxAbsAcc, maxAbsAccRel;
   double maxLeapRel, maxLeapRelAbs;
   double maxLeapAbs, maxLeapAbsRel;

   VALUE firstHitValT, firstHitValE;
   VALUE maxRelAccValT, maxRelAccValE;
   VALUE maxAbsAccValT, maxAbsAccValE;
   VALUE maxLeapRelValT, maxLeapRelValE;
   VALUE maxLeapAbsValT, maxLeapAbsValE;

   UDvmType Line;
   UDvmType HitCount, firstHitLoc;
   int           firstHitLocC;
   UDvmType MaxHitCount; /* used in final protocol only */
   UDvmType maxRelAccLoc,  maxAbsAccLoc;
   int           maxRelAccLocC, maxAbsAccLocC;
   UDvmType LeapsRCount, maxLeapRLoc;
   UDvmType LeapsACount, maxLeapALoc;
   int           maxLeapRLocC, maxLeapALocC;
   DvmType   vtype;
   char   Operand [MaxOperand+1];
   char   File    [MaxSourceFileName + 1];
   byte   *CPULists;          /* used in final protocol only */
}
ERR_INFO;

struct dbg_level
{
   DvmType    val; /* number */
   char    ref; /* 'B' - loop begin, 'E' - loop end */
};

typedef struct tag_NUMBER
{
    /* main level */
    struct dbg_level main;

    /* nested level */
    struct dbg_level nested;

    /* selector: which level to choose, 0 - main, 1 - nested */
    byte selector;

    /* iteration if we are in task region */
    DvmType iter;
}
NUMBER;

typedef struct tag_LIST
{
    byte        collect;
    CHECKSUM    *body;
    int         cursize;
    int         maxsize;
}
LIST;

typedef struct tag_LOOP_INFO
{
    DvmType        No;
/*    byte        LoopType;*/
    byte        Init;        /* flag */
    byte        HasItems;    /* flag */
    byte        BordersSetBack; /* flag */
    byte        Propagate; /* flag */

    s_BLOCK     DoplRes;

    s_BLOCK     *LocParts;
    int         LocHandle;
    int         OldVtr;
    int         BlCount;
    int         Line;
}
LOOP_INFO;

typedef struct tag_STRUCT_INFO
{
    DvmType No;
    char File[MaxSourceFileName + 1];
    UDvmType Line;
    enum_TraceLevel  TraceLevel;
    enum_TraceLevel  RealLevel;
    byte  bSkipExecution;
    TABLE tChildren;

    byte  Type;
    byte  Rank;
    s_REGULARSET  Limit[MAXARRAYDIM];    /* Iteration limits defined
                                            in the file of loops*/
    s_REGULARSET  Current[MAXARRAYDIM];  /* Current iteration range
                                            defined by program */
    s_REGULARSET  Common[MAXARRAYDIM];   /* Global iteration range for all
                                            loop executions.
                                            Write in comment */
    s_REGULARSET  CurLocal[MAXARRAYDIM]; /* Current local iterations
                                            for the construction */

    DvmType          CurIter[MAXARRAYDIM];  /* Current iteration */
    DvmType          TracedIterNum;         /* Number of traced iterations for seq. loop */

    struct tag_STRUCT_INFO* pParent;

    UDvmType Bytes;
    UDvmType StrCount;
    UDvmType Iters;

    LIST          ArrList;                /* arrays list for checksum management purposes */
}
STRUCT_INFO;

typedef struct tag_ARRAY_INFO
{
    char          szFile[MaxSourceFileName + 1];
    char          szOperand[MaxOperand + 1];
    UDvmType          ulLine;
    void         *pAddr;                 /* pointer to array Handle
                                            (if distributed array) or
                                            to array (if replicated
                                            array) */

    byte          bIsDistr;              /* sign of distributed array */
    size_t        nElemSize;             /* size of array element */
    DvmType          lElemType;             /* type of array element */

    enum_TraceLevel eTraceLevel;

    byte          bRank;                 /* rank of array */
    DvmType          rgSize[MAXARRAYDIM];   /* size of every dimension */
    DvmType          lLineSize;             /* size of array */
    s_REGULARSET  rgLimit[MAXARRAYDIM];  /* iteration limits, which are
                                            defined in the configuration
                                            file */
    int           iNumber;               /* number of the array:
                                            used when there are arrays
                                            with the same name in current
                                            context */
    NUMBER        *correctCSPoint;

    UDvmType          ulRead;
    UDvmType          ulWrite;

    /* Remote buffer information */

    byte          bIsRemoteBuffer;
    DvmType          bSourceNo;             /* number of source array
                                            in the tArrays table */
    DvmType          rgRBIndexMap[MAXARRAYDIM];
    TABLE         tErrEntries;
}
dvm_ARRAY_INFO;

typedef struct tag_ANY_RECORD
{
    byte RecordType;
    int  CPU_Num;
    UDvmType Line_num;
}
ANY_RECORD;

typedef struct tag_SKIP
{
    byte RecordType;
    int  CPU_Num;
    UDvmType Line_num;

    char File[MaxSourceFileName + 1];
    UDvmType Line;
}
SKIP;

typedef struct tag_ITERS
{
    DvmType Lower;   /* low value of index */
    DvmType Upper;   /* high value of index */
    DvmType Step;    /* step */
} ITERS;

typedef struct tag_ITERBLOCK
{
   byte   Rank;
   byte   vtr;
   ITERS  Set[MAXARRAYDIM];
}
ITERBLOCK;

typedef struct tag_SORTPAIR
{
   DvmType        LI;
   ITERBLOCK   *iblock;
}
SORTPAIR;

typedef struct tag_CHUNK
{
    byte         RecordType;
    int  CPU_Num;
    UDvmType Line_num;

    ITERBLOCK    block;
}
CHUNK;

typedef struct tag_CHUNKSET
{
    int         Size;
    ITERBLOCK*  Chunks;
}
CHUNKSET;

#ifdef _MPI_PROF_TRAN_

typedef struct tag_MSG_INFO
{
   byte rec_type;
   int  cpu_num;
   int  msg_tag;
   DvmType msg_comm;
   t_tracer_time msg_time;
}
MSG_INFO;

typedef struct tag_MSG_SEND
{
   byte dummy;
   int  receiver_cpu_num;
   int  msg_tag;
   DvmType msg_comm;
   t_tracer_time snd_time;
   t_tracer_time rcv_time;
   /*int  from_cpu_num;*/
}
MSG_SEND;

typedef struct tag_MSG_RECV
{
   byte dummy;
   int  sender_cpu_num;
   int  msg_tag;
   DvmType msg_comm;
   t_tracer_time rcv_time;
   int  receiver_cpu_num;
}
MSG_RECV;

typedef struct tag_ERR_TIME
{
   t_tracer_time err_time;
   int           cpu_num;
}
ERR_TIME;

#endif

typedef struct tag_STRUCT_BEGIN
{
    byte RecordType;
    int  CPU_Num;
    UDvmType Line_num;

    char File[MaxSourceFileName + 1];
    UDvmType Line;

    DvmType   Parent;
    DvmType   LastRec;

    struct tag_STRUCT_INFO *pCnfgInfo;

/* iteration control mechanism for mbodies parallel comparison */
    CHUNKSET               *pChunkSet;
    int                     iCurCPU;
    int                     iCurCPUMaxSize;

    NUMBER *num; /* dynamic point number */
}
STRUCT_BEGIN;

typedef struct tag_STRUCT_END
{
    byte RecordType;
    int  CPU_Num;
    UDvmType Line_num;

    char File[MaxSourceFileName + 1];
    UDvmType Line;

    DvmType   Parent;
    int    csSize;

    CHECKSUM    *checksums;
    NUMBER      *num; /* dynamic point number */

    struct tag_STRUCT_INFO* pCnfgInfo;
}
STRUCT_END;

typedef struct tag_ITERATION
{
    byte RecordType;
    int  CPU_Num;
    UDvmType Line_num;

    DvmType  Index[MAXARRAYDIM];
    DvmType  LI;
    char  Checked;
    DvmType  Parent;
    byte  Rank;
}
ITERATION;

typedef struct tag_VARIABLE
{
    byte RecordType;
    int  CPU_Num;
    UDvmType Line_num;

    DvmType vType;
    char Operand[MaxOperand + 1];
    char File[MaxSourceFileName + 1];
    UDvmType Line;
    byte Reduct;

    VALUE val;
}
VARIABLE;

typedef struct stack {
    LOOP_INFO *items;
    DvmType size;
    DvmType top;
} stack;

typedef struct tag_COVERAGE_INFO
{
    char  FileNames[MaxSourceFileCount][MaxSourceFileName + 1];
    byte *LinesAccessInfo[MaxSourceFileCount];
    int   AccessInfoSizes[MaxSourceFileCount];
    int   LastUsedFile;
/*    byte  FilesCount;*/
}
COVERAGE_INFO;

typedef struct tag_TRACE
{
    TABLE        tTrace;
    TABLE        tStructs;
    HASH_TABLE   hIters;

    TABLE        tArrays;
    HASH_TABLE   hArrayPointers;
    DvmType         lNextArray;
    DvmType         CurIter;
    DvmType         CurStruct;
    DvmType         CurTraceRecord;
    DvmType         CurPreWriteRecord;
    byte         IterFlash;
    byte         ReductExprType;

    STRUCT_INFO* pCurCnfgInfo;

    int ErrCode;
    int Level;

    UDvmType Bytes;
    UDvmType StrCount;
    UDvmType Iters;

    /* array lists for checksum management purposes */
    LIST         CurArrList;
    LIST         AuxArrList;

    /* current value of dynamic point number */
    NUMBER      CurPoint;

    /* trace constrains */
    NUMBER      *StartPoint, *FinishPoint;

    /* variables for iteration constraints management */
    byte         IterControl;
    STRUCT_INFO* IterCtrlInfo;

    /* The VTR variable, responsible for storing global VTR address
       (multiple bodies instrumentation) */
    int         *vtr;
    stack       sLoopsInfo;
    DvmType        ctrlIndex;
    int         convMode;
    s_PARLOOP   *tmpPL;

    FILE* TrcFileHandle;

    FILE* DoplMBFileHandle;

    int	TraceCPUCount;	/* number of CPU of the trace being read */
    int CurCPUNum;      /* = dvm_OneProcSign?dvm_OneProcNum:MPS_CurrentProc */
    int RealCPUCount;       /* = dvm_OneProcSign?dvm_OneProcCount:MPS_ProcCount;*/
    char TraceTime[25]; /* trace creation date */
    UDvmType TraceRecordBase;
    unsigned FloatPrecision;
    unsigned DoublePrecision;

    /* information on whether the array was saved to the file */
    byte         ArrWasSaved;

    /* flag: whether we are in parallel loop (1) or not (0) */
    byte         inParLoop;

    /* dbgtron_ and dbgtroff_ data */
    byte EnableDbgTracingCtrl; /* flag: 0-EnableTrace was initially off,
    no dynamic tracing control directives are listened to, 1 - otherwise. */

    /* dbglparton_ and dbglpartoff_ data */
    byte EnableLoopsPartitioning; /* flag: 1-Enabled, 0-disabled */

    /* variable to pass data to trc_put_variable. Has to be changed */
    byte isDistr;

    /* flag to store information about storing in message trace error information */
    byte ErrorIsFixed;

    /* array with dvm_OneProcCount elements to store CPUs which have already sent messages after err_time */
    byte*  pMSGSentAlready;

    /* CodeCoverage variables */
    byte CodeCovWritten;
    COVERAGE_INFO* CovInfo;

    /* trace merge state variables */
    signed int Mode; /* 0 - compare mode, 1 - add mode, -1 - disabled */
    DvmType LI;
    TABLE        tSkipped;
    TABLE        tVarErrEntries;
    HASH_TABLE   hSkippedPointers;

#ifdef _MPI_PROF_TRAN_
    TABLE        tMessages;   /* table to store message-passing history of current CPU */
    t_tracer_time *ErrTimes;  /* array to store error times of each CPU */
#endif

    /* Number of successfully compared events */
    UDvmType MatchedEvents;
    byte          TerminationStatus;

    UDvmType TraceMarker; /* Special variable to be used while saving the trace */
    UDvmType StrCntr;
    int           CPU_Num;

#ifdef _UNIX_
    struct sigaction old_action;
#else
    LPTOP_LEVEL_EXCEPTION_FILTER previousFilter;
#endif
}
TRACE;

typedef struct tag_DELAY_TRACE
{
    char File[MaxSourceFileName + 1];
    UDvmType Line;
    DvmType Type;
    void *Value;
}
DELAY_TRACE;

typedef struct tag_REDUCT_INFO
{
    char*     Current;
    char*     Initial;
    byte      StartReduct; /* 1 - after asynchronous reduction start */
    s_REDVAR* RVar;
}
REDUCT_INFO;

typedef struct tag_REDUCTION_GROUP
{
    s_COLLECTION RV;
    byte         bInit;
}
REDUCTION_GROUP;

enum
{
    N_FULL = 0,
    N_MODIFY,
    N_MINIMAL,
    N_NONE,
    N_MODE,
    N_EMPTYITER,
    N_SLOOP,
    N_PLOOP,
    N_TASKREGION,
    N_ITERATION,
    N_PRE_WRITE,
    N_POST_WRITE,
    N_R_PRE_WRITE,
    N_R_POST_WRITE,
    N_R_READ,
    N_REDUCT,
    N_READ,
    N_SKIP,
    N_END_LOOP,
    N_END_HEADER,
    N_ARRAY,
    N_MULTIDIM_ARRAY,
    N_DEF_ARR_STEP,
    N_DEF_ITER_STEP,
    N_CHECKSUM,
    N_STARTPOINT,
    N_FINISHPOINT,
    N_IG_LEFT,
    N_IG_RIGHT,
    N_ILOC_LEFT,
    N_ILOC_RIGHT,
    N_IREP_LEFT,
    N_IREP_RIGHT,
    N_LOCITERWIDTH,
    N_REPITERWIDTH,
    N_CPUCOUNT,
    N_TIME,
    N_CHUNK,
    N_CALC_CHECKSUM,
    N_FILE,
    N_DEB_VERSION,
    N_TASK_NAME,
    N_WORK_DIR,
    N_USER_HOST,
    N_ARCH,
    N_OS,
    N_END_TRACE,
    N_EMPTY
};

enum
{
    DSC_FULLSIZE = 0,
    DSC_STRINGCOUNT,
    DSC_ITERCOUNT,
    DSC_BEGIN_HEADER,
    DSC_END_HEADER,
    DSC_MEMORY,
    DSC_READ,
    DSC_WRITE
};

typedef struct tag_TRACE_VTABLE
{
    void (*BeginStruct)(char* pFile, UDvmType ulLine, DvmType lNo, byte bType,
        byte bRank, DvmType* pInit, DvmType* pLast, DvmType* pStep);
    void (*EndStruct)(char* pFile, UDvmType ulLine, DvmType lNo, UDvmType ulBegLine);
    void (*Iter)(DvmType* pIndex);
    void (*Variable)(char* pFile, UDvmType ulLine, char* pOperand, enum_TraceType iType,
        DvmType lType, void* pValue, byte bReduct, void *pArrBase);
    void (*SkipBlock)(char* pFile, UDvmType ulLine);
}
TRACE_VTABLE;

typedef int (*PFN_ARRAY_CAN_TRACE)(void* pAddr, void* pArrBase, enum_TraceType iType);
typedef int (*PFN_COMPARE_VALUE)(VALUE* pValue, void* pMem, DvmType lType, int difType);
typedef int (*PFN_DIVIDE_FUNC)(s_PARLOOP *PL, LOOP_INFO* loop);

typedef enum
{
    TRACE_DONE,
    TRACE_DYN_MEMORY_LIMIT
}
TRACE_WRITE_REASON;

#endif /* _CMPTRACE_TYP_ */
