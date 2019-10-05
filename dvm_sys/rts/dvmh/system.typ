#ifndef _SYSTEM_TYP_
#define _SYSTEM_TYP_
/******************/    /*E0000*/

typedef unsigned char		 byte;
typedef unsigned int		 word;
typedef UDvmType	     	 ulng;
typedef ulng				 uLLng;

typedef void  t_Destructor(void *); /* functions of object deleting */    /*E0001*/

#define rec struct {
#define endrec } 

/*************************\
*  Collection of objects  *
\*************************/    /*E0002*/

typedef rec

int            Count;    /* current number of objects
                            ( pointer to free space) */    /*E0003*/
int            Reserv;   /* current number of objects,
                            that can be allocated in collection */    /*E0004*/
int            CountInc; /* increase Reserv if it is exhausted 
                            memory is reallocated */    /*E0005*/
void         **List;     /* pointer to given collection
                            object address array */    /*E0006*/ 
t_Destructor  *RecDestr; /* pointer to the collection
                            object deleting function */    /*E0007*/      

endrec s_COLLECTION;


/* ------------------------------------------------ */    /*E0008*/


typedef rec

DvmType   Size;  /* size of dynamically allocated memory */    /*E0009*/
void  *Ptr0;  /* address of dynamically allocated memory */    /*E0010*/
void  *Ptr;   /* corrected address of allocated memory */    /*E0011*/

endrec s_DVMMEM;



typedef rec

DvmType Lower;   /* low value of index */    /*E0012*/
DvmType Upper;   /* high value of index */    /*E0013*/
DvmType Size;    /* size: Upper-Lower+1 */    /*E0014*/
DvmType Step;    /* step */    /*E0015*/

endrec s_REGULARSET;   /* Area: Lower:Upper:Step */    /*E0016*/



typedef rec

byte          Rank;
s_REGULARSET  Set[MAXARRAYDIM];

endrec s_BLOCK;



typedef rec

byte  Rank;              /* area rank */    /*E0017*/
DvmType  Size[MAXARRAYDIM]; /* array of sizes for each dimension */    /*E0018*/
DvmType  Mult[MAXARRAYDIM]; /* array of dimension weights */    /*E0019*/

endrec s_SPACE;



typedef rec

s_BLOCK        Block;
word           TLen;
s_DVMMEM       ALoc;
DvmType          *sI;

endrec s_ARRBLOCK;



/*******************\
*  Virtual machine  *
\*******************/    /*E0020*/

typedef rec

int          EnvInd;     /* index of context 
                            in which the processor system has been created;
                            or 1- for static processor system; */    /*E0021*/
int          CrtEnvInd;  /* index of context in which
                            the processor system has been created*/    /*E0022*/
SysHandle   *HandlePtr;  /* pointer to own Handle */    /*E0023*/
int          TreeIndex;  /* distance from processor system
                            tree root */    /*E0024*/
SysHandle   *PHandlePtr; /* pointer to parent processor system Handle
                            or NULL */    /*E0025*/
byte         Static;     /* flag of static virtual machine */    /*E0026*/
byte         HasCurrent; /* flag; current processor belongs to
                            the given processor system */    /*E0027*/
int          CurrentProc;/* current processor internal number in
                            the guven ptocessor system
                            if HasCurrent=1 */    /*E0028*/
DvmType        *CVP;        /* current processor coordinates in the given
                            processor system internal space (spind)
                            if HasCurrent=1 */    /*E0029*/
s_SPACE      Space;      /* given processor system 
                            internal space */    /*E0030*/
s_SPACE      TrueSpace;  /* internal space of the processor system
                            defined by user */    /*E0031*/
DvmType         InitIndex[MAXARRAYDIM]; /* */    /*E0032*/
SysHandle   *VProc;      /* array of SysHandle of processor elements
                            for the given processor system 
                            internal space */    /*E0033*/
int          MasterProc; /* main processor internal number */    /*E0034*/ 
int          IOProc;     /* I/O processor internal number */    /*E0035*/
int          CentralProc;/* central processor internal number */    /*E0036*/
int          VMSCentralProc; /* central processor number in
                                the given processor system */    /*E0037*/ 
DvmType         ProcCount;  /* number of processors in
                            the processor system */    /*E0038*/
s_COLLECTION SubSystem;     /* processor subsystems of the given system */    /*E0039*/
s_COLLECTION RedSubSystem;  /* */    /*E0040*/
s_COLLECTION AMVColl;    /* representation of abstract machines
                            mapped on the processor system */    /*E0041*/
s_COLLECTION AMSColl;    /* list of abstract machines
                            mapped in the processor system */    /*E0042*/ 
double       *CoordWeight[MAXARRAYDIM];/* array of processor coordinate weights  
                                          for each dimension */    /*E0043*/
double       *PrevSumCoordWeight[MAXARRAYDIM];/* array of summing previous
                                                 weights of processor
                                                 coordinates for each
                                                 dimension */    /*E0044*/

/* Current message tags */    /*E0045*/

int tag_common;
int tag_gettar_;
int tag_BroadCast;
int tag_BoundsBuffer;
int tag_DACopy;
int tag_RedVar;
int tag_IdAccess;
int tag_across;
int tag_ProcPowerMeasure;

byte           Is_MPI_COMM;  /* */    /*E0046*/
#ifdef _DVM_MPI_

  MPI_Comm    PS_MPI_COMM;   /* */    /*E0047*/
  MPI_Group   ps_mpi_group;  /* */    /*E0048*/

#endif

void   *ResBuf;         /* */    /*E0049*/
DvmType    MaxBlockSize;   /* */    /*E0050*/
void   *RemBuf;         /* */    /*E0051*/
DvmType    RemBufSize;     /* */    /*E0052*/
byte    FreeRemBuf;     /* */    /*E0053*/

endrec s_VMS;



/*********************************\
* Abstract machine representation *
\*********************************/    /*E0054*/

typedef rec

byte    Attr;
byte    Axis;
byte    PAxis;
double  DisPar;

endrec s_MAP;



typedef rec

int        EnvInd;   /* index of context in which 
                        representation has been created;
                        or -1 for static representation */    /*E0055*/
int        CrtEnvInd;/* index of context in which 
                        representation has been created */    /*E0056*/
SysHandle *HandlePtr;/* pointer to own Handle */    /*E0057*/
byte       Static;   /* flag of static representation */    /*E0058*/
s_SPACE    Space;    /* index area */    /*E0059*/
byte       Repl;     /* flag of fully replicated representation */    /*E0060*/
byte       PartRepl; /* flag of partially replicated representation */    /*E0061*/
byte       Every;    /* flag of presence on each processor 
                        at least one element of representation */    /*E0062*/
byte       HasLocal; /* flag of that local part is */    /*E0063*/
s_BLOCK    Local;    /* local part of representation
                        if HasLocal=1 */    /*E0064*/
DvmType       LinAMInd; /* abstract machine linear index
                        corresponding to representation
                        local part if HasLocal=1 */    /*E0065*/
int        LocAMInd; /* local abstract machine index in the list
                        of interrogated abstract machines if HasLocal=1 */    /*E0066*/
SysHandle *AMHandlePtr;/* reference to abstract machine descriptor */    /*E0067*/
s_VMS     *VMS;      /* system of virtual processors */    /*E0068*/
s_MAP     *DISTMAP;  /* rules of mapping to VMS */    /*E0069*/
int AMVAxis[MAXARRAYDIM];
s_COLLECTION ArrColl;/* list of mapping arrays */    /*E0070*/
s_COLLECTION AMSColl;/* list of created abstract machines */    /*E0071*/

s_VMS     *WeightVMS; /* reference to processor system
                         for which coordinate weights are defined */    /*E0072*/
double    *CoordWeight[MAXARRAYDIM]; /* processor coordinate weight array
                                        for each dimension */    /*E0073*/
double    *PrevSumCoordWeight[MAXARRAYDIM]; /* array of summary preceding
                                               processor coordinate weights
                                               for each dimension */    /*E0074*/
DvmType *GenBlockCoordWeight[MAXARRAYDIM]; /* array of "hard" processor coordinate 
                                          weights for each dimension */    /*E0075*/
DvmType *PrevSumGenBlockCoordWeight[MAXARRAYDIM]; /* array of summary preceding
                                               processor coordinate weights
                                               for each dimension */    /*E0076*/
byte setCW;  /* flag of setting and saving coordinate weight for redistribution*/
s_VMS     *disWeightVMS; /* reference to processor system
                         for which coordinate weights are defined */    /*E0072*/
double    *disCoordWeight[MAXARRAYDIM]; /* processor coordinate weight array
                                        for each dimension */    /*E0073*/
double    *disPrevSumCoordWeight[MAXARRAYDIM]; /* array of summary preceding
                                               processor coordinate weights
                                               for each dimension */    /*E0074*/
DvmType *disGenBlockCoordWeight[MAXARRAYDIM]; /* array of "hard" processor coordinate 
                                          weights for each dimension */    /*E0075*/
DvmType *disPrevSumGenBlockCoordWeight[MAXARRAYDIM]; /* array of summary preceding
                                               processor coordinate weights
                                               for each dimension */    /*E0076*/

byte   TimeMeasure; /* flag of execution time measuring
                       for loop iteration groups mapped
                       in the curreent representation */    /*E0077*/
int    GroupNumber[MAXARRAYDIM]; /* array with i-th element containing
                                    number of partition groups
                                    for (i+1)-th dimension of representation */    /*E0078*/
double *GroupWeightArray[MAXARRAYDIM]; /* array with i-th element containig
                                          pointer to array with weights
                                          of groups of (i+1)-th
                                          dimension of representation */    /*E0079*/
byte    Is_gettar[MAXARRAYDIM]; /* array of flags of performed
                                   queries of arrays with weights */    /*E0080*/

DvmType    Div[MAXARRAYDIM];       /* */    /*E0081*/
DvmType    disDiv[MAXARRAYDIM];       /* */    /*E0081*/
byte    DivReset;               /* */    /*E0082*/
byte    IsGetAMR;               /* */    /*E0083*/
ulng    DVM_LINE;               /* */    /*E0084*/
char   *DVM_FILE;               /* */    /*E0085*/

endrec s_AMVIEW;



/********************\
*  Abstract machine  *
\********************/    /*E0086*/

typedef rec

int           EnvInd;      /* current context index */    /*E0087*/
int           CrtEnvInd;   /* index of contex in which 
                              the abstract machine has been created
                              (index of contex in which 
                              the parent abstract machine
                              representation has been created) */    /*E0088*/
SysHandle    *HandlePtr;   /* pointer to own Handle */    /*E0089*/
int           TreeIndex;   /* distance from the abstract
                              machine tree root */    /*E0090*/
s_AMVIEW     *ParentAMView;/* reference to parent abstract machine
                              representation which the given absrtact
                              machine belongs to; or NULL */    /*E0091*/
s_VMS        *VMS;         /* reference to processor system 
                              which the given absrtact machine
                              is mapped on; or NULL */    /*E0092*/
s_COLLECTION  SubSystem;   /* list of the given abstract machine 
                              representations */    /*E0093*/
double        RunTime;     /* */    /*E0094*/
double        ExecTime;    /* */    /*E0095*/
byte          IsMapAM;     /* */    /*E0096*/
ulng          map_DVM_LINE; /* */    /*E0097*/
char         *map_DVM_FILE; /* */    /*E0098*/
ulng          run_DVM_LINE; /* */    /*E0099*/
char         *run_DVM_FILE; /* */    /*E0100*/
ulng          stop_DVM_LINE;/* */    /*E0101*/
char         *stop_DVM_FILE;/* */    /*E0102*/

endrec  s_AMS;



/****************************************\
* Map of abstract machine representation *
\****************************************/    /*E0103*/

typedef rec

int          EnvInd;      /* index of context in which the map 
                             has been created,
                             or -1 (for static map) */    /*E0104*/
int          CrtEnvInd;   /* index of context in which the map 
                             has been created */    /*E0105*/
SysHandle   *HandlePtr;   /* pointer to own Handle */    /*E0106*/
byte         Static;      /* flag of static map */    /*E0107*/
byte         AMViewRank;  /* rank of AM representation */    /*E0108*/
byte         VMSRank;     /* rank of processor system */    /*E0109*/
s_VMS       *VMS;         /* processor system */    /*E0110*/
s_MAP       *DISTMAP;     /* rules of mapping on
                             processor system */    /*E0111*/
DvmType         Div[MAXARRAYDIM]; /* */    /*E0112*/

endrec s_AMVIEWMAP;



/*******************************************************\
* Group of buffers of remote elements of regular access *
\*******************************************************/    /*E0113*/

typedef rec

int            EnvInd;    /* index of context in which the group 
                             has been created,
                             or -1 (for static group) */    /*E0114*/
int            CrtEnvInd; /* index of context in which the group 
                             has been created */    /*E0115*/
SysHandle     *HandlePtr; /* reference to own Handle */    /*E0116*/
byte           Static;    /* flag on static group */    /*E0117*/
s_COLLECTION   RB;        /* list of buffers included in the buffer */    /*E0118*/
byte           DelBuf;    /* flag: delete all buffers included in group 
                             together with the group deleting */    /*E0119*/
byte           IsLoad;    /* flag: buffer group has been loaded */    /*E0120*/
byte           LoadSign;  /* flag: buffer group is loading now */    /*E0121*/
SysHandle     *LoadAMHandlePtr; /* reference to abstract machine Handle
                                   which started buffer loading */    /*E0122*/
int            LoadEnvInd;      /* index of context
                                   which started buffer loading */    /*E0123*/
byte           ResetSign; /* */    /*E0124*/

endrec s_REGBUFGROUP;


/**********************************************\
* Buffer of remote elements of regular access  *
* (additional structure for distributed array) *  
\**********************************************/    /*E0125*/

typedef rec

SysHandle   *DAHandlePtr; /* distributed array, for its elements
                             buffer has been created */    /*E0126*/
byte         crtrbp_sign; /* flag: buffer creation by crtrbp_ function */    /*E0127*/
DvmType         InitIndex[MAXARRAYDIM]; /* initial values of
                                        distributed array indexes */    /*E0128*/
DvmType         LastIndex[MAXARRAYDIM]; /* final values of
                                        distributed array indexes */    /*E0129*/
DvmType         Step[MAXARRAYDIM];      /* distributed array index
                                        increments */    /*E0130*/
byte         IsLoad;          /* flag: buffer has been loaded */    /*E0131*/
byte         LoadSign;        /* flag: buffer is loading now */    /*E0132*/
SysHandle   *LoadAMHandlePtr; /* reference to abstract machine Handle
                                 which started buffer loading */    /*E0133*/
int          LoadEnvInd;      /* index of context
                                 which started buffer loading */    /*E0134*/
AddrType     CopyFlag;        /* flag of asynchronous coping
                                 while loading buffer */    /*E0135*/
s_REGBUFGROUP *RBG;           /* reference to buffer group
                                 which the buffer belongs to;
                                 or NULL */    /*E0136*/

endrec s_REGBUF;


/* */    /*E0137*/

typedef rec

int            EnvInd;      /* */    /*E0138*/
int            CrtEnvInd;   /* */    /*E0139*/
SysHandle     *HandlePtr;   /* */    /*E0140*/
byte           Static;      /* */    /*E0141*/
s_COLLECTION   RDA;         /* */    /*E0142*/
byte           DelDA;       /* */    /*E0143*/
byte           ConsistSign; /* */    /*E0144*/
byte           ResetSign;   /* */    /*E0145*/

endrec s_DACONSISTGROUP;


/***********************************************************\
* Group of buffers of remote elements of non-regular access *
\***********************************************************/    /*E0146*/

typedef rec

int            EnvInd;    /* index of context in which the group 
                             has been created,
                             or -1 (for static group) */    /*E0147*/
int            CrtEnvInd; /* index of context in which the group 
                             has been created */    /*E0148*/
SysHandle     *HandlePtr; /* reference to own Handle */    /*E0149*/
byte           Static;    /* flag on static group */    /*E0150*/
s_COLLECTION   IB;        /* list of buffers included in the buffer */    /*E0151*/
byte           DelBuf;    /* flag: delete all buffers included in group 
                             together with the group deleting */    /*E0152*/
byte           IsLoad;    /* flag: buffer group has been loaded */    /*E0153*/
byte           LoadSign;  /* flag: buffer group is loading now */    /*E0154*/
SysHandle     *LoadAMHandlePtr; /* reference to abstract machine Handle
                                   which started buffer loading */    /*E0155*/
int            LoadEnvInd;      /* index of context
                                   which started buffer loading */    /*E0156*/

endrec s_IDBUFGROUP;


/*************************************************\
* Buffer of remote elements of non-regular access *
*  (additional structure for distributed array)   *  
\*************************************************/    /*E0157*/

typedef rec

SysHandle   *DAHandlePtr; /* distributed array, for its elements
                             buffer has been created */    /*E0158*/
SysHandle   *MEHandlePtr; /* index matrix */    /*E0159*/
DvmType         ConstArray[MAXARRAYDIM]; /* array of coordinates of replicated
                                         dimensions of remote array */    /*E0160*/
int          DistrAxis;       /* number of replicated dimension
                                 of remote arraay minus 1 */    /*E0161*/
byte         IsLoad;          /* flag: buffer has been loaded */    /*E0162*/
byte         LoadSign;        /* flag: buffer is loading now */    /*E0163*/
SysHandle   *LoadAMHandlePtr; /* reference to abstract machine Handle
                                 which started buffer loading */    /*E0164*/
int          LoadEnvInd;      /* index of context
                                 which started buffer loading */    /*E0165*/
s_IDBUFGROUP *IBG;            /* reference to buffer group
                                 which the buffer belongs to;
                                 or NULL */    /*E0166*/

/* Information left by function StartLoadBuffer
              for the function WaitLoadBuffer       */    /*E0167*/


RTL_Request   *MEReq, *DARecvReq;
void         **DASendBuf, **DARecvBuf;
s_BLOCK      **DALocalBlock, *DABlock;
int           *DARecvSize;

endrec s_IDBUF;


/*********************\
*  Distributed array  *
\*********************/    /*E0168*/

typedef rec

byte        Attr;
byte        Axis;
byte        TAxis;
DvmType        A;
DvmType        B;
DvmType        Bound;

endrec s_ALIGN;



typedef rec

DvmType InitDimIndex[MAXARRAYDIM];    /* initial index values
                                       for dimension restriction */    /*E0169*/
DvmType DimWidth[MAXARRAYDIM];        /* lengths for dimension restriction */    /*E0170*/

int  InitLowShdIndex[MAXARRAYDIM]; /* initial index values
                                      for low edges */    /*E0171*/
int  ResLowShdWidth[MAXARRAYDIM];  /* low edges for
                                      edge exchange */    /*E0172*/
int  InitHiShdIndex[MAXARRAYDIM];  /* initial index values
                                      for hight edges */    /*E0173*/
int  ResHighShdWidth[MAXARRAYDIM]; /* high edges for
                                      edge exchange */    /*E0174*/
int  MaxShdCount;                  /* max. number of dimensions,
                                      taking part in edge forming */    /*E0175*/
byte ShdSign[MAXARRAYDIM];         /* array of flags that dimensions
                                      take part in bound forming */    /*E0176*/
byte UseSign;                      /* */    /*E0177*/

endrec s_SHDWIDTH;



typedef rec

int          EnvInd;    /* index of context in which the array 
                           has been created,
                           or -1 (for static array) */    /*E0178*/
int          CrtEnvInd; /* index of context in which the array 
                           has been created */    /*E0179*/
SysHandle   *HandlePtr; /* pointer to own Handle */    /*E0180*/
void        *BasePtr;   /* base pointer */    /*E0181*/
byte         ExtHdrSign;/* flar: detailed header */    /*E0182*/
byte         Static;    /* flag of static array */    /*E0183*/
byte         ReDistr;   /* flag to allow redis_ */    /*E0184*/
s_SPACE      Space;     /* array area */    /*E0185*/
word         TLen;      /* size of array element in bytes  */    /*E0186*/
int          Type;      /* */    /*E0187*/   
s_AMVIEW    *AMView;    /* AM representation */    /*E0188*/
s_ALIGN     *Align;     /* rule of mapping to AMView */    /*E0189*/
byte         HasLocal;  /* flag that local part is */    /*E0190*/
s_BLOCK      Block;     /* local part of array */    /*E0191*/
s_BLOCK    **VMSBlock;  /* pointer to array of array local part blocks
                          for all processors from the processor system
                          the array is mapped on  */    /*E0192*/
s_BLOCK     *VMSLocalBlock; /* */    /*E0193*/                               
byte        *IsVMSBlock;/* array of flags of created array local parts */    /*E0194*/
s_ARRBLOCK   ArrBlock;  /* local part of array including edges */    /*E0195*/
byte         Repl;      /* flag of fully replicated array */    /*E0196*/
byte         PartRepl;  /* flag of partially replicated array */    /*E0197*/
byte         Every;     /* flag of presence on each processor 
                           at least one element of array */    /*E0198*/
int DAAxis[MAXARRAYDIM];/* */    /*E0199*/
s_COLLECTION BG;               /* list of edge groups 
                                 in which the array is included */    /*E0200*/
s_COLLECTION ResShdWidthColl;  /* list of resulting edge widths */    /*E0201*/

int InitLowShdWidth[MAXARRAYDIM];  /* low edges, specified 
                                  when array was created */    /*E0202*/
int InitHighShdWidth[MAXARRAYDIM]; /* high edges, specified
                                  when array was created */    /*E0203*/
s_BLOCK InitBlock;      /* initial block for sequential
                           polling of index values */    /*E0204*/
s_BLOCK CurrBlock;      /* current block for sequential
                           polling of index values */    /*E0205*/
byte      RegBufSign;   /* */    /*E0206*/
byte      RemBufMem;    /* */    /*E0207*/  
s_REGBUF *RegBuf; /* reference to remote element buffer characteristics,
                     or NULL - if distributed array is not a buffer */    /*E0208*/
s_IDBUF  *IdBuf;  /* reference to characteristics of remote element buffer
                     of non-regular access, or NULL - if distributed array
                     is not a buffer */    /*E0209*/
void     *MemPtr; /* */    /*E0210*/

/* */    /*E0211*/

byte          ConsistSign;  /* */    /*E0212*/
byte          RealConsist;  /* */    /*E0213*/
byte          AllocSign;    /* */    /*E0214*/

int           CWriteBSize;  /* */    /*E0215*/
void         *CWriteBuf;    /* */    /*E0216*/
RTL_Request  *CWriteReq;    /* */    /*E0217*/
s_BLOCK       DArrBlock;    /* */    /*E0218*/

int           WriteBlockNumber; /* */    /*E0219*/
s_BLOCK      *DArrWBlockPtr;    /* */    /*E0220*/
s_VMS       **CentralPSPtr; /* */    /*E0221*/

int          *CReadBSize;   /* */    /*E0222*/
void        **CReadBuf;     /* */    /*E0223*/
RTL_Request  *CReadReq;     /* */    /*E0224*/
s_BLOCK      *CReadBlock;   /* */    /*E0225*/

int          *ReadBlockNumber; /* */    /*E0226*/
s_BLOCK     **CRBlockPtr;      /* */    /*E0227*/

int   DAAxisM1;  /* */    /*E0228*/

char *File;  /* */    /*E0229*/
ulng  Line;  /* */    /*E0230*/

s_DACONSISTGROUP *CG;       /* */    /*E0231*/

int  ConsistProcCount; /* */    /*E0232*/
int *sdispls;          /* */    /*E0233*/
int *sendcounts;       /* */    /*E0234*/
int *rdispls;          /* */    /*E0235*/

byte   IsCheckSum;     /* */    /*E0236*/
double CheckSum;       /* */    /*E0237*/

endrec s_DISARRAY;



/****************************\
*  Map of distributed array  *
\****************************/    /*E0238*/

typedef rec

int          EnvInd;      /* index of context in which the map 
                             has been created,
                             or -1 (for static map) */    /*E0239*/
int          CrtEnvInd;   /* index of context in which the map 
                             has been created */    /*E0240*/ 
SysHandle   *HandlePtr;   /* pointer to own handle */    /*E0241*/
byte         Static;      /* flag of static map */    /*E0242*/
byte         ArrayRank;   /* rank of array */    /*E0243*/
byte         AMViewRank;  /* rank of AM representation */    /*E0244*/
s_AMVIEW    *AMView;      /* AM representation */    /*E0245*/
s_ALIGN     *Align;       /* rules of mapping on AMView */    /*E0246*/

endrec s_ARRAYMAP;



/***************\
* Program block *
\***************/    /*E0247*/

typedef rec

int BlockInd;         /* */    /*E0248*/

/* */    /*E0249*/

int ind_VMS;
int ind_AMView;
int ind_DisArray;
int ind_BoundsGroup;
int ind_RedVars;
int ind_RedGroups;
int ind_ArrMaps;
int ind_AMVMaps;
int ind_RegBufGroups;
int ind_DAConsistGroups;
int ind_IdBufGroups;

endrec s_PRGBLOCK;



/*************\
*  Reduction  *
\*************/    /*E0250*/


typedef rec                  /* reduction group */    /*E0251*/

int            EnvInd;       /* index of context in which the group 
                                has been created,
                                or -1 (for static group) */    /*E0252*/
int            CrtEnvInd;    /* index of context in which the group 
                                has been created */    /*E0253*/
byte           ResetSign;    /* */    /*E0254*/
s_VMS         *VMS;          /* reduction group
                                processor system */    /*E0255*/
s_VMS         *PSSpaceVMS;   /* */    /*E0256*/
DvmType SpaceInitIndex[MAXARRAYDIM],
     SpaceLastIndex[MAXARRAYDIM]; /* */    /*E0257*/
int            CrtPSSign;    /* */    /*E0258*/
byte           CrtVMSSign;   /* */    /*E0259*/
int            EnvDiff;      /* Difference for calculation of
                                effective level */    /*E0260*/
SysHandle     *HandlePtr;    /* pointer to own Handle */    /*E0261*/
byte           Static;       /* flag of static group */    /*E0262*/
byte           DelRed;       /* flag of deleting reduction variables 
                                together with the group */    /*E0263*/
byte           StrtFlag;     /* flag: group has been started */    /*E0264*/
s_COLLECTION   RV;           /* reduction variables */    /*E0265*/
int            BlockSize;    /* common lenght of all group reduction 
                                variables with additional information */    /*E0266*/
char         **NoWaitBufferPtr;  /* pointers to buffer addresses
                                  to receive reduction variables */    /*E0267*/
void          *InitBuf, *ResBuf; /* */    /*E0268*/
RTL_Request   *Req;
int           *Flag;
int            MessageCount; /* a number of coming messages 
                                at group start moment */    /*E0269*/
byte           MPIReduce;    /* */    /*E0270*/
int           *DAAxisPtr;    /* */    /*E0271*/
s_DISARRAY    *DA;           /* */    /*E0272*/
byte           TskRDSign;    /* */    /*E0273*/
byte           SaveSign;     /* */    /*E0274*/
byte           IsBuffers;    /* */    /*E0275*/
byte           IsNewVars;    /* */    /*E0276*/

endrec s_REDGROUP;



typedef rec              /* reduction variables */    /*E0277*/

int         EnvInd;      /* index of context in which the reduction
                            variable has been created,
                            or -1 (for static variable) */    /*E0278*/
int         CrtEnvInd;   /* index of context in which the variable 
                            has been created*/    /*E0279*/ 
SysHandle  *HandlePtr;   /* pointer to own Handle */    /*E0280*/
s_REDGROUP *RG;          /* pointer to reduction group
                            or NULL */    /*E0281*/
int         VarInd;      /* reduction variable number in the list
                            of given group reduction variables */    /*E0282*/
char       *BufAddr;     /* variable address in the buffer
                            saved together with additional information */    /*E0283*/ 
s_AMVIEW   *AMView;      /* reference to representation of
                            subtask group abstract machine
                            or NULL */    /*E0284*/
byte        Static;      /* flag of static reduction variable */    /*E0285*/
byte        Func;        /* reduction function number */    /*E0286*/
char       *Mem;         /* address of reduction array-variable */    /*E0287*/
int         VType;       /* element type of reduction array-variable */    /*E0288*/
int         RedElmLength;/* element size of reduction variable */    /*E0289*/
int         VLength;     /* a number of elements in array-variable */    /*E0290*/
char       *LocMem;      /* address of array with localization information */    /*E0291*/
int         LocElmLength;/* element size of array
                            with localization information */    /*E0292*/
int         LocIndType;  /* type of index variables
                            defined maximum and minimum coordinates
                            in the element of array with additional
                            information */    /*E0293*/
int         BlockSize;   /* common lenght of reduction variable-array 
                            with additional information */    /*E0294*/
int         Already;     /* */    /*E0295*/
int     CommonBlockSize; /* */    /*E0296*/
s_DISARRAY *RedDArr;     /* */    /*E0297*/
s_DISARRAY *LocDArr;     /* */    /*E0298*/
int *DAAxisPtr;          /* */    /*E0299*/

endrec s_REDVAR;


/*****************************\
* Edges of distributed arrays *
\*****************************/    /*E0300*/

typedef rec            /* distributed array edge group*/    /*E0301*/ 

int         EnvInd;    /* index of context in which the group 
                          has been created,
                          or -1 (for static group) */    /*E0302*/
int         CrtEnvInd; /* index of context in which the group 
                          has been created*/    /*E0303*/
SysHandle  *HandlePtr; /* pointer to own Handle */    /*E0304*/
byte        ResetSign; /* */    /*E0305*/
byte        Static;    /* flag of static group of edges */    /*E0306*/
s_COLLECTION  NewArrayColl; /* list of arrays newly 
                               included in group */    /*E0307*/
s_COLLECTION  NewShdWidthColl; /* list of result edge widths
                                  for arrays newly included in the group */    /*E0308*/
s_COLLECTION  ArrayColl;    /* list of all arrays  
                               included in group */    /*E0309*/
SysHandle  *BufPtr;    /* pointer to Handle of first edge buffer */    /*E0310*/
byte        IsStrt;    /* flag: an edge exchange has been started */    /*E0311*/
byte        IsStrtsh;  /* flag: edge exchange is started */    /*E0312*/
byte        IsRecvsh;  /* flag: imported element receiving is started */    /*E0313*/
byte        IsSendsh;  /* flag: exported element sending is started */    /*E0314*/
byte        IsRecvla;  /* flag of started receiving
                          of local part elements */    /*E0315*/
byte        IsSendsa;  /* flag of started sending
                          of local part elements */    /*E0316*/
SysHandle  *ShdAMHandlePtr; /* pointer to Handle of abstract machine 
                               which started edge exchange operation 
                               or operations */    /*E0317*/
int         ShdEnvInd;      /* index of context 
                               which started edge exchange operation 
                               or operations */    /*E0318*/
byte        SaveSign;       /* */    /*E0319*/

endrec s_BOUNDGROUP;



typedef rec

byte           IsShdIn, IsShdOut;
s_BLOCK        BlockIn, BlockOut;
DvmType           Proc;
word           TLen;
void          *BufIn, *BufOut;
RTL_Request    ReqIn,  ReqOut;

endrec s_SHADOW;



typedef rec             /* distributed array edge subgroup */    /*E0320*/

int          EnvInd;    /* index of contex in which
                           the buffer has been created */    /*E0321*/
int          CrtEnvInd; /* index of contex in which
                           the buffer has been created */    /*E0322*/ 
SysHandle   *HandlePtr; /* pointer to own Handle */    /*E0323*/
int          Count;     /* number of arrays in the subgroup */    /*E0324*/
int          ArrCount;  /* number of arrays in the subgroup 
                           ( after group creation is equal to 1,
                           then it is increased till Count)*/    /*E0325*/
int          Rank;      /* dimension of all arrays in the subgroup */    /*E0326*/
int 	     DoWaitIn;
int 	     DoWaitOut;  
s_DISARRAY **ArrList;   /* array of pointers to distributed array descriptors
                           that are included in the subgroup
                           (all arrays are different)*/    /*E0327*/
int          ShdCount;
s_SHADOW    *ShdInfo;
s_BLOCK      DimBlock;  /* block of segments, delimiting
                           shadow parallelepipeds */    /*E0328*/
byte         DimWidthSign[MAXARRAYDIM]; /* array of flags of 
                                           segments in dimensions */    /*E0329*/

endrec s_BOUNDBUF;



/****************************************\
* Information of parallel loop iteration *
*    quantum execution time measuring    * 
\****************************************/    /*E0330*/

typedef rec 

DvmType    InitIndex[MAXARRAYDIM]; /* array of initial values of
                                   loop index variables */    /*E0331*/
DvmType    LastIndex[MAXARRAYDIM]; /* array of final values of
                                   loop index variables */    /*E0332*/
double  Time;                   /* time of the given quant execution */    /*E0333*/

endrec s_QTIME;



typedef rec 

char      DVM_FILE[MaxParFileName+1]; /* name of the file with source
                                         program containing the loop*/    /*E0334*/
DvmType      DVM_LINE;                   /* the name of the line in source
                                         program containing the loop*/    /*E0335*/
int       PLRank;              /* parallel loop
                                  dimensions */    /*E0336*/
int       PLGroupNumber[MAXARRAYDIM]; /* the number of iteration groups 
                                         for each loop dimension */    /*E0337*/
DvmType      InitIndex[MAXARRAYDIM]; /* array of initial
                                     low values of loop index variables */    /*E0338*/
DvmType      LastIndex[MAXARRAYDIM]; /* array of initial
                                     high values of loop index variables */    /*E0339*/
DvmType      PLStep[MAXARRAYDIM]; /* increment of index variable
                                  for each loop dimension */    /*E0340*/
byte      Invers[MAXARRAYDIM]; /* flag of inverse execution of iterations
                                  for each loop dimension */    /*E0341*/
byte      QSign[MAXARRAYDIM];  /* flag: quant  
                                  each loop dimension */    /*E0342*/
DvmType      QSize[MAXARRAYDIM];  /* quant of index variable
                                  for each loop dimension */    /*E0343*/
DvmType      QNumber;             /* the number of elements in QTime array */    /*E0344*/
DvmType      QCount;              /* the number of non free elements 
                                  in QTime array */    /*E0345*/
s_QTIME  *QTime;               /* array with characteristics of 
                                  loop execution quants */    /*E0346*/
DvmType      SInitIndex[MAXARRAYDIM]; /* array of saved index initial values
                                      for the current loop iteration 
                                      group */    /*E0347*/
DvmType      SLastIndex[MAXARRAYDIM]; /* array of saved index final values
                                      for the current loop iteration 
                                      group */    /*E0348*/

endrec s_PLQUANTUM;



/*******************\
*   Parallel loop   *
\*******************/    /*E0349*/

typedef rec              /* information of parallel loop 
                            changing */    /*E0350*/

DvmType      *InitIndexPtr; /* pointer to initial index value */    /*E0351*/
DvmType      *LastIndexPtr; /* pointer to final index value */    /*E0352*/
DvmType      *StepPtr;      /* pointer to index increment (decrement) */    /*E0353*/
void      *LoopVarAddr;  /* index variable address */    /*E0354*/
byte       LoopVarType;  /* index variable type */    /*E0355*/

endrec s_LOOPMAPINFO;



typedef rec                   /* parallel loop */    /*E0356*/

int               EnvInd;     /* current context index */    /*E0357*/
int               CrtEnvInd;  /* index of contex in which
                                 the object has been created */    /*E0358*/
SysHandle        *HandlePtr;  /* pointer to own Handle */    /*E0359*/
int               Rank;       /* loop rank */    /*E0360*/
int               HasAlnLoc;  /* flag that loop has a local part in
                                 index area of template */    /*E0361*/
int               HasLocal;   /* flag that loop has
                                 a local part */    /*E0362*/
byte              Empty;      /* special exit from mappl_
                                 if some initial index is greater than
                                 final one */    /*E0363*/
int               IterFlag;   /* ITER_NORMAL, ITER_BOUNDS_FIRST,
                                 ITER_BOUNDS_LAST */    /*E0364*/

int               AddBnd;     /* flag of loop local part extention
                                 by corresponding distributed array edges
                                 the loop is mapped on */    /*E0365*/
int     LowShd[MAXARRAYDIM];  /* */    /*E0366*/
int     HighShd[MAXARRAYDIM]; /* */    /*E0367*/

int               CurrBlock;  /* a number of calculated 
                                 imported blocks */    /*E0368*/
byte              IsInIter;   /* flag that internal area was
                                 calculated */    /*E0369*/
byte              IsWaitShd;  /* flag that edge exchange was
                                 completed */    /*E0370*/
ShadowGroupRef    BGRef;      /* reference to started edge group */    /*E0371*/
s_AMVIEW         *AMView;     /* AM representation */    /*E0372*/
s_LOOPMAPINFO    *MapList;    /* array of current parameters of a loop */    /*E0373*/
s_REGULARSET     *AlnLoc;     /* local part of loop in 
                                 index area of template */    /*E0374*/
s_REGULARSET     *Local;      /* local part of index
                                 area of loop */    /*E0375*/
s_REGULARSET     *Set;        /* loop index area */    /*E0376*/
s_ALIGN          *Align;      /* rules of mapping on AMView */    /*E0377*/

DvmType InitIndex[MAXARRAYDIM];   /* initial values of loop indexes */    /*E0378*/

int  LowShdWidth[MAXARRAYDIM]; /* low bounds for computation of 
                                  export elements */    /*E0379*/
int  HighShdWidth[MAXARRAYDIM];/* high bounds for computation of
                                  export elements */    /*E0380*/
byte Invers[MAXARRAYDIM];      /* flags of inverse execution of
                                  loop iterations */    /*E0381*/
s_PLQUANTUM *PLQ; /* reference to structure with information of
                     the loop iteration group execution time 
                     or NULL */    /*E0382*/
byte   DoQuantum; /* flag: iteration group execution
                     and execution time calculation */    /*E0383*/
double ret_dopl_time; /* time of dopl_ exit ( to calculate
                         iteration group execution time)*/    /*E0384*/

byte   WaitRedSign;   /* flag: only attempt to complete
                         asynchronous reduction for all started groups
                         at the entrance to dopl_*/    /*E0385*/
byte   QDimSign;      /* there is a dimension divided in portion
                         for outstipping execution of
                         asynchronous reduction
                         and for MPI_Test function calling */    /*E0386*/
int  QDimNumber;              /* */    /*E0387*/
int  QDim[MAXARRAYDIM];       /* number of dimention divided in portions */    /*E0388*/
DvmType QLastIndex[MAXARRAYDIM]; /* saved final index value
                         on the divided dimention  */    /*E0389*/
DvmType QInitIndex[MAXARRAYDIM]; /* */    /*E0390*/
DvmType StdPLQSize[MAXARRAYDIM]; /* number of points in the standard portion
                         of internal loop part */    /*E0391*/
DvmType StdPLQCount[MAXARRAYDIM];/* counter of standard portions of internalloop part
                         ( number of portions - 1)*/    /*E0392*/
DvmType StdPLQNumb[MAXARRAYDIM]; /* */    /*E0393*/

byte   AcrType1; /* flag of execution of ACROSS scheme
                     <sendsh_ - recvsh_> */    /*E0394*/
byte   AcrType2; /* flag of execution of ACROSS scheme
                    <sendsa_ - recvla_>  */    /*E0395*/
byte   PipeLineAxis1[MAXARRAYDIM]; /* */    /*E0396*/
byte   PipeLineAxis2[MAXARRAYDIM]; /* */    /*E0397*/
byte   IsAcrType1Wait; /* flag: there was waiting of recvsh_ */    /*E0398*/
byte   IsAcrType2Wait; /* flag: there was waiting of recvla_ */    /*E0399*/


/* References to edge groups for ACROSS scheme */    /*E0400*/

ShadowGroupRef  AcrShadowGroup1Ref;
ShadowGroupRef  AcrShadowGroup2Ref;
ShadowGroupRef  OldShadowGroup1Ref;
ShadowGroupRef  NewShadowGroup1Ref;
ShadowGroupRef  OldShadowGroup2Ref;
ShadowGroupRef  NewShadowGroup2Ref;

byte  PipeLineSign;   /* flag of pipeline ACROSS loop
                         execution */    /*E0401*/
byte  IsPipeLineInit; /* flag: pipeline has been initialized */    /*E0402*/
int   AcrossGroup;    /* number of groups for quanted dimension partitioning
                         for ACROSS scheme execution */    /*E0403*/
int   AcrossQNumber;  /* number of portions 
                         quanted dimension local part is to be partitioned in
                         for ACROSS scheme execution */    /*E0404*/

s_DISARRAY *TempDArr; /* pointer to distributed array descriptor
                         (if the loop is mapped on the array)
                         or NULL */    /*E0405*/ 
int  DArrAxis[MAXARRAYDIM]; /* number of distributed array dimension
                                     the loop is mapped on
                                     for each loop dimension 
                                     (TempDArr != NULL) */    /*E0406*/
int  PLAxis[MAXARRAYDIM];   /* */    /*E0407*/
int  DDim[MAXARRAYDIM];     /* number of distributed array dimension
                     corresponding to loop quanted dimension 
                     - 1 */    /*E0408*/
byte FirstQuantum;          /* flag of first loop iteration portion */    /*E0409*/
DvmType CurrInit[MAXARRAYDIM]; /* initial quanted loop dimension index value 
                         for current iteration portion */    /*E0410*/
DvmType CurrLast[MAXARRAYDIM]; /* final quanted loop dimension index value 
                         for current iteration portion */    /*E0411*/
DvmType NextInit[MAXARRAYDIM]; /* initial quanted loop dimension index value 
                         for the next iteration portion */    /*E0412*/
DvmType NextLast[MAXARRAYDIM]; /* final quanted loop dimension index value 
                         for the next iteration portion */    /*E0413*/
DvmType RecvInit[MAXARRAYDIM]; /* minimal value of index in array dimension
                         corresponding to loop quanted dimension 
                         during receiving */    /*E0414*/
DvmType RecvLast[MAXARRAYDIM]; /* maximal value of index in array dimension
                         corresponding to loop quanted dimension 
                         during receiving */    /*E0415*/
DvmType SendInit[MAXARRAYDIM]; /* minimal value of index in array dimension
                         corresponding to loop quanted dimension 
                         during sending */    /*E0416*/
DvmType SendLast[MAXARRAYDIM]; /* maximal value of index in array dimension
                         corresponding to loop quanted dimension 
                         during sending */    /*E0417*/
DvmType LowIndex[MAXARRAYDIM]; /* low value of loop index in quanted dimension
                         to refer to 0-th coordinate
                         of distributed array */    /*E0418*/
double      Tc;             /* time of one iteration execution */    /*E0419*/
byte        SetTcSign;      /* flag: it is necessary to change
                          one iteration execution time */    /*E0420*/
double     *PipeLineParPtr; /* pipeline parameter address
                               to write changed iteration execution time */    /*E0421*/

endrec s_PARLOOP;



/****************\
*    CONTEXT     *
\****************/    /*E0422*/

typedef rec

DvmType         EnvProcCount;   /* number of processors that execute 
                                current branch */    /*E0423*/
SysHandle    *AMHandlePtr;   /* reference to the current abstract machine */    /*E0424*/

s_COLLECTION PrgBlock;

/* lists of created objects in current program context */    /*E0425*/

s_COLLECTION VMSList;        /* processor subsystems */    /*E0426*/
s_COLLECTION AMViewList;     /* representations of abstract machines */    /*E0427*/
s_COLLECTION DisArrList;     /* distributed arrays */    /*E0428*/
s_COLLECTION BoundGroupList; /* edge groups of distributed arrays */    /*E0429*/
s_COLLECTION RedVars;        /* reduction variables */    /*E0430*/
s_COLLECTION RedGroups;      /* reduction groups */    /*E0431*/
s_COLLECTION ArrMaps;        /* maps of distributed
                                arrays */    /*E0432*/
s_COLLECTION AMVMaps;        /* maps of abstract machine
                                representation */    /*E0433*/
s_COLLECTION RegBufGroups;   /* buffer groups of regular access */    /*E0434*/
s_COLLECTION DAConsistGroups;/* */    /*E0435*/
s_COLLECTION IdBufGroups;    /* buffer groups of non-regular access */    /*E0436*/

/* ------------------------------------------------------- */    /*E0437*/

s_PARLOOP    *ParLoop;       /* reference to parallel loop
                                if the context was created 
                                at the entrance in thia loop
                                or NULL */    /*E0438*/

endrec s_ENVIRONMENT;


/****************************************\
* Distributed array asynchronous copying *
\****************************************/    /*E0439*/

typedef rec        /* Head structure to continue copying */    /*E0440*/

int     Oper;      /* number of continue copying operation */    /*E0441*/
void   *ContInfo;  /* reference to information for continuation
                      or NULL */    /*E0442*/

endrec s_COPYCONT;



typedef rec        /* information structure to continue
                      "read/write one element" function */    /*E0443*/

int           VMSize;
int          *SendFlag, *RecvFlag;
RTL_Request  *SendReq, *RecvReq;
char         *BufferPtr, *_BufferPtr;
s_DISARRAY   *DArr;
DvmType         *IndexArray;

endrec s_ARWElm;



typedef rec        /* information structure to continue
                      "copy one element" function */    /*E0444*/

int           ReadVMSize, WriteVMSize;
int          *ReadFlag, *WriteFlag;
RTL_Request  *ReadReq, *WriteReq;
void         **ReadBuf, **WriteBuf;
s_DISARRAY   *ToDArr;
DvmType         *ToIndexArray;

endrec s_ACopyElm;



typedef rec  /* information structure to continue block reading */    /*E0445*/

int           VMSize;
int          *SendFlag, *RecvFlag;
RTL_Request  *SendReq, *RecvReq;
void         *SendBuf, **RecvBuf;
s_BLOCK      *RecvBlock;
s_DISARRAY   *DArr;
byte          Step;
char         *BufferPtr;
s_BLOCK       ReadBlock;

endrec s_AGetBlock;



typedef rec  /* information structure to continue
                block writing by I/O processor */    /*E0446*/

int           VMSize;
int          *SendFlag, *RecvFlag;
RTL_Request  *SendReq, *RecvReq;
void         **SendBuf, *RecvBuf;
s_DISARRAY   *DArr;
byte          Step;
s_BLOCK       WriteLocalBlock;

endrec s_AIOPutBlock;



typedef rec  /* information structure to continue block writing
                by I/O processor for replicated array */    /*E0447*/

int           VMSize, BSize;
s_VMS         *VMS;
RTL_Request  *SendReq, *RecvReq;
byte          Step, RecvSign;
char         *BufferPtr, *_BufferPtr;
s_DISARRAY   *DArr;
s_BLOCK       WriteBlock;

endrec s_AIOPutBlockRepl;



typedef rec  /* information structure
                to continue block copying */    /*E0448*/

s_BLOCK         FromBlock, ToBlock;
s_DISARRAY     *ToDArr;
s_VMS          *WriteVMS, *ReadVMS;
DvmType            ReadVMSize, WriteVMSize;
RTL_Request    *ReadReq, *WriteReq;
void          **ReadBuf, **WriteBuf;
int            *IsReadInter, *IsWriteInter;
s_BLOCK        *ReadLocalBlock, *WriteLocalBlock,
               *ToLocalBlock, *CurrWriteBlock;
int            *ReadBSize, *WriteBSize;
DvmType            ReadWeight[MAXARRAYDIM];
byte            FromStep, ToStep, EquSign, ExchangeScheme,
                FromSuperFast, ToSuperFast, Alltoall;
int             FromOnlyAxis, ToOnlyAxis;
byte           *FromVM, *ToVM;

endrec s_ACopyBlock;


/************\
* allocmem.c *
\************/    /*E0449*/

/* structure to keep memory block request parameters */    /*E0450*/

typedef struct {
                 byte       *ptr;
                 DvmType        size;

               } s_ALLOCMEM;



/**************\
*  inputpar.c  *
\**************/    /*E0451*/

typedef struct {
                 char           *pName;
                 DvmType            NameLen;
                 byte            PtrSign;
                 void           *pValue;
                 byte            pType;
                 DvmType            bsize;
                 DvmType            tsize;
                 DvmType            esize;
                 DvmType            isize;

               } s_PARAMETERKEY;



/**************\
*  Statistics  *
\**************/    /*E0452*/

typedef rec

double  CallCount;     /* call count */    /*E0453*/
double  ProductTime;   /* productive time */    /*E0454*/
double  LostTime;      /* lost time */    /*E0455*/

endrec  s_GRPTIMES;    /* description of operations group */    /*E0456*/



typedef rec

int     GrpNumber;     /* number of operations group */    /*E0457*/
double  dvm_time;      /* last system time */    /*E0458*/
double  ProductTime;   /* productive time */    /*E0459*/
double  LostTime;      /* lost time */    /*E0460*/

endrec  s_STATGRP;     /* operations group */    /*E0461*/



typedef rec

double  SendCallTime;     /* */    /*E0462*/
double  MinSendCallTime;  /* */    /*E0463*/
double  MaxSendCallTime;  /* */    /*E0464*/
DvmType    SendCallCount;    /* */    /*E0465*/
double  RecvCallTime;     /* */    /*E0466*/
double  MinRecvCallTime;  /* */    /*E0467*/
double  MaxRecvCallTime;  /* */    /*E0468*/
DvmType    RecvCallCount;    /* */    /*E0469*/

endrec  s_SendRecvTimes;  /* */    /*E0470*/

  
/* */    /*E0471*/

#ifdef _DVM_MPI_

typedef rec

   int             messageTag;       /* */    /*E0472*/
   int             messageSource;    /* */    /*E0473*/
   int             messageDest;      /* */    /*E0474*/
   int             isSend;           /* 0 if MPI_Irecv */    /*E0475*/
   void           *buffer;           /* */    /*E0476*/
   MPI_Datatype    datatype;         /* */    /*E0477*/
   int             count;            /* */    /*E0478*/
   int             isStarted;        /* */    /*E0479*/
   int             isInit;           /* */    /*E0480*/
   unsigned        checksum;         /* */    /*E0481*/
   MPI_Comm        comm;             /* */    /*E0482*/ 
   int             MPITestCount;     /* */    /*E0483*/
   ulng            TotalTestCount;   /* */    /*E0484*/

endrec  nonblockInfo;  /* */    /*E0485*/

typedef rec

   int             proc;    /* */    /*E0486*/
   int             pcount;  /* */    /*E0487*/
   int            *plist;   /* */    /*E0488*/

endrec  commInfo;           /* */    /*E0489*/

typedef rec

   int             tag;              /* */    /*E0490*/
   int             MPITestCount;     /* */    /*E0491*/
   ulng            TotalTestCount;   /* */    /*E0492*/

endrec  s_Iprobe;           /* */    /*E0493*/

#endif


#endif /* _SYSTEM_TYP_ */    /*E0494*/
