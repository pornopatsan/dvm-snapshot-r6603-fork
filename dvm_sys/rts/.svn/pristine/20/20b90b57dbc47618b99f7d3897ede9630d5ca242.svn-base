
#ifndef _DBGDEC_C_
#define _DBGDEC_C_
/***************/

#include <float.h>
//#include <math.h>
#include <ctype.h>

/*
  Checks for NAN in Windows and Linux
*/

int dbg_isNAN(void *Val, DvmType lType)
{
    double val;

    if ((lType == rt_FLOAT) || (lType == rt_DOUBLE))
    {
        val = (lType == rt_DOUBLE)?*((double*)Val):((double)(*((float*)Val)));

        #ifdef _UNIX_
            return !finite(val);
        #else
            return !_finite(val);
        #endif
    }
    return 0;
}

int dbg_isNAN_Val(VALUE *Val, DvmType lType)
{
    double val;

    if ((lType == rt_FLOAT) || (lType == rt_DOUBLE))
    {
        val = (lType == rt_DOUBLE)?(Val->_double):(Val->_float);

        #ifdef _UNIX_
            return !finite(val);
        #else
            return !_finite(val);
        #endif
    }
    return 0;
}

/* Platform-independent file compare */

int strFileCmp(char *s1, char *s2)
{
    if ( TraceOptions.SRCLocCompareMode )
    {
        char *p1, *p2;

        if ( TraceOptions.SRCLocCompareMode == 3 )
            return 0; /* ignore all differences */

        /* otherwise ignore path */
        p1 = strrchr(s1, '\\');
        p2 = strrchr(s1, '/');
        p1 = (p1>p2)?p1:p2;
        if (p1) s1 = p1;

        p1 = strrchr(s2, '\\');
        p2 = strrchr(s2, '/');
        p1 = (p1>p2)?p1:p2;
        if (p1) s2 = p1;
    }

    if(s1 == s2)
        return(0);
    while((*s1 == *s2) || (toupper(*s1) == toupper(*s2)) || ((*s1=='\\') && (*s2=='/')) || ((*s1=='/') && (*s2=='\\')) )
    {
        s2++;
        if(*s1++ == '\0')
            return(0);
    }
    return(*s1 - s2[-1]);
}

int strnFileCmp(char *s1, char *s2, size_t n)
{
    if ( TraceOptions.SRCLocCompareMode )
    {
        char *p1, *p2;
        int   i1=0,  i2=0;

        if ( TraceOptions.SRCLocCompareMode == 3 )
            return 0; /* ignore all differences */

        /* otherwise ignore path */
        p1 = strrchr(s1, '\\');
        p2 = strrchr(s1, '/');
        p1 = (p1>p2)?p1:p2;
        if (p1)
        {
            i1 = p1+1-s1;
            s1 = p1+1;
        }

        p1 = strrchr(s2, '\\');
        p2 = strrchr(s2, '/');
        p1 = (p1>p2)?p1:p2;
        if (p1)
        {
            i2 = p1+1-s2;
            s2 = p1+1;
        }

        i1 = (i1>i2)?i1:i2;
        n -= i1;
    }

    n++;
    if (s1 == s2)
        return (0);
    while (--n != 0 && ((*s1 == *s2) || (toupper(*s1) == toupper(*s2)) || ((*s1=='\\') && (*s2=='/')) || ((*s1=='/') && (*s2=='\\')) ))
    {
        s2++;
        if(*s1++ == '\0')
            return(0);
    }
    return ((n == 0) ? 0 : (*s1 - s2[-1]));
}

/*
 * perform whitespaces insensitive string compare
 * s2 - contains whitespaces !
 * s1 - does not contain
 **/
int strNameCmp(char *s1, char *s2)
{
    if ( strcmp(s1, s2) == 0)
        return 0;

    while (*s2 == ' ') s2++;            // skip leading whites

    while((*s1 == *s2))
    {
        if(*s1++ == '\0')
            return 0;
        s2++;
        while (*s2 == ' ') s2++;            // skip other whites
    }
    return -1;
}

/*
  Saves coverage entry information into global structure
*/
void SaveCoverageInfo(void)
{
    int i, res;

    DBG_ASSERT(__FILE__, __LINE__, DVM_LINE[0] > 0 && DVM_FILE[0]);

    if ( Trace.CovInfo->AccessInfoSizes[Trace.CovInfo->LastUsedFile] > 0 )
    {
        SYSTEM_RET(i, strncmp, (DVM_FILE[0],
               Trace.CovInfo->FileNames[Trace.CovInfo->LastUsedFile], MaxSourceFileName));

        if(i == 0) /* strings are equal */
        {
            if ( DVM_LINE[0] >= Trace.CovInfo->AccessInfoSizes[Trace.CovInfo->LastUsedFile] )
            {   /* save initial size (offset) */
                res = Trace.CovInfo->AccessInfoSizes[Trace.CovInfo->LastUsedFile];

                Trace.CovInfo->AccessInfoSizes[Trace.CovInfo->LastUsedFile] += REALLOC_ADDLINES;

                if ( DVM_LINE[0] >= Trace.CovInfo->AccessInfoSizes[Trace.CovInfo->LastUsedFile] )
                    Trace.CovInfo->AccessInfoSizes[Trace.CovInfo->LastUsedFile] = DVM_LINE[0] + REALLOC_ADDLINES;

                mac_realloc(Trace.CovInfo->LinesAccessInfo[Trace.CovInfo->LastUsedFile], byte *,
                        Trace.CovInfo->LinesAccessInfo[Trace.CovInfo->LastUsedFile],
                        Trace.CovInfo->AccessInfoSizes[Trace.CovInfo->LastUsedFile], 0);

                /* clean the newly allocated memory */
                memset(Trace.CovInfo->LinesAccessInfo[Trace.CovInfo->LastUsedFile] + res, 0,
                       Trace.CovInfo->AccessInfoSizes[Trace.CovInfo->LastUsedFile] - res);
            }

            Trace.CovInfo->LinesAccessInfo[Trace.CovInfo->LastUsedFile][DVM_LINE[0]-1] = 1;
            return;
        }
    }

    /* search for the right string in the array */
    for (i=0; i < MaxSourceFileCount; i++)
    {
        if ( Trace.CovInfo->AccessInfoSizes[i] == 0 )
            break;

        SYSTEM_RET(res, strncmp, (DVM_FILE[0],
               Trace.CovInfo->FileNames[i], MaxSourceFileName));

        if(res == 0)
            break;
    }

    if ( i == MaxSourceFileCount )
    { /* no place to save info */
        pprintf(3, "*** RTS err CMPTRACE: The number of source files exceeds precompiled value "
                "%d. Change the value of MaxSourceFileCount.\n", MaxSourceFileCount);
        EnableCodeCoverage = 0;
        return;
    }

    Trace.CovInfo->LastUsedFile = i; /* information will be saved in this entry */

    if ( Trace.CovInfo->AccessInfoSizes[i] == 0 )
    {   /* nothing found => copy filename */

        SYSTEM(strncpy, (Trace.CovInfo->FileNames[i], DVM_FILE[0], MaxSourceFileName));
        Trace.CovInfo->FileNames[i][MaxSourceFileName] = 0;
    }

     /* allocate memory if necessary */
    if ( Trace.CovInfo->AccessInfoSizes[i] <= DVM_LINE[0] )
    {   /* allocate memory */
        res = Trace.CovInfo->AccessInfoSizes[i]; /* save initial size (offset) */

        Trace.CovInfo->AccessInfoSizes[i] += REALLOC_ADDLINES;

        if ( DVM_LINE[0] >= Trace.CovInfo->AccessInfoSizes[i] )
            Trace.CovInfo->AccessInfoSizes[i] = DVM_LINE[0] + REALLOC_ADDLINES;

        mac_realloc(Trace.CovInfo->LinesAccessInfo[i], byte *, Trace.CovInfo->LinesAccessInfo[i],
                Trace.CovInfo->AccessInfoSizes[i], 0);

        /* clean the newly allocated memory */
        memset(Trace.CovInfo->LinesAccessInfo[i] + res, 0, Trace.CovInfo->AccessInfoSizes[i]-res);
    }

    /* update the entry */
    Trace.CovInfo->LinesAccessInfo[i][DVM_LINE[0]-1] = 1;
}

/* Frees the memory */
void coverage_Done(void)
{
    int i;

    for (i=0; (i < MaxSourceFileCount) && (Trace.CovInfo->AccessInfoSizes[i] != 0); i++)
        free(Trace.CovInfo->LinesAccessInfo[i]);

    mac_free(&Trace.CovInfo);

    Trace.CovInfo = NULL;
}

/*
  Compares blocks. Returns 0 if blocks are identical and 1 otherwise.
*/
byte block_Compare(s_BLOCK *A, s_BLOCK *B)
{
    int i;

    if (A->Rank != B->Rank) return 1;

    for(i=0; i < A->Rank; i++)
       if ( A->Set[i].Lower != B->Set[i].Lower || A->Set[i].Upper != B->Set[i].Upper ||
            A->Set[i].Step  != B->Set[i].Step )
            return 1;
    return 0;
}

byte iters_Compare(ITERBLOCK *A, ITERBLOCK *B)
{
    int i;

    if (A->Rank != B->Rank) return 1;

    for(i=0; i < A->Rank; i++)
       if ( A->Set[i].Lower != B->Set[i].Lower || A->Set[i].Upper != B->Set[i].Upper ||
            A->Set[i].Step  != B->Set[i].Step )
            return 1;
    return 0;
}

/* This function intersects two blocks of iterations and saves the result into
 * the first block (A). Returns 1 if there is non empty intersection of the
 * given blocks and 0 otherwise. If there is no intersection between blocks function
 * also assigns 0 to the rank field of the block A. In case of block A was changed
 * function assigns corrected = 1, otherwise 0.
 */
byte iters_intersect_correct(ITERBLOCK *A, ITERBLOCK *B, byte* corrected)
{
    int i;
    *corrected = 0;

    if(A->Rank == B->Rank)
    {
        for(i=0; i < A->Rank; i++)
        {
            if ( A->Set[i].Step > 0 )
            {
                if (A->Set[i].Lower < B->Set[i].Lower)
                { /* find maximum Lower */
                    *corrected = 1;
                    A->Set[i].Lower = B->Set[i].Lower;
                }
                if (A->Set[i].Upper > B->Set[i].Upper)
                { /* find minimum Upper */
                    *corrected = 1;
                    A->Set[i].Upper = B->Set[i].Upper;
                }
                if ( A->Set[i].Upper - A->Set[i].Lower < 0 )
                { /* intersection is empty */
                    A->Rank = 0;
                    return 0;
                }
            }
            else /* negative step */
            {    /* the first case but vice versa */
                if (A->Set[i].Upper < B->Set[i].Upper)
                { /* find maximum Upper */
                    *corrected = 1;
                    A->Set[i].Upper = B->Set[i].Upper;
                }
                if (A->Set[i].Lower > B->Set[i].Lower)
                { /* find minimum Lower */
                    *corrected = 1;
                    A->Set[i].Lower = B->Set[i].Lower;
                }
                if ( A->Set[i].Lower - A->Set[i].Upper < 0 )
                { /* intersection is empty */
                    A->Rank = 0;
                    return 0;
                }
            }
        }
    }
    else
        DBG_ASSERT(__FILE__, __LINE__, 0);

    return 1;
}

/* This function intersects ITERBLOCK A and s_BLOCK B and saves the result into
 * first parameter (Res). Returns 1 if there is non empty intersection of blocks A
 * and B and 0 otherwise.
 */
byte it_bl_intersect_res(ITERBLOCK *Res, ITERBLOCK *A, s_BLOCK *B)
{
    int i;

    if(A->Rank == B->Rank)
    {
        Res->Rank = A->Rank;
        for(i=0; i < A->Rank; i++)
        {
            if ( A->Set[i].Step > 0 )
            {
                /* find maximum Lower */
                Res->Set[i].Lower = dvm_max(A->Set[i].Lower , B->Set[i].Lower);
                /* find minimum Upper */
                Res->Set[i].Upper = dvm_min(A->Set[i].Upper , B->Set[i].Upper);

                if ( Res->Set[i].Upper - Res->Set[i].Lower < 0 )
                 /* intersection is empty */
                    return 0;
            }
            else /* negative step */
            {   /* the first case but vice versa */
                /* find maximum Upper */
                Res->Set[i].Upper = dvm_max(A->Set[i].Upper , B->Set[i].Upper);

                /* find minimum Lower */
                Res->Set[i].Lower = dvm_min(A->Set[i].Lower , B->Set[i].Lower);

                if ( Res->Set[i].Lower - Res->Set[i].Upper < 0 )
                /* intersection is empty */
                    return 0;
            }
        }
    }
    else
        DBG_ASSERT(__FILE__, __LINE__, 0);

    return 1;
}

/* This function intersects two blocks of iterations.
 * Returns 1 if there is non empty intersection of the
 * given blocks and 0 otherwise.
 * */
byte iters_intersect_test(ITERBLOCK *A, ITERBLOCK *B)
{
    int i;
    DvmType Lower, Upper;

    if(A->Rank == B->Rank)
    {
        for(i=0; i < A->Rank; i++)
        {
            if ( A->Set[i].Step > 0 )
            {
                if (A->Set[i].Lower < B->Set[i].Lower)
                 /* find maximum Lower */
                     Lower = B->Set[i].Lower;
                else Lower = A->Set[i].Lower;

                if (A->Set[i].Upper > B->Set[i].Upper)
                 /* find minimum Upper */
                     Upper = B->Set[i].Upper;
                else Upper = A->Set[i].Upper;
                if ( Upper - Lower < 0 )
                 /* intersection is empty */
                    return 0;

            }
            else /* negative step */
            {    /* the first case but vice versa */
                if (A->Set[i].Upper < B->Set[i].Upper)
                 /* find maximum Upper */
                    Upper = B->Set[i].Upper;
                else
                    Upper = A->Set[i].Upper;

                if (A->Set[i].Lower > B->Set[i].Lower)
                 /* find minimum Lower */
                    Lower = B->Set[i].Lower;
                else
                    Lower = A->Set[i].Lower;

                if ( Lower - Upper < 0 )
                 /* intersection is empty */
                    return 0;
            }
        }
    }
    else
        DBG_ASSERT(__FILE__, __LINE__, 0);

    return 1;
}


/*
 * Compares CHUNKSETs. Returns 1 if identical and 0 otherwise.
 * */
byte chunksets_Compare(CHUNKSET *A, CHUNKSET *B)
{
    int i;

    if ( A->Size != B->Size )
       return 0;

    DBG_ASSERT(__FILE__, __LINE__, A->Chunks && B->Chunks);

    for (i=1; i<A->Size; i++) /* do not compare global iter limits (were already compared) */
       if ( iters_Compare(&A->Chunks[i], &B->Chunks[i]) || (A->Chunks[i].vtr != B->Chunks[i].vtr) )
           return 0;

    return 1;
}

/* Function checks if we trace not-necessary-to-trace iterations of ParLoop PL */
void dont_trace_necessary_check(int i, DvmType *index, s_PARLOOP *PL)
{
    DvmType p, Lower, Upper, Step, LI;
    byte cond;

    if ( i < PL->Rank )
    {
        Lower = *(PL->MapList[i].InitIndexPtr);
        Upper = *(PL->MapList[i].LastIndexPtr);
        Step  = *(PL->MapList[i].StepPtr);
        cond  = Step > 0;
        for (p=Lower; cond?p<=Upper:p>=Upper; p+=Step)
        {
           index[i] = p;

           if ( i == PL->Rank-1 )
           { /* the last dimension */
               LI = Calc_LI(index);
               DBG_ASSERT(__FILE__, __LINE__, Trace.CurStruct != -1);
               if( hash2_Find(&(Trace.hIters), Trace.CurStruct, LI) == -1 )
                   continue;
               else
               {  /* we have found iteration, that should be traced... */
                    DBG_ASSERT(__FILE__, __LINE__, 0);
               }
           }
           else
               dont_trace_necessary_check(i+1, index, PL);
        }
    }
    else
        DBG_ASSERT(__FILE__, __LINE__, 0);
}

DvmType Calc_LI(DvmType *index)
{
    int i;
    DvmType Mult = 1, LI;

    DBG_ASSERT(__FILE__, __LINE__, Trace.pCurCnfgInfo);

    LI = index[Trace.pCurCnfgInfo->Rank-1];

    for (i = Trace.pCurCnfgInfo->Rank - 2; i >= 0; i--)
    {
        Mult *= Trace.pCurCnfgInfo->Current[i+1].Upper - Trace.pCurCnfgInfo->Current[i+1].Lower + 1;
        LI += Mult * index[i];
    }

    return LI;
}

#ifdef WIN32
int __cdecl sortpair_compare(const void *a, const void *b)
#else
int         sortpair_compare(const void *a, const void *b)
#endif
{
    /* avoid integer overflow just in case */
    return (((SORTPAIR*)a)->LI > ((SORTPAIR*)b)->LI) - (((SORTPAIR*)a)->LI < ((SORTPAIR*)b)->LI);
}

#ifdef WIN32
int __cdecl errinfo_compare(const void *a, const void *b)
#else
int         errinfo_compare(const void *a, const void *b)
#endif
{
    double dif;

    if (TraceOptions.ExpIsAbsolute)
        dif = (*((ERR_INFO**)b))->maxAbsAcc - (*((ERR_INFO**)a))->maxAbsAcc;
    else
        dif = (*((ERR_INFO**)b))->maxRelAcc - (*((ERR_INFO**)a))->maxRelAcc;

    /* sort max .... min value */
    return (dif<0)?-1:((dif>0)?1:0);
}

int (*qcompar)(const void *, const void *);

#ifdef WIN32
int __cdecl stablecompar(const void *a, const void *b)
#else
int         stablecompar(const void *a, const void *b)
#endif
{
        int c=(*qcompar)(*(const void **)a,*(const void **)b);
        if( c ){ return c; }
        return *(const char **)a - *(const char **)b;
}

void stablesort( void *base, size_t nel, size_t size,
                  int (*compar)(const void *, const void *) ){
  void **pbase;
  char *p;
  void *temp;
  int i,j,k;
  qcompar = compar;
  pbase = (void **) malloc(nel*sizeof(void *));
  temp = malloc(nel*size);
  for( i=0,p=(char *)base; i<nel; i++,p+=size ){
    pbase[i] = p;
  }
  qsort(pbase,nel,sizeof(void *),stablecompar);
  for( i=0,p=(char *)base; i<nel; i++,p+=size )
  {
    if( pbase[i] )
    {
      memcpy(temp,(char *)base+i*size,size);
      j = i;
      while( (k=((char *)pbase[j]-(char *)base)/size) != i )
      {
        memcpy((char *)base+j*size,pbase[j],size);
        pbase[j]=0;
        j = k;
      }
      memcpy((char *)base+j*size,temp,size);
      pbase[j] = 0;
    }
  }
  free(pbase);
  free(temp);
}

int err_record_compare(const void *a, const void *b)
{
    /* avoid integer overflow just in case */
    return ((*((ERROR_RECORD **)a))->ErrTime > (*((ERROR_RECORD **)b))->ErrTime) - ((*((ERROR_RECORD **)a))->ErrTime < (*((ERROR_RECORD **)b))->ErrTime);
}

/*
 * type_sign == 1 assumed. Could never be called with not allocated memory.
 * Clean appropriate segment of given CPU list.
 */
void clean_update_CPUlist(byte **pLists, byte position, int cpu_num)
{
    if ( PackedCPUSetSize == 0 )
    {
        PackedCPUSetSize = (Trace.RealCPUCount>>3) + ((Trace.RealCPUCount%8)?1:0);
    }

    DBG_ASSERT(__FILE__, __LINE__, *pLists != NULL );

    (*pLists)[position*PackedCPUSetSize + (cpu_num>>3)] = (1 << (cpu_num%8)); /* use = operator instead of |= for cleaning */
}
/*
 * type_sign == 1 for leaps and == 0 for errors. Defines the amount of memory to malloc
 *
 * position: 0 - HitCount
 *           1 - Max hit count
 *           2 - Max first dif
 *           3 - Max relative diff
 *           4 - Max relative leap
 *           5 - Max absolute diff
 *           6 - Max absolute leap
 */
void update_CPUlist(byte **pLists, byte position, byte type_sign, int cpu_num)
{
    if ( PackedCPUSetSize == 0 )
    {
        PackedCPUSetSize = (Trace.RealCPUCount>>3) + ((Trace.RealCPUCount%8)?1:0);
    }

    if ( *pLists == NULL )
    {
        dvm_AllocArray(byte, type_sign?(PackedCPUSetSize*7):PackedCPUSetSize, *pLists);
        memset(*pLists, 0, type_sign?(PackedCPUSetSize*7):PackedCPUSetSize);
    }

    DBG_ASSERT(__FILE__, __LINE__, type_sign || (position == 0));

    (*pLists)[position*PackedCPUSetSize + (cpu_num>>3)] |= (1 << (cpu_num%8));
}

char *writeCPUListString(byte *pLists, byte position)
{
    static char* buf = NULL;
    char *Pnt, empty = 1;
    int l, i, j, base;

    if ( !pLists )
        return "";

    if ( buf == NULL )
    {   /* malloc even more characters that will be required in worst case */
//        mac_malloc(buf, char *, (1+(int)ceil(log10((float)Trace.RealCPUCount+1)))*Trace.RealCPUCount + 6, 0);
        mac_malloc(buf, char *, (1+(int)ceil(log10(Trace.RealCPUCount+1.)))*Trace.RealCPUCount + 6, 0);
        DBG_ASSERT(__FILE__, __LINE__, buf != NULL);
        buf[0]=' ';
        buf[1]='(';
    }

    Pnt = buf+2;

    DBG_ASSERT(__FILE__, __LINE__, PackedCPUSetSize > 0);

    for (i=0; i < PackedCPUSetSize; i++)
    {
        if ( pLists[position*PackedCPUSetSize+i] )
        {
             base = i << 3;
             for (j=0; j<8; j++)
             {
                 if ( pLists[position*PackedCPUSetSize+i] & (1 << j) )
                 {
                     SYSTEM_RET( l, sprintf, ( Pnt, "%d,", base + j ) );
                     Pnt += l;
                     empty = 0;
                 }
             }
        }
    }

    if ( empty )
    {
        return " ()";
    }
    else
    {
        SYSTEM_RET( l, sprintf, ( Pnt-1, ")" ) ); /* remove the last ',' */
        return buf;
    }
}

t_tracer_time dvm_get_time(void)
{
#ifdef _MPI_PROF_TRAN_
    return tracer_get_time_();
#else
    return (t_tracer_time)clock();
#endif
}

#ifdef _MPI_PROF_TRAN_
int errtime_compare(const void *a, const void *b)
{
    /* sort min .... max value */
    return ( ((ERR_TIME*)a)->err_time > ((ERR_TIME*)b)->err_time) - (((ERR_TIME*)a)->err_time  < ((ERR_TIME*)b)->err_time);
}
#endif

void mkStack(stack *s, int n)
{
    s->top = 0;
    s->size = n;
    s->items = (LOOP_INFO *) malloc(sizeof (LOOP_INFO) * n);
    DBG_ASSERT(__FILE__, __LINE__, s->items );
    return;
}

int isEmpty(stack *s) { return s->top == 0; }

int isFull(stack *s) { return s->top == s->size; }

void pushStack(stack *s, DvmType No, int VTR, byte context, unsigned int Line)
{
//    char * ptr;
//    int i;

    /*
    ** Stack full? Grow it.
    */

    if (s->top == s->size)
    {
        s->size += 40;
        s->items = (LOOP_INFO *) realloc(s->items, sizeof (LOOP_INFO) * s->size);
        if ( s->items == NULL )
            epprintf(MultiProcErrReg1, __FILE__,__LINE__, "*** RTS err: No memory.\n");
    }

/*    ptr = (char *) &(s->items[s->top]);

    for (i=0;i<sizeof(LOOP_INFO); i++)
            { *ptr=0; ptr++; }*/

    s->items[s->top].No = No;
    s->items[s->top].Init = 0;
    s->items[s->top].HasItems = 0;
    s->items[s->top].OldVtr = VTR;
    s->items[s->top].Line = Line;

    if ( s->top != 0 )
        s->items[s->top].Propagate = s->items[s->top-1].Propagate || context;
    else
        s->items[s->top].Propagate = context;

//    s->items[s->top].LocParts = NULL;
//    s->items[s->top].DoplRes.Rank = 0;
//    s->items[s->top].BordersSetBack = 0;

    s->top++;
    return;
}

/*LOOP_INFO *stack_GetNew(stack *s)
{
    if (s->top == s->size)
    {
        s->size += 40;
        s->items = (LOOP_INFO *) realloc(s->items, sizeof (LOOP_INFO) * s->size);
        DBG_ASSERT(__FILE__, __LINE__, s->items );
    }

    s->top++;
    return &s->items[s->top - 1];
} */

LOOP_INFO *stackLast(stack *s)
{
    DBG_ASSERT(__FILE__, __LINE__, s->top > 0);
    return &(s->items[s->top-1]);
}

int stackRemoveLast(stack *s)
{
//    char * ptr;
//    int i;

    s->top--;

    return s->items[s->top].OldVtr;
    /* for debugging purposes */
/*    ptr = (char *) &(s->items[s->top]);

    for (i=0;i<sizeof(LOOP_INFO); i++)
            { *ptr=0; ptr++; }*/
}

void stack_destroy(stack *s)
{
    if ( s->items ) free (s->items);
}

/*int popStack(stack *s)
{
    DBG_ASSERT(__FILE__, __LINE__, s->top > 0);
    return s->items[--s->top];
}*/

/*
 * Dummy function to be called
 * */
void dummy_var(char* File, UDvmType Line, char* Operand, enum_TraceType iType,
               DvmType Type, void* Value, byte Reduct, void* pArrBase)
{
    return ;
}

/*
 * Functions for saving array purpose
 */

void save_array(dvm_ARRAY_INFO *pArr, char *point)
{
    DVMFILE    *hFile;
    SysHandle  *ArrayHandlePtr = (SysHandle *)pArr->pAddr;
    UDvmType res = 0;

    if ( ArrayHandlePtr == NULL )
        return;

    Trace.ArrWasSaved = 1;

    /* open file for writing */
    hFile = ( RTL_CALL, dvm_fopen(TraceOptions.SaveArrayFilename, "wb") );

    if ( hFile == NULL )
    {
        pprintf(3, "*** RTS err CMPTRACE: Can't open file %s for writing.\n", TraceOptions.SaveArrayFilename);
    }
    else
    {
        if(pArr->bIsDistr)
        {
            if ( ArrayHandlePtr->Type != sht_DisArray ||
                    ArrayHandlePtr != TstDVMArray((void *)ArrayHandlePtr->HeaderPtr))
            {
                pprintf(3, "*** RTS err CMPTRACE: Array (\"%s\", \"%s\", %ld, %d) cannot be saved at "
                        "the point %s.\n", pArr->szOperand, pArr->szFile, pArr->ulLine, pArr->iNumber, point);
            }
            else
                res = (RTL_CALL, dvm_dfwrite((DvmType*)ArrayHandlePtr->HeaderPtr, 0, hFile));
        }
        else
        {
            size_t typesize;

            switch(pArr->lElemType)
            {
                case rt_CHAR:           typesize = sizeof(char);
                                        break;
                case rt_INT:
                case rt_LOGICAL:        typesize = sizeof(int);
                                        break;
                case rt_LONG:           typesize = sizeof(long);
                                        break;
                case rt_LLONG:          typesize = sizeof(long long);
                                        break;
                case rt_FLOAT:          typesize = sizeof(float);
                                        break;
                case rt_DOUBLE:         typesize = sizeof(double);
                                        break;
                case rt_FLOAT_COMPLEX:  typesize = 2*sizeof(float);
                                        break;
                case rt_DOUBLE_COMPLEX: typesize = 2*sizeof(double);
                                        break;
            }

            res = (UDvmType)dvm_fwrite(pArr->pAddr, typesize, (size_t)pArr->lLineSize, hFile);
        }

        if ( res <= 0 )
        {
            pprintf(3, "*** RTS err CMPTRACE: An error occurred during saving "
                    "array (\"%s\", \"%s\", %ld, %ld) to file %s at the point %s.\n", pArr->szOperand, pArr->szFile,
                    pArr->ulLine, pArr->iNumber, TraceOptions.SaveArrayFilename, point);
        }
        else
        {
            pprintf(3, "*** RTS info CMPTRACE: Array (\"%s\", \"%s\", %ld, %ld) "
                    "has been successfully saved to file %s at the point %s.\n", pArr->szOperand, pArr->szFile,
                    pArr->ulLine, pArr->iNumber, TraceOptions.SaveArrayFilename, point);
        }

        (RTL_CALL, dvm_fclose(hFile));
    }
}

void save_array_with_ID( char *arr_id, char *point )
{
    dvm_ARRAY_INFO  *pInfo;
    char            szOperand[MaxOperand + 1];
    char            szFile[MaxSourceFileName + 1];
    char            tmpArrayID[MaxArrayID+1];
    uLLng            ulLine;
    int             iNumber;
    int             i, found = 0;
    char            *pnt, *tmp;

    SYSTEM(strcpy, (tmpArrayID, arr_id))

    /* delete unnecessary separators */
    pnt = tmp = tmpArrayID;
    while( *tmp )
    {
        while( ( *tmp == ' ' ) || ( *tmp == '\t' ) ) tmp++;
        *(pnt++) = *(tmp++);
    }
    *pnt = 0;

    pnt = trc_rd_split( tmpArrayID, "@,@,!,!", szOperand, szFile, &ulLine, &iNumber );

    if ( pnt != NULL && pnt != tmpArrayID )
    {
        /* find appropriate array */
        for (i = 0; i < table_Count(&Trace.tArrays); i++)
        {
            pInfo = table_At(dvm_ARRAY_INFO, &Trace.tArrays, i);

            if (!pInfo->bIsRemoteBuffer && pInfo->ulLine == ulLine && pInfo->iNumber == iNumber &&
                    strcmp(pInfo->szOperand, szOperand) == 0 && strFileCmp(pInfo->szFile, szFile) == 0)
            {
                found = 1;

                if ( pInfo->pAddr != NULL )
                {
                    /* save array elements to the file */
                    save_array( pInfo, point );
                }
                else
                {
                    pprintf(3, "*** RTS err CMPTRACE: Array (\"%s\", \"%s\", %ld, %d) "
                            "cannot be saved at the point %s.\n", pInfo->szOperand, pInfo->szFile,
                            pInfo->ulLine, pInfo->iNumber, point);
                }

                break;
            }
        }

        if ( !found )
        {
            /* unknown array - output error info */
            pprintf(3, "*** RTS err CMPTRACE: Array (\"%s\", \"%s\", %ld, %d) "
                    "was not found at the point %s.\n", szOperand, szFile, ulLine, iNumber, point);
        }
    }
    else
    {
        pprintf(3, "*** RTS err CMPTRACE: Invalid array ID specification %s.\n", arr_id);
    }
}

/*
 * Functions for iterations management purposes
 */

byte was_not_traced(DvmType *index)
{
    int i;
    DvmType Mult, LI;

    if( Trace.CurStruct == -1 )
    {
        error_CmpTraceExt(-1, DVM_FILE[0], DVM_LINE[0], ERR_CMP_NO_CURSTRUCT);
        Trace.CurTraceRecord = 0;
        return 1;
    }

    Mult = 1;
    LI = index[Trace.pCurCnfgInfo->Rank-1];

    for (i = Trace.pCurCnfgInfo->Rank - 2; i >= 0; i--)
    {
        Mult *= Trace.pCurCnfgInfo->Current[i+1].Upper - Trace.pCurCnfgInfo->Current[i+1].Lower + 1;
        LI += Mult * index[i];
    }

    if( hash2_Find(&(Trace.hIters), Trace.CurStruct, LI) == -1 )
        return 1;
    else
        return 0;
}

byte out_of_bounds(DvmType *index)
{
    int i;
    byte broken;
    STRUCT_INFO   *pInfo;

    pInfo = Trace.pCurCnfgInfo;

    if ( pInfo->Type == 0 )
    {   /* serial loop */
        DBG_ASSERT(__FILE__, __LINE__, pInfo->Rank == 1 );

        if ( TraceOptions.Irep_left != -1 )
        {
            if ( pInfo->TracedIterNum >= TraceOptions.Irep_left )
                return 1;
            else
                pInfo->TracedIterNum++;
        }

        return 0;
    }

    /* parallel loop && not task region */
    if ( TraceOptions.Ig_left != -1 )
    {
        broken = 0;
        for (i = 0; i < pInfo->Rank; i++)
        {
            if ( pInfo->Current[i].Lower == MAXLONG || pInfo->Current[i].Step == MAXLONG)
            {
                /* we don't have full info, trace this iteration */
                return 0;
            }

            if ( index[i] >= pInfo->Current[i].Lower + TraceOptions.Ig_left*pInfo->Current[i].Step )
            {
                broken = 1;
                break;
            }
        }

        if ( !broken )
                return 0;
    }

    if ( TraceOptions.Ig_right != -1 )
    {
        broken = 0;
        for (i = 0; i < pInfo->Rank; i++)
        {
            if ( pInfo->Current[i].Upper == MAXLONG || pInfo->Current[i].Step == MAXLONG )
            {
                /* we don't have full info, trace this iteration */
                return 0;
            }

            if ( index[i] <= pInfo->Current[i].Upper - TraceOptions.Ig_right*pInfo->Current[i].Step )
            {
                broken = 1;
                break;
            }
        }
        if ( !broken )
                return 0;
    }

    /* parallel loop */
    if ( TraceOptions.Iloc_left != -1 )
    {
        if ( TraceOptions.TraceMode == mode_COMPARETRACE )
            return was_not_traced( index );
        else
        {
            if ( pInfo->CurLocal[0].Step != MAXLONG )
            {
                broken = 0;

                for (i = 0; i < pInfo->Rank; i++)
                {
                    if ( index[i] >= pInfo->CurLocal[i].Lower + TraceOptions.Iloc_left*pInfo->CurLocal[i].Step )
                    {
                        broken = 1;
                        break;
                    }
                }

                if ( !broken )
                        return 0;
            }
        }
    }

    if ( TraceOptions.Iloc_right != -1 )
    {
        if ( TraceOptions.TraceMode == mode_COMPARETRACE )
            return was_not_traced( index );
        else
        {
            if ( pInfo->CurLocal[0].Step != MAXLONG )
            {
                for (i = 0; i < pInfo->Rank; i++)
                {
                    if ( pInfo->CurLocal[i].Upper == MAXLONG || pInfo->CurLocal[i].Step == MAXLONG )
                    {
                        /* we don't have full info, trace this iteration */
                        return 0;
                    }

                    if ( index[i] <= pInfo->CurLocal[i].Upper - TraceOptions.Iloc_right*pInfo->CurLocal[i].Step )
                    {
                        broken = 1;
                        break;
                    }
                }
                if ( !broken )
                        return 0;
            }
        }
    }

    return 1;
}

/*
 * Functions for managing LIST structure information
 */

void list_init(LIST *list)
{
    list->collect = 0;
    list->cursize = list->maxsize = 0;
    list->body = NULL;
}

void list_clean(LIST *list)
{
    if ( list->body != NULL && list->maxsize > 0 )
        mac_free( &(list->body) );

    list_init( list );
}

void updateListA( LIST *list, void *pArrBase, byte accType )
{
    int i;

    for ( i=0; i < list->cursize; i++ )
    {
        if ( list->body[i].pAddr == pArrBase )
        {
            list->body[i].accType |= accType;
            return;
        }
    }

    /* we didn't find the array in our list, need to append it */
    if ( list->maxsize == list->cursize )
    {   /* necessary to reallocate the memory */
        mac_realloc(list->body, CHECKSUM *, list->body, (list->maxsize + REALLOC_COUNT)*sizeof(CHECKSUM), 0);
        memset(list->body + list->maxsize, 0, REALLOC_COUNT*sizeof(CHECKSUM));
        list->maxsize += REALLOC_COUNT;
    }

    list->body[list->cursize].pAddr = pArrBase;
    list->body[list->cursize].accType = accType;
    list->body[list->cursize].lArrNo = -1;
    list->cursize++;
    return;
}

void updateListN(LIST *list, DvmType lArrNo, byte accType)
{
    int i;

    for ( i=0; i < list->cursize; i++ )
    {
        if ( list->body[i].lArrNo == lArrNo )
        {
            list->body[i].accType |= accType;
            return;
        }
    }

    /* we didn't find the array in our list, need to append it */
    if ( list->maxsize == list->cursize )
    {   /* necessary to reallocate the memory */
        mac_realloc(list->body, CHECKSUM *, list->body, (list->maxsize + REALLOC_COUNT)*sizeof(CHECKSUM), 0);
        memset(list->body + list->maxsize, 0, REALLOC_COUNT*sizeof(CHECKSUM));
        list->maxsize += REALLOC_COUNT;
    }

    list->body[list->cursize].lArrNo = lArrNo;
    list->body[list->cursize].accType = accType;
    list->cursize++;
    return;
}

void lists_uucopy(LIST *dest, LIST *src)
{
    int     i, j;
    DvmType    lArrayNo;
    static  byte printed = 0;

    for ( i = 0; i < src->cursize; i++ )
    {
        DBG_ASSERT(__FILE__, __LINE__, src->body[i].pAddr );

        lArrayNo = hash1_Find(&Trace.hArrayPointers, src->body[i].pAddr);

        if ( lArrayNo == -1 )
        {
            if ( !printed )
            {
                pprintf(3, "*** RTS warning: Found access to array without registration.\n");
                printed = 1;
            }
            continue;
        }

        if ((table_At(dvm_ARRAY_INFO, &Trace.tArrays, lArrayNo))->bIsRemoteBuffer != 0) continue;

        /* look through the dest, it may already contain this array */
        for ( j = 0; j < dest->cursize; j++ )
        {
            if ( (dest->body[j].pAddr == src->body[i].pAddr) || (dest->body[j].lArrNo == lArrayNo) )
                    /* lArrayNo comparison is necessary for the realign case */
            {
                dest->body[j].accType |= src->body[i].accType;
                break;
            }
        }

        if ( j == dest->cursize ) /* we didn't find match */
        {
            if ( dest->cursize == dest->maxsize )
            {   /* necessary to reallocate the memory */
                mac_realloc(dest->body, CHECKSUM *, dest->body, (dest->maxsize + REALLOC_COUNT)*sizeof(CHECKSUM), 0);
                memset(dest->body + dest->maxsize, 0, REALLOC_COUNT*sizeof(CHECKSUM));
                dest->maxsize += REALLOC_COUNT;
            }

            /* fill in the structure */
            dest->body[dest->cursize].pAddr = src->body[i].pAddr;
            dest->body[dest->cursize].accType = src->body[i].accType;
            dest->body[dest->cursize].lArrNo = lArrayNo;
            dest->cursize++;
        }
    }
}

void list_remove_reduction(LIST *list)
{
    int i, cur_an_pos, zero_num = 0;

    for ( i = 0; i < list->cursize; i++ )
    {
        if ( trcreduct_Find(list->body[i].pAddr) )
        { /* this array is a reduction array. Need to delete it from list */
          /* mark it with NULL and clean the list later */
             list->body[i].pAddr = NULL;
             zero_num++;
        }
    }

    if ( zero_num == 0 )
        return ;

    if ( zero_num == list->cursize )
    {
        list->cursize = 0;
        return ;
    }

    /* there are zeros and non-zeros */
    cur_an_pos = 0;

    for ( i = 0; i < list->cursize-zero_num; i++ )
    {
        while ( (cur_an_pos < list->cursize) && list->body[cur_an_pos].pAddr == NULL ) cur_an_pos++;
        DBG_ASSERT(__FILE__, __LINE__, cur_an_pos < list->cursize );
        list->body[i] = list->body[cur_an_pos];
        cur_an_pos++;
    }

    list->cursize -= zero_num;
}

/*
 * Function gathers information about accesses to arrays from all CPUs (optional) and builds a unique arrays list
 */
int list_build_checksums( LIST *list, CHECKSUM **ppChecksums, byte f_gather )
{
    int  i, j, count = 0, curCS = 0, *IntPtr;
    int  *displs=NULL, offset, finCount=0, charCount;
    char *ArrIds=NULL, *pCurArr=NULL, *allArrIDs=NULL;
    char tmp[ARR_TXTID_LENGTH]; /* temporary store */
    dvm_ARRAY_INFO* pArr;
    static byte printed = 0;

    /* Attention: When array's deletion will be implemented we must
    * check whether array is deleted or not (see documentation for details) */

    *ppChecksums = NULL;

    if (TraceOptions.ChecksumMode == 1)
    {
        for ( i = 0; i < list->cursize; i++ )
        {
            if ( list->body[i].accType & 2 ) count++;
        }
    }
    else count = list->cursize;

    if ( count > 0 )
    {
        if ( f_gather )
        {
            mac_malloc(ArrIds, char *, count*ARR_TXTID_LENGTH, 0);
            pCurArr = ArrIds;
        }
        else
            mac_calloc(*ppChecksums, CHECKSUM *, count, sizeof(CHECKSUM), 0);

       /* curCS = 0; is already */

        for ( i = 0; i < list->cursize; i++ )
        {
            if ( (list->body[i].accType & 2) || TraceOptions.ChecksumMode != 1 )
            {
                if (list->body[i].pAddr)
                {
                    list->body[i].lArrNo = hash1_Find(&Trace.hArrayPointers, list->body[i].pAddr);

                    if ( list->body[i].lArrNo == -1 )
                    {
                        if ( !printed )
                        {
                            pprintf(3, "*** RTS warning: Found access to array without registration.\n");
                            printed = 1;
                        }
                        continue;
                    }
                }

                pArr = table_At(dvm_ARRAY_INFO, &(Trace.tArrays), list->body[i].lArrNo);

                if (pArr->bIsRemoteBuffer ||
                    (TraceOptions.ChecksumDisarrOnly && !pArr->bIsDistr))
                    continue;

                if (pArr->pAddr)
                {
                    if ( f_gather )
                    {
                         sprintf(pCurArr, "%s*%d*%ld*%s", pArr->szOperand,
                                   pArr->iNumber, pArr->ulLine, pArr->szFile);
                         DBG_ASSERT(__FILE__, __LINE__, strlen(pCurArr) < ARR_TXTID_LENGTH);
                         pCurArr += ARR_TXTID_LENGTH;
                    }
                    else
                    {
                        (*ppChecksums)[curCS].pInfo = pArr;
                        (*ppChecksums)[curCS].lArrNo = list->body[i].lArrNo;
                        (*ppChecksums)[curCS].accType = list->body[i].accType;
                    }
                    curCS++;
                }
                else /* it's the case where array is not yet created, but information about it is read from the loops file */
                    continue;
            }
        }
    }

    if ( f_gather )
    {
        /* exchange arrays information between CPUs */
        dvm_AllocArray(int, DVM_VMS->ProcCount, IntPtr);

        i = curCS * ARR_TXTID_LENGTH;

        SYSTEM(MPI_Gather, (&i, 1, MPI_INT, IntPtr, 1, MPI_INT, DVM_VMS->VMSCentralProc, DVM_VMS->PS_MPI_COMM))

        if (MPS_CurrentProc == DVM_VMS->CentralProc)
        {
            dvm_AllocArray(int, DVM_VMS->ProcCount, displs);
            displs[0] = 0;
            for (i=1; i<DVM_VMS->ProcCount; ++i)
            {
                displs[i] = displs[i-1]+IntPtr[i-1];
            }

            dvm_AllocArray(char, displs[DVM_VMS->ProcCount-1]+IntPtr[DVM_VMS->ProcCount-1], allArrIDs);
        }
    /*    else
        {
            displs     = NULL;
            allArrIDs  = NULL;
        }  is already */

        SYSTEM(MPI_Gatherv, ( ArrIds, curCS*ARR_TXTID_LENGTH, MPI_CHAR, allArrIDs, IntPtr,
                                displs, MPI_CHAR, DVM_VMS->VMSCentralProc, DVM_VMS->PS_MPI_COMM ))

        mac_free(&ArrIds);

        if(MPS_CurrentProc == DVM_VMS->CentralProc)
        {
            /* The current processor is central */
            /* create final arrays list */
            charCount = displs[DVM_VMS->ProcCount-1]+IntPtr[DVM_VMS->ProcCount-1];

            offset = 0;

            while ( offset < charCount )
            {
                if ( allArrIDs[offset] != 0 )
                {  /* remove all copies in the tail of the array */

                    finCount++;

                    for (i=offset+ARR_TXTID_LENGTH; i < charCount; i+=ARR_TXTID_LENGTH)
                    {
                        if ( allArrIDs[i] && !strcmp(allArrIDs+offset, allArrIDs+i) )
                            allArrIDs[i]=0; /* remove the copy of current arrId in the tail */
                    }
                }
                else
                { /* check if there are non-empty strings in the tail */
                    for (i=offset+ARR_TXTID_LENGTH; i < charCount; i+=ARR_TXTID_LENGTH)
                        if ( allArrIDs[i] )
                            break;

                    if ( i != charCount )
                    {   /* copy arrayId to the next position and continue search */
                            memcpy(allArrIDs + offset, allArrIDs+i, ARR_TXTID_LENGTH);
                            allArrIDs[i]=0;
                            continue; /* do not update offset value !! */
                    }
                    else
                    {   /* no more arrays, exit main loop */
                            break;
                    }
                }
                offset += ARR_TXTID_LENGTH;
            }

            dvm_FreeArray(displs);
        }

        dvm_FreeArray(IntPtr);

        ( RTL_CALL, rtl_BroadCast(&finCount, 1, sizeof(int), DVM_VMS->CentralProc, NULL) );

        if ( finCount == 0 )
        {
            /* free the memory */
            if(MPS_CurrentProc == DVM_VMS->CentralProc)
                dvm_FreeArray(allArrIDs);
            return 0;
        }

        charCount = finCount*ARR_TXTID_LENGTH;

        if(MPS_CurrentProc != DVM_VMS->CentralProc)
        {  /* alloc array for final checksums */
            dvm_AllocArray(char, finCount*ARR_TXTID_LENGTH, allArrIDs);
        }

        ( RTL_CALL, rtl_BroadCast(allArrIDs, charCount, 1, DVM_VMS->CentralProc, NULL) );

        /* Now every CPU has arrays list. Need to translate it to user structures */
        mac_calloc(*ppChecksums, CHECKSUM *, finCount, sizeof(CHECKSUM), 0);
        memset(*ppChecksums, 0, finCount*sizeof(CHECKSUM));

        for ( i = 0; i < list->cursize; i++ )
        {
            if ( ((list->body[i].accType & 2) || TraceOptions.ChecksumMode != 1)
                    && list->body[i].pAddr && (list->body[i].lArrNo != -1) )
            {
                pArr = table_At(dvm_ARRAY_INFO, &(Trace.tArrays), list->body[i].lArrNo);

                if (pArr->bIsRemoteBuffer ||
                    (TraceOptions.ChecksumDisarrOnly && !pArr->bIsDistr))
                    continue;

                if (pArr->pAddr)
                {
                    sprintf(tmp, "%s*%d*%ld*%s", pArr->szOperand,
                            pArr->iNumber, pArr->ulLine, pArr->szFile);

                    for (offset=0; offset < charCount; offset+=ARR_TXTID_LENGTH)
                         if ( !strcmp(tmp, allArrIDs+offset) )
                         {
                             (*ppChecksums)[offset/ARR_TXTID_LENGTH].pInfo = pArr;
                             (*ppChecksums)[offset/ARR_TXTID_LENGTH].lArrNo = list->body[i].lArrNo;
                             (*ppChecksums)[offset/ARR_TXTID_LENGTH].accType = list->body[i].accType;
                             break;
                         }

                    DBG_ASSERT(__FILE__, __LINE__, offset < charCount); /* otherwise error:
                                                                         * array from this CPU was not included in the list */
                }
                else /* it's the case where array is not yet created, but information about it is read from the loops file */
                    continue;
            }
        }

        /* Now check if all arrays were found (case when there were no accesses on this CPU to some arrays, empty local part) */
        for (j=0;j<finCount;j++)
        {
             if ( !(*ppChecksums)[j].pInfo )
             {  /* need to find this array in arrays table */

                for (i = table_Count(&Trace.tArrays)-1; i>=0; i--)
                {
                    pArr = table_At(dvm_ARRAY_INFO, &Trace.tArrays, i);

                    sprintf(tmp, "%s*%d*%ld*%s", pArr->szOperand,
                            pArr->iNumber, pArr->ulLine, pArr->szFile);

                    if ( !strcmp(tmp, allArrIDs+j*ARR_TXTID_LENGTH) )
                    {
                        (*ppChecksums)[j].pInfo = pArr;
                        (*ppChecksums)[j].lArrNo = i;
                        (*ppChecksums)[j].accType = 0;
                        break;
                    }
                }

                if ( i < 0 )
                {   /* error: array is not registered on this CPU !!! */
                    EnableTrace = 0;
                    epprintf(MultiProcErrReg1,__FILE__,__LINE__,
                        "*** RTS err : Checksum computation: Required array is not registered on current CPU.\n");
                }
             }
        }

        dvm_FreeArray(allArrIDs);
        return finCount;
    }/* if (f_gather) ... */
    else
    {
        if ( curCS == 0 )
        {
            mac_free(ppChecksums);
            *ppChecksums = NULL;
            return 0;
        }
        return curCS;
    }

}

size_t list_to_header_string(char *str, LIST *list)
{
    size_t  size = 0;
    int i;
    dvm_ARRAY_INFO* pArr;

    for ( i = 0; i < list->cursize; i++ )
    {
            pArr = table_At(dvm_ARRAY_INFO, &Trace.tArrays, list->body[i].lArrNo);

            size += sprintf(str + size, " (\"%s\", \"%s\", %ld, %d, \"%s\")", pArr->szOperand,
                            pArr->szFile, pArr->ulLine, pArr->iNumber, (list->body[i].accType==1)?"r":(list->body[i].accType==2)?"w":"rw");
    }

    return size;
}

/*
 * Functions for managing NUMBER structure information
 */

void num_init(NUMBER *num)
{
    num->selector = 0;
    num->iter = -1;
    num->main.val = 0;
    num->main.ref = 'E';
}

char* to_string(NUMBER *num, char *buffer)  /* assume that size of buffer is appropriate */
{
    int res;

    res = sprintf(buffer, "%ld.", num->main.val);

    if ( num->selector == 0 )
    {
        /* main level */
        sprintf(buffer + res, "%c", num->main.ref);
    }
    else
    {
        /* nested level */
        sprintf(buffer + res, "%ld.%ld.%c", num->iter, num->nested.val, num->nested.ref);
    }

    return buffer;
}

byte parse_number(char *str, NUMBER *num)
{
    int result;
    char *p1, *p2, tmp[MAX_NUMBER_LENGTH];

    result = sscanf(str, "%ld.%c", &(num->main.val), &(num->main.ref));

    if ( result < 2 )                  /* syntax error */
        return 0;

    num->main.ref = (char) toupper(num->main.ref);

    if ( num->main.ref != 'B' && num->main.ref != 'E')
    {                                  /* number from task region */
        result = sscanf(str, "%ld.%ld.%ld.%c", &(num->main.val), &(num->iter), &(num->nested.val), &(num->nested.ref));

        num->nested.ref = (char) toupper(num->nested.ref);

        if ( result < 4 || (num->nested.ref != 'B' && num->nested.ref != 'E')
                || num->main.val <= 0 || num->iter < 0 || num->nested.val <= 0)   /* syntax error */
            return 0;

        num->selector = 1;
    }
    else
    {
        if ( num->main.val <= 0 )      /* syntax error */
            return 0;
        num->selector = 0;
    }

    /* remove whitespaces in the string */
    p1 = p2 = str;
    while( *p1 )
    {
        while( ( *p1 == ' ' ) || ( *p1 == '\t' ) ) p1++;
        *(p2++) = (char) toupper(*(p1++));
    }
    *p2 = 0;

    if ( strcmp (str, to_string(num, tmp)) != 0 )
        return 0;

    return 1;
}

/*
 * compare numbers:
 * 0:  no difference;
 * 1:  (num1 - num2) > 0;
 * -1: (num1 - num2) < 0;
 * -2: incomparable, e.g. 1.0.1.E and 1.1.1.E
 */
int num_cmp(NUMBER *num1, NUMBER *num2)
{
    DvmType val;

    DBG_ASSERT(__FILE__, __LINE__, num1 && num2);

    val = num1->main.val - num2->main.val;

    if ( val > 0 ) return 1;
    if ( val < 0 ) return -1;

    if ( num1->selector != num2->selector )
    {
        if ( num1->selector == 1 )
            if ( num2->main.ref == 'E' )
                return -1;
            else return 1;
        else
            if ( num1->main.ref == 'E' )
                return 1;
            else return -1;
    }

    /* selectors are equal, we may use only one of them to differentiate situations */
    if ( num1->selector == 0 )
    {
        /* main level */
        if ( num1->main.ref == num2->main.ref ) return 0;
        if ( num1->main.ref == 'E' ) return 1;
        else return -1;
    }
    else
    {
        /* nested level */
        if ( num1->iter != num2->iter )
            /* incomparable numbers */
            return -2;

        val = num1->nested.val - num2->nested.val;

        if ( val > 0 ) return 1;
        if ( val < 0 ) return -1;

        if ( num1->nested.ref == num2->nested.ref ) return 0;
        if ( num1->nested.ref == 'E' ) return 1;
        else return -1;
    }

    DBG_ASSERT(__FILE__, __LINE__, FALSE);
}

void ploop_beg(NUMBER *num)
{
    if ( num->selector == 0 )
    {
        /* main level */
        DBG_ASSERT(__FILE__, __LINE__, num->main.ref == 'E');

        num->main.val++;
        num->main.ref = 'B';
    }
    else
    {
        /* nested level */
        DBG_ASSERT(__FILE__, __LINE__, num->nested.ref == 'E' && num->iter != -1);

        num->nested.val++;
        num->nested.ref = 'B';
    }
}

void ploop_end(NUMBER *num)
{
    if ( num->selector == 0 )
    {
        /* main level */
        DBG_ASSERT(__FILE__, __LINE__, num->main.ref == 'B');

        num->main.ref = 'E';
    }
    else
    {
        /* nested level */
        DBG_ASSERT(__FILE__, __LINE__, num->nested.ref == 'B' && num->iter != -1);

        num->nested.ref = 'E';
    }
}

void tr_beg(NUMBER *num)
{
    DBG_ASSERT(__FILE__, __LINE__, num->selector == 0 && num->main.ref == 'E');

    num->main.val++;
    num->main.ref = 'B';
    num->iter = -1;
}

void tr_it(NUMBER *num, DvmType iter)
{
    DBG_ASSERT(__FILE__, __LINE__, num->main.ref == 'B');

    num->selector = 1;
    num->iter = iter;
    num->nested.val = 0;
    num->nested.ref = 'E';
}

void tr_end(NUMBER *num)
{
    DBG_ASSERT(__FILE__, __LINE__, num->selector == 1 && num->main.ref == 'B' && num->nested.ref == 'E');

    num->selector = 0;
    num->main.ref = 'E';
    num->iter = -1;
}

void renameCArrays(char *arr_name)
{
    static char tmp0 [128];
    static char tmp1 [128];
    static char tmp2 [128];
    static char tmp3 [128];
    static char tmp4 [128];
    static char tmp5 [128];
    static char tmp6 [128];
    char *pnt;

    SYSTEM_RET( pnt, strstr, ( arr_name, "DVMda" ) );
    if (pnt == NULL)
	    SYSTEM_RET( pnt, strstr, ( arr_name, "DAElm" ) );
    if ( pnt )
    {
        pnt += 5;

        switch((*pnt))
        {
           case '1' :
             if (sscanf(pnt+1, " ( %s , %*s , %[^)]", tmp0, tmp1) != 2)
             {
                pprintf(3, "*** RTS err: Debugger: name conversion error!");
                return ;
             }
             sprintf(arr_name, "%s [ %s]", tmp0, tmp1);
             return;

           case '2' :
             if (sscanf(pnt+1, " ( %s , %*s , %[^,], %[^)]", tmp0, tmp1, tmp2) != 3)
             {
                pprintf(3, "*** RTS err: Debugger: name conversion error!");
                return ;
             }
             sprintf(arr_name, "%s [ %s][ %s]", tmp0, tmp1, tmp2);
             return;

           case '3' :
             if (sscanf(pnt+1, " ( %s , %*s , %[^,], %[^,], %[^)]", tmp0, tmp1, tmp2, tmp3) != 4)
             {
                pprintf(3, "*** RTS err: Debugger: name conversion error!");
                return ;
             }
             sprintf(arr_name, "%s [ %s][ %s][ %s]", tmp0, tmp1, tmp2, tmp3);
             return;

           case '4' :
             if (sscanf(pnt+1, " ( %s , %*s , %[^,], %[^,], %[^,], %[^)]", tmp0, tmp1, tmp2, tmp3, tmp4) != 5)
             {
                pprintf(3, "*** RTS err: Debugger: name conversion error!");
                return ;
             }
             sprintf(arr_name, "%s [ %s][ %s][ %s][ %s]", tmp0, tmp1, tmp2, tmp3, tmp4);
             return;

           case '5' :
             if (sscanf(pnt+1, " ( %s , %*s , %[^,], %[^,], %[^,], %[^,], %[^)]", tmp0, tmp1, tmp2, tmp3, tmp4, tmp5 ) != 6)
             {
                pprintf(3, "*** RTS err: Debugger: name conversion error!");
                return ;
             }
             sprintf(arr_name, "%s [ %s][ %s][ %s][ %s][ %s]", tmp0, tmp1, tmp2, tmp3, tmp4, tmp5);
             return;

           case '6' :
             if (sscanf(pnt+1, " ( %s , %*s , %[^,], %[^,], %[^,], %[^,], %[^,], %[^)]", tmp0, tmp1, tmp2, tmp3, tmp4, tmp5, tmp6) != 7)
             {
                pprintf(3, "*** RTS err: Debugger: name conversion error!");
                return ;
             }
             sprintf(arr_name, "%s [ %s][ %s][ %s][ %s][ %s][ %s]", tmp0, tmp1, tmp2, tmp3, tmp4, tmp5, tmp6);
             return;

           default:
             pprintf(3, "*** RTS err: Debugger: name conversion error!");
             return;
        }
    } else {
{
	char *res, *res1;
	char rrr[256];
	char sss[256];	

	strcpy(sss,arr_name);
	rrr[0]=0;
	if ((res = strstr(sss, " ( ")) != NULL) {
		*res=0;
		strcat(rrr,sss);
		res+=3;
		while(1) {
			if ((res1 = strstr(res, " , ")) == NULL)
				if ((res1 = strstr(res, " )")) == NULL)break;
			*res1=0;
			strcat(rrr," [ ");
			strcat(rrr,res);
			strcat (rrr," ]");
			res=res1+3;
		}
		strcpy(arr_name,rrr);
//		printf("===%s===\n",rrr);
	}		
}
   }
}

/****************************************************************/

DvmType __callstd dprstv_(DvmType* plTypePtr, AddrType* pAddr, DvmType* pHandle, char* szOperand,
                          DvmType lOperLength)
{
    PRESAVE_VARIABLE* pPreSaveVar = NULL;

    DVMFTimeStart(call_dprstv_);
    dyntime_Check();

    pHandle = (NULL != pHandle) ? (DvmType*)(*pHandle) : NULL;

    if (DEB_TRACE)
    dvm_trace(call_dprstv_,
    "Type=%ld; *addr=%lx; *Handle=%lx; OperLength=%ld;\n",
    plTypePtr ? *plTypePtr : -1, pAddr ? *pAddr : 0, pHandle ? *pHandle : 0,
    lOperLength);

    if (EnableCodeCoverage)   SaveCoverageInfo();

    if (EnableDynControl || EnableTrace)
    {
        /* Allocate PreSaveVar structure */

        pPreSaveVar = table_GetNew(PRESAVE_VARIABLE, &gPreSaveVars);

        if (-1 == lOperLength || lOperLength > MaxOperand)
            lOperLength = MaxOperand;

        SYSTEM(strncpy, (pPreSaveVar->szFileName, DVM_FILE[0], MaxSourceFileName));
        pPreSaveVar->szFileName[MaxSourceFileName] = 0;

        SYSTEM(strncpy, (pPreSaveVar->szOperand, szOperand, lOperLength));
        pPreSaveVar->szOperand[lOperLength] = 0;
        renameCArrays(pPreSaveVar->szOperand);

        if (DEB_TRACE)
           tprintf("Operand=%s\n", pPreSaveVar->szOperand);

        pPreSaveVar->ulLine = DVM_LINE[0];
        pPreSaveVar->lType = plTypePtr ? *plTypePtr : -1;
        pPreSaveVar->pAddr = pAddr ? (void *)*pAddr : NULL;
        pPreSaveVar->pSysHandle = pHandle ? TstDVMArray((void*)pHandle) : NULL;

        if (EnableDynControl)
        {
            dyn_CheckVar(pPreSaveVar->szOperand, pPreSaveVar->pAddr, pPreSaveVar->pSysHandle, 2);
        }

        if (EnableTrace)
        {
            Trace.isDistr = pPreSaveVar->pSysHandle != NULL; /* need to be fixed: pass as a parameter !!! */

            pPreSaveVar->bIsReduct = (byte)trcreduct_IsReduct(pPreSaveVar->pAddr);
            if (NULL != pPreSaveVar->pSysHandle)
            {
                pPreSaveVar->pArrBase = pPreSaveVar->pSysHandle;
            }
            else
            {
                pPreSaveVar->pArrBase = (void*)pHandle;
            }
            switch (pPreSaveVar->lType)
            {
                case rt_INT :
                case rt_LOGICAL :
                case rt_LONG :
                case rt_LLONG:
                case rt_FLOAT :
                case rt_DOUBLE :
                case rt_FLOAT_COMPLEX :
                case rt_DOUBLE_COMPLEX :
                    pCmpOperations->Variable(pPreSaveVar->szFileName, pPreSaveVar->ulLine,
                        pPreSaveVar->szOperand, trc_PREWRITEVAR, pPreSaveVar->lType,
                        pPreSaveVar->pAddr, pPreSaveVar->bIsReduct, pPreSaveVar->pArrBase);
                    break;
            }
        }
    }

    if (DEB_TRACE)
        dvm_trace(ret_dprstv_, "\n");

    DVMFTimeFinish(ret_dprstv_);
    return 0;
}



DvmType __callstd dstv_(void)
{
    PRESAVE_VARIABLE* pPreSaveVar = NULL;

    DVMFTimeStart(call_dstv_);
    dyntime_Check();

    if (DEB_TRACE)
       dvm_trace(call_dstv_, "\n");

	if (EnableDynControl || EnableTrace)
	{
		pPreSaveVar = table_GetBack(PRESAVE_VARIABLE, &gPreSaveVars);

		if (EnableDynControl)
			dyn_CheckVar(pPreSaveVar->szOperand, pPreSaveVar->pAddr, pPreSaveVar->pSysHandle, 1);

		if (EnableTrace)
		{
            Trace.isDistr = pPreSaveVar->pSysHandle != NULL; /* need to be fixed: pass as a parameter !!! */

			switch (pPreSaveVar->lType)
			{
				case rt_INT :
                case rt_LOGICAL :
				case rt_LONG :
                case rt_LLONG:
				case rt_FLOAT :
				case rt_DOUBLE :
                case rt_FLOAT_COMPLEX :
                case rt_DOUBLE_COMPLEX :
                    pCmpOperations->Variable(pPreSaveVar->szFileName, pPreSaveVar->ulLine,
                        pPreSaveVar->szOperand, trc_POSTWRITEVAR, pPreSaveVar->lType,
                        pPreSaveVar->pAddr, pPreSaveVar->bIsReduct, pPreSaveVar->pArrBase);
					break;
			}
		}

		table_RemoveLast(&gPreSaveVars);
	}

    if (DEB_TRACE)
       dvm_trace(ret_dstv_, "\n");

    DVMFTimeFinish(ret_dstv_);
    return 0;
}



DvmType __callstd dldv_(DvmType* plTypePtr, AddrType* pAddr, DvmType* pHandle, char* szOperand, DvmType lOperLength)
{
    char szVarOperand[MaxOperand + 1];
    SysHandle* pSysHandle = NULL;
    void* pArrBase = NULL;

    DVMFTimeStart(call_dldv_);
    dyntime_Check();

    pHandle = (NULL != pHandle) ? (DvmType*)(*pHandle) : NULL;

    if(DEB_TRACE)
        dvm_trace(call_dldv_,
        "Type=%ld; *addr=%lx; *Handle=%lx; OperLength=%ld;\n",
        plTypePtr ? *plTypePtr : -1, pAddr ? *pAddr : 0, pHandle ? *pHandle : 0,
        lOperLength);

    if (EnableCodeCoverage)   SaveCoverageInfo();

    if (-1 == lOperLength || lOperLength > MaxOperand)
        lOperLength = MaxOperand;

    SYSTEM(strncpy, (szVarOperand, szOperand, lOperLength));
    szVarOperand[lOperLength] = 0;
    renameCArrays(szVarOperand);

    pSysHandle = pHandle ? TstDVMArray((void*)pHandle) : NULL;

    if (DEB_TRACE)
        tprintf("Operand=%s\n", szVarOperand);


    if (EnableDynControl)
        dyn_CheckVar(szVarOperand, pAddr ? (void*)(*pAddr) : NULL, pSysHandle, 0);

    if (EnableTrace)
    {
        if (plTypePtr != NULL)
        {
            byte Reduct = (byte)(pAddr ? trcreduct_IsReduct((void *)(*pAddr)) : 0);

            Trace.isDistr = pSysHandle != NULL; /* need to be fixed: pass as a parameter !!! */

            if (NULL != pSysHandle)
            {
                pArrBase = pSysHandle;
            }
            else
            {
                pArrBase = (void*)pHandle;
            }
            switch (*plTypePtr)
            {
                case rt_INT :
                case rt_LOGICAL :
                case rt_LONG :
                case rt_LLONG:
                case rt_FLOAT :
                case rt_DOUBLE :
                case rt_FLOAT_COMPLEX :
                case rt_DOUBLE_COMPLEX :
                    pCmpOperations->Variable(DVM_FILE[0], DVM_LINE[0], szVarOperand, trc_READVAR,
                        *plTypePtr, pAddr ? (void *)(*pAddr) : NULL, Reduct, pArrBase);
                    break;
            }
        }
    }

    if(DEB_TRACE)
       dvm_trace(ret_dldv_, "\n");

    DVMFTimeFinish(ret_dldv_);
    return 0;
}



DvmType __callstd dbegpl_(DvmType *Rank, DvmType *No, DvmType Init[], DvmType Last[], DvmType Step[])
{
    DVMFTimeStart(call_dbegpl_);
    dyntime_Check();

    if(DEB_TRACE)
        dvm_trace(call_dbegpl_,
        "*Rank=%ld; *No=%ld; *Init=%ld; *Last=%ld; *Step=%ld;\n",
        Rank ? *Rank:1, No ? *No:0, *Init, *Last, *Step);

    ploop_beg(&Trace.CurPoint);  /* increment dynamic point counter */

    cntx_LevelInit((int)(No ? *No : 0), (byte)(Rank ? *Rank : 1 ), (byte)1, Init, Last, Step);

    if ( TraceOptions.IterTraceMode >= 0 )
    {
        pushStack(&(Trace.sLoopsInfo), *No, Trace.vtr?*Trace.vtr:0, 1, (unsigned int) DVM_LINE[0]);

        if ( Trace.vtr && *Trace.vtr && TraceOptions.LocIterWidth != 0)
        {
            DBG_ASSERT(__FILE__, __LINE__, ! ((Trace.ctrlIndex != -1) && (Trace.ctrlIndex != Trace.sLoopsInfo.top - 2)) );

            Trace.ctrlIndex++;
        }
    }

    if ( EnableTrace )
    {
        if ( !Trace.vtr || (Trace.vtr && *Trace.vtr) )
        {
            pCmpOperations->BeginStruct(DVM_FILE[0], DVM_LINE[0], No ? *No : 0,
                                        1, (byte)(Rank ? *Rank : 1 ), Init, Last, Step );
            Trace.inParLoop = 1; /* do it after BeginStruct called */
        }
    }

    if(DEB_TRACE)
        dvm_trace(ret_dbegpl_, "\n");

    DVMFTimeFinish(ret_dbegpl_);
    return 0;
}


DvmType __callstd dbegsl_(DvmType *No)
{
#ifdef DOSL_TRACE

    char buffer[999];


    sprintf(buffer, "\ncall dbegsl_, loop %ld, UserLine=%ld, UserFile=%s\n", *No, DVM_LINE[0], DVM_FILE[0]);
    SYSTEM(fputs, (buffer, Trace.DoplMBFileHandle));

#endif

    DVMFTimeStart(call_dbegsl_);
    dyntime_Check();

    if(DEB_TRACE)
        dvm_trace(call_dbegsl_, "*No=%ld;\n", No ? *No:0);

    cntx_LevelInit((int)(No ? *No : 0), (byte)1, (byte)0, NULL, NULL, NULL);

    if ( TraceOptions.IterTraceMode >= 0 )
    {
        pushStack(&(Trace.sLoopsInfo), *No, Trace.vtr?*Trace.vtr:0, 0, (unsigned int) DVM_LINE[0]);

        if ( Trace.vtr && *Trace.vtr && TraceOptions.RepIterWidth != 0)
        {
            DBG_ASSERT(__FILE__, __LINE__, ! ((Trace.ctrlIndex != -1) && (Trace.ctrlIndex != Trace.sLoopsInfo.top - 2)) );

            Trace.ctrlIndex++;
        }
    }

    if ( EnableTrace && ( !Trace.vtr || (Trace.vtr && *Trace.vtr) ))
    {
        pCmpOperations->BeginStruct(DVM_FILE[0], DVM_LINE[0], No ? *No : 0,
                                    0, 1, NULL, NULL, NULL);
    }

    if(DEB_TRACE)
        dvm_trace(ret_dbegsl_, "\n");

    DVMFTimeFinish(ret_dbegsl_);

#ifdef DOSL_TRACE

    sprintf(buffer, "\nret dbegsl_, loop %ld, UserLine=%ld, UserFile=%s\n", *No, DVM_LINE[0], DVM_FILE[0]);
    SYSTEM(fputs, (buffer, Trace.DoplMBFileHandle));

#endif
    return 0;
}


DvmType __callstd dendl_(DvmType *No, UDvmType *Line)
{
    dvm_CONTEXT* pCntx;
    int iVal;
#ifdef DOSL_TRACE

    char buffer[999];

    sprintf(buffer, "\ncall dendl_, loop %ld, UserLine=%ld, UserFile=%s\n", *No, DVM_LINE[0], DVM_FILE[0]);
    SYSTEM(fputs, (buffer, Trace.DoplMBFileHandle));

#endif

    DVMFTimeStart(call_dendl_);
    dyntime_Check();

    if (DEB_TRACE)
        dvm_trace(call_dendl_, "*No=%ld; *Line=%ld;\n",
                            No ? *No:0, Line ? *Line:0);

    if ( TraceOptions.IterTraceMode >= 0 )
        if ( Trace.sLoopsInfo.items[Trace.sLoopsInfo.top-1].No == *No )
        {
            iVal = stackRemoveLast(&Trace.sLoopsInfo);
            if ( Trace.vtr )
            {
                *Trace.vtr = iVal; /* restore old VTR value */

                if (Trace.ctrlIndex == Trace.sLoopsInfo.top)   Trace.ctrlIndex--;
            }
        }

    pCntx = cntx_CurrentLevel();

    /* let the counter know that parallel loop or task region has finished */
    switch ( pCntx->Type )
    {
        case 1 :    ploop_end(&Trace.CurPoint);
                    break;
        case 2 :    tr_end(&Trace.CurPoint);
    }

    if ((EnableDynControl || EnableTrace) && (!Trace.vtr || (Trace.vtr && *Trace.vtr)))
    {
        if(EnableTrace)
        {
            if (pCntx->Type == 1)
                Trace.inParLoop = 0; /* do it before EndStruct called  */

            pCmpOperations->EndStruct(DVM_FILE[0], DVM_LINE[0], No ? *No : 0, Line ? *Line : 0);
        }

        if (pCntx && pCntx->Type != 0)
        {
            if (EnableTrace && ManualReductCalc)
                trcreduct_Calculate();

            if (EnableDynControl)
                dyn_LevelDone();
        }
    }

    cntx_LevelDone();

    if (DEB_TRACE)
        dvm_trace(ret_dendl_, "\n");

    DVMFTimeFinish(ret_dendl_);
#ifdef DOSL_TRACE

    sprintf(buffer, "\nret dendl_, loop %ld, UserLine=%ld, UserFile=%s\n", *No, DVM_LINE[0], DVM_FILE[0]);
    SYSTEM(fputs, (buffer, Trace.DoplMBFileHandle));

#endif

    return 0;
}



DvmType __callstd diter_(AddrType index[], DvmType IndexTypes[])
{
    dvm_CONTEXT* pCntx = NULL;
    int i;

    DVMFTimeStart(call_diter_);
    dyntime_Check();

    if (DEB_TRACE)
    {
        dvm_trace(call_diter_, "\n");

        if (diter_Trace && TstTraceEvent(call_diter_))
        {
        /* Detail trace for call_diter_ event is needed
            and has been turned on */

            if ((pCntx = cntx_CurrentLevel()) != NULL)
            {
                for (i = 0; i < pCntx->Rank; i++)
                    tprintf("index[%d]=%lx;\n", i, index[i]);
                if (IndexTypes != NULL)
                {
                    for (i = 0; i < pCntx->Rank; i++)
                        tprintf("IndexTypes[%d]=%ld;\n", i, IndexTypes[i]);
                }
                else
                    tprintf("IndexTypes=NULL;\n");
            }
        }
    }

    pCntx = cntx_CurrentLevel();

    if (EnableDynControl || EnableTrace)
    {
        if (EnableTrace && ManualReductCalc)
        {
            if (pCntx && pCntx->Type != 0 && pCntx->ItersInit != 0)
                trcreduct_Calculate();
        }

        cntx_SetIters(index, IndexTypes);
    }

    if (pCntx)
    {
        if (EnableDynControl)
            for (i = 0; i < pCntx->Rank; i++)
                dyn_CheckVar("iter", (void *)(index[i]), NULL, (byte)1);

        if ( pCntx->Type == 2 )     /* set the iteration number for task region on the counter */
        {
            switch (IndexTypes[0])
            {
                case 0 :
                    tr_it(&Trace.CurPoint, PT_LONG(index[0]));
                    break;
                case 1 :
                    tr_it(&Trace.CurPoint, (DvmType)PT_INT(index[0]));
                    break;
                case 2 :
                    tr_it(&Trace.CurPoint, (DvmType)PT_SHORT(index[0]));
                    break;
                case 3 :
                    tr_it(&Trace.CurPoint, (DvmType)PT_CHAR(index[0]));
                    break;
            }
        }

        if (EnableTrace)
            pCmpOperations->Iter(pCntx->Iters);
    }

    if (DEB_TRACE)
        dvm_trace(ret_diter_, "\n");

    DVMFTimeFinish(ret_diter_);
    return 0;
}



DvmType __callstd drmbuf_(DvmType *ArrSrc, AddrType *RmtBuff, DvmType *Rank, DvmType Index[])
{
    SysHandle *SrcHandle, *DstHandle;
    void* pAddrBase = NULL;
    void* pAddr = NULL;

    if( !Trace.vtr || (Trace.vtr && *Trace.vtr) )
    {
        DVMFTimeStart(call_drmbuf_);
        dyntime_Check();

        if (DEB_TRACE)
        {
            dvm_trace(call_drmbuf_,
            "*ArrSrc=%lx; *RmtBuff=%lx; *Rank=%ld;\n",
            ArrSrc ? *ArrSrc:0, RmtBuff ? *RmtBuff:0, Rank ? *Rank:0);

            if( drmbuf_Trace && TstTraceEvent(call_drmbuf_) && Rank)

                /*  Detail trace for call_drmbuf_ event is needed
                    and has been turned on */
            {
                int i;
                for (i = 0; i < *Rank; i++)
                    tprintf("Index[%d]=%lx;\n", i, Index[i]);
            }
        }

        SrcHandle = TstDVMArray((void *)ArrSrc);
        if (SrcHandle != NULL && RmtBuff != NULL)
        {
            if (EnableDynControl)
            {
                if (Rank != NULL && *Rank > 0)
                {
                    DstHandle = TstDVMArray((void *)(*RmtBuff));
                    if (DstHandle != NULL)
                        dyn_DefineRemoteBufferArray(SrcHandle, DstHandle, Index);
                }
                else
                {
                    dyn_DefineRemoteBufferScalar(SrcHandle, (void *)*RmtBuff, Index);
                }
            }

            if (EnableTrace)
            {
                DstHandle = TstDVMArray((void *)(*RmtBuff));
                pAddr = (NULL != DstHandle) ?
                    (void*)DstHandle : (void*)(*RmtBuff);
                pAddrBase = (NULL != DstHandle) ?
                    ((s_DISARRAY*)(DstHandle->pP))->ArrBlock.ALoc.Ptr : (void*)(*RmtBuff);
                trc_ArrayRegisterRemoteBuffer(SrcHandle, pAddr, pAddrBase, Index);
            }
        }

        if (DEB_TRACE)
            dvm_trace(ret_drmbuf_, "\n");

        DVMFTimeFinish(ret_drmbuf_);
    }
    return 0;
}



DvmType __callstd dskpbl_(void)
{
    if( !Trace.vtr || (Trace.vtr && *Trace.vtr) )
    {
        DVMFTimeStart(call_dskpbl_);
        dyntime_Check();

        if (DEB_TRACE)
            dvm_trace(call_dskpbl_, "\n");

        if (EnableTrace)
            pCmpOperations->SkipBlock(DVM_FILE[0], DVM_LINE[0]);

        if (DEB_TRACE)
            dvm_trace(ret_dskpbl_, "\n");

        DVMFTimeFinish(ret_dskpbl_);
    }
    return 0;
}



DvmType __callstd dbegtr_(DvmType *No)
{
    tr_beg(&Trace.CurPoint);  /* prepare the counter for task region */

    cntx_LevelInit((int)(No ? *No : 0), (byte)1, (byte)2, NULL, NULL, NULL);

    if( !Trace.vtr || (Trace.vtr && *Trace.vtr) )
    {
        DVMFTimeStart(call_dbegtr_);
        dyntime_Check();

        if (DEB_TRACE)
            dvm_trace(call_dbegtr_, "*No=%ld;\n", No ? *No : 0);

        if (EnableTrace)
        {
            pCmpOperations->BeginStruct(DVM_FILE[0], DVM_LINE[0], No ? *No : 0,
                    2, 1, NULL, NULL, NULL);
        }

        if (DEB_TRACE)
            dvm_trace(ret_dbegtr_, "\n");

        DVMFTimeFinish(ret_dbegtr_);
    }
    return (DVM_RET, 0);
}



DvmType __callstd dread_(AddrType* ppAddr)
{
    SysHandle* pHandle = NULL;

    if( !Trace.vtr || (Trace.vtr && *Trace.vtr) )
    {
        DVMFTimeStart(call_dread_);
        dyntime_Check();

        if (DEB_TRACE)
            dvm_trace(call_dread_, "*ppAddr=%lx\n", ppAddr ? (void*)*ppAddr : NULL);

        if (EnableDynControl)
        {
            pHandle = TstDVMArray((void*)(*ppAddr));

            if (pHandle != NULL)
                dyn_InitializeSetArr(pHandle);
            else
                dyn_InitializeSetScal((void*)(*ppAddr));
        }

        if (DEB_TRACE)
            dvm_trace(ret_dread_, "\n");

        DVMFTimeFinish(ret_dread_);
    }
    return (DVM_RET, 0);
}



DvmType __callstd dreada_(AddrType* ppAddr, DvmType* pElemntLength, DvmType* pArrLength)
{
    char* pArrPtr = NULL;
    DvmType  i, count, size;

    if( !Trace.vtr || (Trace.vtr && *Trace.vtr) )
    {
        DVMFTimeStart(call_dread_);
        dyntime_Check();

        if (DEB_TRACE)
            dvm_trace(call_dread_, "ppAddr=%lx; *pElemntLength = %ld; *pArrLength = %ld\n",
                    ppAddr ? (void*)*ppAddr : NULL, pElemntLength ? *pElemntLength : 0l, pArrLength ? *pArrLength : 0l);

        if (EnableDynControl)
        {
            pArrPtr = ppAddr != NULL ? (char*)(*ppAddr) : NULL;

            if (pArrPtr != NULL && pElemntLength != NULL && pArrLength != NULL)
            {
                count = *pArrLength;
                size = *pElemntLength;

                for (i = 0; i < count; i++)
                {
                    dyn_InitializeSetScal((void*)pArrPtr);
                    pArrPtr += size;
                }
            }
        }

        if (DEB_TRACE)
            dvm_trace(ret_dread_, "\n");

        DVMFTimeFinish(ret_dread_);
    }
    return (DVM_RET, 0);
}



DvmType  __callstd  drarr_(DvmType  *plRank, DvmType  *plTypePtr, DvmType  *pHandle,
                           DvmType  *pSize, char  *szOperand,
                           DvmType  lOperLength)
{
    char        szVarOperand[MaxOperand + 1];
    SysHandle  *pSysHandle = NULL;
    void       *pArrBase = NULL;
    int         i;

    if( TraceOptions.drarr == 0) /* in checksum mode drarr is always equal 1 */
       return  0;

    DVMFTimeStart(call_drarr_);
    dyntime_Check();

    pHandle = (NULL != pHandle) ? (DvmType*)(*pHandle) : NULL;

    if (-1 == lOperLength || lOperLength > MaxOperand)
        lOperLength = MaxOperand;

    SYSTEM(strncpy, (szVarOperand, szOperand, lOperLength));
    szVarOperand[lOperLength] = 0;

    if(DEB_TRACE)
    {  dvm_trace(call_drarr_,
                "Rank=%ld; Type=%ld; *Handle=%lx; OperLength=%ld; "
                "Operand=%s\n",
                plRank ? *plRank : 0, plTypePtr ? *plTypePtr : 0,
                pHandle ? *pHandle : 0, lOperLength, szVarOperand);

        if(TstTraceEvent(call_drarr_))
        {  for(i=0; i < *plRank; i++)
                tprintf("pSize[%d]=%ld; ", i, pSize[i]);
            tprintf(" \n");
            tprintf(" \n");
        }
    }

    pSysHandle = pHandle ? TstDVMArray((void*)pHandle) : NULL;

    if(NULL != pSysHandle)
        pArrBase = (void*)pSysHandle;
    else
        pArrBase = (void*)pHandle;

    if(EnableTrace && NULL != plTypePtr)
    {
        trc_ArrayRegister((byte)(plRank ? *plRank : 0l), pSize,
                            *plTypePtr, DVM_FILE[0], DVM_LINE[0],
                            szVarOperand, pArrBase,
                            (byte)(NULL != pSysHandle));
    }

    if(DEB_TRACE)
        dvm_trace(ret_drarr_, "\n");

    DVMFTimeFinish(ret_drarr_);
    return  0;
}



DvmType __callstd dcrtrg_(void)
{
    REDUCTION_GROUP* pRG = NULL;

    if( !Trace.vtr || (Trace.vtr && *Trace.vtr) )
    {
        DVMFTimeStart(call_dcrtrg_);
        dyntime_Check();

        dynmem_CallocStruct(REDUCTION_GROUP, pRG);
        pRG->RV = coll_Init(RedVarGrpCount, RedVarGrpCount, NULL);
        pRG->bInit = 1;

        if (DEB_TRACE)
            dvm_trace(call_dcrtrg_, "Handle=%lx;\n", pRG);

        if (DEB_TRACE)
            dvm_trace(ret_dcrtrg_, "\n");

        DVMFTimeFinish(ret_dcrtrg_);
    }
    return (ObjectRef)pRG;
}



DvmType __callstd dinsrd_(ObjectRef* pDebRedGroup, DvmType* RedFuncNumbPtr, void* RedArrayPtr,
    DvmType* RedArrayTypePtr, DvmType* RedArrayLengthPtr, void* LocArrayPtr, DvmType* LocElmLengthPtr,
    DvmType* LocIndTypePtr)
{
    static byte printed  = 0;
    REDUCTION_GROUP* pRG = NULL;
    s_REDVAR* pRVar = NULL;

    if( !Trace.vtr || (Trace.vtr && *Trace.vtr) )
    {
        if ( *RedArrayLengthPtr > 1 && TraceOptions.DisableRedArrays )
            return 0;

        DVMFTimeStart(call_dinsrd_);
        dyntime_Check();

        if (DEB_TRACE)
            dvm_trace(call_dinsrd_,
                "RedFuncNumb=%ld; RedArrayPtr=%lx; RedArrayType=%ld; "
                "RedArrayLength=%ld; LocArrayPtr=%lx; "
                "LocElmLength=%ld; "
                "LocIndTypePtr=%ld;\n",
                *RedFuncNumbPtr,(uLLng)RedArrayPtr,*RedArrayTypePtr,
                *RedArrayLengthPtr,(uLLng)LocArrayPtr,
                *LocElmLengthPtr,*LocIndTypePtr);

        pRG = (REDUCTION_GROUP*)*pDebRedGroup;

        if (!pRG->bInit)
            epprintf(MultiProcErrReg1, __FILE__,__LINE__,
                "*** RTS err: inserting reduction variable after "
                "completing reduction operation.\n");

        if ( !TstDVMArray(RedArrayPtr) )
        {
            dynmem_CallocStruct(s_REDVAR, pRVar);

            pRVar->Func = (byte)*RedFuncNumbPtr;
            pRVar->Mem = (char *)RedArrayPtr;
            pRVar->VType = (int)*RedArrayTypePtr;
            pRVar->VLength = (int)*RedArrayLengthPtr;
            pRVar->LocMem = (char *)LocArrayPtr;
            pRVar->LocElmLength = (int)*LocElmLengthPtr;
            pRVar->Static = (byte)0;
            pRVar->RG = NULL;
            pRVar->AMView = NULL;
            pRVar->BufAddr = NULL;
            pRVar->LocIndType = (int)*LocIndTypePtr;

            switch (pRVar->VType)
            {

                case rt_INT :
                case rt_LOGICAL : pRVar->RedElmLength = sizeof(int); break;
                case rt_LONG: pRVar->RedElmLength = sizeof(long); break;
                case rt_LLONG: pRVar->RedElmLength = sizeof(long long); break;
                case rt_DOUBLE : pRVar->RedElmLength = sizeof(double); break;
                case rt_FLOAT : pRVar->RedElmLength = sizeof(float); break;
                case rt_DOUBLE_COMPLEX : pRVar->RedElmLength = 2 * sizeof(double); break;
                case rt_FLOAT_COMPLEX : pRVar->RedElmLength = 2 * sizeof(float); break;
                default:
                    epprintf(MultiProcErrReg1, __FILE__,__LINE__,
                                "*** RTS err: wrong call dinsrd_\n"
                                "(invalid number of reduction variable type; "
                                "RedArrayType=%d)\n", pRVar->VType);
            }

            if (pRVar->Func != rf_SUM && pRVar->Func != rf_MULT &&
                pRVar->Func != rf_MAX && pRVar->Func != rf_MIN &&
                pRVar->Func != rf_AND && pRVar->Func != rf_OR &&
                pRVar->Func != rf_XOR && pRVar->Func != rf_EQU &&
                pRVar->Func != rf_EQ && pRVar->Func != rf_NE)
                epprintf(MultiProcErrReg1, __FILE__, __LINE__,
                    "*** RTS err: wrong call dinsrd_\n"
                    "(invalid number of reduction function; "
                    "RedFuncNumb=%d)\n", (int)pRVar->Func);


            if ((pRVar->Func == rf_AND || pRVar->Func == rf_OR ||
                pRVar->Func == rf_XOR || pRVar->Func == rf_EQU) &&
                pRVar->VType != rt_INT && pRVar->VType != rt_LONG && pRVar->VType != rt_LLONG)
                epprintf(MultiProcErrReg1, __FILE__, __LINE__,
                    "*** RTS err: wrong call dinsrd_\n"
                    "(invalid reduction variable type; "
                    "RedFuncNumb=%d; RedArrayType=%d)\n",
                    (int)pRVar->Func, pRVar->VType);

            if ((pRVar->VType == rt_FLOAT_COMPLEX ||
                pRVar->VType == rt_DOUBLE_COMPLEX)  &&
                pRVar->Func != rf_SUM && pRVar->Func != rf_MULT)
                epprintf(MultiProcErrReg1, __FILE__, __LINE__,
                    "*** RTS err: wrong call dinsrd_\n"
                    "(invalid complex type of the reduction variable; "
                    "RedFuncNumb=%d; RedArrayType=%d)\n",
                    (int)pRVar->Func, pRVar->VType);

            if ((pRVar->Func == rf_MIN || pRVar->Func == rf_MAX) &&
                pRVar->LocElmLength != 0 && pRVar->LocMem != NULL)
            {
                switch(pRVar->Func)
                {
                case rf_MIN: pRVar->Func = rf_MINLOC; break;
                case rf_MAX: pRVar->Func = rf_MAXLOC; break;
                }
            }
            else
            {
                pRVar->LocElmLength = 0;
                pRVar->LocMem = NULL;
            }

            pRVar->BlockSize = pRVar->VLength *
                (pRVar->RedElmLength + pRVar->LocElmLength);

            coll_Insert(&pRG->RV, pRVar);

            if (EnableTrace)
                trcreduct_Insert(pRVar);
        }
        else
        {
            if ( !printed )
            {
                pprintf(3, "*** RTS warning: Distributed array as a reduction"
                    " array is not allowed for debugging. Ignored.\n");
                printed = 1;
            }
        }

        if (DEB_TRACE)
            dvm_trace(ret_dinsrd_, "\n");

        DVMFTimeFinish(ret_dinsrd_);
    }
    return 0;
}


/* For FORTRAN */


DvmType __callstd dinsrf_(ObjectRef* pDebRedGroup, DvmType* RedFuncNumbPtr,
    AddrType* RedArrayAddrPtr, DvmType* RedArrayTypePtr,
    DvmType* RedArrayLengthPtr, void* LocArrayPtr,
    DvmType* LocElmLengthPtr, DvmType* LocIndTypePtr)
{
  return  dinsrd_(pDebRedGroup, RedFuncNumbPtr, (void *)*RedArrayAddrPtr,
                  RedArrayTypePtr, RedArrayLengthPtr, LocArrayPtr,
                  LocElmLengthPtr, LocIndTypePtr);
}


DvmType __callstd dsavrg_(ObjectRef* pDebRedGroup)
{
    REDUCTION_GROUP* pRG = NULL;
    s_REDVAR* pRVar = NULL;
    int i;

    if( !Trace.vtr || (Trace.vtr && *Trace.vtr) )
    {
        /* DVMFTimeStart(call_dsavrg_); */

        dyntime_Check();

        /* if (DEB_TRACE)
        dvm_trace(call_dsavrg_, "DebRedGroup=%lx;\n", *pDebRedGroup); */

        pRG = (REDUCTION_GROUP*)*pDebRedGroup;

        if (!pRG->bInit && EnableTrace)
        {
            for (i = 0; i < pRG->RV.Count; i++)
            {
                pRVar = coll_At(s_REDVAR*, &pRG->RV, i);
                trcreduct_Insert(pRVar);
            }
        }
        pRG->bInit = 1;

        /* if (DEB_TRACE)
        dvm_trace(ret_dsavrg_, "\n"); */

        /* DVMFTimeFinish(ret_dsavrg_); */
    }
    return 0;
}

DvmType __callstd dclcrg_(ObjectRef* pDebRedGroup)
{
    REDUCTION_GROUP* pRG = NULL;
    s_REDVAR* pRVar = NULL;
    int i;

    if( !Trace.vtr || (Trace.vtr && *Trace.vtr) )
    {
        /* DVMFTimeStart(call_dclcrg_); */
        dyntime_Check();

        /* if (DEB_TRACE)
        dvm_trace(call_dclcrg_, "DebRedGroup=%lx;\n", *pDebRedGroup); */

        pRG = (REDUCTION_GROUP*)*pDebRedGroup;

        if (pRG->bInit && EnableTrace)
        {
            if (ManualReductCalc)
            trcreduct_CopyResult(&pRG->RV);

            for (i = 0; i < pRG->RV.Count; i++)
            {
                pRVar = coll_At(s_REDVAR*, &pRG->RV, i);
                trcreduct_Remove(pRVar);
            }
        }
        pRG->bInit = 0;

        /* if (DEB_TRACE)
        dvm_trace(ret_dclcrg_, "\n"); */

        /* DVMFTimeFinish(ret_dclcrg_); */
    }
    return 0;
}

DvmType __callstd ddelrg_(ObjectRef* pDebRedGroup)
{
    REDUCTION_GROUP* pRG = NULL;
    s_REDVAR* pRVar = NULL;
    int i;

    if( !Trace.vtr || (Trace.vtr && *Trace.vtr) )
    {
        DVMFTimeStart(call_ddelrg_);
        dyntime_Check();

        if (DEB_TRACE)
            dvm_trace(call_ddelrg_, "DebRedGroup=%lx;\n", *pDebRedGroup);

        pRG = (REDUCTION_GROUP*)*pDebRedGroup;

        if (pRG->bInit)
            dclcrg_(pDebRedGroup);

        for (i = 0; i < pRG->RV.Count; i++)
        {
            pRVar = coll_At(s_REDVAR*, &pRG->RV, i);
            pRVar->RG = NULL;
            dynmem_free(&pRVar, sizeof(s_REDVAR));
        }

        dvm_FreeArray(pRG->RV.List);
        dynmem_free(&pRG, sizeof(REDUCTION_GROUP));

        if (DEB_TRACE)
            dvm_trace(ret_ddelrg_, "\n");

        DVMFTimeFinish(ret_ddelrg_);
    }
    return 0;
}

void __callstd dbgtron_(void)
{
    if ( Trace.EnableDbgTracingCtrl )
    {
        if ( EnableTrace == 1 )
        {
            epprintf(MultiProcErrReg1, __FILE__,__LINE__,
            "*** RTS err: dbgtron_: Enabling tracing when it is already enabled is incorrect! Program is terminated.\n");
        }
        EnableTrace = 1;
        //TraceOptions.IterTraceMode = Trace.OldIterTraceMode ;
        //*Trace.vtr = Trace.OldVtr;
    }
}

void __callstd dbgtroff_(void)
{
    if ( Trace.EnableDbgTracingCtrl )
    {
        if ( EnableTrace == 0 )
        {
            epprintf(MultiProcErrReg1, __FILE__,__LINE__,
            "*** RTS err: dbgtroff_: Disabling tracing when it is already disabled is incorrect! Program is terminated.\n");
        }
        EnableTrace = 0;
        /*Trace.OldIterTraceMode = TraceOptions.IterTraceMode;
        TraceOptions.IterTraceMode = -1;
        Trace.OldVtr = *Trace.vtr;
        *Trace.vtr = 0;*/
    }
}

void __callstd dbglparton_(void)
{
    if ( Trace.EnableLoopsPartitioning )
    {
        epprintf(MultiProcErrReg1, __FILE__,__LINE__,
        "*** RTS err: dbglparton_: Enabling loops partitioning when it is already enabled is incorrect! Program is terminated.\n");
        EnableTrace = 0;
    }
    Trace.EnableLoopsPartitioning = 1;
}

void __callstd dbglpartoff_(void)
{
    if ( !Trace.EnableLoopsPartitioning )
    {
        epprintf(MultiProcErrReg1, __FILE__,__LINE__,
        "*** RTS err: dbglpartoff_: Disabling loops partitioning when it is already disabled is incorrect! Program is terminated.\n");
        EnableTrace = 0;
    }
    Trace.EnableLoopsPartitioning = 0;
}

#endif   /* _DEBUG_C_ */
