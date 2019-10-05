#ifndef _CNTRLERR_TYP_
#define _CNTRLERR_TYP_
/********************/

typedef struct _tag_CONTEXT
{
    byte Rank;
    /* Context type:
       0 - sequential loop
       1 - parallel loop
       2 - task region
    */
    byte Type;
    int  No;
    byte ItersInit;
    DvmType Iters[MAXARRAYDIM];

    s_REGULARSET Limits[MAXARRAYDIM];
}
dvm_CONTEXT;

enum
{
    MAX_ERR_MESSAGE = 256,
    MAX_ERR_CONTEXT = 256,
    MAX_ERR_FILENAME = 128,
    MAX_NUMBER_LENGTH = 128,  /* max. string length of the NUMBER */
    MAXDEBSTRINGLENGTH = 512  /* max. debugging string length */
};

typedef struct _tag_ERROR_RECORD
{
    int  StructNo;
    DvmType CntxNo;
    char Context[MAX_ERR_CONTEXT + 1];
    char Message[MAX_ERR_MESSAGE + 1];
    char File[MAX_ERR_FILENAME + 1];
    UDvmType Line;
    UDvmType TrcLine;
    int Count;
    int TrcCPU;
    int RealCPU;
    byte *CPUList;
    t_tracer_time ErrTime;
#ifdef _MPI_PROF_TRAN_
    signed char Primary;   /* flag: -1 - pot. secondary, 1 - primary, 0 - undefined */
#endif
}
ERROR_RECORD;

typedef struct _tag_ERRORTABLE
{
    TABLE tErrors;
    int MaxErrors;
    int ErrCount;
}
ERRORTABLE;

enum
{
    SUCCESS = 0,
    ERR_RD_STRUCT,              /* Wrong file structure */
    ERR_RD_UNDEF_KEY,           /* Unknown keyword or its wrong usage */
    ERR_RD_SYNTAX,              /* Wrong command syntax */
    ERR_RD_UNDEFINED,           /* Reference to undefined array */
    ERR_RD_ARR_MISMATCH,        /* Array access mismatch */
    ERR_RD_FAILED_CS,           /* Warning: failed checksums found. They won't be recounted */
    ERR_RD_OPENFILE,            /* Opening file error */
    ERR_RD_EMPTY,               /* Empty trace file */
    ERR_RD_TRACEF_MISMATCH,     /* Input trace files mismatch */
    ERR_RD_TRACE_INCOMPLETE,    /* Input trace file is incomplete */
    ERR_RD_LAST,
    ERR_TR_NO_CURSTRUCT,        /* Current loop is absent */
    ERR_TR_TERM_BY_SIGNAL,      /* Terminated by signal */
    ERR_TR_LAST,
    ERR_CMP_NO_CURSTRUCT,       /* Current loop is absent */
    ERR_CMP_NO_ITER,            /* Superfluous iteration */
    ERR_CMP_DUAL_ITER,          /* Repeated execution of iteration */
    ERR_CMP_NO_STRUCT,          /* Non-executed loop */
    ERR_CMP_OUT_STRUCT,         /* Abnormal exit from loop */
    ERR_CMP_NO_INFO,            /* Superfluous reference to variable */
    ERR_CMP_NO_TRACE,           /* Corresponding trace record is missing */
    ERR_CMP_NO_SKIP,            /* Corresponding skip record is missing */
    ERR_CMP_NAN_VALUE,          /* NAN or INF value is found during the execution */
    ERR_CMP_DIFF_INT_VAL,       /* Different values */
    ERR_CMP_DIFF_BOOL_VAL,      /* Different values */
    ERR_CMP_DIFF_LONG_VAL,      /* Different values */
    ERR_CMP_DIFF_LLONG_VAL,     /* Different values */
    ERR_CMP_DIFF_FLOAT_VAL,     /* Different values */
    ERR_CMP_DIFF_DBL_VAL,       /* Different values */
    ERR_CMP_DIFF_COMPLEX_FLOAT_VAL,     /* Different values */
    ERR_CMP_DIFF_COMPLEX_DBL_VAL,       /* Different values */
    ERR_CMP_DIFF_REDUCT_INT_VAL,   /* Different values of reduction variables */
    ERR_CMP_DIFF_REDUCT_BOOL_VAL,  /* Different values of reduction variables */
    ERR_CMP_DIFF_REDUCT_LONG_VAL,  /* Different values of reduction variables */
    ERR_CMP_DIFF_REDUCT_LLONG_VAL, /* Different values of reduction variables */
    ERR_CMP_DIFF_REDUCT_FLOAT_VAL, /* Different values of reduction variables */
    ERR_CMP_DIFF_REDUCT_DBL_VAL,   /* Different values of reduction variables */
    ERR_CMP_DIFF_REDUCT_COMPLEX_FLOAT_VAL, /* Different values of reduction variables */
    ERR_CMP_DIFF_REDUCT_COMPLEX_DBL_VAL,   /* Different values of reduction variables */
    ERR_CMP_ARRAY_OUTOFBOUND,    /* Array index out of bounds */
    ERR_CMP_DIFF_CS_VALUES,      /* Different checksum values for array */
    ERR_CMP_FAILED_CS,           /* Unable to compute checksum for array */
    ERR_CMP_DIFF_REPL_ARR,       /* Instances of replicated array (\"%s\", \"%s\", %ld, %d) do not match at the point %s */
    ERR_CMP_LAST,
    ERR_DYN_WRITE_RO,           /* Writing to ReadOnly variable */
    ERR_DYN_PRIV_NOTINIT,       /* Using non-initialized private variable */
    ERR_DYN_DISARR_NOTINIT,     /* Using non-initialized element */
    ERR_DYN_ARED_NOCOMPLETE,    /* Using variable before asynchronous reduction completion */
    ERR_DYN_NONLOCAL_ACCESS,    /* Using macros for non-local element */
    ERR_DYN_WRITE_IN_BOUND,     /* Writing to shadow element of array */
    ERR_DYN_DATA_DEPEND,        /* Loop dependence on data */
    ERR_DYN_BOUND_RENEW_NOCOMPLETE, /* Using shadow element %s before asynchronous shadow renew competed */
    ERR_DYN_WRITE_REMOTEBUFF,       /* Write to remote buffer */
    ERR_DYN_SEQ_WRITEARRAY,         /* Writing to remote element in sequential branch */
    ERR_DYN_SEQ_READARRAY,          /* Reading remote element in sequential branch */
    ERR_DYN_REDUCT_WAIT_BSTART,        /* Call reduction wait before starting */
    ERR_DYN_DISARR_LIMIT,              /* Using an element outside of array bounds */
    ERR_DYN_REDUCT_START_WITHOUT_WAIT, /* START for reduction without WAIT */
    ERR_DYN_REDUCT_NOT_STARTED,        /* Reduction operation was not started */
    ERR_DYN_LAST,
    ERR_LAST  = ERR_DYN_LAST
};


#endif  /* _CNTRLERR_TYP_ */
