#ifndef _USER_TYP_
#define _USER_TYP_
/****************/    /*E0000*/

/***************************************************************\
* Types to use in user's program *
\***************************************************************/    /*E0001*/ 


#ifdef _UNIX_
   typedef long long int               t_tracer_time;
   typedef unsigned long long int      t_u_tracer_time;
#else
   typedef long                        t_tracer_time;
   typedef unsigned long               t_u_tracer_time;
#endif


typedef UDvmType AMRef;          /* abstarct machine reference */    /*E0002*/
typedef UDvmType AMViewRef;      /* abstract machine view
                                         reference */    /*E0003*/
typedef UDvmType PatternRef;     /* reference to a pattern*/    /*E0004*/
typedef UDvmType PSSpaceRef;     /* reference to processor space
                                         descriptor */    /*E0005*/
typedef UDvmType PSRef;          /* processor system 
                                         reference */    /*E0006*/
typedef UDvmType ArrayMapRef;    /* reference to distributed 
                                         array map */    /*E0007*/
typedef UDvmType AMViewMapRef;   /* reference to abstract machine 
                                         view map */    /*E0008*/
typedef UDvmType BlockRef;       /* reference to program block */    /*E0009*/
typedef UDvmType LoopRef;        /* reference to parallel loop */    /*E0010*/
typedef UDvmType RedRef;         /* reference to a reduction 
                                         variable */    /*E0011*/
typedef UDvmType RedGroupRef;    /* reference to a reduction
                                         group */    /*E0012*/
typedef UDvmType ShadowGroupRef; /* reference to a shadows group */    /*E0013*/
typedef UDvmType RegularAccessGroupRef; /* reference to regular 
                                                remote access buffers*/    /*E0014*/
typedef UDvmType DAConsistGroupRef; /* */    /*E0015*/
typedef UDvmType IndirectAccessGroupRef;/* reference to non regular 
                                                remote access buffer*/    /*E0016*/
typedef UDvmType ObjectRef;      /* reference to an object */    /*E0017*/
typedef UDvmType AddrType;       /* address type defined as unsigned long */    /*E0018*/ 


typedef rec

DvmType        FileID;  /* */    /*E0019*/
FILE          *File;    /* */    /*E0020*/
FILE          *ScanFile;/* */    /*E0021*/
unsigned char  zip;     /* */    /*E0022*/
unsigned char  ParIOType; /* */    /*E0023*/
unsigned char  LocIOType; /* */    /*E0024*/
unsigned char  W;       /* */    /*E0025*/
int            flush;   /* */    /*E0026*/

#ifdef _DVM_ZLIB_
   gzFile  zlibFile;    /* */    /*E0027*/
   char    Type[16];    /* */    /*E0028*/
   DvmType ScanFileID;  /* */    /*E0029*/
#endif

endrec DVMFILE;



typedef rec

DvmType  FileID;
int      Handle;

endrec DVMHANDLE;



typedef rec

unsigned char  SendSign;   /* */    /*E0030*/
int          ProcNumber;   /*inner processor number which send or receive messages  */    /*E0031*/
double       SendRecvTime; /*time of beginning of  NO WAIT exchange  */    /*E0032*/
char        *BufAddr;      /*address of sent or received message */    /*E0033*/
DvmType      BufLength;    /*length  of sent or received message */    /*E0034*/
MPS_Status   Status;       /* */    /*E0035*/
MPS_Request  MPSFlag;      /*MPS- exchange flag */    /*E0036*/

/* */    /*E0037*/

int          FlagNumber;   /* */    /*E0038*/
MPS_Request *MPSFlagArray; /* */    /*E0039*/
unsigned char *EndExchange;/* */    /*E0040*/

/* */    /*E0041*/

int MsgLength; /* */    /*E0042*/
int Remainder; /* */    /*E0043*/
int Init;      /* */    /*E0044*/
int Last;      /* */    /*E0045*/
int Chan;      /* */    /*E0046*/
int tag;       /* */    /*E0047*/
int CurrOper;  /* */    /*E0048*/

/* */    /*E0049*/

char          *CompressBuf;      /* */    /*E0050*/
int            ResCompressIndex; /* */    /*E0051*/ 
int            CompressMsgLen;   /* */    /*E0052*/
unsigned char  IsCompressSize;   /* */    /*E0053*/

endrec RTL_Request;



/*******************\
*  Object's handle  *
\*******************/    /*E0054*/

typedef rec

void          *BasePtr;         /* */    /*E0055*/
UDvmType      HeaderPtr;       /* pointer to array header
                                   or Type field repetition */    /*E0056*/
void          *NextHandlePtr;   /* pointer to the next Handle or NULL,
                                   used to arrange distributed array
                                   edge group buffers */    /*E0057*/
unsigned char  Type;            /* type of an object */    /*E0058*/
int            EnvInd;          /* index of context 
                                   in which the object has been created;
                                   or -1 for static object;
                                   current context index - for anabstract
                                   machine and for parallel loop */    /*E0059*/
int            CrtEnvInd;       /* index of context in which the object
                                   has been created */    /*E0060*/
int            InitCrtBlockInd; /* */    /*E0061*/
int            CrtBlockInd;     /* */    /*E0062*/
void          *CrtAMHandlePtr;  /* pointer to Handle of an abstract machine
                                   of subtask that create the object */    /*E0063*/
DvmType        lP;              /* number of processor element (sht_VProc) in 
                                   initial processor system;
                                   linear number of an abstract machine (sht_AMS)
                                   in its parent abstract machine representation;
                                   NULL for other objects */    /*e0064*/
void          *pP;              /* pointer to data that depends on
                                   object's type; for processor element
                                   (sht_VProc) - NULL */    /*E0065*/
endrec  SysHandle;


#endif /* _USER_TYP_ */    /*E0066*/
