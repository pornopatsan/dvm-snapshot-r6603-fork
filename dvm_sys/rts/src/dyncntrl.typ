#ifndef _DYNCNTRL_TYP_
#define _DYNCNTRL_TYP_
/********************/

typedef void (*PFN_TABLE_ELEMDESTRUCTOR)(void *Elem);

typedef struct tag_TABLE
{
    byte                  IsInit;
    s_COLLECTION          cTable;
    size_t                TableSize;
    size_t                CurSize;
    size_t                ElemSize;
    DvmType		  lastAccessed;
    PFN_TABLE_ELEMDESTRUCTOR  Destruct;
}
TABLE;

typedef struct tag_HASH_LIST
{
    DvmType    lNextElem;
    DvmType    lValue;
    DvmType    rgKey[1];
}
HASH_LIST;

typedef size_t HASH_VALUE;

struct tag_HASH_TABLE;
typedef HASH_VALUE (*PFN_CALC_HASH_FUNC)(struct tag_HASH_TABLE* pHashTable, DvmType* lKey);

typedef struct tag_HASH_TABLE
{
    TABLE       tElements;
    DvmType*       plIndex;
    size_t      nIndexSize;
    short       sKeyRank;
    PFN_CALC_HASH_FUNC pfnHashFunc;

    /* fields for statistics accumulation */

    UDvmType* puElements;
    UDvmType  lStatCompare, lStatPut, lStatFind;
}
HASH_TABLE;

typedef struct tag_VAR_TABLE
{
    HASH_TABLE   hIndex;
    TABLE        vTable;
}
VAR_TABLE;

struct tag_VarInfo;
typedef void (*PFN_VARTABLE_ELEMDESTRUCTOR)( struct tag_VarInfo * );

typedef struct tag_VarInfo
{
    byte        Busy;
    void*       VarAddr;
    DvmType        PrevNo;
    int         EnvirIndex;
    byte        Type;
    SysHandle*  Handle;
    void*       Info;
    int         Tag;
    byte        IsInit;
    PFN_VARTABLE_ELEMDESTRUCTOR  pfnDestructor;
}
VarInfo;

enum
{
    dtype_UNDEF = 0,
    dtype_READONLY,
    dtype_PRIVATE,
    dtype_REDUCT,
    dtype_DISARRAY,
    dtype_TRCREDUCT
};

enum
{
    dar_CALC = 0,
    dar_WAIT,
    dar_STARTED,
    dar_COMPLETED
};

typedef struct tag_DISARR_SHADOW
{
    int            ResLowShdWidth[MAXARRAYDIM];
    int            ResHighShdWidth[MAXARRAYDIM];
    int            MaxShdCount;
    byte           ShdSign[MAXARRAYDIM];
    s_BOUNDGROUP*  pBndGroup;
    byte           bValid;
}
DISARR_SHADOW;

typedef struct tag_DISARR_INFO
{
    size_t    ArrSize;
    DvmType      CurrIter;
    s_BLOCK   LocalBlock;
    byte      HasLocal;
    byte      FlashBoundState;
    byte      Across;
    byte      DisableLocalCheck;
    byte      CollapseRank;
    byte      ShadowCalculated;

    byte*     Init;
    DvmType*     Iter;
    short*    Attr;

    /* Array shadows fields */

    s_COLLECTION collShadows;

    /* Remote buffer fields */

    byte      RemoteBuffer;
    VarInfo*  RB_Source;
    byte      RB_Rank;
    DvmType*     RB_MapIndex;
}
DISARR_INFO;

/* 8 bit - element is accessed or not */
/* 7 bit - access type: read or write */
/* 6 bit - element is accessed from multiple iters or not */
enum tag_DISARR_STATE
{
    DDS_NOACCESS      = 0x0,  /* 8 bit state */
    DDS_ACCESSED      = 0x1,  /* 8 bit state */
    DDS_READ          = 0x00, /* 7 bit state */
    DDS_WRITE         = 0x10, /* 7 bit state */
    DDS_SINGLEITER    = 0x000, /* 6 bit state */
    DDS_MULTIPLEITERS = 0x100  /* 6 bit state */
};

typedef struct tag_PRIVATE_INFO
{
    /* Remote buffer fields */

    byte      RemoteBuffer;
    VarInfo*  RB_Source;
    byte      RB_Rank;
    DvmType*     RB_MapIndex;
}
PRIVATE_INFO;

typedef void (*PFN_HASHITERATION)( DvmType, void * );

typedef void (*PFN_TABLEITERATION)( void *Elem, void *Param1, void *Param2 );

typedef void (*PFN_VARTABLEITERATION)( VarInfo * );


typedef struct tag_PRESAVE_VARIABLE
{
    char          szOperand[MaxOperand + 1];
    char          szFileName[MaxSourceFileName + 1];
    UDvmType ulLine;
    DvmType          lType;
    void*         pAddr;
    byte          bIsReduct;
    SysHandle*    pSysHandle;
    void*         pArrBase;
}
PRESAVE_VARIABLE;

#endif  /* _DYNCNTRL_TYP_ */
