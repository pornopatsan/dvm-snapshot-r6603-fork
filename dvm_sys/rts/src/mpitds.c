#ifndef _MPITDS_C_
#define _MPITDS_C_
/****************/    /*E0000*/

#include "dvmlib.inc"
#include "sysext.inc"

#if defined(_NT_MPI_) || defined(_UNIX_)
#if !defined(_MPI_PROF_EXT_) && defined(_DVM_MPI_PROF_)


/***********************\
*   Utility Functions   *
\***********************/    /*E0001*/

static  int   getcomm(MPI_Comm  comm, commInfo  **commInfoPtrPtr)
{  int     ind;

   ind = coll_IndexOf(CommColl, comm);

   if(ind >= 0)
      *commInfoPtrPtr = coll_At(commInfo *, CommStructColl, ind);
   else
      *commInfoPtrPtr = NULL;

   return  ind;
}



static  int   _perror(char *str)
{
   epprintf(1 ,__FILE__, __LINE__,
            "$*** dynamic analyzer err: %s$\n", str);
   return  0;
}



static  void   _assert(int  result, char  *str)
{
   if(result != MPI_SUCCESS)
      _perror(str);
}



static  void   _printOp(MPI_Op  op)
{
   if(op == MPI_MAX)
   {  tprintf("MPI_MAX");
      return;
   }

   if(op == MPI_MIN)
   {  tprintf("MPI_MIN");
      return;
   }

   if(op == MPI_SUM)
   {  tprintf("MPI_SUM");
      return;
   }

   if(op == MPI_PROD)
   {  tprintf("MPI_PROD");
      return;
   }

   if(op == MPI_LAND)
   {  tprintf("MPI_LAND");
      return;
   }

   if(op == MPI_BAND)
   {  tprintf("MPI_BAND");
      return;
   }

   if(op == MPI_LOR)
   {  tprintf("MPI_LOR");
      return;
   }

   if(op == MPI_BOR)
   {  tprintf("MPI_BOR");
      return;
   }

   if(op == MPI_LXOR)
   {  tprintf("MPI_LXOR");
      return;
   }

   if(op == MPI_BXOR)
   {  tprintf("MPI_BXOR");
      return;
   }

   if(op == MPI_MAXLOC)
   {  tprintf("MPI_MAXLOC");
      return;
   }

   if(op == MPI_MINLOC)
   {  tprintf("MPI_MINLOC");
      return;
   }

   tprintf("USER_DEF");
   return;

/*
   switch(op)
   {
      case MPI_MAX:    tprintf("MPI_MAX");
                       break;
      case MPI_MIN:    tprintf("MPI_MIN");
                       break;
      case MPI_SUM:    tprintf("MPI_SUM");
                       break;
      case MPI_PROD:   tprintf("MPI_PROD");
                       break;
      case MPI_LAND:   tprintf("MPI_LAND");
                       break;
      case MPI_BAND:   tprintf("MPI_BAND");
                       break;
      case MPI_LOR:    tprintf("MPI_LOR");
                       break;
      case MPI_BOR:    tprintf("MPI_BOR");
                       break;
      case MPI_LXOR:   tprintf("MPI_LXOR");
                       break;
      case MPI_BXOR:   tprintf("MPI_BXOR");
                       break;
      case MPI_MAXLOC: tprintf("MPI_MAXLOC");
                       break;
      case MPI_MINLOC: tprintf("MPI_MINLOC");
                       break;
      default:         tprintf("USER_DEF");
                       break;
   }
*/    /*E0002*/
}



static  unsigned   _typeCheckSum(void  *elem, MPI_Datatype  type,
                                 int  *fTypeDerived)
{
   unsigned        sum = 0;
   int            *typeInts, intsQTY, aintsQTY, dtsQTY, combiner, ts,
                   i, j, fSubtypeDerived;
   unsigned char  *curPtr;
   MPI_Aint       *typeAints;
   MPI_Datatype   *typeDts;
   MPI_Aint        typeExtent;

   PMPI_Type_get_envelope(type, &intsQTY, &aintsQTY, &dtsQTY, &combiner);
   PMPI_Type_size(type, &ts);

   if(combiner == MPI_COMBINER_NAMED)         /*  Basic MPI type  */    /*E0003*/
   {
      curPtr = (unsigned char *)elem;
      *fTypeDerived = 0;

      for(i=0; i < ts; i++) 
      {
         sum += *curPtr;
         curPtr++;
      }

      return  sum;
   }

   *fTypeDerived = 1;

   i = sizeof(int)*intsQTY;
   mac_malloc(typeInts, int *, i, 0);

   i = sizeof(MPI_Aint)*aintsQTY;
   mac_malloc(typeAints, MPI_Aint *, i, 0);

   i = sizeof(MPI_Datatype)*dtsQTY;
   mac_malloc(typeDts, MPI_Datatype *, i, 0);

   #ifdef _DVM_MPI2_
 
   PMPI_Type_get_contents(type, intsQTY, aintsQTY, dtsQTY, typeInts,
                          typeAints, typeDts);    /* MPI-2 !!! */    /*E0004*/

   #endif

   fSubtypeDerived = 0;

   switch(combiner)
   {
      case MPI_COMBINER_CONTIGUOUS:   /* Type was created with
                                         MPI_Type_contiguous */    /*E0005*/

           curPtr = (unsigned char *)elem;

           PMPI_Type_extent(typeDts[0], &typeExtent);

           for(i=0; i < typeInts[0]; i++)
           {
              sum += _typeCheckSum(curPtr, typeDts[0], &fSubtypeDerived);
              curPtr += typeExtent;
           }

           if(fSubtypeDerived)
              PMPI_Type_free(&typeDts[0]);

           break;

      case MPI_COMBINER_VECTOR:       /* Type was created with
                                         MPI_Type_vector */    /*E0006*/

           PMPI_Type_extent(typeDts[0], &typeExtent);

           curPtr = (unsigned char *)elem;

           for(i=0; i < typeInts[0]; i++)
           {
              for(j=0; j < typeInts[1]; j++)
                  sum += _typeCheckSum(curPtr + typeExtent * j,
                                       typeDts[0], &fSubtypeDerived);

              curPtr += typeInts[2] * typeExtent;
           }

           if(fSubtypeDerived)
              PMPI_Type_free(&typeDts[0]);

           break;

      case MPI_COMBINER_HVECTOR:      /* Type was created with
                                         MPI_Type_hvector */    /*E0007*/

           PMPI_Type_extent(typeDts[0], &typeExtent);

           curPtr = (unsigned char *)elem;

           for(i=0; i < typeInts[0]; i++)
           {
              for(j=0; j < typeInts[1]; j++)
                  sum += _typeCheckSum(curPtr + typeExtent * j,
                                       typeDts[0], &fSubtypeDerived);

              curPtr += typeAints[0];
           }

           if(fSubtypeDerived)
              PMPI_Type_free(&typeDts[0]);

           break;

      case MPI_COMBINER_INDEXED:      /* Type was created with
                                         MPI_Type_indexed */    /*E0008*/

           PMPI_Type_extent(typeDts[0], &typeExtent);

           for(i=0; i < typeInts[0]; i++)
           {
              curPtr = (unsigned char *)elem +
                       typeInts[typeInts[0] + i + 1] * typeExtent;

              for(j=0; j < typeInts[i+1]; j++)
                  sum += _typeCheckSum(curPtr + typeExtent * j,
                                       typeDts[0], &fSubtypeDerived);

           }

           if(fSubtypeDerived)
              PMPI_Type_free(&typeDts[0]);

           break;

      case MPI_COMBINER_HINDEXED:      /* Type was created with
                                          MPI_Type_hindexed */    /*E0009*/

           PMPI_Type_extent(typeDts[0], &typeExtent);

           for(i=0; i < typeInts[0]; i++)
           {
              curPtr = (unsigned char *)elem + typeAints[i];

              for(j=0; j < typeInts[i+1]; j++)
                  sum += _typeCheckSum(curPtr + typeExtent * j,
                                       typeDts[0], &fSubtypeDerived);
           }

           if(fSubtypeDerived)
              PMPI_Type_free(&typeDts[0]);

           break;

      case MPI_COMBINER_STRUCT:      /* Type was created with
                                        MPI_Type_struct */    /*E0010*/

           for(i=0; i < typeInts[0]; i++)
           {
              PMPI_Type_extent(typeDts[i], &typeExtent);

              curPtr = (unsigned char *)elem + typeAints[i];

              for(j=0; j < typeInts[i+1]; j++)
                  sum += _typeCheckSum(curPtr + typeExtent * j,
                                       typeDts[i], &fSubtypeDerived);

              if(fSubtypeDerived)
                 PMPI_Type_free(&typeDts[i]);
           }

           break;

      default:
           _perror("Unknown combiner");
   }

   mac_free((void **)&typeInts);
   mac_free((void **)&typeAints);
   mac_free((void **)&typeDts);

   return sum;
}



static  unsigned   _checkSum(void  *buf, int  count, MPI_Datatype  type)
{
   unsigned    sum = 0;
   char       *curPtr = (char *) buf;
   int         fDerived, i;
   MPI_Aint    Len;

   PMPI_Type_extent(type, &Len);

   for(i=0; i < count; i++)
   {
      sum += _typeCheckSum(curPtr, type, &fDerived);
      curPtr += Len;
   }

   return sum;
}



static  int   _size(int  count, MPI_Datatype  type)
{
   int    typeSize, retcode;

   retcode = PMPI_Type_size(type, &typeSize);
   _assert(retcode, "MPI_Type_size");

   return   count*typeSize;
}



static  int   _extent(int  count, MPI_Datatype  type)
{
   int         retcode;
   MPI_Aint    Len;

   retcode = PMPI_Type_extent(type, &Len);
   _assert(retcode, "MPI_Type_size");

   return   count*Len;
}



static  int   _commWorldRank(int  rank, MPI_Comm  comm)
{
   int         fInter, retcode, rank2;
   MPI_Group   commGroup, worldGroup;

   if(rank == MPI_ANY_SOURCE || comm == MPI_COMM_WORLD)
      return  rank;

   if(comm == DVM_COMM_WORLD || comm == DVM_VMS->PS_MPI_COMM)
      return  ApplProcessNumber[rank];

   PMPI_Comm_test_inter(comm, &fInter);

   if(fInter)
   {
      retcode = PMPI_Comm_remote_group(comm, &commGroup);
      _assert(retcode, "MPI_Comm_remote_group");
   }
   else
   {
      retcode = PMPI_Comm_group(comm, &commGroup);
      _assert(retcode, "MPI_Comm_group");
   }

   retcode = PMPI_Comm_group(MPI_COMM_WORLD, &worldGroup);
   _assert(retcode, "MPI_Comm_group");
   retcode = PMPI_Group_translate_ranks(commGroup, 1, &rank, worldGroup,
                                        &rank2);
   _assert(retcode, "MPI_Group_Translate_ranks");

   return  rank2;
}



/* */    /*E0011*/

static  void   _commProcs(MPI_Comm  comm, int  *prQty, int  **procs)
{
   int         fInter, retcode, size, *ranks1, *ranks2, i;
   MPI_Group   commGroup, worldGroup;

   PMPI_Comm_test_inter(comm, &fInter);

   if(fInter)
   {
      retcode = PMPI_Comm_remote_group(comm, &commGroup);
      _assert(retcode, "MPI_Comm_remote_group");
   }
   else
   {
      retcode = PMPI_Comm_group(comm, &commGroup);
      _assert(retcode, "MPI_Comm_group");
   }

   retcode = PMPI_Comm_group(MPI_COMM_WORLD, &worldGroup);
   _assert(retcode, "MPI_Comm_group");

   retcode = PMPI_Group_size(commGroup, &size);
   _assert(retcode, "MPI_Group_size");

   i = sizeof(int)*size;
   mac_malloc(ranks1, int *, i, 0);
   mac_malloc(ranks2, int *, i, 0);

   for(i=0; i < size; i++)
       ranks1[i] = i;

   retcode = PMPI_Group_translate_ranks(commGroup, size, ranks1,
                                        worldGroup, ranks2);
   _assert(retcode, "MPI_Group_translate_ranks");

   mac_free((void **)&ranks1);

   *prQty = size;
   *procs = ranks2;
}



/* */    /*E0012*/

double  LastEventTime;

int  MPI_Init(int *argc, char ***argv)
{
   int     retval, argc1 = 1;
   DvmType    InitParam = 0;
   char   *name = "user", *argv1;

   MPI_ProfInitSign = 1;  /* */    /*E0013*/

   retval = PMPI_Init(argc, argv);

   if(RTS_Call_MPI == 0)
   {
      if(argc == NULL)
         argc1 = 1;
      else
         argc1 = *argc;

      if(argv == NULL)
         argv1 = name;
      else
         argv1 = **argv;          

      retval = (int)rtl_init(InitParam, argc1, &argv1);
   }

   LastEventTime = dvm_time();

   return  retval;
}


#ifdef _DVM_MPI2_

int  MPI_Init_thread(int  *argc, char  ***argv, int  required,
                     int  *provided)  /* MPI-2 !!! */    /*E0014*/
{
   int     retval, argc1 = 1;
   DvmType    InitParam = 0;
   char   *name = "user", *argv1;

   MPI_ProfInitSign = 1;  /* */    /*E0015*/

   retval = PMPI_Init_thread(argc, argv, required, provided);

   dvm_init_thread = 1;      /* */    /*E0016*/
   dvm_required = required;  /* */    /*E0017*/
   dvm_provided = *provided; /* */    /*E0018*/

   if(RTS_Call_MPI == 0)
   {
      if(argc == NULL)
         argc1 = 1;
      else
         argc1 = *argc;

      if(argv == NULL)
         argv1 = name;
      else
         argv1 = **argv;          

      retval = (int)rtl_init(InitParam, argc1, &argv1);
   }

   return  retval;
}

#endif



int  MPI_Initialized(int  *flag)
{
   int   retval;
   int   system, trace;

   system = (RTS_Call_MPI || DVMCallLevel);
   trace  = (MPI_BotsulaProf && (MPI_TraceLevel || system == 0));

   retval = PMPI_Initialized(flag);

   return  retval;
}



/* */    /*E0019*/

int  MPI_Finalize(void)
{
   int         retval = 0;
   int         system, trace, debug;
   double      start, finish;

   system = (RTS_Call_MPI || DVMCallLevel);
   trace  = (MPI_BotsulaProf && (MPI_TraceLevel || system == 0));
   debug  = (MPI_BotsulaDeb && (MPI_TraceLevel || system == 0));

   if(debug || trace)
   {
      if(trace)
      {
         if(MPI_SlashOut)
            tprintf("\n");  

         tprintf("$call_MPI_Finalize\t");

         if(MPI_TraceTime)
         {
            start = dvm_time();

            if(MPI_TraceTimeReg)
            {
               tprintf("t=%lf\t", start-LastEventTime);
               LastEventTime = start;
            }
            else
               tprintf("t=%lf\t", start);
         }
  
         if(MPI_TraceFileLine)
            tprintf("f=%s\tl=%ld\t", DVM_FILE[0], DVM_LINE[0]);

         tprintf("$\n");
      }
   }

   RTS_Call_MPI = 1;

#ifdef _MPI_PROF_TRAN_

   if(1 /*CallDbgCond*/    /*E0020*/ /*EnableTrace && dvm_OneProcSign*/    /*E0021*/)
      SYSTEM(MPI_Finalize, ())
   else
   {
      if(RTS_Call_MPI == 0)
         dvm_exit(0);
      else
         retval = PMPI_Finalize();
   }

#else

   if(RTS_Call_MPI == 0)
      dvm_exit(0);
   else
      retval = PMPI_Finalize();

#endif

   if(debug || trace)
   {
      if(trace)
      {
         tprintf("$ret_MPI_Finalize\t");

         if(MPI_TraceTime)
         {
            finish = dvm_time();

            if(MPI_TraceTimeReg)
            {
               tprintf("t=%lf\t", finish-LastEventTime);
               LastEventTime = finish;
            }
            else
               tprintf("t=%lf\t", finish);
         }

         tprintf("$\n");
      }
   }

   return  retval;
}


int  MPI_Abort(MPI_Comm  comm, int  errorcode)
{
   int         retval;
   int         system, trace, debug;
   double      start, finish;
   commInfo   *commInfoPtr;
   MPI_Comm    c;   

   system = (RTS_Call_MPI || DVMCallLevel);
   trace  = (MPI_BotsulaProf && (MPI_TraceLevel || system == 0));
   debug  = (MPI_BotsulaDeb && (MPI_TraceLevel || system == 0));
   retval = (system == 0 && comm == MPI_COMM_WORLD && dvm_OneProcSign == 0);

   if(retval)
      c = DVM_COMM_WORLD;
   else
      c = comm;

   if(debug || trace)
   {
      getcomm(c, &commInfoPtr);

      if(commInfoPtr == NULL)
         epprintf(1 ,__FILE__, __LINE__,
                  "$*** dynamic analyzer err: wrong call MPI_Abort "
                  "(non-existent communicator %u)$\n", comm);

      if(trace)
      {
         if(MPI_SlashOut)
            tprintf("\n");

         tprintf("$call_MPI_Abort\tcomm=%u\terrcode=%d\t", c, errorcode);

         if(MPI_TraceTime)
         {
            start = dvm_time();

            if(MPI_TraceTimeReg)
            {
               tprintf("t=%lf\t", start-LastEventTime);
               LastEventTime = start;
            }
            else
               tprintf("t=%lf\t", start);
         }

         if(MPI_TraceFileLine)
            tprintf("f=%s\tl=%ld\t", DVM_FILE[0], DVM_LINE[0]);

         tprintf("$\n");
      }
   }

   RTS_Call_MPI = 1;

#ifdef _MPI_PROF_TRAN_

   if(1 /*CallDbgCond*/    /*E0022*/ /*EnableTrace && dvm_OneProcSign*/    /*E0023*/)
      SYSTEM(MPI_Finalize, ())
   else
   {
      if(RTS_Call_MPI == 0)
      {
         retval = errorcode;
         dvm_exit(errorcode);
      }
      else
         retval = PMPI_Abort(c, errorcode);
   }

#else

   if(RTS_Call_MPI == 0)
   {
      retval = errorcode;
      dvm_exit(errorcode);
   }
   else
      retval = PMPI_Abort(c, errorcode);

#endif

   if(debug || trace)
   {
      if(trace)
      {
         tprintf("$ret_MPI_Abort\t");

         if(MPI_TraceTime)
         {
            finish = dvm_time();

            if(MPI_TraceTimeReg)
            {
               tprintf("t=%lf\t", finish-LastEventTime);
               LastEventTime = finish;
            }
            else
               tprintf("t=%lf\t", finish);
         }

         tprintf("$\n");
      }
   }

   return  retval;
}



#ifdef _DVM_MPI2_

DVMUSERFUN
int  PMPI_Finalized(int  *flag);

int  MPI_Finalized(int  *flag)
{
   int   retval;
   int   system, trace;

   system = (RTS_Call_MPI || DVMCallLevel);
   trace  = (MPI_BotsulaProf && (MPI_TraceLevel || system == 0));

   retval = PMPI_Finalized(flag);
   return  retval;
}

#endif


/*******************\
*   Time function   *
\*******************/    /*E0024*/

double  MPI_Wtime(void)
{
   double  retval;
   int     system;

   system = (RTS_Call_MPI || DVMCallLevel);

   retval = PMPI_Wtime();
   return  retval;
}



double  MPI_Wtick(void)
{
   double  retval;
   int     system;

   system = (RTS_Call_MPI || DVMCallLevel);

   retval = PMPI_Wtick();
   return  retval;
}



/***************************************\
*   Blocking Message Passing Routines   *
\***************************************/    /*E0025*/

 
int  MPI_Send(void  *buf, int  count, MPI_Datatype  datatype, int  dest,
              int  tag, MPI_Comm  comm)
{
   int         retval;
   double      start, finish;
   int         system, trace, debug;
   commInfo   *commInfoPtr;
   MPI_Comm    c;
   unsigned    checksum;

   system = (RTS_Call_MPI || DVMCallLevel);
   trace  = (MPI_BotsulaProf && (MPI_TraceLevel || system == 0));
   debug  = (MPI_BotsulaDeb && (MPI_TraceLevel || system == 0));
   retval = (system == 0 && comm == MPI_COMM_WORLD && dvm_OneProcSign == 0);

   if(retval)
      c = DVM_COMM_WORLD;
   else
      c = comm;

   if(debug || trace)
   {
      getcomm(c, &commInfoPtr);

      if(commInfoPtr == NULL)
         epprintf(1 ,__FILE__, __LINE__,
                  "$*** dynamic analyzer err: wrong call MPI_Send "
                  "(non-existent communicator %u)$\n", comm);

      if(dest < 0 || dest >= commInfoPtr->pcount)
         epprintf(1 ,__FILE__, __LINE__,
                  "$*** dynamic analyzer err: wrong call MPI_Send "
                  "(incorrect dest %d)$\n", dest);

      if(MPI_TraceMsgChecksum || MPI_DebugMsgChecksum)
         checksum = _checkSum(buf, count, datatype);

      if(trace)
      {
        if(MPI_SlashOut)
           tprintf("\n");

        tprintf("$call_MPI_Send\tbuf=%lx\tcount=%d\tdtype=%u\t"
                "size=%d\t\ndest=%d\tidest=%d\ttag=%d\tcomm=%u\t\n",
                (uLLng)buf, count, datatype, _size(count, datatype),
                dest, _commWorldRank(dest, c), tag, c);

        if(MPI_TraceMsgChecksum)
           tprintf("sum=%u\t", checksum);

        if(MPI_TraceTime)
        {  start = dvm_time();

           if(MPI_TraceTimeReg)
           {  tprintf("t=%lf\t", start-LastEventTime);
              LastEventTime = start;
           }
           else
              tprintf("t=%lf\t", start);
        }

        if(MPI_TraceFileLine)
           tprintf("f=%s\tl=%ld\t", DVM_FILE[0], DVM_LINE[0]);
        tprintf("$\n");
      }
   }

   retval = PMPI_Send(buf, count, datatype, dest, tag, c);

   if(trace)
   {
      tprintf("$ret_MPI_Send\t");

      if(MPI_TraceTime)
      {  finish = dvm_time();

         if(MPI_TraceTimeReg)
         {  tprintf("t=%lf\t", finish-LastEventTime);
            LastEventTime = finish;
         }
         else
            tprintf("t=%lf\t", finish);
      } 

      tprintf("$\n");
   }

   return  retval;
}



int  MPI_Bsend(void  *buf, int  count, MPI_Datatype  datatype, int  dest,
               int  tag, MPI_Comm  comm)
{
   int         retval;
   double      start, finish;
   int         system, trace, debug;
   commInfo   *commInfoPtr;
   MPI_Comm    c;
   unsigned    checksum;

   system = (RTS_Call_MPI || DVMCallLevel);
   trace  = (MPI_BotsulaProf && (MPI_TraceLevel || system == 0));
   debug  = (MPI_BotsulaDeb && (MPI_TraceLevel || system == 0));
   retval = (system == 0 && comm == MPI_COMM_WORLD && dvm_OneProcSign == 0);

   if(retval)
      c = DVM_COMM_WORLD;
   else
      c = comm;

   if(debug || trace)
   {
      getcomm(c, &commInfoPtr);

      if(commInfoPtr == NULL)
         epprintf(1 ,__FILE__, __LINE__,
                  "$*** dynamic analyzer err: wrong call MPI_Bsend "
                  "(non-existent communicator %u)$\n", comm);

      if(dest < 0 || dest >= commInfoPtr->pcount)
         epprintf(1 ,__FILE__, __LINE__,
                  "$*** dynamic analyzer err: wrong call MPI_Bsend "
                  "(incorrect dest %d)$\n", dest);

      if(MPI_TraceMsgChecksum || MPI_DebugMsgChecksum)
         checksum = _checkSum(buf, count, datatype);

      if(trace)
      {
        if(MPI_SlashOut)
           tprintf("\n");

        tprintf("$call_MPI_Bsend\tbuf=%lx\tcount=%d\tdtype=%u\t"
                "size=%d\t\ndest=%d\tidest=%d\ttag=%d\tcomm=%u\t\n",
                (uLLng)buf, count, datatype, _size(count, datatype),
                dest, _commWorldRank(dest, c), tag, c);

        if(MPI_TraceMsgChecksum)
           tprintf("sum=%u\t", checksum);

        if(MPI_TraceTime)
        {  start = dvm_time();

           if(MPI_TraceTimeReg)
           {  tprintf("t=%lf\t", start-LastEventTime);
              LastEventTime = start;
           }
           else
              tprintf("t=%lf\t", start);
        }

        if(MPI_TraceFileLine)
           tprintf("f=%s\tl=%ld\t", DVM_FILE[0], DVM_LINE[0]);
        tprintf("$\n");
      }
   }

   retval = PMPI_Bsend(buf, count, datatype, dest, tag, c);

   if(trace)
   {
      tprintf("$ret_MPI_Bsend\t");

      if(MPI_TraceTime)
      {  finish = dvm_time();

         if(MPI_TraceTimeReg)
         {  tprintf("t=%lf\t", finish-LastEventTime);
            LastEventTime = finish;
         }
         else
            tprintf("t=%lf\t", finish);
      }

      tprintf("$\n");
   }

   return  retval;
}



int  MPI_Ssend(void  *buf, int  count, MPI_Datatype  datatype, int  dest,
              int  tag, MPI_Comm  comm)
{
   int         retval;
   double      start, finish;
   int         system, trace, debug;
   commInfo   *commInfoPtr;
   MPI_Comm    c;
   unsigned    checksum;

   system = (RTS_Call_MPI || DVMCallLevel);
   trace  = (MPI_BotsulaProf && (MPI_TraceLevel || system == 0));
   debug  = (MPI_BotsulaDeb && (MPI_TraceLevel || system == 0));
   retval = (system == 0 && comm == MPI_COMM_WORLD && dvm_OneProcSign == 0);

   if(retval)
      c = DVM_COMM_WORLD;
   else
      c = comm;

   if(debug || trace)
   {
      getcomm(c, &commInfoPtr);

      if(commInfoPtr == NULL)
         epprintf(1 ,__FILE__, __LINE__,
                  "$*** dynamic analyzer err: wrong call MPI_Ssend "
                  "(non-existent communicator %u)$\n", comm);

      if(dest < 0 || dest >= commInfoPtr->pcount)
         epprintf(1 ,__FILE__, __LINE__,
                  "$*** dynamic analyzer err: wrong call MPI_Ssend "
                  "(incorrect dest %d)$\n", dest);

      if(MPI_TraceMsgChecksum || MPI_DebugMsgChecksum)
         checksum = _checkSum(buf, count, datatype);

      if(trace)
      {
        if(MPI_SlashOut)
           tprintf("\n");

        tprintf("$call_MPI_Ssend\tbuf=%lx\tcount=%d\tdtype=%u\t"
                "size=%d\t\ndest=%d\tidest=%d\ttag=%d\tcomm=%u\t\n",
                (uLLng)buf, count, datatype, _size(count, datatype),
                dest, _commWorldRank(dest, c), tag, c);

        if(MPI_TraceMsgChecksum)
           tprintf("sum=%u\t", checksum);

        if(MPI_TraceTime)
        {  start = dvm_time();

           if(MPI_TraceTimeReg)
           {  tprintf("t=%lf\t", start-LastEventTime);
              LastEventTime = start;
           }
           else
              tprintf("t=%lf\t", start);
        }

        if(MPI_TraceFileLine)
           tprintf("f=%s\tl=%ld\t", DVM_FILE[0], DVM_LINE[0]);
        tprintf("$\n");
      }
   }

   retval = PMPI_Ssend(buf, count, datatype, dest, tag, c);

   if(trace)
   {
      tprintf("$ret_MPI_Ssend\t");

      if(MPI_TraceTime)
      {  finish = dvm_time();

         if(MPI_TraceTimeReg)
         {  tprintf("t=%lf\t", finish-LastEventTime);
            LastEventTime = finish;
         }
         else
            tprintf("t=%lf\t", finish);
      }

      tprintf("$\n");
   }

   return  retval;
}



int  MPI_Rsend(void  *buf, int  count, MPI_Datatype  datatype, int  dest,
               int  tag, MPI_Comm  comm)
{
   int         retval;
   double      start, finish;
   int         system, trace, debug;
   commInfo   *commInfoPtr;
   MPI_Comm    c;
   unsigned    checksum;

   system = (RTS_Call_MPI || DVMCallLevel);
   trace  = (MPI_BotsulaProf && (MPI_TraceLevel || system == 0));
   debug  = (MPI_BotsulaDeb && (MPI_TraceLevel || system == 0));
   retval = (system == 0 && comm == MPI_COMM_WORLD && dvm_OneProcSign == 0);

   if(retval)
      c = DVM_COMM_WORLD;
   else
      c = comm;

   if(debug || trace)
   {
      getcomm(c, &commInfoPtr);

      if(commInfoPtr == NULL)
         epprintf(1 ,__FILE__, __LINE__,
                  "$*** dynamic analyzer err: wrong call MPI_Rsend "
                  "(non-existent communicator %u)$\n", comm);

      if(dest < 0 || dest >= commInfoPtr->pcount)
         epprintf(1 ,__FILE__, __LINE__,
                  "$*** dynamic analyzer err: wrong call MPI_Rsend "
                  "(incorrect dest %d)$\n", dest);

      if(MPI_TraceMsgChecksum || MPI_DebugMsgChecksum)
         checksum = _checkSum(buf, count, datatype);

      if(trace)
      {
        if(MPI_SlashOut)
           tprintf("\n");

        tprintf("$call_MPI_Rsend\tbuf=%lx\tcount=%d\tdtype=%u\t"
                "size=%d\t\ndest=%d\tidest=%d\ttag=%d\tcomm=%u\t\n",
                (uLLng)buf, count, datatype, _size(count, datatype),
                dest, _commWorldRank(dest, c), tag, c);

        if(MPI_TraceMsgChecksum)
           tprintf("sum=%u\t", checksum);

        if(MPI_TraceTime)
        {  start = dvm_time();

           if(MPI_TraceTimeReg)
           {  tprintf("t=%lf\t", start-LastEventTime);
              LastEventTime = start;
           }
           else
              tprintf("t=%lf\t", start);
        }

        if(MPI_TraceFileLine)
           tprintf("f=%s\tl=%ld\t", DVM_FILE[0], DVM_LINE[0]);
        tprintf("$\n");
      }
   }

   retval = PMPI_Rsend(buf, count, datatype, dest, tag, c);

   if(trace)
   {
      tprintf("$ret_MPI_Rsend\t");

      if(MPI_TraceTime)
      {  finish = dvm_time();

         if(MPI_TraceTimeReg)
         {  tprintf("t=%lf\t", finish-LastEventTime);
            LastEventTime = finish;
         }
         else
            tprintf("t=%lf\t", finish);
      }

      tprintf("$\n");
   }

   return  retval;
}



int  MPI_Recv(void  *buf, int  count, MPI_Datatype  datatype,
              int  source, int  tag, MPI_Comm  comm, MPI_Status  *status)
{
   int         retval;
   double      start, finish;
   int         system, trace, debug, post_count;
   commInfo   *commInfoPtr;
   MPI_Comm    c;
   unsigned    checksum;

   system = (RTS_Call_MPI || DVMCallLevel);
   trace  = (MPI_BotsulaProf && (MPI_TraceLevel || system == 0));
   debug  = (MPI_BotsulaDeb && (MPI_TraceLevel || system == 0));
   retval = (system == 0 && comm == MPI_COMM_WORLD && dvm_OneProcSign == 0);

   if(retval)
      c = DVM_COMM_WORLD;
   else
      c = comm;

   if(debug || trace)
   {
      getcomm(c, &commInfoPtr);

      if(commInfoPtr == NULL)
         epprintf(1 ,__FILE__, __LINE__,
                  "$*** dynamic analyzer err: wrong call MPI_Recv "
                  "(non-existent communicator %u)$\n", comm);

      if(source != MPI_ANY_SOURCE &&
         (source < 0 || source >= commInfoPtr->pcount))
         epprintf(1 ,__FILE__, __LINE__,
                  "$*** dynamic analyzer err: wrong call MPI_Recv "
                  "(incorrect source %d)$\n", source);

      if(trace)
      {
         if(MPI_SlashOut)
            tprintf("\n");

         tprintf("$call_MPI_Recv\tbuf=%lx\tcount=%d\tdtype=%u\t"
                 "size=%d\t\nsource=%d\tisource=%d\ttag=%d\tcomm=%u\t\n",
                 (uLLng)buf, count, datatype, _size(count, datatype),
                 source, _commWorldRank(source, c), tag, c);

         if(MPI_TraceTime)
         {  start = dvm_time();

            if(MPI_TraceTimeReg)
            {  tprintf("t=%lf\t", start-LastEventTime);
               LastEventTime = start;
            }
            else
               tprintf("t=%lf\t", start);
         }

         if(MPI_TraceFileLine)
            tprintf("f=%s\tl=%ld\t", DVM_FILE[0], DVM_LINE[0]);
         tprintf("$\n");
      }
   }

   retval = PMPI_Recv(buf, count, datatype, source, tag, c, status);

   if(debug || trace)
   {
      PMPI_Get_count(status, datatype, &post_count);

      if(MPI_TraceMsgChecksum || MPI_DebugMsgChecksum)
         checksum = _checkSum(buf, post_count, datatype);

      if(trace)
      {
         tprintf("$ret_MPI_Recv\tcount=%d\tsize=%d\t"
                 "source=%d\tisource=%d\t\ntag=%d\t",
                 post_count, _size(post_count, datatype),
                 status->MPI_SOURCE,
                 _commWorldRank(status->MPI_SOURCE, c), status->MPI_TAG);

         if(MPI_TraceMsgChecksum)
            tprintf("sum=%u\t", checksum);

         if(MPI_TraceTime)
         {  finish = dvm_time();

            if(MPI_TraceTimeReg)
            {  tprintf("t=%lf\t", finish-LastEventTime);
               LastEventTime = finish;
            }
            else
               tprintf("t=%lf\t", finish);
         }

         tprintf("$\n");
      }
   }

   return  retval;
}



int  MPI_Probe(int  source, int  tag, MPI_Comm  comm,
               MPI_Status  *status)
{
   int         retval;
   double      start, finish;
   int         system, trace, debug;
   commInfo   *commInfoPtr;
   MPI_Comm    c;

   system = (RTS_Call_MPI || DVMCallLevel);
   trace  = (MPI_BotsulaProf && (MPI_TraceLevel || system == 0));
   debug  = (MPI_BotsulaDeb && (MPI_TraceLevel || system == 0));
   retval = (system == 0 && comm == MPI_COMM_WORLD && dvm_OneProcSign == 0);

   if(retval)
      c = DVM_COMM_WORLD;
   else
      c = comm;

   if(debug || trace)
   {
      getcomm(c, &commInfoPtr);

      if(commInfoPtr == NULL)
         epprintf(1 ,__FILE__, __LINE__,
                  "$*** dynamic analyzer err: wrong call MPI_Probe "
                  "(non-existent communicator %u)$\n", comm);

      if(source != MPI_ANY_SOURCE &&
         (source < 0 || source >= commInfoPtr->pcount))
         epprintf(1 ,__FILE__, __LINE__,
                  "$*** dynamic analyzer err: wrong call MPI_Probe "
                  "(incorrect source %d)$\n", source);

      if(trace)
      {
         if(MPI_SlashOut)
            tprintf("\n");

         tprintf("$call_MPI_Probe\tsource=%d\tisource=%d\ttag=%d\t"
                 "comm=%u\t\n",
                 source, _commWorldRank(source, c), tag, c);

         if(MPI_TraceTime)
         {  start = dvm_time();

            if(MPI_TraceTimeReg)
            {  tprintf("t=%lf\t", start-LastEventTime);
               LastEventTime = start;
            }
            else
               tprintf("t=%lf\t", start);
         }

         if(MPI_TraceFileLine)
            tprintf("f=%s\tl=%ld\t", DVM_FILE[0], DVM_LINE[0]);
         tprintf("$\n");
      }
   }

   retval = PMPI_Probe(source, tag, c, status);

   if(debug || trace)
   {
      if(trace)
      {
         tprintf("$ret_MPI_Probe\tcount=%d\t"
                 "source=%d\tisource=%d\ttag=%d\t",
                 status->count, status->MPI_SOURCE,
                 _commWorldRank(status->MPI_SOURCE, c),
                 status->MPI_TAG);

         if(MPI_TraceTime)
         {  finish = dvm_time();

            if(MPI_TraceTimeReg)
            {  tprintf("\nt=%lf\t", finish-LastEventTime);
               LastEventTime = finish;
            }
            else
               tprintf("\nt=%lf\t", finish);
         }

         tprintf("$\n");
      }
   }

   return  retval;
}



int  MPI_Sendrecv(void  *sendbuf, int  sendcount, MPI_Datatype  sendtype,
                  int  dest, int  sendtag, void  *recvbuf,
                  int  recvcount, MPI_Datatype  recvtype, int  source,
                  int  recvtag, MPI_Comm  comm, MPI_Status  *status)
{
   int         retval;
   double      start, finish;
   int         system, trace, debug, post_count;
   commInfo   *commInfoPtr;
   MPI_Comm    c;
   unsigned    checksum;

   system = (RTS_Call_MPI || DVMCallLevel);
   trace  = (MPI_BotsulaProf && (MPI_TraceLevel || system == 0));
   debug  = (MPI_BotsulaDeb && (MPI_TraceLevel || system == 0));
   retval = (system == 0 && comm == MPI_COMM_WORLD && dvm_OneProcSign == 0);

   if(retval)
      c = DVM_COMM_WORLD;
   else
      c = comm;

   if(debug || trace)
   {
      getcomm(c, &commInfoPtr);

      if(commInfoPtr == NULL)
         epprintf(1 ,__FILE__, __LINE__,
                  "$*** dynamic analyzer err: wrong call MPI_Sendrecv "
                  "(non-existent communicator %u)$\n", comm);

      if(dest < 0 || dest >= commInfoPtr->pcount)
         epprintf(1 ,__FILE__, __LINE__,
                  "$*** dynamic analyzer err: wrong call MPI_Sendrecv "
                  "(incorrect dest %d)$\n", dest);

      if(source != MPI_ANY_SOURCE &&
         (source < 0 || source >= commInfoPtr->pcount))
         epprintf(1 ,__FILE__, __LINE__,
                  "$*** dynamic analyzer err: wrong call MPI_Sendrecv "
                  "(incorrect source %d)$\n", source);

      if(MPI_TraceMsgChecksum || MPI_DebugMsgChecksum)
         checksum = _checkSum(sendbuf, sendcount, sendtype);

      if(trace)
      {
        if(MPI_SlashOut)
           tprintf("\n");

        tprintf("$call_MPI_Sendrecv\tsbuf=%lx\tscount=%d\tsdtype=%u\t"
                "ssize=%d\t\ndest=%d\tidest=%d\tstag=%d\tcomm=%u\t",
                (uLLng)sendbuf, sendcount, sendtype,
                _size(sendcount, sendtype), dest,
                _commWorldRank(dest, c), sendtag, c);

        if(MPI_TraceMsgChecksum)
           tprintf("ssum=%u\t\n", checksum);

        tprintf("\nrbuf=%lx\trcount=%d\trdtype=%u\trsize=%d\t"
                "source=%d\tisource=%d\t\nrtag=%d\t",
                (uLLng)recvbuf, recvcount, recvtype,
                _size(recvcount, recvtype), source,
                _commWorldRank(source, c), recvtag);

        if(MPI_TraceTime)
        {  start = dvm_time();

           if(MPI_TraceTimeReg)
           {  tprintf("t=%lf\t", start-LastEventTime);
              LastEventTime = start;
           }
           else
              tprintf("t=%lf\t", start);
        }

        if(MPI_TraceFileLine)
           tprintf("f=%s\tl=%ld\t", DVM_FILE[0], DVM_LINE[0]);
        tprintf("$\n");
      }
   }

   retval = PMPI_Sendrecv(sendbuf, sendcount, sendtype, dest, sendtag,
                          recvbuf, recvcount, recvtype, source, recvtag,
                          c, status);

   if(debug || trace)
   {
     PMPI_Get_count(status, recvtype, &post_count);

     if(MPI_TraceMsgChecksum || MPI_DebugMsgChecksum)
        checksum = _checkSum(recvbuf, post_count, recvtype);

     if(trace)
     {
        tprintf("$ret_MPI_Sendrecv\trcount=%d\trsize=%d\t"
                "source=%d\tisource=%d\t\nrtag=%d\t",
                post_count, _size(post_count, recvtype),
                status->MPI_SOURCE,
                _commWorldRank(status->MPI_SOURCE, c),
                status->MPI_TAG);

        if(MPI_TraceMsgChecksum)
           tprintf("rsum=%u\t", checksum);

        if(MPI_TraceTime)
        {  finish = dvm_time();

           if(MPI_TraceTimeReg)
           {  tprintf("t=%lf\t", finish-LastEventTime);
              LastEventTime = finish;
           }
           else
              tprintf("t=%lf\t", finish);
        }

        tprintf("$\n");
     }
   }

   return  retval;
}



int  MPI_Sendrecv_replace(void  *buf, int  count, MPI_Datatype  datatype,
                          int  dest, int  sendtag, int  source,
                          int  recvtag, MPI_Comm  comm,
                          MPI_Status  *status)
{
   int         retval;
   double      start, finish;
   int         system, trace, debug, post_count;
   commInfo   *commInfoPtr;
   MPI_Comm    c;
   unsigned    checksum;

   system = (RTS_Call_MPI || DVMCallLevel);
   trace  = (MPI_BotsulaProf && (MPI_TraceLevel || system == 0));
   debug  = (MPI_BotsulaDeb && (MPI_TraceLevel || system == 0));
   retval = (system == 0 && comm == MPI_COMM_WORLD && dvm_OneProcSign == 0);

   if(retval)
      c = DVM_COMM_WORLD;
   else
      c = comm;

   if(debug || trace)
   {
      getcomm(c, &commInfoPtr);

      if(commInfoPtr == NULL)
         epprintf(1 ,__FILE__, __LINE__,
           "$*** dynamic analyzer err: wrong call MPI_Sendrecv_replace "
           "(non-existent communicator %u)$\n", comm);

      if(dest < 0 || dest >= commInfoPtr->pcount)
         epprintf(1 ,__FILE__, __LINE__,
           "$*** dynamic analyzer err: wrong call MPI_Sendrecv_replace "
           "(incorrect dest %d)$\n", dest);

      if(source != MPI_ANY_SOURCE &&
         (source < 0 || source >= commInfoPtr->pcount))
         epprintf(1 ,__FILE__, __LINE__,
           "$*** dynamic analyzer err: wrong call MPI_Sendrecv_replace "
           "(incorrect source %d)$\n", source);

      if(MPI_TraceMsgChecksum || MPI_DebugMsgChecksum)
         checksum = _checkSum(buf, count, datatype);

      if(trace)
      {
        if(MPI_SlashOut)
           tprintf("\n");

        tprintf("$call_MPI_Sendrecv_replace\tbuf=%lx\tcount=%d\t"
                "dtype=%u\tsize=%d\t\ndest=%d\tidest=%d\tstag=%d\t"
                "comm=%u\t",
                (uLLng)buf, count, datatype, _size(count, datatype),
                dest, _commWorldRank(dest, c), sendtag, c);

        if(MPI_TraceMsgChecksum)
           tprintf("ssum=%u\t", checksum);

        tprintf("\nsource=%d\tisource=%d\trtag=%d\t\n",
                source, _commWorldRank(source, c), recvtag);

        if(MPI_TraceTime)
        {  start = dvm_time();

           if(MPI_TraceTimeReg)
           {  tprintf("t=%lf\t", start-LastEventTime);
              LastEventTime = start;
           }
           else
              tprintf("t=%lf\t", start);
        }

        if(MPI_TraceFileLine)
           tprintf("f=%s\tl=%ld\t", DVM_FILE[0], DVM_LINE[0]);
        tprintf("$\n");
      }
   }

   retval = PMPI_Sendrecv_replace(buf, count, datatype, dest, sendtag,
                                  source, recvtag, c,
                                  status);

   if(debug || trace)
   {
     PMPI_Get_count(status, datatype, &post_count);

     if(MPI_TraceMsgChecksum || MPI_DebugMsgChecksum)
        checksum = _checkSum(buf, post_count, datatype);

     if(trace)
     {
        tprintf("$ret_MPI_Sendrecv_replace\trcount=%d\trsize=%d\t"
                "source=%d\tisource=%d\t\nrtag=%d\t",
                post_count, _size(post_count, datatype),
                status->MPI_SOURCE,
                _commWorldRank(status->MPI_SOURCE, c), status->MPI_TAG);

        if(MPI_TraceMsgChecksum)
           tprintf("rsum=%u\t", checksum);

        if(MPI_TraceTime)
        {  finish = dvm_time();

           if(MPI_TraceTimeReg)
           {  tprintf("t=%lf\t", finish-LastEventTime);
              LastEventTime = finish;
           }
           else
              tprintf("t=%lf\t", finish);
        }

        tprintf("$\n");
     }
   }

   return  retval;
}



/*******************************************\
*   Non-Blocking Message Passing Routines   *
\*******************************************/    /*E0026*/


/**************************\
*   MPI_Ixxxxx functions   *
\**************************/    /*E0027*/

int  MPI_Isend(void  *buf, int  count, MPI_Datatype  datatype, int  dest,
               int  tag, MPI_Comm  comm, MPI_Request  *request)
{
   int             retval;
   double          start, finish;
   int             system, trace, debug, mpi_dest_rank, index;
   nonblockInfo   *infPtr;
   unsigned        checksum; 
   commInfo       *commInfoPtr;
   MPI_Comm        c;

   system = (RTS_Call_MPI || DVMCallLevel);
   trace  = (MPI_BotsulaProf && (MPI_TraceLevel || system == 0));
   debug  = (MPI_BotsulaDeb && (MPI_TraceLevel || system == 0));
   retval = (system == 0 && comm == MPI_COMM_WORLD && dvm_OneProcSign == 0);

   if(retval)
      c = DVM_COMM_WORLD;
   else
      c = comm;

   if(debug || trace)
   {
      getcomm(c, &commInfoPtr);

      if(commInfoPtr == NULL)
         epprintf(1 ,__FILE__, __LINE__,
                  "$*** dynamic analyzer err: wrong call MPI_Isend "
                  "(non-existent communicator %u)$\n", comm);

      if(dest < 0 || dest >= commInfoPtr->pcount)
         epprintf(1 ,__FILE__, __LINE__,
                  "$*** dynamic analyzer err: wrong call MPI_Isend "
                  "(incorrect dest %d)$\n", dest);

      index = coll_IndexOf(RequestColl, request);

      if(index >= 0)      
         epprintf(1 ,__FILE__, __LINE__,
                  "$*** dynamic analyzer err: wrong call MPI_Isend "
                  "(request %lx already exists)$\n", (uLLng)request);

      if(MPI_TraceMsgChecksum || MPI_DebugBufChecksum ||
         MPI_DebugMsgChecksum)
         checksum = _checkSum(buf, count, datatype);
      else
         checksum = 0;

      mpi_dest_rank = _commWorldRank(dest, c);

      coll_Insert(RequestColl, request);
      dvm_AllocStruct(nonblockInfo, infPtr);
      coll_Insert(ReqStructColl, infPtr);

      infPtr->messageTag = tag;
      infPtr->messageSource = MPS_CurrentProcIdent;
      infPtr->messageDest = mpi_dest_rank;
      infPtr->isSend = 1;
      infPtr->buffer = buf;
      infPtr->count = count;
      infPtr->datatype = datatype;
      infPtr->isStarted = 1;
      infPtr->isInit = 0;
      infPtr->checksum = checksum;
      infPtr->MPITestCount = MPI_TestTraceCount;
      infPtr->TotalTestCount = 0;
      infPtr->comm = c;

      if(trace)
      {
        if(MPI_SlashOut)
           tprintf("\n");

        tprintf("$call_MPI_Isend\tbuf=%lx\tcount=%d\tdtype=%u\t"
                "size=%d\t\ndest=%d\tidest=%d\ttag=%d\tcomm=%u\t",
                (uLLng)buf, count, datatype, _size(count, datatype),
                dest, mpi_dest_rank, tag, c);

        if(MPI_TraceMsgChecksum)
           tprintf("sum=%u\t", checksum);

        tprintf("\nreq=%lx\t", (uLLng)request);

        if(MPI_TraceTime)
        {  start = dvm_time();

           if(MPI_TraceTimeReg)
           {  tprintf("t=%lf\t", start-LastEventTime);
              LastEventTime = start;
           }
           else
              tprintf("t=%lf\t", start);
        }

        if(MPI_TraceFileLine)
           tprintf("f=%s\tl=%ld\t", DVM_FILE[0], DVM_LINE[0]);
        tprintf("$\n");
      }
   }

   retval = PMPI_Isend(buf, count, datatype, dest, tag, c, request);

   if(trace)
   {
      tprintf("$ret_MPI_Isend\t");

      if(MPI_TraceTime)
      {  finish = dvm_time();

         if(MPI_TraceTimeReg)
         {  tprintf("t=%lf\t", finish-LastEventTime);
            LastEventTime = finish;
         }
         else
            tprintf("t=%lf\t", finish);
      }

      tprintf("$\n");
   }

   return  retval;
}



int  MPI_Ibsend(void  *buf, int  count, MPI_Datatype  datatype,
                int  dest, int  tag, MPI_Comm  comm,
                MPI_Request  *request)
{
   int             retval;
   double          start, finish;
   int             system, trace, debug, mpi_dest_rank, index;
   nonblockInfo   *infPtr;
   unsigned        checksum; 
   commInfo       *commInfoPtr;
   MPI_Comm        c;   

   system = (RTS_Call_MPI || DVMCallLevel);
   trace  = (MPI_BotsulaProf && (MPI_TraceLevel || system == 0));
   debug  = (MPI_BotsulaDeb && (MPI_TraceLevel || system == 0));
   retval = (system == 0 && comm == MPI_COMM_WORLD && dvm_OneProcSign == 0);

   if(retval)
      c = DVM_COMM_WORLD;
   else
      c = comm;

   if(debug || trace)
   {
      getcomm(c, &commInfoPtr);

      if(commInfoPtr == NULL)
         epprintf(1 ,__FILE__, __LINE__,
                  "$*** dynamic analyzer err: wrong call MPI_Ibsend "
                  "(non-existent communicator %u)$\n", comm);

      if(dest < 0 || dest >= commInfoPtr->pcount)
         epprintf(1 ,__FILE__, __LINE__,
                  "$*** dynamic analyzer err: wrong call MPI_Ibsend "
                  "(incorrect dest %d)$\n", dest);

      index = coll_IndexOf(RequestColl, request);

      if(index >= 0)      
         epprintf(1 ,__FILE__, __LINE__,
                  "$*** dynamic analyzer err: wrong call MPI_Ibsend "
                  "(request %lx already exists)$\n", (uLLng)request);

      if(MPI_TraceMsgChecksum || MPI_DebugBufChecksum ||
         MPI_DebugMsgChecksum)
         checksum = _checkSum(buf, count, datatype);
      else
         checksum = 0;

      mpi_dest_rank = _commWorldRank(dest, c);

      coll_Insert(RequestColl, request);
      dvm_AllocStruct(nonblockInfo, infPtr);
      coll_Insert(ReqStructColl, infPtr);

      infPtr->messageTag = tag;
      infPtr->messageSource = MPS_CurrentProcIdent;
      infPtr->messageDest = mpi_dest_rank;
      infPtr->isSend = 1;
      infPtr->buffer = buf;
      infPtr->count = count;
      infPtr->datatype = datatype;
      infPtr->isStarted = 1;
      infPtr->isInit = 0;
      infPtr->checksum = checksum;
      infPtr->MPITestCount = MPI_TestTraceCount;
      infPtr->TotalTestCount = 0;
      infPtr->comm = c;

      if(trace)
      {
        if(MPI_SlashOut)
           tprintf("\n");

        tprintf("$call_MPI_Ibsend\tbuf=%lx\tcount=%d\tdtype=%u\t"
                "size=%d\t\ndest=%d\tidest=%d\ttag=%d\tcomm=%u\t",
                (uLLng)buf, count, datatype, _size(count, datatype),
                dest, mpi_dest_rank, tag, c);

        if(MPI_TraceMsgChecksum)
           tprintf("sum=%u\t", checksum);

        tprintf("\nreq=%lx\t", (uLLng)request);

        if(MPI_TraceTime)
        {  start = dvm_time();

           if(MPI_TraceTimeReg)
           {  tprintf("t=%lf\t", start-LastEventTime);
              LastEventTime = start;
           }
           else
              tprintf("t=%lf\t", start);
        }

        if(MPI_TraceFileLine)
           tprintf("f=%s\tl=%ld\t", DVM_FILE[0], DVM_LINE[0]);
        tprintf("$\n");
      }
   }

   retval = PMPI_Ibsend(buf, count, datatype, dest, tag, c, request);

   if(trace)
   {
      tprintf("$ret_MPI_Ibsend\t");

      if(MPI_TraceTime)
      {  finish = dvm_time();

         if(MPI_TraceTimeReg)
         {  tprintf("t=%lf\t", finish-LastEventTime);
            LastEventTime = finish;
         }
         else
            tprintf("t=%lf\t", finish);
      }

      tprintf("$\n");
   }

   return  retval;
}



int  MPI_Issend(void  *buf, int  count, MPI_Datatype  datatype,
                int  dest, int  tag, MPI_Comm  comm,
                MPI_Request  *request)
{
   int             retval;
   double          start, finish;
   int             system, trace, debug, mpi_dest_rank, index;
   nonblockInfo   *infPtr;
   unsigned        checksum; 
   commInfo       *commInfoPtr;
   MPI_Comm        c;   

   system = (RTS_Call_MPI || DVMCallLevel);
   trace  = (MPI_BotsulaProf && (MPI_TraceLevel || system == 0));
   debug  = (MPI_BotsulaDeb && (MPI_TraceLevel || system == 0));
   retval = (system == 0 && comm == MPI_COMM_WORLD && dvm_OneProcSign == 0);

   if(retval)
      c = DVM_COMM_WORLD;
   else
      c = comm;

   if(debug || trace)
   {
      getcomm(c, &commInfoPtr);

      if(commInfoPtr == NULL)
         epprintf(1 ,__FILE__, __LINE__,
                  "$*** dynamic analyzer err: wrong call MPI_Issend "
                  "(non-existent communicator %u)$\n", comm);

      if(dest < 0 || dest >= commInfoPtr->pcount)
         epprintf(1 ,__FILE__, __LINE__,
                  "$*** dynamic analyzer err: wrong call MPI_Issend "
                  "(incorrect dest %d)$\n", dest);

      index = coll_IndexOf(RequestColl, request);

      if(index >= 0)      
         epprintf(1 ,__FILE__, __LINE__,
                  "$*** dynamic analyzer err: wrong call MPI_Issend "
                  "(request %lx already exists)$\n", (uLLng)request);

      if(MPI_TraceMsgChecksum || MPI_DebugBufChecksum ||
         MPI_DebugMsgChecksum)
         checksum = _checkSum(buf, count, datatype);
      else
         checksum = 0;

      mpi_dest_rank = _commWorldRank(dest, c);

      coll_Insert(RequestColl, request);
      dvm_AllocStruct(nonblockInfo, infPtr);
      coll_Insert(ReqStructColl, infPtr);

      infPtr->messageTag = tag;
      infPtr->messageSource = MPS_CurrentProcIdent;
      infPtr->messageDest = mpi_dest_rank;
      infPtr->isSend = 1;
      infPtr->buffer = buf;
      infPtr->count = count;
      infPtr->datatype = datatype;
      infPtr->isStarted = 1;
      infPtr->isInit = 0;
      infPtr->checksum = checksum;
      infPtr->MPITestCount = MPI_TestTraceCount;
      infPtr->TotalTestCount = 0;
      infPtr->comm = c;

      if(trace)
      {
        if(MPI_SlashOut)
           tprintf("\n");

        tprintf("$call_MPI_Issend\tbuf=%lx\tcount=%d\tdtype=%u\t"
                "size=%d\t\ndest=%d\tidest=%d\ttag=%d\tcomm=%u\t",
                (uLLng)buf, count, datatype, _size(count, datatype),
                dest, mpi_dest_rank, tag, c);

        if(MPI_TraceMsgChecksum)
           tprintf("sum=%u\t", checksum);

        tprintf("\nreq=%lx\t", (uLLng)request);

        if(MPI_TraceTime)
        {  start = dvm_time();

           if(MPI_TraceTimeReg)
           {  tprintf("t=%lf\t", start-LastEventTime);
              LastEventTime = start;
           }
           else
              tprintf("t=%lf\t", start);
        }

        if(MPI_TraceFileLine)
           tprintf("f=%s\tl=%ld\t", DVM_FILE[0], DVM_LINE[0]);
        tprintf("$\n");
      }
   }

   retval = PMPI_Issend(buf, count, datatype, dest, tag, c, request);

   if(trace)
   {
      tprintf("$ret_MPI_Issend\t");

      if(MPI_TraceTime)
      {  finish = dvm_time();

         if(MPI_TraceTimeReg)
         {  tprintf("t=%lf\t", finish-LastEventTime);
            LastEventTime = finish;
         }
         else
            tprintf("t=%lf\t", finish);
      }

      tprintf("$\n");
   }

   return  retval;
}



int  MPI_Irsend(void  *buf, int  count, MPI_Datatype  datatype,
                int  dest, int  tag, MPI_Comm  comm,
                MPI_Request  *request)
{
   int             retval;
   double          start, finish;
   int             system, trace, debug, mpi_dest_rank, index;
   nonblockInfo   *infPtr;
   unsigned        checksum; 
   commInfo       *commInfoPtr;
   MPI_Comm        c;   

   system = (RTS_Call_MPI || DVMCallLevel);
   trace  = (MPI_BotsulaProf && (MPI_TraceLevel || system == 0));
   debug  = (MPI_BotsulaDeb && (MPI_TraceLevel || system == 0));
   retval = (system == 0 && comm == MPI_COMM_WORLD && dvm_OneProcSign == 0);

   if(retval)
      c = DVM_COMM_WORLD;
   else
      c = comm;

   if(debug || trace)
   {
      getcomm(c, &commInfoPtr);

      if(commInfoPtr == NULL)
         epprintf(1 ,__FILE__, __LINE__,
                  "$*** dynamic analyzer err: wrong call MPI_Irsend "
                  "(non-existent communicator %u)$\n", comm);

      if(dest < 0 || dest >= commInfoPtr->pcount)
         epprintf(1 ,__FILE__, __LINE__,
                  "$*** dynamic analyzer err: wrong call MPI_Irsend "
                  "(incorrect dest %d)$\n", dest);

      index = coll_IndexOf(RequestColl, request);

      if(index >= 0)      
         epprintf(1 ,__FILE__, __LINE__,
                  "$*** dynamic analyzer err: wrong call MPI_Irsend "
                  "(request %lx already exists)$\n", (uLLng)request);

      if(MPI_TraceMsgChecksum || MPI_DebugBufChecksum ||
         MPI_DebugMsgChecksum)
         checksum = _checkSum(buf, count, datatype);
      else
         checksum = 0; 

      mpi_dest_rank = _commWorldRank(dest, c);

      coll_Insert(RequestColl, request);
      dvm_AllocStruct(nonblockInfo, infPtr);
      coll_Insert(ReqStructColl, infPtr);

      infPtr->messageTag = tag;
      infPtr->messageSource = MPS_CurrentProcIdent;
      infPtr->messageDest = mpi_dest_rank;
      infPtr->isSend = 1;
      infPtr->buffer = buf;
      infPtr->count = count;
      infPtr->datatype = datatype;
      infPtr->isStarted = 1;
      infPtr->isInit = 0;
      infPtr->checksum = checksum;
      infPtr->MPITestCount = MPI_TestTraceCount;
      infPtr->TotalTestCount = 0;
      infPtr->comm = c;

      if(trace)
      {
        if(MPI_SlashOut)
           tprintf("\n");

        tprintf("$call_MPI_Irsend\tbuf=%lx\tcount=%d\tdtype=%u\t"
                "size=%d\t\ndest=%d\tidest=%d\ttag=%d\tcomm=%u\t",
                (uLLng)buf, count, datatype, _size(count, datatype),
                dest, mpi_dest_rank, tag, c);

        if(MPI_TraceMsgChecksum)
           tprintf("sum=%u\t", checksum);

        tprintf("\nreq=%lx\t", (uLLng)request);

        if(MPI_TraceTime)
        {  start = dvm_time();

           if(MPI_TraceTimeReg)
           {  tprintf("t=%lf\t", start-LastEventTime);
              LastEventTime = start;
           }
           else
              tprintf("t=%lf\t", start);
        }

        if(MPI_TraceFileLine)
           tprintf("f=%s\tl=%ld\t", DVM_FILE[0], DVM_LINE[0]);
        tprintf("$\n");
      }
   }

   retval = PMPI_Irsend(buf, count, datatype, dest, tag, c, request);

   if(trace)
   {
      tprintf("$ret_MPI_Irsend\t");

      if(MPI_TraceTime)
      {  finish = dvm_time();

         if(MPI_TraceTimeReg)
         {  tprintf("t=%lf\t", finish-LastEventTime);
            LastEventTime = finish;
         }
         else
            tprintf("t=%lf\t", finish);
      }

      tprintf("$\n");
   }

   return  retval;
}



int  MPI_Irecv(void  *buf, int  count, MPI_Datatype  datatype,
               int  source, int  tag, MPI_Comm  comm,
               MPI_Request  *request)
{
   int             retval;
   double          start, finish;
   int             system, trace, debug, mpi_source_rank, index;
   commInfo       *commInfoPtr;
   nonblockInfo   *infPtr;
   MPI_Comm        c;   

   system = (RTS_Call_MPI || DVMCallLevel);
   trace  = (MPI_BotsulaProf && (MPI_TraceLevel || system == 0));
   debug  = (MPI_BotsulaDeb && (MPI_TraceLevel || system == 0));
   retval = (system == 0 && comm == MPI_COMM_WORLD && dvm_OneProcSign == 0);

   if(retval)
      c = DVM_COMM_WORLD;
   else
      c = comm;

   if(debug || trace)
   {
      getcomm(c, &commInfoPtr);

      if(commInfoPtr == NULL)
         epprintf(1 ,__FILE__, __LINE__,
                  "$*** dynamic analyzer err: wrong call MPI_Irecv "
                  "(non-existent communicator %u)$\n", comm);

      if(source != MPI_ANY_SOURCE &&
         (source < 0 || source >= commInfoPtr->pcount))
         epprintf(1 ,__FILE__, __LINE__,
                  "$*** dynamic analyzer err: wrong call MPI_Irecv "
                  "(incorrect source %d)$\n", source);

      index = coll_IndexOf(RequestColl, request);

      if(index >= 0)      
         epprintf(1 ,__FILE__, __LINE__,
                  "$*** dynamic analyzer err: wrong call MPI_Irecv "
                  "(request %lx already exists)\n", (uLLng)request);

      mpi_source_rank = _commWorldRank(source, c);

      coll_Insert(RequestColl, request);
      dvm_AllocStruct(nonblockInfo, infPtr);
      coll_Insert(ReqStructColl, infPtr);

      infPtr->messageTag = tag;
      infPtr->messageDest = MPS_CurrentProcIdent;
      infPtr->messageSource = mpi_source_rank;
      infPtr->isSend = 0;
      infPtr->buffer = buf;
      infPtr->count = count;
      infPtr->datatype = datatype;
      infPtr->isStarted = 1;
      infPtr->isInit = 0;
      infPtr->MPITestCount = MPI_TestTraceCount;
      infPtr->TotalTestCount = 0;
      infPtr->comm = c;

      if(trace)
      {
         if(MPI_SlashOut)
            tprintf("\n");

         tprintf("$call_MPI_Irecv\tbuf=%lx\tcount=%d\tdtype=%u\t"
                 "size=%d\t\nsource=%d\tisource=%d\ttag=%d\tcomm=%u\t\n"
                 "req=%lx\t",
                 (uLLng)buf, count, datatype, _size(count, datatype),
                 source, mpi_source_rank, tag, c, (uLLng)request);

         if(MPI_TraceTime)
         {  start = dvm_time();

            if(MPI_TraceTimeReg)
            {  tprintf("t=%lf\t", start-LastEventTime);
               LastEventTime = start;
            }
            else
               tprintf("t=%lf\t", start);
         }

         if(MPI_TraceFileLine)
            tprintf("f=%s\tl=%ld\t", DVM_FILE[0], DVM_LINE[0]);
         tprintf("$\n");
      }
   }

   retval = PMPI_Irecv(buf, count, datatype, source, tag, c, request);

   if(trace)
   {
      tprintf("$ret_MPI_Irecv\t");

      if(MPI_TraceTime)
      {  finish = dvm_time();

         if(MPI_TraceTimeReg)
         {  tprintf("t=%lf\t", finish-LastEventTime);
            LastEventTime = finish;
         }
         else
            tprintf("t=%lf\t", finish);
      }

      tprintf("$\n");
   }

   return  retval;
}



/*****************************\
*   MPI_Xxxx_init functions   *
\*****************************/    /*E0028*/


int  MPI_Send_init(void  *buf, int  count, MPI_Datatype  datatype,
                   int  dest, int  tag, MPI_Comm  comm,
                   MPI_Request  *request)
{
   int             retval;
   double          start, finish;
   int             system, trace, debug, mpi_dest_rank, index;
   nonblockInfo   *infPtr;
   commInfo       *commInfoPtr;
   MPI_Comm        c;   

   system = (RTS_Call_MPI || DVMCallLevel);
   trace  = (MPI_BotsulaProf && (MPI_TraceLevel || system == 0));
   debug  = (MPI_BotsulaDeb && (MPI_TraceLevel || system == 0));
   retval = (system == 0 && comm == MPI_COMM_WORLD && dvm_OneProcSign == 0);

   if(retval)
      c = DVM_COMM_WORLD;
   else
      c = comm;

   if(debug || trace)
   {
      getcomm(c, &commInfoPtr);

      if(commInfoPtr == NULL)
         epprintf(1 ,__FILE__, __LINE__,
                  "$*** dynamic analyzer err: wrong call MPI_Send_init "
                  "(non-existent communicator %u)$\n", comm);

      if(dest < 0 || dest >= commInfoPtr->pcount)
         epprintf(1 ,__FILE__, __LINE__,
                  "$*** dynamic analyzer err: wrong call MPI_Send_init "
                  "(incorrect dest %d)$\n", dest);

      index = coll_IndexOf(RequestColl, request);

      if(index >= 0)      
         epprintf(1 ,__FILE__, __LINE__,
                  "$*** dynamic analyzer err: wrong call MPI_Send_init "
                  "(request %lx already exists)$\n", (uLLng)request);

      mpi_dest_rank = _commWorldRank(dest, c);

      coll_Insert(RequestColl, request);
      dvm_AllocStruct(nonblockInfo, infPtr);
      coll_Insert(ReqStructColl, infPtr);

      infPtr->messageTag = tag;
      infPtr->messageSource = MPS_CurrentProcIdent;
      infPtr->messageDest = mpi_dest_rank;
      infPtr->isSend = 1;
      infPtr->buffer = buf;
      infPtr->count = count;
      infPtr->datatype = datatype;
      infPtr->isStarted = 0;
      infPtr->isInit = 1;
      infPtr->checksum = 0;
      infPtr->MPITestCount = MPI_TestTraceCount;
      infPtr->TotalTestCount = 0;
      infPtr->comm = c;

      if(trace)
      {
        if(MPI_SlashOut)
           tprintf("\n");

        tprintf("$call_MPI_Send_init\tbuf=%lx\tcount=%d\tdtype=%u\t"
                "size=%d\t\ndest=%d\tidest=%d\ttag=%d\tcomm=%u\t\n"
                "req=%lx\t",
                (uLLng)buf, count, datatype, _size(count, datatype),
                dest, mpi_dest_rank, tag, c, (uLLng)request);

        if(MPI_TraceTime)
        {  start = dvm_time();

           if(MPI_TraceTimeReg)
           {  tprintf("t=%lf\t", start-LastEventTime);
              LastEventTime = start;
           }
           else
              tprintf("t=%lf\t", start);
        }

        if(MPI_TraceFileLine)
           tprintf("f=%s\tl=%ld\t", DVM_FILE[0], DVM_LINE[0]);
        tprintf("$\n");
      }
   }

   retval = PMPI_Send_init(buf, count, datatype, dest, tag, c, request);

   if(trace)
   {
      tprintf("$ret_MPI_Send_init\t");

      if(MPI_TraceTime)
      {  finish = dvm_time();

         if(MPI_TraceTimeReg)
         {  tprintf("t=%lf\t", finish-LastEventTime);
            LastEventTime = finish;
         }
         else
            tprintf("t=%lf\t", finish);
      }

      tprintf("$\n");
   }

   return  retval;
}



int  MPI_Bsend_init(void  *buf, int  count, MPI_Datatype  datatype,
                    int  dest, int  tag, MPI_Comm  comm,
                    MPI_Request  *request)
{
   int             retval;
   double          start, finish;
   int             system, trace, debug, mpi_dest_rank, index;
   nonblockInfo   *infPtr;
   commInfo       *commInfoPtr;
   MPI_Comm        c;   

   system = (RTS_Call_MPI || DVMCallLevel);
   trace  = (MPI_BotsulaProf && (MPI_TraceLevel || system == 0));
   debug  = (MPI_BotsulaDeb && (MPI_TraceLevel || system == 0));
   retval = (system == 0 && comm == MPI_COMM_WORLD && dvm_OneProcSign == 0);

   if(retval)
      c = DVM_COMM_WORLD;
   else
      c = comm;

   if(debug || trace)
   {
      getcomm(c, &commInfoPtr);

      if(commInfoPtr == NULL)
         epprintf(1 ,__FILE__, __LINE__,
                  "$*** dynamic analyzer err: wrong call MPI_Bsend_init "
                  "(non-existent communicator %u)$\n", comm);

      if(dest < 0 || dest >= commInfoPtr->pcount)
         epprintf(1 ,__FILE__, __LINE__,
                  "$*** dynamic analyzer err: wrong call MPI_Bsend_init "
                  "(incorrect dest %d)$\n", dest);

      index = coll_IndexOf(RequestColl, request);

      if(index >= 0)      
         epprintf(1 ,__FILE__, __LINE__,
                  "$*** dynamic analyzer err: wrong call MPI_Bsend_init "
                  "(request %lx already exists)$\n", (uLLng)request);

      mpi_dest_rank = _commWorldRank(dest, c);

      coll_Insert(RequestColl, request);
      dvm_AllocStruct(nonblockInfo, infPtr);
      coll_Insert(ReqStructColl, infPtr);

      infPtr->messageTag = tag;
      infPtr->messageSource = MPS_CurrentProcIdent;
      infPtr->messageDest = mpi_dest_rank;
      infPtr->isSend = 1;
      infPtr->buffer = buf;
      infPtr->count = count;
      infPtr->datatype = datatype;
      infPtr->isStarted = 0;
      infPtr->isInit = 1;
      infPtr->checksum = 0;
      infPtr->MPITestCount = MPI_TestTraceCount;
      infPtr->TotalTestCount = 0;
      infPtr->comm = c;

      if(trace)
      {
        if(MPI_SlashOut)
           tprintf("\n");

        tprintf("$call_MPI_Bsend_init\tbuf=%lx\tcount=%d\tdtype=%u\t"
                "size=%d\t\ndest=%d\tidest=%d\ttag=%d\tcomm=%u\t\n"
                "req=%lx\t",
                (uLLng)buf, count, datatype, _size(count, datatype),
                dest, mpi_dest_rank, tag, c, (uLLng)request);

        if(MPI_TraceTime)
        {  start = dvm_time();

           if(MPI_TraceTimeReg)
           {  tprintf("t=%lf\t", start-LastEventTime);
              LastEventTime = start;
           }
           else
              tprintf("t=%lf\t", start);
        }

        if(MPI_TraceFileLine)
           tprintf("f=%s\tl=%ld\t", DVM_FILE[0], DVM_LINE[0]);
        tprintf("$\n");
      }
   }

   retval = PMPI_Bsend_init(buf, count, datatype, dest, tag, c, request);

   if(trace)
   {
      tprintf("$ret_MPI_Bsend_init\t");

      if(MPI_TraceTime)
      {  finish = dvm_time();

         if(MPI_TraceTimeReg)
         {  tprintf("t=%lf\t", finish-LastEventTime);
            LastEventTime = finish;
         }
         else
            tprintf("t=%lf\t", finish);
      }

      tprintf("$\n");
   }

   return  retval;
}



int  MPI_Ssend_init(void  *buf, int  count, MPI_Datatype  datatype,
                    int  dest, int  tag, MPI_Comm  comm,
                    MPI_Request  *request)
{
   int             retval;
   double          start, finish;
   int             system, trace, debug, mpi_dest_rank, index;
   nonblockInfo   *infPtr;
   commInfo       *commInfoPtr;
   MPI_Comm        c;   

   system = (RTS_Call_MPI || DVMCallLevel);
   trace  = (MPI_BotsulaProf && (MPI_TraceLevel || system == 0));
   debug  = (MPI_BotsulaDeb && (MPI_TraceLevel || system == 0));
   retval = (system == 0 && comm == MPI_COMM_WORLD && dvm_OneProcSign == 0);

   if(retval)
      c = DVM_COMM_WORLD;
   else
      c = comm;

   if(debug || trace)
   {
      getcomm(c, &commInfoPtr);

      if(commInfoPtr == NULL)
         epprintf(1 ,__FILE__, __LINE__,
                  "$*** dynamic analyzer err: wrong call MPI_Ssend_init "
                  "(non-existent communicator %u)$\n", comm);

      if(dest < 0 || dest >= commInfoPtr->pcount)
         epprintf(1 ,__FILE__, __LINE__,
                  "$*** dynamic analyzer err: wrong call MPI_Ssend_init "
                  "(incorrect dest %d)$\n", dest);

      index = coll_IndexOf(RequestColl, request);

      if(index >= 0)      
         epprintf(1 ,__FILE__, __LINE__,
                  "$*** dynamic analyzer err: wrong call MPI_Ssend_init "
                  "(request %lx already exists)$\n", (uLLng)request);

      mpi_dest_rank = _commWorldRank(dest, c);

      coll_Insert(RequestColl, request);
      dvm_AllocStruct(nonblockInfo, infPtr);
      coll_Insert(ReqStructColl, infPtr);

      infPtr->messageTag = tag;
      infPtr->messageSource = MPS_CurrentProcIdent;
      infPtr->messageDest = mpi_dest_rank;
      infPtr->isSend = 1;
      infPtr->buffer = buf;
      infPtr->count = count;
      infPtr->datatype = datatype;
      infPtr->isStarted = 0;
      infPtr->isInit = 1;
      infPtr->checksum = 0;
      infPtr->MPITestCount = MPI_TestTraceCount;
      infPtr->TotalTestCount = 0;
      infPtr->comm = c;

      if(trace)
      {
        if(MPI_SlashOut)
           tprintf("\n");

        tprintf("$call_MPI_Ssend_init\tbuf=%lx\tcount=%d\tdtype=%u\t"
                "size=%d\t\ndest=%d\tidest=%d\ttag=%d\tcomm=%u\t\n"
                "req=%lx\t",
                (uLLng)buf, count, datatype, _size(count, datatype),
                dest, mpi_dest_rank, tag, c, (uLLng)request);

        if(MPI_TraceTime)
        {  start = dvm_time();

           if(MPI_TraceTimeReg)
           {  tprintf("t=%lf\t", start-LastEventTime);
              LastEventTime = start;
           }
           else
              tprintf("t=%lf\t", start);
        } 

        if(MPI_TraceFileLine)
           tprintf("f=%s\tl=%ld\t", DVM_FILE[0], DVM_LINE[0]);
        tprintf("$\n");
      }
   }

   retval = PMPI_Ssend_init(buf, count, datatype, dest, tag, c, request);

   if(trace)
   {
      tprintf("$ret_MPI_Ssend_init\t");

      if(MPI_TraceTime)
      {  finish = dvm_time();

         if(MPI_TraceTimeReg)
         {  tprintf("t=%lf\t", finish-LastEventTime);
            LastEventTime = finish;
         }
         else
            tprintf("t=%lf\t", finish);
      }

      tprintf("$\n");
   }

   return  retval;
}



int  MPI_Rsend_init(void  *buf, int  count, MPI_Datatype  datatype,
                    int  dest, int  tag, MPI_Comm  comm,
                    MPI_Request  *request)
{
   int             retval;
   double          start, finish;
   int             system, trace, debug, mpi_dest_rank, index;
   nonblockInfo   *infPtr;
   commInfo       *commInfoPtr;
   MPI_Comm        c;   

   system = (RTS_Call_MPI || DVMCallLevel);
   trace  = (MPI_BotsulaProf && (MPI_TraceLevel || system == 0));
   debug  = (MPI_BotsulaDeb && (MPI_TraceLevel || system == 0));
   retval = (system == 0 && comm == MPI_COMM_WORLD && dvm_OneProcSign == 0);

   if(retval)
      c = DVM_COMM_WORLD;
   else
      c = comm;

   if(debug || trace)
   {
      getcomm(c, &commInfoPtr);

      if(commInfoPtr == NULL)
         epprintf(1 ,__FILE__, __LINE__,
                  "$*** dynamic analyzer err: wrong call MPI_Rsend_init "
                  "(non-existent communicator %u)$\n", comm);

      if(dest < 0 || dest >= commInfoPtr->pcount)
         epprintf(1 ,__FILE__, __LINE__,
                  "$*** dynamic analyzer err: wrong call MPI_Rsend_init "
                  "(incorrect dest %d)$\n", dest);

      index = coll_IndexOf(RequestColl, request);

      if(index >= 0)      
         epprintf(1 ,__FILE__, __LINE__,
                  "$*** dynamic analyzer err: wrong call MPI_Rsend_init "
                  "(request %lx already exists)$\n", (uLLng)request);

      mpi_dest_rank = _commWorldRank(dest, c);

      coll_Insert(RequestColl, request);
      dvm_AllocStruct(nonblockInfo, infPtr);
      coll_Insert(ReqStructColl, infPtr);

      infPtr->messageTag = tag;
      infPtr->messageSource = MPS_CurrentProcIdent;
      infPtr->messageDest = mpi_dest_rank;
      infPtr->isSend = 1;
      infPtr->buffer = buf;
      infPtr->count = count;
      infPtr->datatype = datatype;
      infPtr->isStarted = 0;
      infPtr->isInit = 1;
      infPtr->checksum = 0;
      infPtr->MPITestCount = MPI_TestTraceCount;
      infPtr->TotalTestCount = 0;
      infPtr->comm = c;

      if(trace)
      {
        if(MPI_SlashOut)
           tprintf("\n");

        tprintf("$call_MPI_Rsend_init\tbuf=%lx\tcount=%d\tdtype=%u\t"
                "size=%d\t\ndest=%d\tidest=%d\ttag=%d\tcomm=%u\t\n"
                "req=%lx\t",
                (uLLng)buf, count, datatype, _size(count, datatype),
                dest, mpi_dest_rank, tag, c, (uLLng)request);

        if(MPI_TraceTime)
        {  start = dvm_time();

           if(MPI_TraceTimeReg)
           {  tprintf("t=%lf\t", start-LastEventTime);
              LastEventTime = start;
           }
           else
              tprintf("t=%lf\t", start);
        }

        if(MPI_TraceFileLine)
           tprintf("f=%s\tl=%ld\t", DVM_FILE[0], DVM_LINE[0]);
        tprintf("$\n");
      }
   }

   retval = PMPI_Rsend_init(buf, count, datatype, dest, tag, c, request);

   if(trace)
   {
      tprintf("$ret_MPI_Rsend_init\t");

      if(MPI_TraceTime)
      {  finish = dvm_time();

         if(MPI_TraceTimeReg)
         {  tprintf("t=%lf\t", finish-LastEventTime);
            LastEventTime = finish;
         }
         else
            tprintf("t=%lf\t", finish);
      }

      tprintf("$\n");
   }

   return  retval;
}



int  MPI_Recv_init(void  *buf, int  count, MPI_Datatype  datatype,
                   int  source, int  tag, MPI_Comm  comm,
                   MPI_Request  *request)
{
   int            retval;
   double         start, finish;
   int            system, trace, debug, mpi_source_rank, index;
   commInfo      *commInfoPtr;
   nonblockInfo  *infPtr;
   MPI_Comm       c;   

   system = (RTS_Call_MPI || DVMCallLevel);
   trace  = (MPI_BotsulaProf && (MPI_TraceLevel || system == 0));
   debug  = (MPI_BotsulaDeb && (MPI_TraceLevel || system == 0));
   retval = (system == 0 && comm == MPI_COMM_WORLD && dvm_OneProcSign == 0);

   if(retval)
      c = DVM_COMM_WORLD;
   else
      c = comm;

   if(debug || trace)
   {
      getcomm(c, &commInfoPtr);

      if(commInfoPtr == NULL)
         epprintf(1 ,__FILE__, __LINE__,
                  "$*** dynamic analyzer err: wrong call MPI_Recv_init "
                  "(non-existent communicator %u)$\n", comm);

      if(source != MPI_ANY_SOURCE &&
         (source < 0 || source >= commInfoPtr->pcount))
         epprintf(1 ,__FILE__, __LINE__,
                  "$*** dynamic analyzer err: wrong call MPI_Recv_init "
                  "(incorrect source %d)$\n", source);

      index = coll_IndexOf(RequestColl, request);

      if(index >= 0)      
         epprintf(1 ,__FILE__, __LINE__,
                  "$*** dynamic analyzer err: wrong call MPI_Recv_init "
                  "(request %lx already exists)\n", (uLLng)request);

      mpi_source_rank = _commWorldRank(source, c);

      coll_Insert(RequestColl, request);
      dvm_AllocStruct(nonblockInfo, infPtr);
      coll_Insert(ReqStructColl, infPtr);

      infPtr->messageTag = tag;
      infPtr->messageDest = MPS_CurrentProcIdent;
      infPtr->messageSource = mpi_source_rank;
      infPtr->isSend = 0;
      infPtr->buffer = buf;
      infPtr->count = count;
      infPtr->datatype = datatype;
      infPtr->isStarted = 0;
      infPtr->isInit = 1;
      infPtr->MPITestCount = MPI_TestTraceCount;
      infPtr->TotalTestCount = 0;
      infPtr->comm = c;

      if(trace)
      {
         if(MPI_SlashOut)
            tprintf("\n");

         tprintf("$call_MPI_Recv_init\tbuf=%lx\tcount=%d\tdtype=%u\t"
                 "size=%d\t\nsource=%d\tisource=%d\ttag=%d\tcomm=%u\t\n"
                 "req=%lx\t",
                 (uLLng)buf, count, datatype, _size(count, datatype),
                 source, mpi_source_rank, tag, c, (uLLng)request);

         if(MPI_TraceTime)
         {  start = dvm_time();

            if(MPI_TraceTimeReg)
            {  tprintf("t=%lf\t", start-LastEventTime);
               LastEventTime = start;
            }
            else
               tprintf("t=%lf\t", start);
         }

         if(MPI_TraceFileLine)
            tprintf("f=%s\tl=%ld\t", DVM_FILE[0], DVM_LINE[0]);
         tprintf("$\n");
      }
   }

   retval = PMPI_Recv_init(buf, count, datatype, source, tag,
                           c, request);

   if(trace)
   {
      tprintf("$ret_MPI_Recv_init\t");

      if(MPI_TraceTime)
      {  finish = dvm_time();

         if(MPI_TraceTimeReg)
         {  tprintf("t=%lf\t", finish-LastEventTime);
            LastEventTime = finish;
         }
         else
            tprintf("t=%lf\t", finish);
      }

      tprintf("$\n");
   }

   return  retval;
}



/***********************************************\
*   MPI_Waitxxxxx and MPI_Testxxxxx functions   *
\***********************************************/    /*E0029*/


int  MPI_Wait(MPI_Request  *request, MPI_Status  *status)
{
   int             retval = 0;
   double          start, finish;
   nonblockInfo   *infPtr = NULL;
   int             system, trace, debug, post_count, index = -1;
   unsigned        checksum;

   system = (RTS_Call_MPI || DVMCallLevel);
   trace  = (MPI_BotsulaProf && (MPI_TraceLevel || system == 0));
   debug  = (MPI_BotsulaDeb && (MPI_TraceLevel || system == 0));

   if(debug || trace)
   {
      index = coll_IndexOf(RequestColl, request);

      if(index >= 0)
      {  infPtr = coll_At(nonblockInfo *, ReqStructColl, index);

         if(infPtr->isStarted)
         {
            if(infPtr->isSend)
            {  
               if(MPI_TraceMsgChecksum || MPI_DebugBufChecksum)
                  checksum = _checkSum(infPtr->buffer, infPtr->count,
                                       infPtr->datatype);

               if(trace)
               {
                  if(MPI_SlashOut)
                     tprintf("\n");

                  tprintf("$call_MPI_Wait\treq=%lx\t", (uLLng)request);

                  if(MPI_TraceMsgChecksum)
                     tprintf("sum=%u\t", checksum);

                  if(MPI_TraceTime)
                  {  start = dvm_time();

                     if(MPI_TraceTimeReg)
                     {  tprintf("\nt=%lf\t", start-LastEventTime);
                        LastEventTime = start;
                     }
                     else
                        tprintf("\nt=%lf\t", start);
                  }

                  if(MPI_TraceFileLine)
                     tprintf("f=%s\tl=%ld\t", DVM_FILE[0], DVM_LINE[0]);
                  tprintf("$\n");
               }

               if(MPI_DebugBufChecksum)
                  if(checksum != infPtr->checksum)
                     epprintf(1 ,__FILE__, __LINE__,
                        "$*** dynamic analyzer err: wrong call MPI_Wait "
                        "(wrong send message checksum)$\n");
            }
            else
            {
               if(trace)
               {
                  if(MPI_SlashOut)
                     tprintf("\n");

                  tprintf("$call_MPI_Wait\treq=%lx\t", (uLLng)request);

                  if(MPI_TraceMsgChecksum)
                     tprintf("sum=0\t");

                  if(MPI_TraceTime)
                  {  start = dvm_time();

                     if(MPI_TraceTimeReg)
                     {  tprintf("\nt=%lf\t", start-LastEventTime);
                        LastEventTime = start;
                     }
                     else
                        tprintf("\nt=%lf\t", start);
                  }

                  if(MPI_TraceFileLine)
                     tprintf("f=%s\tl=%ld\t", DVM_FILE[0], DVM_LINE[0]);
                  tprintf("$\n");
               }
            }
         }
      }
   }

   retval = PMPI_Wait(request, status);

   if(debug || trace)
   {
      if(index >= 0)
      {
         if(infPtr->isStarted)
         {
            if(infPtr->isSend == 0)
            {
               if(MPI_TraceMsgChecksum || MPI_DebugMsgChecksum || trace)
                  PMPI_Get_count(status, infPtr->datatype, &post_count);

               if(MPI_TraceMsgChecksum || MPI_DebugMsgChecksum)
                  checksum = _checkSum(infPtr->buffer, post_count,
                                       infPtr->datatype);
            }

            if(trace)
            {
               if(infPtr->isSend == 0)
               {
                  tprintf("$ret_MPI_Wait\t"
                          "recv=1\tcount=%d\tsize=%d\tproc=%d\t\n"
                          "iproc=%d\ttag=%d\t",
                          post_count, _size(post_count,
                                            infPtr->datatype),
                          status->MPI_SOURCE,
                          _commWorldRank(status->MPI_SOURCE,
                                         infPtr->comm),
                          status->MPI_TAG);

                  if(MPI_TraceMsgChecksum)
                     tprintf("sum=%u\t", checksum);

                  if(MPI_TraceTime)
                  {  finish = dvm_time();

                     if(MPI_TraceTimeReg)
                     {  tprintf("t=%lf\t", finish-LastEventTime);
                        LastEventTime = finish;
                     }
                     else
                        tprintf("t=%lf\t", finish);
                  }

                  tprintf("$\n");
               }
               else
               {
                  tprintf("$ret_MPI_Wait\trecv=0\t");

                  if(MPI_TraceTime)
                  {  finish = dvm_time();

                     if(MPI_TraceTimeReg)
                     {  tprintf("t=%lf\t", finish-LastEventTime);
                        LastEventTime = finish;
                     }
                     else
                        tprintf("t=%lf\t", finish);
                  }

                  tprintf("$\n");
               }
            }

            if(infPtr->isInit)
            {
               infPtr->isStarted = 0;
               infPtr->MPITestCount = MPI_TestTraceCount;
               infPtr->TotalTestCount = 0;
            }
            else
            {               
               coll_AtDelete(RequestColl, index);
               coll_AtDelete(ReqStructColl, index);
               dvm_FreeStruct(infPtr);
            } 
         }
      }
   }

   return  retval;
}



int  MPI_Test(MPI_Request  *request, int  *flag, MPI_Status  *status)
{
   int             retval = 0;
   double          start, finish;
   nonblockInfo   *infPtr;
   int             system, trace, debug, post_count, index = -1;
   unsigned        checksum;

   system = (RTS_Call_MPI || DVMCallLevel);
   trace  = (MPI_BotsulaProf && (MPI_TraceLevel || system == 0));
   debug  = (MPI_BotsulaDeb && (MPI_TraceLevel || system == 0));

   if(debug || trace)
   {
      index = coll_IndexOf(RequestColl, request);

      if(index >= 0)
      {
         infPtr = coll_At(nonblockInfo *, ReqStructColl, index);

         if(infPtr->isStarted)
         {
            infPtr->TotalTestCount++;
            infPtr->MPITestCount++;

            if(infPtr->isSend)
            {  
               if(MPI_TraceMsgChecksum || MPI_DebugBufChecksum)
                  checksum = _checkSum(infPtr->buffer, infPtr->count,
                                       infPtr->datatype);

               if(trace)
               {
                  if(infPtr->MPITestCount >= MPI_TestTraceCount)
                  {  infPtr->MPITestCount = 0;

                     if(MPI_SlashOut)
                        tprintf("\n");

                     tprintf("$call_MPI_Test\treq=%lx\t", (uLLng)request);

                     if(MPI_TraceMsgChecksum)
                        tprintf("sum=%u\t", checksum);

                     if(MPI_TraceTime)
                     {  start = dvm_time();

                        if(MPI_TraceTimeReg)
                        {  tprintf("\nt=%lf\t", start-LastEventTime);
                           LastEventTime = start;
                        }
                        else
                           tprintf("\nt=%lf\t", start);
                     }

                     if(MPI_TraceFileLine)
                        tprintf("f=%s\tl=%ld\t",
                                DVM_FILE[0], DVM_LINE[0]);
                     tprintf("$\n");
                  }
               } 

               if(MPI_DebugBufChecksum)
                  if(checksum != infPtr->checksum)
                     epprintf(1 ,__FILE__, __LINE__,
                        "$*** dynamic analyzer err: wrong call MPI_Test "
                        "(wrong send message checksum)$\n");
            }
            else
            {
               if(trace)
               {
                  if(infPtr->MPITestCount >= MPI_TestTraceCount)
                  {  infPtr->MPITestCount = 0;

                     if(MPI_SlashOut)
                        tprintf("\n");

                     tprintf("$call_MPI_Test\treq=%lx\t", (uLLng)request);

                     if(MPI_TraceMsgChecksum)
                        tprintf("sum=0\t");

                     if(MPI_TraceTime)
                     {  start = dvm_time();

                        if(MPI_TraceTimeReg)
                        {  tprintf("\nt=%lf\t", start-LastEventTime);
                           LastEventTime = start;
                        }
                        else
                           tprintf("\nt=%lf\t", start);
                     }

                     if(MPI_TraceFileLine)
                        tprintf("f=%s\tl=%ld\t",
                                DVM_FILE[0], DVM_LINE[0]);
                     tprintf("$\n");
                  }
               }
            }
         }
      }
   }

   retval = PMPI_Test(request, flag, status);

   if(debug || trace)
   {
      if(index >= 0 && infPtr->isStarted)
      {
         if(*flag)
         {
            if(trace && infPtr->MPITestCount != 0)
            {  /* */    /*E0030*/

               if(infPtr->isSend)
               {  
                  if(MPI_SlashOut)
                     tprintf("\n");

                  tprintf("$call_MPI_Test\treq=%lx\t", (uLLng)request);

                  if(MPI_TraceMsgChecksum)
                     tprintf("sum=%u\t", checksum);

                  if(MPI_TraceTime)
                  {
                     if(MPI_TraceTimeReg)
                     {  tprintf("\nt=%lf\t", start-LastEventTime);
                        LastEventTime = start;
                     }
                     else
                        tprintf("\nt=%lf\t", start);
                  }

                  if(MPI_TraceFileLine)
                     tprintf("f=%s\tl=%ld\t", DVM_FILE[0], DVM_LINE[0]);
                  tprintf("$\n");
               }
               else
               {
                  if(MPI_SlashOut)
                     tprintf("\n");

                  tprintf("$call_MPI_Test\treq=%lx\t", (uLLng)request);

                  if(MPI_TraceMsgChecksum)
                     tprintf("sum=0\t");

                  if(MPI_TraceTime)
                  {
                     if(MPI_TraceTimeReg)
                     {  tprintf("\nt=%lf\t", start-LastEventTime);
                        LastEventTime = start;
                     }
                     else
                        tprintf("\nt=%lf\t", start);
                  }

                  if(MPI_TraceFileLine)
                     tprintf("f=%s\tl=%ld\t", DVM_FILE[0], DVM_LINE[0]);
                  tprintf("$\n");
               }
            }

            if(infPtr->isSend == 0)
            {
               if(MPI_TraceMsgChecksum || MPI_DebugMsgChecksum || trace)
                  PMPI_Get_count(status, infPtr->datatype, &post_count);

               if(MPI_TraceMsgChecksum || MPI_DebugMsgChecksum)
                  checksum = _checkSum(infPtr->buffer, post_count,
                                       infPtr->datatype);
            }

            if(infPtr->isSend == 0)
            {
               if(trace)
               {
                  tprintf("$ret_MPI_Test\t"
                          "flag=1\trecv=1\tcount=%d\tsize=%d\t\nproc=%d\t"
                          "iproc=%d\ttag=%d\t",
                          post_count,
                          _size(post_count, infPtr->datatype),
                          status->MPI_SOURCE,
                          _commWorldRank(status->MPI_SOURCE,
                                         infPtr->comm), status->MPI_TAG);

                  if(MPI_TraceMsgChecksum)
                     tprintf("sum=%u\t", checksum);

                  if(MPI_TraceTime)
                  {  finish = dvm_time();

                     if(MPI_TraceTimeReg)
                     {  tprintf("t=%lf\t", finish-LastEventTime);
                        LastEventTime = finish;
                     }
                     else
                        tprintf("t=%lf\t", finish);
                  }

                  tprintf("$\n");
               } 
            }
            else
            {
               if(trace)
               {
                  tprintf("$ret_MPI_Test\tflag=1\trecv=0\t");

                  if(MPI_TraceTime)
                  {  finish = dvm_time();

                     if(MPI_TraceTimeReg)
                     {  tprintf("t=%lf\t", finish-LastEventTime);
                        LastEventTime = finish;
                     }
                     else
                        tprintf("t=%lf\t", finish);
                  }

                  tprintf("$\n");
               }
            }

            if(infPtr->isInit)
            {
               infPtr->isStarted = 0;
               infPtr->MPITestCount = MPI_TestTraceCount;
               infPtr->TotalTestCount = 0;
            }
            else
            {  coll_AtDelete(RequestColl, index);
               coll_AtDelete(ReqStructColl, index);
               dvm_FreeStruct(infPtr);
            }
         }
         else
         {
            if(trace && infPtr->MPITestCount == 0)
            {
               tprintf("$ret_MPI_Test\tflag=0\t");

               if(MPI_TraceTime)
               {  finish = dvm_time();

                  if(MPI_TraceTimeReg)
                  {  tprintf("t=%lf\t", finish-LastEventTime);
                     LastEventTime = finish;
                  }
                  else
                     tprintf("t=%lf\t", finish);
               }

               tprintf("$\n");
            }
         }
      }
   }

   return  retval;
}



int  MPI_Waitall(int  count, MPI_Request  array_of_requests[],
                 MPI_Status  array_of_statuses[])
{
   int             retval = 0, i, j, k = 0, n = 0;
   double          start, finish;
   nonblockInfo   *infPtr;
   int             system, trace, debug, index = -1;
   unsigned       *checksum = NULL;
   int            *post_count = NULL;

   system = (RTS_Call_MPI || DVMCallLevel);
   trace  = (MPI_BotsulaProf && (MPI_TraceLevel || system == 0));
   debug  = (MPI_BotsulaDeb && (MPI_TraceLevel || system == 0));

   if(debug || trace)
   {
      if(count <= 0)
         epprintf(1 ,__FILE__, __LINE__,
                  "$*** dynamic analyzer err: wrong call MPI_Waitall "
                  "(count=%d)$\n", count);

      if(MPI_TraceMsgChecksum || MPI_DebugBufChecksum ||
         MPI_DebugMsgChecksum)
      {
         mac_malloc(checksum, unsigned *, count, 0);
      }

      for(i=0,k=0,n=0; i < count; i++)
      {
         index = coll_IndexOf(RequestColl, &array_of_requests[i]);

         if(index < 0)
            continue;

         infPtr = coll_At(nonblockInfo *, ReqStructColl, index);

         if(infPtr->isStarted == 0)
            continue;

         if(infPtr->isSend)
         {  
            if(MPI_TraceMsgChecksum || MPI_DebugBufChecksum)
               checksum[i] = _checkSum(infPtr->buffer, infPtr->count,
                                       infPtr->datatype);

            if(MPI_DebugBufChecksum)
               if(checksum[i] != infPtr->checksum)
                  epprintf(1 ,__FILE__, __LINE__,
                     "$*** dynamic analyzer err: wrong call MPI_Waitall "
                     "(wrong send message checksum. index=%d)$\n", i);

            k++;  /* */    /*E0031*/
         }
         else
            n++;  /* */    /*E0032*/
      }

      if(trace)
      {
         if(k+n != 0)
         {
            if(MPI_SlashOut)
               tprintf("\n");

            tprintf("$call_MPI_Waitall\tcount=%d\t\nreqlist=\n", k+n);

            for(i=0,j=0; i < count; i++)
            {
               index = coll_IndexOf(RequestColl, &array_of_requests[i]);

               if(index < 0)
                  continue;

               infPtr = coll_At(nonblockInfo *, ReqStructColl, index);

               if(infPtr->isStarted == 0)
                  continue;

               if(j == ListCount)
               {  j = 0;
                  tprintf("\n");
               }

               tprintf("%lx,", (uLLng)&array_of_requests[i]);
               j++;
            }

            tprintf("\t\nscount=%d", k);
         }
         else
         {
            if(MPI_SlashOut)
               tprintf("\n");

            tprintf("$call_MPI_Waitall\tcount=0");
         }

         if(k > 0 && MPI_TraceMsgChecksum)
         {
            tprintf("\t\nsumlist=\n");

            for(i=0,j=0; i < count; i++)
            {
               index = coll_IndexOf(RequestColl, &array_of_requests[i]);

               if(index < 0)
                  continue;

               infPtr = coll_At(nonblockInfo *, ReqStructColl, index);

               if(infPtr->isStarted == 0 || infPtr->isSend == 0)
                  continue;

               if(j == ListCount)
               {  j = 0;
                  tprintf("\n");
               }

               tprintf("%u,", checksum[i]);

               j++;
            }
         }

         tprintf("\t\n");

         if(MPI_TraceTime)
         {  start = dvm_time();

            if(MPI_TraceTimeReg)
            {  tprintf("t=%lf\t", start-LastEventTime);
               LastEventTime = start;
            }
            else
               tprintf("t=%lf\t", start);
         }

         if(MPI_TraceFileLine)
            tprintf("f=%s\tl=%ld\t", DVM_FILE[0], DVM_LINE[0]);
         tprintf("$\n");
      }
   }

   retval = PMPI_Waitall(count, array_of_requests, array_of_statuses);

   if(debug || trace)
   {
      if(n > 0)
      {
         if(MPI_TraceMsgChecksum || MPI_DebugMsgChecksum || trace)
         {
            mac_malloc(post_count, int *, count, 0);

            for(i=0; i < count; i++)
            {
               index = coll_IndexOf(RequestColl, &array_of_requests[i]);

               if(index < 0)
                  continue;

               infPtr = coll_At(nonblockInfo *, ReqStructColl, index);
      
               if(infPtr->isSend == 0 && infPtr->isStarted != 0)
                  PMPI_Get_count(&array_of_statuses[i], infPtr->datatype,
                                 &post_count[i]);
            }
         }

         if(MPI_TraceMsgChecksum || MPI_DebugMsgChecksum)
         {
            for(i=0; i < count; i++)
            {
               index = coll_IndexOf(RequestColl, &array_of_requests[i]);

               if(index < 0)
                  continue;

               infPtr = coll_At(nonblockInfo *, ReqStructColl, index);
      
               if(infPtr->isSend == 0 && infPtr->isStarted != 0)
                  checksum[i] = _checkSum(infPtr->buffer, post_count[i],
                                          infPtr->datatype);
            }
         }
      }

      if(trace)
      {
         if(n > 0)
         {
            tprintf("$ret_MPI_Waitall\trcount=%d\t\ncountlist=\n", n);

            for(i=0,j=0; i < count; i++)
            {
               index = coll_IndexOf(RequestColl, &array_of_requests[i]);

               if(index < 0)
                  continue;

               infPtr = coll_At(nonblockInfo *, ReqStructColl, index);
      
               if(infPtr->isSend == 0 && infPtr->isStarted != 0)
               {
                  if(j == ListCount)
                  {  j = 0;
                     tprintf("\n");
                  }

                  tprintf("%d,", post_count[i]);

                  j++;
               }
            }

            tprintf("\t\nsizelist=\n");

            for(i=0,j=0; i < count; i++)
            {
               index = coll_IndexOf(RequestColl, &array_of_requests[i]);

               if(index < 0)
                  continue;

               infPtr = coll_At(nonblockInfo *, ReqStructColl, index);
      
               if(infPtr->isSend == 0 && infPtr->isStarted != 0)
               {
                  if(j == ListCount)
                  {  j = 0;
                     tprintf("\n");
                  }

                  tprintf("%d,", _size(post_count[i], infPtr->datatype));

                  j++;
               }
            }

            tprintf("\t\nplist=\n");

            for(i=0,j=0; i < count; i++)
            {
               index = coll_IndexOf(RequestColl, &array_of_requests[i]);

               if(index < 0)
                  continue;

               infPtr = coll_At(nonblockInfo *, ReqStructColl, index);
      
               if(infPtr->isSend == 0 && infPtr->isStarted != 0)
               {
                  if(j == ListCount)
                  {  j = 0;
                     tprintf("\n");
                  }

                  tprintf("%d,", array_of_statuses[i].MPI_SOURCE);

                  j++;
               }
            }

            tprintf("\t\niplist=\n");

            for(i=0,j=0; i < count; i++)
            {
               index = coll_IndexOf(RequestColl, &array_of_requests[i]);

               if(index < 0)
                  continue;

               infPtr = coll_At(nonblockInfo *, ReqStructColl, index);
      
               if(infPtr->isSend == 0 && infPtr->isStarted != 0)
               {
                  if(j == ListCount)
                  {  j = 0;
                     tprintf("\n");
                  }

                  tprintf("%d,",
                          _commWorldRank(array_of_statuses[i].MPI_SOURCE,
                                         infPtr->comm));
                  j++;
               }
            }

            tprintf("\t\ntaglist=\n");

            for(i=0,j=0; i < count; i++)
            {
               index = coll_IndexOf(RequestColl, &array_of_requests[i]);

               if(index < 0)
                  continue;

               infPtr = coll_At(nonblockInfo *, ReqStructColl, index);
      
               if(infPtr->isSend == 0 && infPtr->isStarted != 0)
               {
                  if(j == ListCount)
                  {  j = 0;
                     tprintf("\n");
                  }

                  tprintf("%d,", array_of_statuses[i].MPI_TAG);

                  j++;
               }
            }

            if(MPI_TraceMsgChecksum)
            {
               tprintf("\t\nsumlist=\n");

               for(i=0,j=0; i < count; i++)
               {
                  index = coll_IndexOf(RequestColl,
                                       &array_of_requests[i]);

                  if(index < 0)
                     continue;

                  infPtr = coll_At(nonblockInfo *, ReqStructColl, index);
      
                  if(infPtr->isSend == 0 && infPtr->isStarted != 0)
                  {
                     if(j == ListCount)
                     {  j = 0;
                        tprintf("\n");
                     }

                     tprintf("%u,", checksum[i]);
                     j++;
                  }
               }
            }

            tprintf("\t\n");

            if(MPI_TraceTime)
            {  finish = dvm_time();

               if(MPI_TraceTimeReg)
               {  tprintf("t=%lf\t", finish-LastEventTime);
                  LastEventTime = finish;
               }
               else
                  tprintf("t=%lf\t", finish);
            }

            tprintf("$\n");
         }
         else
         {
            tprintf("$ret_MPI_Waitall\trcount=0\t");

            if(MPI_TraceTime)
            {  finish = dvm_time();

               if(MPI_TraceTimeReg)
               {  tprintf("t=%lf\t", finish-LastEventTime);
                  LastEventTime = finish;
               }
               else
                  tprintf("t=%lf\t", finish);
            }

            tprintf("$\n");
         }
      }

      for(i=0; i < count; i++)
      {
         index = coll_IndexOf(RequestColl, &array_of_requests[i]);

         if(index < 0)
            continue;

         infPtr = coll_At(nonblockInfo *, ReqStructColl, index);
      
         if(infPtr->isInit)
         { 
            infPtr->isStarted = 0;
            infPtr->MPITestCount = MPI_TestTraceCount;
            infPtr->TotalTestCount = 0;
         }
         else
         {
            coll_AtDelete(RequestColl, index);
            coll_AtDelete(ReqStructColl, index);
            dvm_FreeStruct(infPtr);
         }
      }

      mac_free((void **)&post_count);
      mac_free((void **)&checksum);
   }

   return  retval;
}



int  MPI_Testall(int  count, MPI_Request  array_of_requests[],
                 int  *flag, MPI_Status  array_of_statuses[])
{
   int             retval = 0, i, j, k, n;
   double          start, finish;
   nonblockInfo   *infPtr;
   int             system, trace, debug, index = -1;
   unsigned       *checksum = NULL;
   int            *post_count = NULL;
   int             TotalTestCount = -1, MPITestCount = -1;

   system = (RTS_Call_MPI || DVMCallLevel);
   trace  = (MPI_BotsulaProf && (MPI_TraceLevel || system == 0));
   debug  = (MPI_BotsulaDeb && (MPI_TraceLevel || system == 0));


   if(debug || trace)
   {
      if(count <= 0)
         epprintf(1 ,__FILE__, __LINE__,
                  "$*** dynamic analyzer err: wrong call MPI_Testall "
                  "(count=%d)$\n", count);

      if(MPI_TraceMsgChecksum || MPI_DebugBufChecksum ||
         MPI_DebugMsgChecksum)
      {
         mac_malloc(checksum, unsigned *, count, 0);
      }

      for(i=0,k=0,n=0; i < count; i++)
      {
         index = coll_IndexOf(RequestColl, &array_of_requests[i]);

         if(index < 0)
            continue;

         infPtr = coll_At(nonblockInfo *, ReqStructColl, index);

         if(infPtr->isStarted == 0)
            continue;

         if(trace)
         {
            infPtr->TotalTestCount++;
            infPtr->MPITestCount++;

            TotalTestCount = dvm_max(TotalTestCount,
                                     infPtr->TotalTestCount);
            MPITestCount = dvm_max(MPITestCount, infPtr->MPITestCount);

            infPtr->TotalTestCount = TotalTestCount;
            infPtr->MPITestCount = MPITestCount;
         }

         if(infPtr->isSend)
         {  
            if(MPI_TraceMsgChecksum || MPI_DebugBufChecksum)
               checksum[i] = _checkSum(infPtr->buffer, infPtr->count,
                                       infPtr->datatype);

            if(MPI_DebugBufChecksum)
               if(checksum[i] != infPtr->checksum)
                  epprintf(1 ,__FILE__, __LINE__,
                     "$*** dynamic analyzer err: wrong call MPI_Testall "
                     "(wrong send message checksum. index=%d)$\n", i);

            k++;  /* */    /*E0033*/
         }
         else
            n++;  /* */    /*E0034*/
      }

      if(trace && MPITestCount >= MPI_TestTraceCount)
      {
         MPITestCount = 0;

         if(n+k != 0)
         {
            if(MPI_SlashOut)
               tprintf("\n");

            tprintf("$call_MPI_Testall\tcount=%d\t\nreqlist=\n", n+k);

            for(i=0,j=0; i < count; i++)
            {
               index = coll_IndexOf(RequestColl, &array_of_requests[i]);

               if(index < 0)
                  continue;

               infPtr = coll_At(nonblockInfo *, ReqStructColl, index);

               if(infPtr->isStarted == 0)
                  continue;

               infPtr->MPITestCount = 0;

               if(j == ListCount)
               {  j = 0;
                  tprintf("\n");
               }

               tprintf("%lx,", (uLLng)&array_of_requests[i]);
               j++;
            }

            tprintf("\t\nscount=%d", k);
         }
         else
         {
            if(MPI_SlashOut)
               tprintf("\n");

            tprintf("$call_MPI_Testall\tcount=0");
         }

         if(k > 0 && MPI_TraceMsgChecksum)
         {
            tprintf("\t\nsumlist=\n");

            for(i=0,j=0; i < count; i++)
            {
               index = coll_IndexOf(RequestColl, &array_of_requests[i]);

               if(index < 0)
                  continue;

               infPtr = coll_At(nonblockInfo *, ReqStructColl, index);

               if(infPtr->isStarted == 0 || infPtr->isSend == 0)
                  continue;

               if(j == ListCount)
               {  j = 0;
                  tprintf("\n");
               }

               tprintf("%u,", checksum[i]);

               j++;
            }
         }

         tprintf("\t\n");

         if(MPI_TraceTime)
         {  start = dvm_time();

            if(MPI_TraceTimeReg)
            {  tprintf("t=%lf\t", start-LastEventTime);
               LastEventTime = start;
            }
            else
               tprintf("t=%lf\t", start);
         }

         if(MPI_TraceFileLine)
            tprintf("f=%s\tl=%ld\t", DVM_FILE[0], DVM_LINE[0]);
         tprintf("$\n");
      }
   }

   retval = PMPI_Testall(count, array_of_requests, flag,
                         array_of_statuses);

   if(debug || trace)
   {
      if(trace)
      {
         if(*flag && MPITestCount != 0)
         {  /* */    /*E0035*/

            MPITestCount = 0;

            if(n+k != 0)
            {
               if(MPI_SlashOut)
                  tprintf("\n");

               tprintf("$call_MPI_Testall\tcount=%d\t\nreqlist=\n", n+k);

               for(i=0,j=0; i < count; i++)
               {
                  index = coll_IndexOf(RequestColl,
                                       &array_of_requests[i]);

                  if(index < 0)
                     continue;

                  infPtr = coll_At(nonblockInfo *, ReqStructColl, index);

                  if(infPtr->isStarted == 0)
                     continue;

                  infPtr->MPITestCount = 0;

                  if(j == ListCount)
                  {  j = 0;
                     tprintf("\n");
                  }

                  tprintf("%lx,", (uLLng)&array_of_requests[i]);
                  j++;
               }

               tprintf("\t\nscount=%d", k);
            }
            else
            {  
               if(MPI_SlashOut)
                  tprintf("\n");

               tprintf("$call_MPI_Testall\tcount=0");
            }

            if(k > 0 && MPI_TraceMsgChecksum)
            {
               tprintf("\t\nsumlist=\n");

               for(i=0,j=0; i < count; i++)
               {
                  index = coll_IndexOf(RequestColl,
                                       &array_of_requests[i]);

                  if(index < 0)
                     continue;

                  infPtr = coll_At(nonblockInfo *, ReqStructColl, index);

                  if(infPtr->isStarted == 0 || infPtr->isSend == 0)
                     continue;

                  if(j == ListCount)
                  {  j = 0;
                     tprintf("\n");
                  }

                  tprintf("%u,", checksum[i]);

                  j++;
               }
            }

            tprintf("\t\n");

            if(MPI_TraceTime)
            {
               if(MPI_TraceTimeReg)
               {  tprintf("t=%lf\t", start-LastEventTime);
                  LastEventTime = start;
               }
               else
                  tprintf("t=%lf\t", start);
            }

            if(MPI_TraceFileLine)
               tprintf("f=%s\tl=%ld\t", DVM_FILE[0], DVM_LINE[0]);
            tprintf("$\n");
         }
      }

      if(n > 0 && *flag)
      {
         if(MPI_TraceMsgChecksum || MPI_DebugMsgChecksum || trace)
         {
            mac_malloc(post_count, int *, count, 0);

            for(i=0; i < count; i++)
            {
               index = coll_IndexOf(RequestColl, &array_of_requests[i]);

               if(index < 0)
                  continue;

               infPtr = coll_At(nonblockInfo *, ReqStructColl, index);
      
               if(infPtr->isSend == 0 && infPtr->isStarted != 0)
                  PMPI_Get_count(&array_of_statuses[i], infPtr->datatype,
                                 &post_count[i]);
            }
         }

         if(MPI_TraceMsgChecksum || MPI_DebugMsgChecksum)
         {
            for(i=0; i < count; i++)
            {
               index = coll_IndexOf(RequestColl, &array_of_requests[i]);

               if(index < 0)
                  continue;

               infPtr = coll_At(nonblockInfo *, ReqStructColl, index);
      
               if(infPtr->isSend == 0 && infPtr->isStarted != 0)
                  checksum[i] = _checkSum(infPtr->buffer, post_count[i],
                                          infPtr->datatype);
            }
         }
      }

      if(trace)
      {
         if(n > 0 && *flag && MPITestCount == 0)
         {
            tprintf("$ret_MPI_Testall\t"
                    "flag=%d\trcount=%d\t\ncountlist=\n",
                    *flag, n);

            for(i=0,j=0; i < count; i++)
            {
               index = coll_IndexOf(RequestColl, &array_of_requests[i]);

               if(index < 0)
                  continue;

               infPtr = coll_At(nonblockInfo *, ReqStructColl, index);
      
               if(infPtr->isSend == 0 && infPtr->isStarted != 0)
               {
                  if(j == ListCount)
                  {  j = 0;
                     tprintf("\n");
                  }

                  tprintf("%d,", post_count[i]);

                  j++;
               }
            }

            tprintf("\t\nsizelist=\n");

            for(i=0,j=0; i < count; i++)
            {
               index = coll_IndexOf(RequestColl, &array_of_requests[i]);

               if(index < 0)
                  continue;

               infPtr = coll_At(nonblockInfo *, ReqStructColl, index);
      
               if(infPtr->isSend == 0 && infPtr->isStarted != 0)
               {
                  if(j == ListCount)
                  {  j = 0;
                     tprintf("\n");
                  }

                  tprintf("%d,", _size(post_count[i], infPtr->datatype));

                  j++;
               }
            }

            tprintf("\t\nplist=\n");

            for(i=0,j=0; i < count; i++)
            {
               index = coll_IndexOf(RequestColl, &array_of_requests[i]);

               if(index < 0)
                  continue;

               infPtr = coll_At(nonblockInfo *, ReqStructColl, index);
      
               if(infPtr->isSend == 0 && infPtr->isStarted != 0)
               {
                  if(j == ListCount)
                  {  j = 0;
                     tprintf("\n");
                  }

                  tprintf("%d,", array_of_statuses[i].MPI_SOURCE);

                  j++;
               }
            }

            tprintf("\t\niplist=\n");

            for(i=0,j=0; i < count; i++)
            {
               index = coll_IndexOf(RequestColl, &array_of_requests[i]);

               if(index < 0)
                  continue;

               infPtr = coll_At(nonblockInfo *, ReqStructColl, index);
      
               if(infPtr->isSend == 0 && infPtr->isStarted != 0)
               {
                  if(j == ListCount)
                  {  j = 0;
                     tprintf("\n");
                  }

                  tprintf("%d,",
                          _commWorldRank(array_of_statuses[i].MPI_SOURCE,
                                         infPtr->comm));
                  j++;
               }
            }

            tprintf("\t\ntaglist=\n");

            for(i=0,j=0; i < count; i++)
            {
               index = coll_IndexOf(RequestColl, &array_of_requests[i]);

               if(index < 0)
                  continue;

               infPtr = coll_At(nonblockInfo *, ReqStructColl, index);
      
               if(infPtr->isSend == 0 && infPtr->isStarted != 0)
               {
                  if(j == ListCount)
                  {  j = 0;
                     tprintf("\n");
                  }

                  tprintf("%d,", array_of_statuses[i].MPI_TAG);

                  j++;
               }
            }

            if(MPI_TraceMsgChecksum)
            {
               tprintf("\t\nsumlist=\n");

               for(i=0,j=0; i < count; i++)
               {
                  index = coll_IndexOf(RequestColl,
                                       &array_of_requests[i]);

                  if(index < 0)
                     continue;

                  infPtr = coll_At(nonblockInfo *, ReqStructColl, index);
      
                  if(infPtr->isSend == 0 && infPtr->isStarted != 0)
                  {
                     if(j == ListCount)
                     {  j = 0;
                        tprintf("\n");
                     }

                     tprintf("%u,", checksum[i]);
                     j++;
                  }
               }
            }

            tprintf("\t\n");

            if(MPI_TraceTime)
            {  finish = dvm_time();

               if(MPI_TraceTimeReg)
               {  tprintf("t=%lf\t", finish-LastEventTime);
                  LastEventTime = finish;
               }
               else
                  tprintf("t=%lf\t", finish);
            }

            tprintf("$\n");
         }
         else
         {  if(MPITestCount == 0)
            {
               if(*flag == 0)
               {
                  tprintf("$ret_MPI_Testall\tflag=0\t");

                  if(MPI_TraceTime)
                  {  finish = dvm_time();

                     if(MPI_TraceTimeReg)
                     {  tprintf("t=%lf\t", finish-LastEventTime);
                        LastEventTime = finish;
                     }
                     else
                        tprintf("t=%lf\t", finish);
                  }

                  tprintf("$\n");
               }
               else
               {
                  tprintf("$ret_MPI_Testall\tflag=%d\trcount=0\t",
                          *flag);

                  if(MPI_TraceTime)
                  {  finish = dvm_time();

                     if(MPI_TraceTimeReg)
                     {  tprintf("t=%lf\t", finish-LastEventTime);
                        LastEventTime = finish;
                     }
                     else
                        tprintf("t=%lf\t", finish);
                  }

                  tprintf("$\n");
               }
            }
         }
      }

      if(*flag)
      {
         for(i=0; i < count; i++)
         {
            index = coll_IndexOf(RequestColl, &array_of_requests[i]);

            if(index < 0)
               continue;

            infPtr = coll_At(nonblockInfo *, ReqStructColl, index);
      
            if(infPtr->isInit)
            {
               infPtr->isStarted = 0;
               infPtr->MPITestCount = MPI_TestTraceCount;
               infPtr->TotalTestCount = 0;
            }
            else
            {
               coll_AtDelete(RequestColl, index);
               coll_AtDelete(ReqStructColl, index);
               dvm_FreeStruct(infPtr);
            }
         } 
      }

      mac_free((void **)&post_count);
      mac_free((void **)&checksum);
   }

   return  retval;
}



int  MPI_Waitany(int  count, MPI_Request  array_of_requests[],
                 int  *retindex, MPI_Status  *status)
{
   int             retval = 0, i, j, k = 0, n = 0;
   double          start, finish;
   nonblockInfo   *infPtr;
   int             system, trace, debug, index = -1;
   unsigned       *checksum = NULL;
   int             post_count;

   system = (RTS_Call_MPI || DVMCallLevel);
   trace  = (MPI_BotsulaProf && (MPI_TraceLevel || system == 0));
   debug  = (MPI_BotsulaDeb && (MPI_TraceLevel || system == 0));

   if(debug || trace)
   {
      if(count <= 0)
         epprintf(1 ,__FILE__, __LINE__,
                  "$*** dynamic analyzer err: wrong call MPI_Waitany "
                  "(count=%d)$\n", count);

      if(MPI_TraceMsgChecksum || MPI_DebugBufChecksum ||
         MPI_DebugMsgChecksum)
      {
         mac_malloc(checksum, unsigned *, count, 0);
      }

      for(i=0,k=0,n=0; i < count; i++)
      {
         index = coll_IndexOf(RequestColl, &array_of_requests[i]);

         if(index < 0)
            continue;

         infPtr = coll_At(nonblockInfo *, ReqStructColl, index);

         if(infPtr->isStarted == 0)
            continue;

         if(infPtr->isSend)
         {  
            if(MPI_TraceMsgChecksum || MPI_DebugBufChecksum)
               checksum[i] = _checkSum(infPtr->buffer, infPtr->count,
                                       infPtr->datatype);

            if(MPI_DebugBufChecksum)
               if(checksum[i] != infPtr->checksum)
                  epprintf(1 ,__FILE__, __LINE__,
                     "$*** dynamic analyzer err: wrong call MPI_Waitany "
                     "(wrong send message checksum. index=%d)$\n", i);

            k++;  /* */    /*E0036*/
         }
         else
            n++;  /* */    /*E0037*/
      }

      if(trace)
      {
         if(k+n != 0)
         {
            if(MPI_SlashOut)
               tprintf("\n");

            tprintf("$call_MPI_Waitany\tcount=%d\t\nreqlist=\n", k+n);

            for(i=0,j=0; i < count; i++)
            {
               index = coll_IndexOf(RequestColl, &array_of_requests[i]);

               if(index < 0)
                  continue;

               infPtr = coll_At(nonblockInfo *, ReqStructColl, index);

               if(infPtr->isStarted == 0)
                  continue;

               if(j == ListCount)
               {  j = 0;
                  tprintf("\n");
               }

               tprintf("%lx,", (uLLng)&array_of_requests[i]);
               j++;
            }

            tprintf("\t\nscount=%d", k);
         }
         else
         {
            if(MPI_SlashOut)
               tprintf("\n");

            tprintf("$call_MPI_Waitany\tcount=0");
         } 

         if(k > 0 && MPI_TraceMsgChecksum)
         {
            tprintf("\t\nsumlist=\n");

            for(i=0,j=0; i < count; i++)
            {
               index = coll_IndexOf(RequestColl, &array_of_requests[i]);

               if(index < 0)
                  continue;

               infPtr = coll_At(nonblockInfo *, ReqStructColl, index);

               if(infPtr->isStarted == 0 || infPtr->isSend == 0)
                  continue;

               if(j == ListCount)
               {  j = 0;
                  tprintf("\n");
               }

               tprintf("%u,", checksum[i]);

               j++;
            }
         }

         tprintf("\t\n");

         if(MPI_TraceTime)
         {  start = dvm_time();

            if(MPI_TraceTimeReg)
            {  tprintf("t=%lf\t", start-LastEventTime);
               LastEventTime = start;
            }
            else
               tprintf("t=%lf\t", start);
         }

         if(MPI_TraceFileLine)
            tprintf("f=%s\tl=%ld\t", DVM_FILE[0], DVM_LINE[0]);
         tprintf("$\n");
      }
   }

   retval = PMPI_Waitany(count, array_of_requests, retindex, status);

   if(debug || trace)
   {
      i = *retindex;

      if(i >= 0)
      {
         index = coll_IndexOf(RequestColl, &array_of_requests[i]);

         if(index >= 0)
            infPtr = coll_At(nonblockInfo *, ReqStructColl, index);
      }
      else
         index = -1;

      if(index >= 0)
      {
         if(infPtr->isStarted)
         {
            if(infPtr->isSend == 0)
            {
               if(MPI_TraceMsgChecksum || MPI_DebugMsgChecksum || trace)
                  PMPI_Get_count(status, infPtr->datatype, &post_count);

               if(MPI_TraceMsgChecksum || MPI_DebugMsgChecksum)
                  checksum[0] = _checkSum(infPtr->buffer, post_count,
                                          infPtr->datatype);
            }
         }
      } 

      if(trace)
      {
         if(index >= 0)
         {
            if(infPtr->isStarted)
            {
               if(infPtr->isSend == 0)
               {
                  tprintf("$ret_MPI_Waitany\treq=%lx\trecv=1\t\n",
                          (uLLng)&array_of_requests[i]);

                  tprintf("count=%d\tsize=%d\tproc=%d\t"
                          "iproc=%d\t\ntag=%d\t",
                          post_count, _size(post_count,
                                            infPtr->datatype),
                          status->MPI_SOURCE,
                          _commWorldRank(status->MPI_SOURCE,
                                         infPtr->comm),
                          status->MPI_TAG);

                  if(MPI_TraceMsgChecksum)
                     tprintf("sum=%u\t", checksum[0]);
               }
               else
                  tprintf("$ret_MPI_Waitany\treq=%lx\trecv=0\t",
                          (uLLng)&array_of_requests[i]);
            }
            else
               tprintf("$ret_MPI_Waitany\treq=0\t");
         }
         else
            tprintf("$ret_MPI_Waitany\treq=0\t");

         if(MPI_TraceTime)
         {  finish = dvm_time();

            if(MPI_TraceTimeReg)
            {  tprintf("t=%lf\t", finish-LastEventTime);
               LastEventTime = finish;
            }
            else
               tprintf("t=%lf\t", finish);
         } 

         tprintf("$\n");
      }

      if(index >= 0)
      {
         if(infPtr->isInit)
         {
            infPtr->isStarted = 0;
            infPtr->MPITestCount = MPI_TestTraceCount;
            infPtr->TotalTestCount = 0;
         }
         else
         {               
            coll_AtDelete(RequestColl, index);
            coll_AtDelete(ReqStructColl, index);
            dvm_FreeStruct(infPtr);
         }
      }

      mac_free((void **)&checksum);
   }

   return  retval;
}



int  MPI_Testany(int  count, MPI_Request  array_of_requests[],
                 int  *retindex, int  *flag, MPI_Status  *status)
{
   int             retval = 0, i, j, k, n;
   double          start, finish;
   nonblockInfo   *infPtr;
   int             system, trace, debug, index = -1;
   unsigned       *checksum = NULL;
   int             post_count;
   int             TotalTestCount = -1, MPITestCount = -1;

   system = (RTS_Call_MPI || DVMCallLevel);
   trace  = (MPI_BotsulaProf && (MPI_TraceLevel || system == 0));
   debug  = (MPI_BotsulaDeb && (MPI_TraceLevel || system == 0));

   if(debug || trace)
   {
      if(count <= 0)
         epprintf(1 ,__FILE__, __LINE__,
                  "$*** dynamic analyzer err: wrong call MPI_Testany "
                  "(count=%d)$\n", count);

      if(MPI_TraceMsgChecksum || MPI_DebugBufChecksum ||
         MPI_DebugMsgChecksum)
      {
         mac_malloc(checksum, unsigned *, count, 0);
      }

      for(i=0,k=0,n=0; i < count; i++)
      {
         index = coll_IndexOf(RequestColl, &array_of_requests[i]);

         if(index < 0)
            continue;

         infPtr = coll_At(nonblockInfo *, ReqStructColl, index);

         if(infPtr->isStarted == 0)
            continue;

         if(trace)
         {
            infPtr->TotalTestCount++;
            infPtr->MPITestCount++;

            TotalTestCount = dvm_max(TotalTestCount,
                                     infPtr->TotalTestCount);
            MPITestCount = dvm_max(MPITestCount, infPtr->MPITestCount);

            infPtr->TotalTestCount = TotalTestCount;
            infPtr->MPITestCount = MPITestCount;
         }

         if(infPtr->isSend)
         {  
            if(MPI_TraceMsgChecksum || MPI_DebugBufChecksum)
               checksum[i] = _checkSum(infPtr->buffer, infPtr->count,
                                       infPtr->datatype);

            if(MPI_DebugBufChecksum)
               if(checksum[i] != infPtr->checksum)
                  epprintf(1 ,__FILE__, __LINE__,
                     "$*** dynamic analyzer err: wrong call MPI_Testany "
                     "(wrong send message checksum. index=%d)$\n", i);

            k++;  /* */    /*E0038*/
         }
         else
            n++;  /* */    /*E0039*/
      }

      if(trace && MPITestCount >= MPI_TestTraceCount)
      {
         MPITestCount = 0;

         if(k+n != 0)
         {
            if(MPI_SlashOut)
               tprintf("\n");

            tprintf("$call_MPI_Testany\tcount=%d\t\nreqlist=\n", k+n);

            for(i=0,j=0; i < count; i++)
            {
               index = coll_IndexOf(RequestColl, &array_of_requests[i]);

               if(index < 0)
                  continue;

               infPtr = coll_At(nonblockInfo *, ReqStructColl, index);

               if(infPtr->isStarted == 0)
                  continue;

               infPtr->MPITestCount = 0;

               if(j == ListCount)
               {  j = 0;
                  tprintf("\n");
               }

               tprintf("%lx,", (uLLng)&array_of_requests[i]);
               j++;
            }

            tprintf("\t\nscount=%d", k);
         }
         else
         {
            if(MPI_SlashOut)
               tprintf("\n");

            tprintf("$call_MPI_Testany\tcount=0");
         }

         if(k > 0 && MPI_TraceMsgChecksum)
         {
            tprintf("\t\nsumlist=\n");

            for(i=0,j=0; i < count; i++)
            {
               index = coll_IndexOf(RequestColl, &array_of_requests[i]);

               if(index < 0)
                  continue;

               infPtr = coll_At(nonblockInfo *, ReqStructColl, index);

               if(infPtr->isStarted == 0 || infPtr->isSend == 0)
                  continue;

               if(j == ListCount)
               {  j = 0;
                  tprintf("\n");
               }

               tprintf("%u,", checksum[i]);

               j++;
            }
         }

         tprintf("\t\n");

         if(MPI_TraceTime)
         {  start = dvm_time();

            if(MPI_TraceTimeReg)
            {  tprintf("t=%lf\t", start-LastEventTime);
               LastEventTime = start;
            }
            else
               tprintf("t=%lf\t", start);
         }

         if(MPI_TraceFileLine)
            tprintf("f=%s\tl=%ld\t", DVM_FILE[0], DVM_LINE[0]);
         tprintf("$\n");
      }
   }

   retval = PMPI_Testany(count, array_of_requests, retindex, flag,
                         status);

   if(debug || trace)
   {
      /* */    /*E0040*/

      if(trace)
      {
         if(*flag && MPITestCount != 0)
         {  /* */    /*E0041*/

            MPITestCount = 0;

            if(k+n != 0)
            {
               if(MPI_SlashOut)
                  tprintf("\n");

               tprintf("$call_MPI_Testany\tcount=%d\t\nreqlist=\n", k+n);

               for(i=0,j=0; i < count; i++)
               {
                  index = coll_IndexOf(RequestColl,
                                       &array_of_requests[i]);

                  if(index < 0)
                     continue;

                  infPtr = coll_At(nonblockInfo *, ReqStructColl, index);

                  if(infPtr->isStarted == 0)
                     continue;

                  infPtr->MPITestCount = 0;

                  if(j == ListCount)
                  {  j = 0;
                     tprintf("\n");
                  }

                  tprintf("%lx,", (uLLng)&array_of_requests[i]);
                  j++;
               }

               tprintf("\t\nscount=%d", k);
            }
            else
            {
               if(MPI_SlashOut)
                  tprintf("\n");

               tprintf("$call_MPI_Testany\tcount=0");
            }

            if(k > 0 && MPI_TraceMsgChecksum)
            {
               tprintf("\t\nsumlist=\n");

               for(i=0,j=0; i < count; i++)
               {
                  index = coll_IndexOf(RequestColl,
                                       &array_of_requests[i]);

                  if(index < 0)
                     continue;

                  infPtr = coll_At(nonblockInfo *, ReqStructColl, index);

                  if(infPtr->isStarted == 0 || infPtr->isSend == 0)
                     continue;

                  if(j == ListCount)
                  {  j = 0;
                     tprintf("\n");
                  }

                  tprintf("%u,", checksum[i]);

                  j++;
               }
            }

            tprintf("\t\n");

            if(MPI_TraceTime)
            {
               if(MPI_TraceTimeReg)
               {  tprintf("t=%lf\t", start-LastEventTime);
                  LastEventTime = start;
               }
               else
                  tprintf("t=%lf\t", start);
            }

            if(MPI_TraceFileLine)
               tprintf("f=%s\tl=%ld\t", DVM_FILE[0], DVM_LINE[0]);
            tprintf("$\n");
         }
      }

      /* */    /*E0042*/

      if(*flag)
         i = *retindex;
      else
         i = -1;

      if(i >= 0)
      {
         index = coll_IndexOf(RequestColl, &array_of_requests[i]);

         if(index >= 0)
            infPtr = coll_At(nonblockInfo *, ReqStructColl, index);
      }
      else
         index = -1;

      if(*flag && index >= 0)
      {
         if(infPtr->isStarted)
         {
            if(infPtr->isSend == 0)
            {
               if(MPI_TraceMsgChecksum || MPI_DebugMsgChecksum || trace)
                  PMPI_Get_count(status, infPtr->datatype, &post_count);

               if(MPI_TraceMsgChecksum || MPI_DebugMsgChecksum)
                  checksum[0] = _checkSum(infPtr->buffer, post_count,
                                          infPtr->datatype);
            }
         }
      } 

      if(trace)
      {
         /* */    /*E0043*/

         if(MPITestCount == 0)
         {
            if(*flag)
            {
               if(index >= 0)
               {
                  if(infPtr->isStarted)
                  {
                     if(infPtr->isSend == 0)
                     {
                        tprintf("$ret_MPI_Testany\t"
                                "flag=1\treq=%lx\trecv=1\t\n",
                                (uLLng)&array_of_requests[i]);

                        tprintf("count=%d\tsize=%d\tproc=%d\t"
                                "iproc=%d\t\ntag=%d\t",
                                post_count, _size(post_count,
                                                  infPtr->datatype),
                                status->MPI_SOURCE,
                                _commWorldRank(status->MPI_SOURCE,
                                               infPtr->comm),
                                status->MPI_TAG);

                        if(MPI_TraceMsgChecksum)
                           tprintf("sum=%u\t", checksum[0]);
                     }
                     else
                        tprintf("$ret_MPI_Testany\t"
                                "flag=1\treq=%lx\trecv=0\t",
                                (uLLng)&array_of_requests[i]);
                  }
                  else
                     tprintf("$ret_MPI_Testany\tflag=1\treq=0\t");
               }
               else
                  tprintf("$ret_MPI_Testany\tflag=1\treq=0\t");
            }
            else
               tprintf("$ret_MPI_Testany\tflag=0\t");

            if(MPI_TraceTime)
            {  finish = dvm_time();

               if(MPI_TraceTimeReg)
               {  tprintf("t=%lf\t", finish-LastEventTime);
                  LastEventTime = finish;
               }
               else
                  tprintf("t=%lf\t", finish);
            }

            tprintf("$\n");
         }
      }

      if(index >= 0)
      {
         if(infPtr->isInit)
         {
            infPtr->isStarted = 0;
            infPtr->MPITestCount = MPI_TestTraceCount;
            infPtr->TotalTestCount = 0;
         }
         else
         {               
            coll_AtDelete(RequestColl, index);
            coll_AtDelete(ReqStructColl, index);
            dvm_FreeStruct(infPtr);
         }
      }

      mac_free((void **)&checksum);
   }

   return  retval;
}



int  MPI_Waitsome(int  incount, MPI_Request  array_of_requests[],
                  int  *outcount, int  array_of_indices[], 
                  MPI_Status  array_of_statuses[])
{
   int             retval = 0, i, j, k = 0, n = 0, m;
   double          start, finish;
   nonblockInfo   *infPtr;
   int             system, trace, debug, index = -1, count;
   unsigned       *checksum = NULL;
   int            *post_count = NULL;

   system = (RTS_Call_MPI || DVMCallLevel);
   trace  = (MPI_BotsulaProf && (MPI_TraceLevel || system == 0));
   debug  = (MPI_BotsulaDeb && (MPI_TraceLevel || system == 0));

   if(debug || trace)
   {
      if(incount <= 0)
         epprintf(1 ,__FILE__, __LINE__,
                  "$*** dynamic analyzer err: wrong call MPI_Waitsome "
                  "(incount=%d)$\n", incount);

      if(MPI_TraceMsgChecksum || MPI_DebugBufChecksum ||
         MPI_DebugMsgChecksum)
      {
         mac_malloc(checksum, unsigned *, incount, 0);
      }

      for(i=0,k=0,n=0; i < incount; i++)
      {
         index = coll_IndexOf(RequestColl, &array_of_requests[i]);

         if(index < 0)
            continue;

         infPtr = coll_At(nonblockInfo *, ReqStructColl, index);

         if(infPtr->isStarted == 0)
            continue;

         if(infPtr->isSend)
         {  
            if(MPI_TraceMsgChecksum || MPI_DebugBufChecksum)
               checksum[i] = _checkSum(infPtr->buffer, infPtr->count,
                                       infPtr->datatype);

            if(MPI_DebugBufChecksum)
               if(checksum[i] != infPtr->checksum)
                  epprintf(1 ,__FILE__, __LINE__,
                    "$*** dynamic analyzer err: wrong call MPI_Waitsome "
                    "(wrong send message checksum. index=%d)$\n", i);

            k++;  /* */    /*E0044*/
         }
         else
            n++;  /* */    /*E0045*/
      }

      if(trace)
      {
         if(k+n != 0)
         {
            if(MPI_SlashOut)
               tprintf("\n");

            tprintf("$call_MPI_Waitsome\tincount=%d\t\nreqlist=\n", k+n);

            for(i=0,j=0; i < incount; i++)
            {
               index = coll_IndexOf(RequestColl, &array_of_requests[i]);

               if(index < 0)
                  continue;

               infPtr = coll_At(nonblockInfo *, ReqStructColl, index);

               if(infPtr->isStarted == 0)
                  continue;

               if(j == ListCount)
               {  j = 0;
                  tprintf("\n");
               }

               tprintf("%lx,", (uLLng)&array_of_requests[i]);
               j++;
            }

            tprintf("\t\nscount=%d", k);
         }
         else
         {
            if(MPI_SlashOut)
               tprintf("\n");

            tprintf("$call_MPI_Waitsome\tincount=0");
         }

         if(k > 0 && MPI_TraceMsgChecksum)
         {
            tprintf("\t\nsumlist=\n");

            for(i=0,j=0; i < incount; i++)
            {
               index = coll_IndexOf(RequestColl, &array_of_requests[i]);

               if(index < 0)
                  continue;

               infPtr = coll_At(nonblockInfo *, ReqStructColl, index);

               if(infPtr->isStarted == 0 || infPtr->isSend == 0)
                  continue;

               if(j == ListCount)
               {  j = 0;
                  tprintf("\n");
               }

               tprintf("%u,", checksum[i]);

               j++;
            }
         }

         tprintf("\t\n");

         if(MPI_TraceTime)
         {  start = dvm_time();

            if(MPI_TraceTimeReg)
            {  tprintf("t=%lf\t", start-LastEventTime);
               LastEventTime = start;
            }
            else
               tprintf("t=%lf\t", start);
         }

         if(MPI_TraceFileLine)
            tprintf("f=%s\tl=%ld\t", DVM_FILE[0], DVM_LINE[0]);
         tprintf("$\n");
      }
   }

   retval = PMPI_Waitsome(incount, array_of_requests, outcount,
                          array_of_indices, array_of_statuses);

   if(debug || trace)
   {
      count = *outcount;
      n = 0;

      if(MPI_TraceMsgChecksum || MPI_DebugMsgChecksum || trace)
      { 
         for(j=0,k=0,n=0; j < count; j++)
         {
            i = array_of_indices[j];

            index = coll_IndexOf(RequestColl, &array_of_requests[i]);

            if(index < 0)
               continue;

            infPtr = coll_At(nonblockInfo *, ReqStructColl, index);

            if(infPtr->isStarted == 0)
               continue;

            if(infPtr->isSend)
               k++;  /* */    /*E0046*/
            else
               n++;  /* */    /*E0047*/
         }
      }

      if(n > 0)
      {
         if(MPI_TraceMsgChecksum || MPI_DebugMsgChecksum || trace)
         {
            mac_malloc(post_count, int *, count, 0);

            for(j=0; j < count; j++)
            {
               i = array_of_indices[j];

               index = coll_IndexOf(RequestColl, &array_of_requests[i]);

               if(index < 0)
                  continue;

               infPtr = coll_At(nonblockInfo *, ReqStructColl, index);
      
               if(infPtr->isSend == 0 && infPtr->isStarted != 0)
                  PMPI_Get_count(&array_of_statuses[j], infPtr->datatype,
                                 &post_count[j]);
            }
         }

         if(MPI_TraceMsgChecksum || MPI_DebugMsgChecksum)
         {
            for(j=0; j < count; j++)
            {
               i = array_of_indices[j];

               index = coll_IndexOf(RequestColl, &array_of_requests[i]);

               if(index < 0)
                  continue;

               infPtr = coll_At(nonblockInfo *, ReqStructColl, index);
      
               if(infPtr->isSend == 0 && infPtr->isStarted != 0)
                  checksum[j] = _checkSum(infPtr->buffer, post_count[j],
                                          infPtr->datatype);
            }
         }
      }

      if(trace)
      {
         tprintf("$ret_MPI_Waitsome\toutcount=%d", k+n);

         if(k+n != 0)
         {
            tprintf("\t\nreqlist=\n");

            for(j=0,m=0; j < count; j++)
            {
               i = array_of_indices[j];

               index = coll_IndexOf(RequestColl, &array_of_requests[i]);

               if(index < 0)
                  continue;

               infPtr = coll_At(nonblockInfo *, ReqStructColl, index);
      
               if(infPtr->isStarted == 0)
                  continue; 

               if(m == ListCount)
               {  m = 0;
                  tprintf("\n");
               }

               tprintf("%lx,", (uLLng)&array_of_requests[i]);
               m++;
            }

            tprintf("\t\nrcount=%d", n);
         }

         if(n > 0)
         {
            tprintf("\t\ncountlist=\n");

            for(j=0,m=0; j < count; j++)
            {
               i = array_of_indices[j];

               index = coll_IndexOf(RequestColl, &array_of_requests[i]);

               if(index < 0)
                  continue;

               infPtr = coll_At(nonblockInfo *, ReqStructColl, index);
      
               if(infPtr->isSend || infPtr->isStarted == 0)
                  continue;

               if(m == ListCount)
               {  m = 0;
                  tprintf("\n");
               }

               tprintf("%d,", post_count[j]);
               m++;
            }

            tprintf("\t\nsizelist=\n");

            for(j=0,m=0; j < count; j++)
            {
               i = array_of_indices[j];

               index = coll_IndexOf(RequestColl, &array_of_requests[i]);

               if(index < 0)
                  continue;

               infPtr = coll_At(nonblockInfo *, ReqStructColl, index);
      
               if(infPtr->isSend || infPtr->isStarted == 0)
                  continue;

               if(m == ListCount)
               {  m = 0;
                  tprintf("\n");
               }

               tprintf("%d,", _size(post_count[j], infPtr->datatype));
               m++;
            }

            tprintf("\t\nplist=\n");

            for(j=0,m=0; j < count; j++)
            {
               i = array_of_indices[j];

               index = coll_IndexOf(RequestColl, &array_of_requests[i]);

               if(index < 0)
                  continue;

               infPtr = coll_At(nonblockInfo *, ReqStructColl, index);
      
               if(infPtr->isSend || infPtr->isStarted == 0)
                  continue;

               if(m == ListCount)
               {  m = 0;
                  tprintf("\n");
               }

               tprintf("%d,", array_of_statuses[j].MPI_SOURCE);
               m++;
            }

            tprintf("\t\niplist=\n");
   
            for(j=0,m=0; j < count; j++)
            {
               i = array_of_indices[j];

               index = coll_IndexOf(RequestColl, &array_of_requests[i]);

               if(index < 0)
                  continue;

               infPtr = coll_At(nonblockInfo *, ReqStructColl, index);
      
               if(infPtr->isSend || infPtr->isStarted == 0)
                  continue;

               if(m == ListCount)
               {  m = 0;
                  tprintf("\n");
               }

               tprintf("%d,",
                       _commWorldRank(array_of_statuses[j].MPI_SOURCE,
                                      infPtr->comm));
               m++;
            }

            tprintf("\t\ntaglist=\n");

            for(j=0,m=0; j < count; j++)
            {
               i = array_of_indices[j];

               index = coll_IndexOf(RequestColl, &array_of_requests[i]);

               if(index < 0)
                  continue;

               infPtr = coll_At(nonblockInfo *, ReqStructColl, index);
      
               if(infPtr->isSend || infPtr->isStarted == 0)
                  continue;

               if(m == ListCount)
               {  m = 0;
                  tprintf("\n");
               }

               tprintf("%d,", array_of_statuses[j].MPI_TAG);
               m++;
            }

            if(MPI_TraceMsgChecksum)
            {
               tprintf("\t\nsumlist=\n");

               for(j=0,m=0; j < count; j++)
               {
                  i = array_of_indices[j];

                  index = coll_IndexOf(RequestColl,
                                       &array_of_requests[i]);

                  if(index < 0)
                     continue;

                  infPtr = coll_At(nonblockInfo *, ReqStructColl, index);
      
                  if(infPtr->isSend || infPtr->isStarted == 0)
                     continue;

                  if(m == ListCount)
                  {  m = 0;
                     tprintf("\n");
                  }

                  tprintf("%u,", checksum[j]);
                  m++;
               }
            }
         }

         tprintf("\t\n");

         if(MPI_TraceTime)
         {  finish = dvm_time();

            if(MPI_TraceTimeReg)
            {  tprintf("t=%lf\t", finish-LastEventTime);
               LastEventTime = finish;
            }
            else
               tprintf("t=%lf\t", finish);
         }

         tprintf("$\n");
      }

      for(j=0; j < count; j++)
      {
         i = array_of_indices[j];

         index = coll_IndexOf(RequestColl, &array_of_requests[i]);

         if(index < 0)
            continue;

         infPtr = coll_At(nonblockInfo *, ReqStructColl, index);
      
         if(infPtr->isInit)
         {
            infPtr->isStarted = 0;
            infPtr->MPITestCount = MPI_TestTraceCount;
            infPtr->TotalTestCount = 0;
         }
         else
         {
            coll_AtDelete(RequestColl, index);
            coll_AtDelete(ReqStructColl, index);
            dvm_FreeStruct(infPtr);
         }
      }

      mac_free((void **)&post_count);
      mac_free((void **)&checksum);
   }

   return  retval;
}



int  MPI_Testsome(int  incount, MPI_Request  array_of_requests[],
                  int  *outcount, int  array_of_indices[], 
                  MPI_Status  array_of_statuses[])
{
   int             retval = 0, i, j, k = 0, n = 0, m;
   double          start, finish;
   nonblockInfo   *infPtr;
   int             system, trace, debug, index = -1, count;
   unsigned       *checksum = NULL;
   int            *post_count = NULL;
   int             TotalTestCount = -1, MPITestCount = -1;

   system = (RTS_Call_MPI || DVMCallLevel);
   trace  = (MPI_BotsulaProf && (MPI_TraceLevel || system == 0));
   debug  = (MPI_BotsulaDeb && (MPI_TraceLevel || system == 0));

   if(debug || trace)
   {
      if(incount <= 0)
         epprintf(1 ,__FILE__, __LINE__,
                  "$*** dynamic analyzer err: wrong call MPI_Testsome "
                  "(incount=%d)$\n", incount);

      if(MPI_TraceMsgChecksum || MPI_DebugBufChecksum ||
         MPI_DebugMsgChecksum)
      {
         mac_malloc(checksum, unsigned *, incount, 0);
      }

      for(i=0,k=0,n=0; i < incount; i++)
      {
         index = coll_IndexOf(RequestColl, &array_of_requests[i]);

         if(index < 0)
            continue;

         infPtr = coll_At(nonblockInfo *, ReqStructColl, index);

         if(infPtr->isStarted == 0)
            continue;

         if(trace)
         {
            infPtr->TotalTestCount++;
            infPtr->MPITestCount++;

            TotalTestCount = dvm_max(TotalTestCount,
                                     infPtr->TotalTestCount);
            MPITestCount = dvm_max(MPITestCount, infPtr->MPITestCount);

            infPtr->TotalTestCount = TotalTestCount;
            infPtr->MPITestCount = MPITestCount;
         }

         if(infPtr->isSend)
         {  
            if(MPI_TraceMsgChecksum || MPI_DebugBufChecksum)
               checksum[i] = _checkSum(infPtr->buffer, infPtr->count,
                                       infPtr->datatype);

            if(MPI_DebugBufChecksum)
               if(checksum[i] != infPtr->checksum)
                  epprintf(1 ,__FILE__, __LINE__,
                    "$*** dynamic analyzer err: wrong call MPI_Testsome "
                    "(wrong send message checksum. index=%d)$\n", i);

            k++;  /* */    /*E0048*/
         }
         else
            n++;  /* */    /*E0049*/
      }

      if(trace && MPITestCount >= MPI_TestTraceCount)
      {
         MPITestCount = 0;

         if(k+n != 0)
         {
            if(MPI_SlashOut)
               tprintf("\n");

            tprintf("$call_MPI_Testsome\tincount=%d\t\nreqlist=\n", k+n);

            for(i=0,j=0; i < incount; i++)
            {
               index = coll_IndexOf(RequestColl, &array_of_requests[i]);

               if(index < 0)
                  continue;

               infPtr = coll_At(nonblockInfo *, ReqStructColl, index);

               if(infPtr->isStarted == 0)
                  continue;

               if(j == ListCount)
               {  j = 0;
                  tprintf("\n");
               }

               tprintf("%lx,", (uLLng)&array_of_requests[i]);
               j++;
            }

            tprintf("\t\nscount=%d", k);
         }
         else
         {
            if(MPI_SlashOut)
               tprintf("\n");

            tprintf("$call_MPI_Testsome\tincount=0");
         }

         if(k > 0 && MPI_TraceMsgChecksum)
         {
            tprintf("\t\nsumlist=\n");

            for(i=0,j=0; i < incount; i++)
            {
               index = coll_IndexOf(RequestColl, &array_of_requests[i]);

               if(index < 0)
                  continue;

               infPtr = coll_At(nonblockInfo *, ReqStructColl, index);

               if(infPtr->isStarted == 0 || infPtr->isSend == 0)
                  continue;

               if(j == ListCount)
               {  j = 0;
                  tprintf("\n");
               }

               tprintf("%u,", checksum[i]);

               j++;
            }
         }

         tprintf("\t\n");

         if(MPI_TraceTime)
         {  start = dvm_time();

            if(MPI_TraceTimeReg)
            {  tprintf("t=%lf\t", start-LastEventTime);
               LastEventTime = start;
            }
            else
               tprintf("t=%lf\t", start);
         } 

         if(MPI_TraceFileLine)
            tprintf("f=%s\tl=%ld\t", DVM_FILE[0], DVM_LINE[0]);
         tprintf("$\n");
      }
   }

   retval = PMPI_Testsome(incount, array_of_requests, outcount,
                          array_of_indices, array_of_statuses);

   if(debug || trace)
   {
      count = *outcount;

      if(trace)
      {
         if(count > 0 && MPITestCount != 0)
         {  /* */    /*E0050*/

            MPITestCount = 0;

            if(k+n != 0)
            {
               if(MPI_SlashOut)
                  tprintf("\n");

               tprintf("$call_MPI_Testsome\tincount=%d\t\nreqlist=\n",
                        k+n);

               for(i=0,j=0; i < incount; i++)
               {
                  index = coll_IndexOf(RequestColl,
                                       &array_of_requests[i]);

                  if(index < 0)
                     continue;

                  infPtr = coll_At(nonblockInfo *, ReqStructColl, index);

                 if(infPtr->isStarted == 0)
                     continue;

                  if(j == ListCount)
                  {  j = 0;
                     tprintf("\n");
                  }

                  tprintf("%lx,", (uLLng)&array_of_requests[i]);
                  j++;
               }

               tprintf("\t\nscount=%d", k);
            }
            else
            {
               if(MPI_SlashOut)
                  tprintf("\n");

               tprintf("$call_MPI_Testsome\tincount=0");
            }

            if(k > 0 && MPI_TraceMsgChecksum)
            {
               tprintf("\t\nsumlist=\n");

               for(i=0,j=0; i < incount; i++)
               {
                  index = coll_IndexOf(RequestColl,
                                       &array_of_requests[i]);

                  if(index < 0)
                     continue;

                  infPtr = coll_At(nonblockInfo *, ReqStructColl, index);

                  if(infPtr->isStarted == 0 || infPtr->isSend == 0)
                     continue;

                  if(j == ListCount)
                  {  j = 0;
                     tprintf("\n");
                  }

                  tprintf("%u,", checksum[i]);
                  j++;
              }
            }

            tprintf("\t\n");

            if(MPI_TraceTime)
            {
               if(MPI_TraceTimeReg)
               {  tprintf("t=%lf\t", start-LastEventTime);
                  LastEventTime = start;
               }
               else
                  tprintf("t=%lf\t", start);
            }

            if(MPI_TraceFileLine)
               tprintf("f=%s\tl=%ld\t", DVM_FILE[0], DVM_LINE[0]);
            tprintf("$\n");
         }
      }

      n = 0;

      if(MPI_TraceMsgChecksum || MPI_DebugMsgChecksum || trace)
      { 
         for(j=0,k=0,n=0; j < count; j++)
         {
            i = array_of_indices[j];

            index = coll_IndexOf(RequestColl, &array_of_requests[i]);

            if(index < 0)
               continue;

            infPtr = coll_At(nonblockInfo *, ReqStructColl, index);

            if(infPtr->isStarted == 0)
               continue;

            if(infPtr->isSend)
               k++;  /* */    /*E0051*/
            else
               n++;  /* */    /*E0052*/
         }
      }

      if(n > 0)
      {
         if(MPI_TraceMsgChecksum || MPI_DebugMsgChecksum || trace)
         {
            mac_malloc(post_count, int *, count, 0);

            for(j=0; j < count; j++)
            {
               i = array_of_indices[j];

               index = coll_IndexOf(RequestColl, &array_of_requests[i]);

               if(index < 0)
                  continue;

               infPtr = coll_At(nonblockInfo *, ReqStructColl, index);
      
               if(infPtr->isSend == 0 && infPtr->isStarted != 0)
                  PMPI_Get_count(&array_of_statuses[j], infPtr->datatype,
                                 &post_count[j]);
            }
         }

         if(MPI_TraceMsgChecksum || MPI_DebugMsgChecksum)
         {
            for(j=0; j < count; j++)
            {
               i = array_of_indices[j];

               index = coll_IndexOf(RequestColl, &array_of_requests[i]);

               if(index < 0)
                  continue;

               infPtr = coll_At(nonblockInfo *, ReqStructColl, index);
      
               if(infPtr->isSend == 0 && infPtr->isStarted != 0)
                  checksum[j] = _checkSum(infPtr->buffer, post_count[j],
                                          infPtr->datatype);
            }
         }
      }

      if(trace)
      {
         if(MPITestCount == 0)
            tprintf("$ret_MPI_Testsome\toutcount=%d", k+n);

         if(k+n != 0 && MPITestCount == 0)
         {
            tprintf("\t\nreqlist=\n");

            for(j=0,m=0; j < count; j++)
            {
               i = array_of_indices[j];

               index = coll_IndexOf(RequestColl, &array_of_requests[i]);

               if(index < 0)
                  continue;

               infPtr = coll_At(nonblockInfo *, ReqStructColl, index);
      
               if(infPtr->isStarted == 0)
                  continue; 

               if(m == ListCount)
               {  m = 0;
                  tprintf("\n");
               }

               tprintf("%lx,", (uLLng)&array_of_requests[i]);
               m++;
            }

            tprintf("\t\nrcount=%d", n);
         }

         if(n > 0 && MPITestCount == 0)
         {
            mac_malloc(post_count, int *, count, 0);

            tprintf("\t\ncountlist=\n");

            for(j=0,m=0; j < count; j++)
            {
               i = array_of_indices[j];

               index = coll_IndexOf(RequestColl, &array_of_requests[i]);

               if(index < 0)
                  continue;

               infPtr = coll_At(nonblockInfo *, ReqStructColl, index);
      
               if(infPtr->isSend || infPtr->isStarted == 0)
                  continue;

               if(m == ListCount)
               {  m = 0;
                  tprintf("\n");
               }

               tprintf("%d,", post_count[j]);
               m++;
            }

            tprintf("\t\nsizelist=\n");

            for(j=0,m=0; j < count; j++)
            {
               i = array_of_indices[j];

               index = coll_IndexOf(RequestColl, &array_of_requests[i]);

               if(index < 0)
                  continue;

               infPtr = coll_At(nonblockInfo *, ReqStructColl, index);
      
               if(infPtr->isSend || infPtr->isStarted == 0)
                  continue;

               if(m == ListCount)
               {  m = 0;
                  tprintf("\n");
               }

               tprintf("%d,", _size(post_count[j], infPtr->datatype));
               m++;
            }

            tprintf("\t\nplist=\n");

            for(j=0,m=0; j < count; j++)
            {
               i = array_of_indices[j];

               index = coll_IndexOf(RequestColl, &array_of_requests[i]);

               if(index < 0)
                  continue;

               infPtr = coll_At(nonblockInfo *, ReqStructColl, index);
      
               if(infPtr->isSend || infPtr->isStarted == 0)
                  continue;

               if(m == ListCount)
               {  m = 0;
                  tprintf("\n");
               }

               tprintf("%d,", array_of_statuses[j].MPI_SOURCE);
               m++;
            }

            tprintf("\t\niplist=\n");
   
            for(j=0,m=0; j < count; j++)
            {
               i = array_of_indices[j];

               index = coll_IndexOf(RequestColl, &array_of_requests[i]);

               if(index < 0)
                  continue;

               infPtr = coll_At(nonblockInfo *, ReqStructColl, index);
      
               if(infPtr->isSend || infPtr->isStarted == 0)
                  continue;

               if(m == ListCount)
               {  m = 0;
                  tprintf("\n");
               }

               tprintf("%d,",
                       _commWorldRank(array_of_statuses[j].MPI_SOURCE,
                                      infPtr->comm));
               m++;
            }

            tprintf("\t\ntaglist=\n");

            for(j=0,m=0; j < count; j++)
            {
               i = array_of_indices[j];

               index = coll_IndexOf(RequestColl, &array_of_requests[i]);

               if(index < 0)
                  continue;

               infPtr = coll_At(nonblockInfo *, ReqStructColl, index);
      
               if(infPtr->isSend || infPtr->isStarted == 0)
                  continue;

               if(m == ListCount)
               {  m = 0;
                  tprintf("\n");
               }

               tprintf("%d,", array_of_statuses[j].MPI_TAG);
               m++;
            }

            if(MPI_TraceMsgChecksum)
            {
               tprintf("\t\nsumlist=\n");

               for(j=0,m=0; j < count; j++)
               {
                  i = array_of_indices[j];

                  index = coll_IndexOf(RequestColl,
                                       &array_of_requests[i]);

                  if(index < 0)
                     continue;

                  infPtr = coll_At(nonblockInfo *, ReqStructColl, index);
      
                  if(infPtr->isSend || infPtr->isStarted == 0)
                     continue;

                  if(m == ListCount)
                  {  m = 0;
                     tprintf("\n");
                  }

                  tprintf("%u,", checksum[j]);
                  m++;
               }
            }
         }

         if(MPITestCount == 0)
         {
            tprintf("\t\n");

            if(MPI_TraceTime)
            {  finish = dvm_time();

               if(MPI_TraceTimeReg)
               {  tprintf("t=%lf\t", finish-LastEventTime);
                  LastEventTime = finish;
               }
               else
                  tprintf("t=%lf\t", finish);
            }

            tprintf("$\n");
         }
      }

      for(j=0; j < count; j++)
      {
         i = array_of_indices[j];

         index = coll_IndexOf(RequestColl, &array_of_requests[i]);

         if(index < 0)
            continue;

         infPtr = coll_At(nonblockInfo *, ReqStructColl, index);
      
         if(infPtr->isInit)
         {
            infPtr->isStarted = 0;
            infPtr->MPITestCount = MPI_TestTraceCount;
            infPtr->TotalTestCount = 0;
         }
         else
         {
            coll_AtDelete(RequestColl, index);
            coll_AtDelete(ReqStructColl, index);
            dvm_FreeStruct(infPtr);
         }
      }

      mac_free((void **)&post_count);
      mac_free((void **)&checksum);
   }

   return  retval;
}



s_Iprobe   *IprobeInf = NULL;


int  MPI_Iprobe(int  source, int  tag, MPI_Comm  comm, int  *flag,
                MPI_Status  *status)
{
   int         retval, i, iproc;
   double      start, finish;
   int         system, trace, debug;
   commInfo   *commInfoPtr;
   MPI_Comm    c;   

   system = (RTS_Call_MPI || DVMCallLevel);
   trace  = (MPI_BotsulaProf && (MPI_TraceLevel || system == 0));
   debug  = (MPI_BotsulaDeb && (MPI_TraceLevel || system == 0));
   retval = (system == 0 && comm == MPI_COMM_WORLD && dvm_OneProcSign == 0);

   if(retval)
      c = DVM_COMM_WORLD;
   else
      c = comm;

   if(debug || trace)
   {
      getcomm(c, &commInfoPtr);

      if(commInfoPtr == NULL)
         epprintf(1 ,__FILE__, __LINE__,
                  "$*** dynamic analyzer err: wrong call MPI_Iprobe "
                  "(non-existent communicator %u)$\n", comm);

      if(source != MPI_ANY_SOURCE &&
         (source < 0 || source >= commInfoPtr->pcount))
         epprintf(1 ,__FILE__, __LINE__,
                  "$*** dynamic analyzer err: wrong call MPI_Iprobe "
                  "(incorrect source %d)$\n", source);

      if(trace)
      {
         /* */    /*E0053*/

         if(IprobeInf == NULL)
         {
            dvm_AllocArray(s_Iprobe, MPS_ProcCount+1, IprobeInf);

            for(i=0; i <= MPS_ProcCount; i++)
            {  IprobeInf[i].tag = MPI_ANY_TAG - 1;
               IprobeInf[i].MPITestCount = MPI_TestTraceCount;
               IprobeInf[i].TotalTestCount = 0;
            } 
         }

         /* ---------------------------------------- */    /*E0054*/

         iproc = _commWorldRank(source, c);

         if(iproc == MPI_ANY_SOURCE)
            i = MPS_ProcCount;
         else
            i = iproc;

         if(IprobeInf[i].tag != tag)
         {
            IprobeInf[i].tag = tag;
            IprobeInf[i].MPITestCount = MPI_TestTraceCount;
            IprobeInf[i].TotalTestCount = 0;
         }

         IprobeInf[i].TotalTestCount++;
         IprobeInf[i].MPITestCount++;

         if(IprobeInf[i].MPITestCount >= MPI_TestTraceCount)
         {  IprobeInf[i].MPITestCount = 0;

            if(MPI_SlashOut)
               tprintf("\n");

            tprintf("$call_MPI_Iprobe\t"
                    "source=%d\tisource=%d\ttag=%d\t"
                    "comm=%u\t\n", source, iproc, tag, c);

            if(MPI_TraceTime)
            {  start = dvm_time();

               if(MPI_TraceTimeReg)
               {  tprintf("t=%lf\t", start-LastEventTime);
                  LastEventTime = start;
               }
               else
                  tprintf("t=%lf\t", start);
            }

            if(MPI_TraceFileLine)
               tprintf("f=%s\tl=%ld\t", DVM_FILE[0], DVM_LINE[0]);
            tprintf("$\n");
         }
      }
   }

   retval = PMPI_Iprobe(source, tag, c, flag, status);

   if(debug || trace)
   {
      if(trace)
      {
         if(*flag && IprobeInf[i].MPITestCount != 0)
         {  /* */    /*E0055*/

            IprobeInf[i].MPITestCount = 0;

            if(MPI_SlashOut)
               tprintf("\n");

            tprintf("$call_MPI_Iprobe\t"
                    "source=%d\tisource=%d\ttag=%d\t"
                    "comm=%u\t\n", source, iproc, tag, c);

            if(MPI_TraceTime)
            {
               if(MPI_TraceTimeReg)
               {  tprintf("t=%lf\t", start-LastEventTime);
                  LastEventTime = start;
               }
               else
                  tprintf("t=%lf\t", start);
            } 

            if(MPI_TraceFileLine)
               tprintf("f=%s\tl=%ld\t", DVM_FILE[0], DVM_LINE[0]);
            tprintf("$\n");
         }

         if(IprobeInf[i].MPITestCount == 0)
         {
            if(*flag)
            {
              tprintf("$ret_MPI_Iprobe\tflag=1\tcount=%d\t"
                      "source=%d\tisource=%d\t\ntag=%d\t",
                      status->count, status->MPI_SOURCE,
                      _commWorldRank(status->MPI_SOURCE, c),
                      status->MPI_TAG);

              if(MPI_TraceTime)
              {  finish = dvm_time();

                 if(MPI_TraceTimeReg)
                 {  tprintf("t=%lf\t", finish-LastEventTime);
                    LastEventTime = finish;
                 }
                 else
                    tprintf("t=%lf\t", finish);
              }

              tprintf("$\n");
            }
            else
            {
               tprintf("$ret_MPI_Iprobe\tflag=0\t");
               if(MPI_TraceTime)
               {  finish = dvm_time();

                  if(MPI_TraceTimeReg)
                  {  tprintf("t=%lf\t", finish-LastEventTime);
                     LastEventTime = finish;
                  }
                  else
                     tprintf("t=%lf\t", finish);
               }

               tprintf("$\n");
            }
         }
      }
   }

   return  retval;
}



int  MPI_Request_free(MPI_Request  *request)
{
   int             retval = 0;
   double          start, finish;
   nonblockInfo   *infPtr;
   int             system, trace, debug, index = 0;
   unsigned        checksum;

   system = (RTS_Call_MPI || DVMCallLevel);
   trace  = (MPI_BotsulaProf && (MPI_TraceLevel || system == 0));
   debug  = (MPI_BotsulaDeb && (MPI_TraceLevel || system == 0));

   if(debug || trace)
   {
      index = coll_IndexOf(RequestColl, request);

      if(index >= 0)
      {  infPtr = coll_At(nonblockInfo *, ReqStructColl, index);

         if(infPtr->isSend)
         {  
            if(infPtr->isStarted)
               checksum = _checkSum(infPtr->buffer, infPtr->count,
                                    infPtr->datatype);

            if(trace)
            {
              if(MPI_SlashOut)
                 tprintf("\n");

              if(infPtr->isStarted)
              {
                 tprintf("$call_MPI_Request_free\treq=%lx\t",
                         (uLLng)request);

                 tprintf("sum=%u\t\n", checksum);

                 if(MPI_TraceTime)
                 {  start = dvm_time();

                    if(MPI_TraceTimeReg)
                    {  tprintf("t=%lf\t", start-LastEventTime);
                       LastEventTime = start;
                    }
                    else
                       tprintf("t=%lf\t", start);
                 }

                 if(MPI_TraceFileLine)
                    tprintf("f=%s\tl=%ld\t", DVM_FILE[0], DVM_LINE[0]);
                 tprintf("$\n");
              }
              else
              {
                 tprintf("$call_MPI_Request_free\treq=%lx\t",
                         (uLLng)request);

                 tprintf("sum=0\t\n");

                 if(MPI_TraceTime)
                 {  start = dvm_time();

                    if(MPI_TraceTimeReg)
                    {  tprintf("t=%lf\t", start-LastEventTime);
                       LastEventTime = start;
                    }
                    else
                       tprintf("t=%lf\t", start);
                 }

                 if(MPI_TraceFileLine)
                    tprintf("f=%s\tl=%ld\t", DVM_FILE[0], DVM_LINE[0]);
                 tprintf("$\n");
              }
            }

            if(infPtr->isStarted)
            { 
               if(checksum != infPtr->checksum)
                  epprintf(1 ,__FILE__, __LINE__,
                           "$*** dynamic analyzer err: "
                           "wrong call MPI_Request_free "
                           "(wrong send message checksum. req=%lx)$\n",
                           (uLLng)request);
            }
         }
         else
         {
            if(trace)
            {
               if(MPI_SlashOut)
                  tprintf("\n");

               tprintf("$call_MPI_Request_free\treq=%lx\t",
                       (uLLng)request);

               tprintf("sum=0\t\n");

               if(MPI_TraceTime)
               {  start = dvm_time();

                  if(MPI_TraceTimeReg)
                  {  tprintf("t=%lf\t", start-LastEventTime);
                     LastEventTime = start;
                  }
                  else
                     tprintf("t=%lf\t", start);
               }

               if(MPI_TraceFileLine)
                  tprintf("f=%s\tl=%ld\t", DVM_FILE[0], DVM_LINE[0]);
               tprintf("$\n");
            }
         }
      }
   }

   retval = PMPI_Request_free(request);

   if(debug || trace)
   {
      if(index >= 0)
      {
         if(trace)
         {
            tprintf("$ret_MPI_Request_free\t");

            if(MPI_TraceTime)
            {  finish = dvm_time();

               if(MPI_TraceTimeReg)
               {  tprintf("t=%lf\t", finish-LastEventTime);
                  LastEventTime = finish;
               }
               else
                  tprintf("t=%lf\t", finish);
            }

            tprintf("$\n");
         }

         coll_AtDelete(RequestColl, index);
         coll_AtDelete(ReqStructColl, index);
         dvm_FreeStruct(infPtr);
      }
   }

   return  retval;
}



int  MPI_Cancel(MPI_Request  *request)
{
   int             retval = 0;
   double          start, finish;
   nonblockInfo   *infPtr;
   int             system, trace, debug, index = -1;
   unsigned        checksum;

   system = (RTS_Call_MPI || DVMCallLevel);
   trace  = (MPI_BotsulaProf && (MPI_TraceLevel || system == 0));
   debug  = (MPI_BotsulaDeb && (MPI_TraceLevel || system == 0));

   if(debug || trace)
   {
      index = coll_IndexOf(RequestColl, request);

      if(index >= 0)
      {  infPtr = coll_At(nonblockInfo *, ReqStructColl, index);

         if(infPtr->isSend)
         {  
            if(infPtr->isStarted)
               checksum = _checkSum(infPtr->buffer, infPtr->count,
                                    infPtr->datatype);

            if(trace)
            {
              if(MPI_SlashOut)
                 tprintf("\n");

              if(infPtr->isStarted)
              {
                 tprintf("$call_MPI_Cancel\treq=%lx\t", (uLLng)request);

                 tprintf("sum=%u\t\n", checksum);

                 if(MPI_TraceTime)
                 {  start = dvm_time();

                    if(MPI_TraceTimeReg)
                    {  tprintf("t=%lf\t", start-LastEventTime);
                       LastEventTime = start;
                    }
                    else
                       tprintf("t=%lf\t", start);
                 }

                 if(MPI_TraceFileLine)
                    tprintf("f=%s\tl=%ld\t", DVM_FILE[0], DVM_LINE[0]);
                 tprintf("$\n");
              } 
              else
              {
                 tprintf("$call_MPI_Cancel\treq=%lx\t", (uLLng)request);

                 tprintf("sum=0\t\n");

                 if(MPI_TraceTime)
                 {  start = dvm_time();

                    if(MPI_TraceTimeReg)
                    {  tprintf("t=%lf\t", start-LastEventTime);
                       LastEventTime = start;
                    }
                    else
                       tprintf("t=%lf\t", start);
                 } 

                 if(MPI_TraceFileLine)
                    tprintf("f=%s\tl=%ld\t", DVM_FILE[0], DVM_LINE[0]);
                 tprintf("$\n");
              }
            }

            if(infPtr->isStarted)
            {
               if(checksum != infPtr->checksum)
                  epprintf(1 ,__FILE__, __LINE__,
                           "$*** dynamic analyzer err: "
                           "wrong call MPI_Cancel "
                           "(wrong send message checksum. req=%lx)$\n",
                           (uLLng)request);
            }
         }
         else
         {
            if(trace)
            {
               if(MPI_SlashOut)
                  tprintf("\n");

               tprintf("$call_MPI_Cancel\treq=%lx\t",
                       (uLLng)request);

               tprintf("sum=0\t\n");

               if(MPI_TraceTime)
               {  start = dvm_time();

                  if(MPI_TraceTimeReg)
                  {  tprintf("t=%lf\t", start-LastEventTime);
                     LastEventTime = start;
                  }
                  else
                     tprintf("t=%lf\t", start);
               }

               if(MPI_TraceFileLine)
                  tprintf("f=%s\tl=%ld\t", DVM_FILE[0], DVM_LINE[0]);
               tprintf("$\n");
            }
         }
      }
   }

   retval = PMPI_Cancel(request);

   if(debug || trace)
   {
      if(index >= 0)
      {
         if(trace)
         {
            tprintf("$ret_MPI_Cancel\t");

            if(MPI_TraceTime)
            {  finish = dvm_time();

               if(MPI_TraceTimeReg)
               {  tprintf("t=%lf\t", finish-LastEventTime);
                  LastEventTime = finish;
               }
               else
                  tprintf("t=%lf\t", finish);
            } 

            tprintf("$\n");
         }

         infPtr->MPITestCount = MPI_TestTraceCount;
         infPtr->TotalTestCount = 0;
         infPtr->isStarted = 0;
      }
   }

   return  retval;
}



/****************************\
*   MPI_Startxxx functions   *
\****************************/    /*E0056*/


int  MPI_Start(MPI_Request  *request)
{
   int             retval = 0;
   double          start, finish;
   nonblockInfo   *infPtr;
   int             system, trace, debug, index = 0;
   unsigned        checksum;

   system = (RTS_Call_MPI || DVMCallLevel);
   trace  = (MPI_BotsulaProf && (MPI_TraceLevel || system == 0));
   debug  = (MPI_BotsulaDeb && (MPI_TraceLevel || system == 0));

   if(debug || trace)
   {
      index = coll_IndexOf(RequestColl, request);

      if(index < 0)      
         epprintf(1 ,__FILE__, __LINE__,
                  "$*** dynamic analyzer err: wrong call MPI_Start "
                  "(request %lx does not exist)$\n", (uLLng)request);

      if(index >= 0)
      {
         infPtr = coll_At(nonblockInfo *, ReqStructColl, index);

         if(infPtr->isSend)
         {  
            checksum = _checkSum(infPtr->buffer, infPtr->count,
                                 infPtr->datatype);

            if(trace)
            {
               if(MPI_SlashOut)
                  tprintf("\n");

               tprintf("$call_MPI_Start\treq=%lx\t", (uLLng)request);

               tprintf("sum=%u\t\n", checksum);

               if(MPI_TraceTime)
               {  start = dvm_time();

                  if(MPI_TraceTimeReg)
                  {  tprintf("t=%lf\t", start-LastEventTime);
                     LastEventTime = start;
                  }
                  else
                     tprintf("$t=%lf\t", start);
               }

               if(MPI_TraceFileLine)
                  tprintf("f=%s\tl=%ld\t", DVM_FILE[0], DVM_LINE[0]);
               tprintf("$\n");
            }

            if(infPtr->isStarted != 0)
               epprintf(1 ,__FILE__, __LINE__,
                     "$*** dynamic analyzer err: wrong call MPI_Start "
                     "(send has already been srarted. request=%lx)$\n",
                     (uLLng)request);
         }
         else
         {
            if(trace)
            {
               if(MPI_SlashOut)
                  tprintf("\n");

               tprintf("$call_MPI_Start\treq=%lx\t", (uLLng)request);

               tprintf("sum=0\t\n");

               if(MPI_TraceTime)
               {  start = dvm_time();

                  if(MPI_TraceTimeReg)
                  {  tprintf("t=%lf\t", start-LastEventTime);
                     LastEventTime = start;
                  }
                  else
                     tprintf("t=%lf\t", start);
               }

               if(MPI_TraceFileLine)
                  tprintf("f=%s\tl=%ld\t", DVM_FILE[0], DVM_LINE[0]);
               tprintf("$\n");
            }

            if(infPtr->isStarted != 0)
               epprintf(1 ,__FILE__, __LINE__,
                     "$*** dynamic analyzer err: wrong call MPI_Start "
                     "(recv has already been started. request=%lx)$\n",
                     (uLLng)request);
         }

         infPtr->isStarted = 1;
         infPtr->MPITestCount = MPI_TestTraceCount;
         infPtr->TotalTestCount = 0;
      }
   }

   retval = PMPI_Start(request);

   if(debug || trace)
   {
      if(index >= 0)
      {
         if(trace)
         {
            tprintf("$ret_MPI_Start\t");

            if(MPI_TraceTime)
            {  finish = dvm_time();

               if(MPI_TraceTimeReg)
               {  tprintf("t=%lf\t", finish-LastEventTime);
                  LastEventTime = finish;
               }
               else
                  tprintf("t=%lf\t", finish);
            }

            tprintf("$\n");
         }
      }
   }

   return  retval;
}



int  MPI_Startall(int  count, MPI_Request  array_of_requests[])
{
   int             retval = 0, i, j, k;
   double          start, finish;
   nonblockInfo   *infPtr;
   int             system, trace, debug, index;
   unsigned        checksum;

   system = (RTS_Call_MPI || DVMCallLevel);
   trace  = (MPI_BotsulaProf && (MPI_TraceLevel || system == 0));
   debug  = (MPI_BotsulaDeb && (MPI_TraceLevel || system == 0));

   if(debug || trace)
   {
      if(count <= 0)
         epprintf(1 ,__FILE__, __LINE__,
                  "$*** dynamic analyzer err: wrong call MPI_Startall "
                  "(count=%d)$\n", count);

      for(i=0; i < count; i++)
      {
         index = coll_IndexOf(RequestColl, &array_of_requests[i]);

         if(index < 0)      
            epprintf(1 ,__FILE__, __LINE__,
                  "$*** dynamic analyzer err: wrong call MPI_Startall "
                  "(request does not exist. index=%d; request=%lx)$\n",
                  i, (uLLng)&array_of_requests[i]);

         if(index < 0)
            continue;

         infPtr = coll_At(nonblockInfo *, ReqStructColl, index);

         if(infPtr->isSend)
         {  
            if(infPtr->isStarted != 0)
               epprintf(1 ,__FILE__, __LINE__,
                    "$*** dynamic analyzer err: wrong call MPI_Startall "
                    "(send has already been started. "
                    "index=%d; request=%lx)$\n",
                    i, (uLLng)&array_of_requests[i]);
         }
         else
         {
            if(infPtr->isStarted != 0)
               epprintf(1 ,__FILE__, __LINE__,
                    "$*** dynamic analyzer err: wrong call MPI_Startall "
                    "(recv has already been started. "
                    "index=%d; request=%lx)$\n",
                    i, (uLLng)&array_of_requests[i]);
         }

         infPtr->isStarted = 1;
         infPtr->MPITestCount = MPI_TestTraceCount;
         infPtr->TotalTestCount = 0;
      }

      if(trace)
      {
         if(MPI_SlashOut)
            tprintf("\n");

         tprintf("$call_MPI_Startall_s\tcount=%d\t\nreqlist=\n", count);

         for(i=0,j=0,k=0; i < count; i++)
         {
            index = coll_IndexOf(RequestColl, &array_of_requests[i]);

            if(index < 0)
               continue;

            if(j == ListCount)
            {  j = 0;
               tprintf("\n");
            }

            infPtr = coll_At(nonblockInfo *, ReqStructColl, index);

            if(infPtr->isSend)
               k++;

            tprintf("%lx,", (uLLng)&array_of_requests[i]);
            j++;
         }

         tprintf("\t\nscount=%d", k);

         if(k > 0)
         {
            tprintf("\t\nsumlist=\n");

            for(i=0,j=0; i < count; i++)
            {
               index = coll_IndexOf(RequestColl, &array_of_requests[i]);

               if(index < 0)
                  continue;

               if(j == ListCount)
               {  j = 0;
                  tprintf("\n");
               }

               infPtr = coll_At(nonblockInfo *, ReqStructColl, index);

               if(infPtr->isSend)
               {  checksum = _checkSum(infPtr->buffer, infPtr->count,
                                       infPtr->datatype);
                  tprintf("%u,", checksum);

                  j++;
               }
            }
         }

         tprintf("\t\n");

         if(MPI_TraceTime)
         {  start = dvm_time();

            if(MPI_TraceTimeReg)
            {  tprintf("t=%lf\t", start-LastEventTime);
               LastEventTime = start;
            }
            else
               tprintf("t=%lf\t", start);
         }

         if(MPI_TraceFileLine)
            tprintf("f=%s\tl=%ld\t", DVM_FILE[0], DVM_LINE[0]);
         tprintf("$\n");
      }
   }

   retval = PMPI_Startall(count, array_of_requests);

   if(trace)
   {
      tprintf("$ret_MPI_Startall\t");

      if(MPI_TraceTime)
      {  finish = dvm_time();

         if(MPI_TraceTimeReg)
         {  tprintf("t=%lf\t", finish-LastEventTime);
            LastEventTime = finish;
         }
         else
            tprintf("t=%lf\t", finish);
      }

      tprintf("$\n");
   }

   return  retval;
}



/***************************************\
*   Collective Communication Routines   *
\***************************************/    /*E0057*/


int  MPI_Barrier(MPI_Comm  comm)
{
   int         retval;
   double      start, finish;
   int         system, trace, debug;
   commInfo   *commInfoPtr;
   MPI_Comm    c;   

   system = (RTS_Call_MPI || DVMCallLevel);
   trace  = (MPI_BotsulaProf && (MPI_TraceLevel || system == 0));
   debug  = (MPI_BotsulaDeb && (MPI_TraceLevel || system == 0));
   retval = (system == 0 && comm == MPI_COMM_WORLD && dvm_OneProcSign == 0);

   if(retval)
      c = DVM_COMM_WORLD;
   else
      c = comm;

   if(debug || trace)
   {
      getcomm(c, &commInfoPtr);

      if(commInfoPtr == NULL)
         epprintf(1 ,__FILE__, __LINE__,
                  "$*** dynamic analyzer err: wrong call MPI_Barrier "
                  "(non-existent communicator %u)$\n", comm);
      if(trace)
      {
        if(MPI_SlashOut)
           tprintf("\n");

        tprintf("$call_MPI_Barrier\tcomm=%u\t", c);

        if(MPI_TraceTime)
        {  start = dvm_time();

           if(MPI_TraceTimeReg)
           {  tprintf("t=%lf\t", start-LastEventTime);
              LastEventTime = start;
           }
           else
              tprintf("t=%lf\t", start);
        }

        if(MPI_TraceFileLine)
           tprintf("f=%s\tl=%ld\t", DVM_FILE[0], DVM_LINE[0]);
        tprintf("$\n");
      }
   }

   retval = PMPI_Barrier(c);

   if(debug || trace)
   {
      if(trace)
      {
         tprintf("$ret_MPI_Barrier\t");

         if(MPI_TraceTime)
         {  finish = dvm_time();

            if(MPI_TraceTimeReg)
            {  tprintf("t=%lf\t", finish-LastEventTime);
               LastEventTime = finish;
            }
            else
               tprintf("t=%lf\t", finish);
         }

         tprintf("$\n");
      }
   }

   return  retval;
}



int  MPI_Bcast(void  *buffer, int  count, MPI_Datatype  datatype,
               int  root, MPI_Comm  comm)
{
   int         retval;
   double      start, finish;
   int         system, trace, debug;
   unsigned    checksum;
   commInfo   *commInfoPtr;
   MPI_Comm    c;   

   system = (RTS_Call_MPI || DVMCallLevel);
   trace  = (MPI_BotsulaProf && (MPI_TraceLevel || system == 0));
   debug  = (MPI_BotsulaDeb && (MPI_TraceLevel || system == 0));
   retval = (system == 0 && comm == MPI_COMM_WORLD && dvm_OneProcSign == 0);

   if(retval)
      c = DVM_COMM_WORLD;
   else
      c = comm;

   if(debug || trace)
   {
      getcomm(c, &commInfoPtr);

      if(commInfoPtr == NULL)
         epprintf(1 ,__FILE__, __LINE__,
                  "$*** dynamic analyzer err: wrong call MPI_Bcast "
                  "(non-existent communicator %u)$\n", comm);

      if(root < 0 || root >= commInfoPtr->pcount)
         epprintf(1 ,__FILE__, __LINE__,
                  "$*** dynamic analyzer err: wrong call MPI_Bcast "
                  "(incorrect root %d)$\n", root);

      if(trace)
      {
         if(commInfoPtr->proc == root)
            checksum = _checkSum(buffer, count, datatype);
         else
            checksum = 0;

         if(MPI_SlashOut)
            tprintf("\n");

         tprintf("$call_MPI_Bcast\tbuf=%lx\tcount=%d\tdtype=%u\t"
                 "size=%d\t\nroot=%d\tiroot=%d\tcomm=%u\tsum=%u\t\n",
                 (uLLng)buffer, count, datatype, _size(count, datatype),
                 root, _commWorldRank(root, c), c, checksum);

         if(MPI_TraceTime)
         {  start = dvm_time();

            if(MPI_TraceTimeReg)
            {  tprintf("t=%lf\t", start-LastEventTime);
               LastEventTime = start;
            }
            else
               tprintf("t=%lf\t", start);
         }

         if(MPI_TraceFileLine)
            tprintf("f=%s\tl=%ld\t", DVM_FILE[0], DVM_LINE[0]);
         tprintf("$\n");
      }
   }

   retval = PMPI_Bcast(buffer, count, datatype, root, c);

   if(debug || trace)
   {
      if(trace)
      {
         tprintf("$ret_MPI_Bcast\tsum=%u\t",
                 _checkSum(buffer, count, datatype));

         if(MPI_TraceTime)
         {  finish = dvm_time();

            if(MPI_TraceTimeReg)
            {  tprintf("t=%lf\t", finish-LastEventTime);
               LastEventTime = finish;
            }
            else
               tprintf("t=%lf\t", finish);
         }

         tprintf("$\n");
      }
   }

   return  retval;
}



int  MPI_Gather(void  *sendbuf, int  sendcount, MPI_Datatype  sendtype,
                void  *recvbuf,	int  recvcount, MPI_Datatype  recvtype,
                int  root, MPI_Comm  comm)
{
   int         retval, i;
   double      start, finish;
   unsigned    checksum;
   int         system, trace, debug, sendsize, recvsize;
   commInfo   *commInfoPtr;
   MPI_Comm    c;   

   system = (RTS_Call_MPI || DVMCallLevel);
   trace  = (MPI_BotsulaProf && (MPI_TraceLevel || system == 0));
   debug  = (MPI_BotsulaDeb && (MPI_TraceLevel || system == 0));
   retval = (system == 0 && comm == MPI_COMM_WORLD && dvm_OneProcSign == 0);

   if(retval)
      c = DVM_COMM_WORLD;
   else
      c = comm;

   if(debug || trace)
   {
      getcomm(c, &commInfoPtr);

      if(commInfoPtr == NULL)
         epprintf(1 ,__FILE__, __LINE__,
                  "$*** dynamic analyzer err: wrong call MPI_Gather "
                  "(non-existent communicator %u)$\n", comm);

      if(root < 0 || root >= commInfoPtr->pcount)
         epprintf(1 ,__FILE__, __LINE__,
                  "$*** dynamic analyzer err: wrong call MPI_Gather "
                  "(incorrect root %d)$\n", root);

      if(trace)
      {
         sendsize = _size(sendcount, sendtype);

         if(commInfoPtr->proc == root)
         {
            recvsize = _size(recvcount, recvtype);

            if(MPI_SlashOut)
               tprintf("\n");

            tprintf("$call_MPI_Gather\t"
                    "sbuf=%lx\tscount=%d\tsdtype=%u\tssize=%d\t\n"
                    "rbuf=%lx\trcount=%d\trdtype=%u\trsize=%d\t\n",
                    (uLLng)sendbuf, sendcount, sendtype, sendsize,
                    (uLLng)recvbuf, recvcount, recvtype, recvsize);
         }
         else
         {
            if(MPI_SlashOut)
               tprintf("\n");

            tprintf("$call_MPI_Gather\t"
                    "sbuf=%lx\tscount=%d\tsdtype=%u\tssize=%d\t\n"
                    "rbuf=0\t\n",
                    (uLLng)sendbuf, sendcount, sendtype, sendsize);
         }

         tprintf("root=%d\tiroot=%d\tcomm=%u\tsum=%u\t\n",
                 root, _commWorldRank(root, c), c,
                 _checkSum(sendbuf, sendcount, sendtype));

         if(MPI_TraceTime)
         {  start = dvm_time();

            if(MPI_TraceTimeReg)
            {  tprintf("t=%lf\t", start-LastEventTime);
               LastEventTime = start;
            }
            else
               tprintf("t=%lf\t", start);
         }

         if(MPI_TraceFileLine)
            tprintf("f=%s\tl=%ld\t", DVM_FILE[0], DVM_LINE[0]);
         tprintf("$\n");
      }
   }

   retval = PMPI_Gather(sendbuf, sendcount, sendtype, recvbuf,
                        recvcount, recvtype, root, c);

   if(debug || trace)
   {
      if(trace)
      {
         checksum = 0;

         if(commInfoPtr->proc == root)
         {
            recvsize = _extent(recvcount, recvtype);

            for(i=0; i < commInfoPtr->pcount; i++)
                checksum += _checkSum(((char *)recvbuf) + (i*recvsize),
                                      recvcount, recvtype);
         }

         tprintf("$ret_MPI_Gather\tsum=%u\t", checksum);

         if(MPI_TraceTime)
         {  finish = dvm_time();

            if(MPI_TraceTimeReg)
            {  tprintf("t=%lf\t", finish-LastEventTime);
               LastEventTime = finish;
            }
            else
               tprintf("t=%lf\t", finish);
         }

         tprintf("$\n");
      }
   }

   return  retval;
}



int MPI_Allgather(void  *sendbuf, int  sendcount, MPI_Datatype  sendtype,
                  void  *recvbuf, int  recvcount, MPI_Datatype  recvtype,
                  MPI_Comm  comm)
{
   int         retval, i;
   double      start, finish;
   unsigned    checksum;
   int         system, trace, debug, sendsize, recvsize;
   commInfo   *commInfoPtr;
   MPI_Comm    c;   

   system = (RTS_Call_MPI || DVMCallLevel);
   trace  = (MPI_BotsulaProf && (MPI_TraceLevel || system == 0));
   debug  = (MPI_BotsulaDeb && (MPI_TraceLevel || system == 0));
   retval = (system == 0 && comm == MPI_COMM_WORLD && dvm_OneProcSign == 0);

   if(retval)
      c = DVM_COMM_WORLD;
   else
      c = comm;

   if(debug || trace)
   {
      getcomm(c, &commInfoPtr);

      if(commInfoPtr == NULL)
         epprintf(1 ,__FILE__, __LINE__,
                  "$*** dynamic analyzer err: wrong call MPI_Allgather "
                  "(non-existent communicator %u)$\n", comm);

      if(trace)
      {
         sendsize = _size(sendcount, sendtype);
         recvsize = _size(recvcount, recvtype);

         if(MPI_SlashOut)
            tprintf("\n");

         tprintf("$call_MPI_Allgather\t"
                 "sbuf=%lx\tscount=%d\tsdtype=%u\tssize=%d\t\n"
                 "rbuf=%lx\trcount=%d\trdtype=%u\trsize=%d\t\n"
                 "comm=%u\tsum=%u\t\n",
                 (uLLng)sendbuf, sendcount, sendtype, sendsize,
                 (uLLng)recvbuf, recvcount, recvtype, recvsize,
                 c, _checkSum(sendbuf, sendcount, sendtype));

         if(MPI_TraceTime)
         {  start = dvm_time();

            if(MPI_TraceTimeReg)
            {  tprintf("t=%lf\t", start-LastEventTime);
               LastEventTime = start;
            }
            else
               tprintf("t=%lf\t", start);
         }

         if(MPI_TraceFileLine)
            tprintf("f=%s\tl=%ld\t", DVM_FILE[0], DVM_LINE[0]);
         tprintf("$\n");
      }
   }

   retval = PMPI_Allgather(sendbuf, sendcount, sendtype, recvbuf,
                           recvcount, recvtype, c);

   if(debug || trace)
   {
      if(trace)
      {
         recvsize = _extent(recvcount, recvtype);

         checksum = 0;

         for(i=0; i < commInfoPtr->pcount; i++)
             checksum += _checkSum(((char *)recvbuf) + (i*recvsize),
                                   recvcount, recvtype);

         tprintf("$ret_MPI_Allgather\tsum=%u\t", checksum);

         if(MPI_TraceTime)
         {  finish = dvm_time();

            if(MPI_TraceTimeReg)
            {  tprintf("t=%lf\t", finish-LastEventTime);
               LastEventTime = finish;
            }
            else
               tprintf("t=%lf\t", finish);
         }

         tprintf("$\n");
      }
   }

   return  retval;
}



int  MPI_Gatherv(void  *sendbuf, int  sendcount, MPI_Datatype  sendtype,
                 void  *recvbuf, int  *recvcounts, int  *displs,
                 MPI_Datatype  recvtype, int  root, MPI_Comm  comm)
{
   int         retval, i, j;
   double      start, finish;
   unsigned    checksum;
   int         system, trace, debug, sendsize;
   commInfo   *commInfoPtr;
   MPI_Comm    c;   

   system = (RTS_Call_MPI || DVMCallLevel);
   trace  = (MPI_BotsulaProf && (MPI_TraceLevel || system == 0));
   debug  = (MPI_BotsulaDeb && (MPI_TraceLevel || system == 0));
   retval = (system == 0 && comm == MPI_COMM_WORLD && dvm_OneProcSign == 0);

   if(retval)
      c = DVM_COMM_WORLD;
   else
      c = comm;

   if(debug || trace)
   {
      getcomm(c, &commInfoPtr);

      if(commInfoPtr == NULL)
         epprintf(1 ,__FILE__, __LINE__,
                  "$*** dynamic analyzer err: wrong call MPI_Gatherv "
                  "(non-existent communicator %u)$\n", comm);

      if(root < 0 || root >= commInfoPtr->pcount)
         epprintf(1 ,__FILE__, __LINE__,
                  "$*** dynamic analyzer err: wrong call MPI_Gatherv "
                  "(incorrect root %d)$\n", root);

      if(trace)
      {
         sendsize = _size(sendcount, sendtype);

         if(MPI_SlashOut)
            tprintf("\n");

         tprintf("$call_MPI_Gatherv\t"
                 "sbuf=%lx\tscount=%d\tsdtype=%u\tssize=%d\t\n",
                 (uLLng)sendbuf, sendcount, sendtype, sendsize);

         if(commInfoPtr->proc == root)
         {  tprintf("pcount=%d\trbuf=%lx\t\nrcounts=\n",
                    commInfoPtr->pcount, (uLLng)recvbuf); 

            for(i=0,j=0; i < commInfoPtr->pcount; i++)
            {
               if(j == ListCount)
               {  j = 0;
                  tprintf("\n");
               }

               tprintf("%d,", recvcounts[i]);
               j++;
            }

            tprintf("\t\ndispls=\n");

            for(i=0,j=0; i < commInfoPtr->pcount; i++)
            {
               if(j == ListCount)
               {  j = 0;
                  tprintf("\n");
               }

               tprintf("%d,", displs[i]);
               j++;
            }

            tprintf("\t\nrdtype=%u\t\nrsizes=\n", recvtype);

            for(i=0,j=0; i < commInfoPtr->pcount; i++)
            {
               if(j == ListCount)
               {  j = 0;
                  tprintf("\n");
               }

               tprintf("%d,", _size(recvcounts[i], recvtype));
               j++;
            }
         }
         else
            tprintf("pcount=0");
    
         tprintf("\t\nroot=%d\tiroot=%d\tcomm=%u\tsum=%u\t\n",
                 root, _commWorldRank(root, c), c,
                 _checkSum(sendbuf, sendcount, sendtype));

         if(MPI_TraceTime)
         {  start = dvm_time();

            if(MPI_TraceTimeReg)
            {  tprintf("t=%lf\t", start-LastEventTime);
               LastEventTime = start;
            }
            else
               tprintf("t=%lf\t", start);
         }

         if(MPI_TraceFileLine)
            tprintf("f=%s\tl=%ld\t", DVM_FILE[0], DVM_LINE[0]);
         tprintf("$\n");
      }
   }

   retval = PMPI_Gatherv(sendbuf, sendcount, sendtype, recvbuf,
                         recvcounts, displs, recvtype, root, c);

   if(debug || trace)
   {
      if(trace)
      {
         checksum = 0;

         if(commInfoPtr->proc == root)
            for(i=0; i < commInfoPtr->pcount; i++)
                checksum += _checkSum(((char *)recvbuf) +
                                      (_extent(displs[i], recvtype)),
                                      recvcounts[i], recvtype);

         tprintf("$ret_MPI_Gatherv\tsum=%u\t", checksum);

         if(MPI_TraceTime)
         {  finish = dvm_time();

            if(MPI_TraceTimeReg)
            {  tprintf("t=%lf\t", finish-LastEventTime);
               LastEventTime = finish;
            }
            else
               tprintf("t=%lf\t", finish);
         }

         tprintf("$\n");
      }
   }

   return  retval;
}



int  MPI_Allgatherv(void  *sendbuf, int  sendcount,
                    MPI_Datatype  sendtype, void  *recvbuf,
                    int  *recvcounts, int  *displs,
                    MPI_Datatype  recvtype, MPI_Comm  comm)
{
   int         retval, i, j;
   double      start, finish;
   unsigned    checksum;
   int         system, trace, debug, sendsize;
   commInfo   *commInfoPtr;
   MPI_Comm    c;   

   system = (RTS_Call_MPI || DVMCallLevel);
   trace  = (MPI_BotsulaProf && (MPI_TraceLevel || system == 0));
   debug  = (MPI_BotsulaDeb && (MPI_TraceLevel || system == 0));
   retval = (system == 0 && comm == MPI_COMM_WORLD && dvm_OneProcSign == 0);

   if(retval)
      c = DVM_COMM_WORLD;
   else
      c = comm;

   if(debug || trace)
   {
      getcomm(c, &commInfoPtr);

      if(commInfoPtr == NULL)
         epprintf(1 ,__FILE__, __LINE__,
                  "$*** dynamic analyzer err: wrong call MPI_Allgatherv "
                  "(non-existent communicator %u)$\n", comm);

      if(trace)
      {
         sendsize = _size(sendcount, sendtype);

         if(MPI_SlashOut)
            tprintf("\n");

         tprintf("$call_MPI_Allgatherv\t"
                 "sbuf=%lx\tscount=%d\tsdtype=%u\tssize=%d\t\n"
                 "pcount=%d\trbuf=%lx\t\nrcounts=\n",
                 (uLLng)sendbuf, sendcount, sendtype, sendsize,
                 commInfoPtr->pcount, (uLLng)recvbuf);

         for(i=0,j=0; i < commInfoPtr->pcount; i++)
         {
            if(j == ListCount)
            {  j = 0;
               tprintf("\n");
            }

            tprintf("%d,", recvcounts[i]);
            j++;
         }

         tprintf("\t\ndispls=\n");

         for(i=0,j=0; i < commInfoPtr->pcount; i++)
         {
            if(j == ListCount)
            {  j = 0;
               tprintf("\n");
            }

            tprintf("%d,", displs[i]);
            j++;
         }

         tprintf("\t\nrdtype=%u\t\nrsizes=\n", recvtype);

         for(i=0,j=0; i < commInfoPtr->pcount; i++)
         {
            if(j == ListCount)
            {  j = 0;
               tprintf("\n");
            }

            tprintf("%d,", _size(recvcounts[i], recvtype));
            j++;
         }
    
         tprintf("\t\ncomm=%u\tsum=%u\t\n",
                 c, _checkSum(sendbuf, sendcount, sendtype));

         if(MPI_TraceTime)
         {  start = dvm_time();

            if(MPI_TraceTimeReg)
            {  tprintf("t=%lf\t", start-LastEventTime);
               LastEventTime = start;
            }
            else
               tprintf("t=%lf\t", start);
         }

         if(MPI_TraceFileLine)
            tprintf("f=%s\tl=%ld\t", DVM_FILE[0], DVM_LINE[0]);
         tprintf("$\n");
      }
   }

   retval = PMPI_Allgatherv(sendbuf, sendcount, sendtype, recvbuf,
                            recvcounts, displs, recvtype, c);

   if(debug || trace)
   {
      if(trace)
      {
         checksum = 0;

         for(i=0; i < commInfoPtr->pcount; i++)
             checksum += _checkSum(((char *)recvbuf) +
                                   (_extent(displs[i], recvtype)),
                                   recvcounts[i], recvtype);

         tprintf("$ret_MPI_Allgatherv\tsum=%u\t", checksum);

         if(MPI_TraceTime)
         {  finish = dvm_time();

            if(MPI_TraceTimeReg)
            {  tprintf("t=%lf\t", finish-LastEventTime);
               LastEventTime = finish;
            }
            else
               tprintf("t=%lf\t", finish);
         }

         tprintf("$\n");
      }
   }

   return  retval;
}



int  MPI_Scatter(void  *sendbuf, int  sendcount, MPI_Datatype  sendtype,
                 void  *recvbuf, int  recvcount, MPI_Datatype  recvtype,
                 int  root, MPI_Comm  comm)
{
   int         retval, i;
   double      start, finish;
   unsigned    checksum;
   int         system, trace, debug, sendsize, recvsize;
   commInfo   *commInfoPtr;
   MPI_Comm    c;   

   system = (RTS_Call_MPI || DVMCallLevel);
   trace  = (MPI_BotsulaProf && (MPI_TraceLevel || system == 0));
   debug  = (MPI_BotsulaDeb && (MPI_TraceLevel || system == 0));
   retval = (system == 0 && comm == MPI_COMM_WORLD && dvm_OneProcSign == 0);

   if(retval)
      c = DVM_COMM_WORLD;
   else
      c = comm;

   if(debug || trace)
   {
      getcomm(c, &commInfoPtr);

      if(commInfoPtr == NULL)
         epprintf(1 ,__FILE__, __LINE__,
                  "$*** dynamic analyzer err: wrong call MPI_Scatter "
                  "(non-existent communicator %u)$\n", comm);

      if(root < 0 || root >= commInfoPtr->pcount)
         epprintf(1 ,__FILE__, __LINE__,
                  "$*** dynamic analyzer err: wrong call MPI_Scatter "
                  "(incorrect root %d)$\n", root);

      if(trace)
      {
         recvsize = _size(recvcount, recvtype);

         if(commInfoPtr->proc == root)
         {  sendsize = _extent(sendcount, sendtype);
            checksum = 0;

            for(i=0; i < commInfoPtr->pcount; i++)
                checksum += _checkSum(((char *)sendbuf) + (i*sendsize),
                                      sendcount, sendtype);

            if(MPI_SlashOut)
               tprintf("\n");

            tprintf("$call_MPI_Scatter\t"
               "sbuf=%lx\tscount=%d\tsdtype=%u\tssize=%d\t\nsum=%u\t\n",
               (uLLng)sendbuf, sendcount, sendtype,
               _size(sendcount, sendtype), checksum);
         }
         else
         {
            if(MPI_SlashOut)
               tprintf("\n");

            tprintf("$call_MPI_Scatter\tsbuf=0\t\n");
         }

         tprintf("rbuf=%lx\trcount=%d\trdtype=%u\trsize=%d\t\n"
                 "root=%d\tiroot=%d\tcomm=%u\t\n",
                 (uLLng)recvbuf, recvcount, recvtype, recvsize,
                 root, _commWorldRank(root, c), c);

         if(MPI_TraceTime)
         {  start = dvm_time();

            if(MPI_TraceTimeReg)
            {  tprintf("t=%lf\t", start-LastEventTime);
               LastEventTime = start;
            }
            else
               tprintf("t=%lf\t", start);
         }

         if(MPI_TraceFileLine)
            tprintf("f=%s\tl=%ld\t", DVM_FILE[0], DVM_LINE[0]);
         tprintf("$\n");
      }
   }

   retval = PMPI_Scatter(sendbuf, sendcount, sendtype, recvbuf,
                         recvcount, recvtype, root, c);

   if(debug || trace)
   {
      if(trace)
      {
         tprintf("$ret_MPI_Scatter\tsum=%u\t",
                 _checkSum(recvbuf, recvcount, recvtype));

         if(MPI_TraceTime)
         {  finish = dvm_time();

            if(MPI_TraceTimeReg)
            {  tprintf("t=%lf\t", finish-LastEventTime);
               LastEventTime = finish;
            }
            else
               tprintf("t=%lf\t", finish);
         }

         tprintf("$\n");
      }
   }

   return  retval;
}



int  MPI_Scatterv(void  *sendbuf, int  *sendcnts, int  *displs,
                  MPI_Datatype  sendtype, void  *recvbuf, int  recvcnt,
                  MPI_Datatype  recvtype, int  root, MPI_Comm  comm)
{
   int         retval, i, j;
   double      start, finish;
   unsigned    checksum;
   int         system, trace, debug, recvsize;
   commInfo   *commInfoPtr;
   MPI_Comm    c;   

   system = (RTS_Call_MPI || DVMCallLevel);
   trace  = (MPI_BotsulaProf && (MPI_TraceLevel || system == 0));
   debug  = (MPI_BotsulaDeb && (MPI_TraceLevel || system == 0));
   retval = (system == 0 && comm == MPI_COMM_WORLD && dvm_OneProcSign == 0);

   if(retval)
      c = DVM_COMM_WORLD;
   else
      c = comm;

   if(debug || trace)
   {
      getcomm(c, &commInfoPtr);

      if(commInfoPtr == NULL)
         epprintf(1 ,__FILE__, __LINE__,
                  "$*** dynamic analyzer err: wrong call MPI_Scatterv "
                  "(non-existent communicator %u)$\n", comm);

      if(root < 0 || root >= commInfoPtr->pcount)
         epprintf(1 ,__FILE__, __LINE__,
                  "$*** dynamic analyzer err: wrong call MPI_Scatterv "
                  "(incorrect root %d)$\n", root);

      if(trace)
      {
         recvsize = _size(recvcnt, recvtype);

         if(commInfoPtr->proc == root)
         {
            if(MPI_SlashOut)
               tprintf("\n");

            tprintf("$call_MPI_Scatterv\t"
                    "pcount=%d\tsbuf=%lx\t\nscounts=\n",
                    commInfoPtr->pcount, (uLLng)sendbuf);

            for(i=0,j=0; i < commInfoPtr->pcount; i++)
            {
               if(j == ListCount)
               {  j = 0;
                  tprintf("\n");
               }

               tprintf("%d,", sendcnts[i]);
               j++;
            }

            tprintf("\t\ndispls=\n");

            for(i=0,j=0; i < commInfoPtr->pcount; i++)
            {
               if(j == ListCount)
               {  j = 0;
                  tprintf("\n");
               }

               tprintf("%d,", displs[i]);
               j++;
            }

            tprintf("\t\nsdtype=%u\t\nssizes=\n", sendtype);

            for(i=0,j=0; i < commInfoPtr->pcount; i++)
            {
               if(j == ListCount)
               {  j = 0;
                  tprintf("\n");
               }

               tprintf("%d,", _size(sendcnts[i], sendtype));
               j++;
            }

            checksum = 0;

            for(i=0; i < commInfoPtr->pcount; i++)
                checksum += _checkSum(((char *)sendbuf) +
                                      (_extent(displs[i], sendtype)),
                                      sendcnts[i], sendtype);

            tprintf("\t\nsum=%u\t\n", checksum);
         }
         else
         {
            if(MPI_SlashOut)
               tprintf("\n");

            tprintf("$call_MPI_Scatterv\tpcount=0\t\n");
         }

         tprintf("rbuf=%lx\trcount=%d\trdtype=%u\trsize=%d\t\n"
                 "root=%d\tiroot=%d\tcomm=%u\t\n",
                 (uLLng)recvbuf, recvcnt, recvtype, recvsize,
                 root, _commWorldRank(root, c), c);

         if(MPI_TraceTime)
         {  start = dvm_time();

            if(MPI_TraceTimeReg)
            {  tprintf("t=%lf\t", start-LastEventTime);
               LastEventTime = start;
            }
            else
               tprintf("t=%lf\t", start);
         }

         if(MPI_TraceFileLine)
            tprintf("f=%s\tl=%ld\t", DVM_FILE[0], DVM_LINE[0]);
         tprintf("$\n");
      }
   }

   retval = PMPI_Scatterv(sendbuf, sendcnts, displs, sendtype,
                          recvbuf, recvcnt, recvtype, root, c);

   if(debug || trace)
   {
      if(trace)
      {
         tprintf("$ret_MPI_Scatterv\tsum=%u\t",
                 _checkSum(recvbuf, recvcnt, recvtype));

         if(MPI_TraceTime)
         {  finish = dvm_time();

            if(MPI_TraceTimeReg)
            {  tprintf("t=%lf\t", finish-LastEventTime);
               LastEventTime = finish;
            }
            else
               tprintf("t=%lf\t", finish);
         }

         tprintf("$\n");
      }
   }

   return  retval;
}



int  MPI_Reduce(void  *sendbuf, void  *recvbuf, int  count,
                MPI_Datatype  datatype, MPI_Op  op, int  root,
                MPI_Comm  comm)
{
   int         retval;
   double      start, finish;
   int         system, trace, debug;
   commInfo   *commInfoPtr;
   MPI_Comm    c;   

   system = (RTS_Call_MPI || DVMCallLevel);
   trace  = (MPI_BotsulaProf && (MPI_TraceLevel || system == 0));
   debug  = (MPI_BotsulaDeb && (MPI_TraceLevel || system == 0));
   retval = (system == 0 && comm == MPI_COMM_WORLD && dvm_OneProcSign == 0);

   if(retval)
      c = DVM_COMM_WORLD;
   else
      c = comm;

   if(debug || trace)
   {
      getcomm(c, &commInfoPtr);

      if(commInfoPtr == NULL)
         epprintf(1 ,__FILE__, __LINE__,
                  "$*** dynamic analyzer err: wrong call MPI_Reduce "
                  "(non-existent communicator %u)$\n", comm);

      if(root < 0 || root >= commInfoPtr->pcount)
         epprintf(1 ,__FILE__, __LINE__,
                  "$*** dynamic analyzer err: wrong call MPI_Reduce "
                  "(incorrect root %d)$\n", root);

      if(trace)
      {
         if(MPI_SlashOut)
            tprintf("\n");

         tprintf("$call_MPI_Reduce\t"
              "sbuf=%lx\trbuf=%lx\tcount=%d\tdtype=%u\t\nsize=%d\top=",
              (uLLng)sendbuf, (uLLng)recvbuf, count, datatype,
              _size(count, datatype));

         _printOp(op);

         tprintf("\troot=%d\tiroot=%d\tcomm=%u\t\n",
                 root, _commWorldRank(root, c), c);

         if(MPI_TraceTime)
         {  start = dvm_time();

            if(MPI_TraceTimeReg)
            {  tprintf("t=%lf\t", start-LastEventTime);
               LastEventTime = start;
            }
            else
               tprintf("t=%lf\t", start);
         }

         if(MPI_TraceFileLine)
            tprintf("f=%s\tl=%ld\t", DVM_FILE[0], DVM_LINE[0]);
         tprintf("$\n");
      }
   }

   retval = PMPI_Reduce(sendbuf, recvbuf, count, datatype, op, root, c);

   if(debug || trace)
   {
      if(trace)
      {
         tprintf("$ret_MPI_Reduce\t");

         if(MPI_TraceTime)
         {  finish = dvm_time();

            if(MPI_TraceTimeReg)
            {  tprintf("t=%lf\t", finish-LastEventTime);
               LastEventTime = finish;
            }
            else
               tprintf("t=%lf\t", finish);
         }

         tprintf("$\n");
      }
   }

   return  retval;
}



int  MPI_Allreduce(void  *sendbuf, void  *recvbuf, int  count,
                   MPI_Datatype  datatype, MPI_Op  op, MPI_Comm  comm)
{
   int         retval;
   double      start, finish;
   int         system, trace, debug;
   commInfo   *commInfoPtr;
   MPI_Comm    c;   

   system = (RTS_Call_MPI || DVMCallLevel);
   trace  = (MPI_BotsulaProf && (MPI_TraceLevel || system == 0));
   debug  = (MPI_BotsulaDeb && (MPI_TraceLevel || system == 0));
   retval = (system == 0 && comm == MPI_COMM_WORLD && dvm_OneProcSign == 0);

   if(retval)
      c = DVM_COMM_WORLD;
   else
      c = comm;

   if(debug || trace)
   {
      getcomm(c, &commInfoPtr);

      if(commInfoPtr == NULL)
         epprintf(1 ,__FILE__, __LINE__,
                  "$*** dynamic analyzer err: wrong call MPI_Allreduce "
                  "(non-existent communicator %u)$\n", comm);

      if(trace)
      {
         if(MPI_SlashOut)
            tprintf("\n");

         tprintf("$call_MPI_Allreduce\t"
              "sbuf=%lx\trbuf=%lx\tcount=%d\tdtype=%u\t\nsize=%d\top=",
              (uLLng)sendbuf, (uLLng)recvbuf, count, datatype,
              _size(count, datatype));

         _printOp(op);

         tprintf("\tcomm=%u\t\n", c);

         if(MPI_TraceTime)
         {  start = dvm_time();

            if(MPI_TraceTimeReg)
            {  tprintf("t=%lf\t", start-LastEventTime);
               LastEventTime = start;
            }
            else
               tprintf("t=%lf\t", start);
         }

         if(MPI_TraceFileLine)
            tprintf("f=%s\tl=%ld\t", DVM_FILE[0], DVM_LINE[0]);
         tprintf("$\n");
      }
   }

   retval = PMPI_Allreduce(sendbuf, recvbuf, count, datatype, op, c);

   if(debug || trace)
   {
      if(trace)
      {
         tprintf("$ret_MPI_Allreduce\tsum=%u\t",
                 _checkSum(recvbuf, count, datatype));

         if(MPI_TraceTime)
         {  finish = dvm_time();

            if(MPI_TraceTimeReg)
            {  tprintf("t=%lf\t", finish-LastEventTime);
               LastEventTime = finish;
            }
            else
               tprintf("t=%lf\t", finish);
         }

         tprintf("$\n");
      }
   }

   return  retval;
}



int  MPI_Reduce_scatter(void  *sendbuf, void  *recvbuf, int  *recvcnts,
                        MPI_Datatype  datatype, MPI_Op  op,
                        MPI_Comm  comm)
{
   int         retval, i, j;
   double      start, finish;
   int         system, trace, debug;
   commInfo   *commInfoPtr;
   MPI_Comm    c;   

   system = (RTS_Call_MPI || DVMCallLevel);
   trace  = (MPI_BotsulaProf && (MPI_TraceLevel || system == 0));
   debug  = (MPI_BotsulaDeb && (MPI_TraceLevel || system == 0));
   retval = (system == 0 && comm == MPI_COMM_WORLD && dvm_OneProcSign == 0);

   if(retval)
      c = DVM_COMM_WORLD;
   else
      c = comm;

   if(debug || trace)
   {
      getcomm(c, &commInfoPtr);

      if(commInfoPtr == NULL)
         epprintf(1 ,__FILE__, __LINE__,
             "$*** dynamic analyzer err: wrong call MPI_Reduce_scatter "
             "(non-existent communicator %u)$\n", comm);

      if(trace)
      {
         if(MPI_SlashOut)
            tprintf("\n");

         tprintf("$call_MPI_Reduce_scatter\t"
                 "sbuf=%lx\trbuf=%lx\tpcount=%d\t\nrcounts=\n",
                 (uLLng)sendbuf, (uLLng)recvbuf, commInfoPtr->pcount);

         for(i=0,j=0; i < commInfoPtr->pcount; i++)
         {
            if(j == ListCount)
            {  j = 0;
               tprintf("\n");
            }

            tprintf("%d,", recvcnts[i]);
            j++;
         }

         tprintf("\t\ndtype=%u\t\nsizes=\n", datatype);

         for(i=0,j=0; i < commInfoPtr->pcount; i++)
         {
            if(j == ListCount)
            {  j = 0;
               tprintf("\n");
            }

            tprintf("%d,", _size(recvcnts[i], datatype));
            j++;
         }

         tprintf("\t\nop=");
         _printOp(op);

         tprintf("\tcomm=%u\t\n", c);

         if(MPI_TraceTime)
         {  start = dvm_time();

            if(MPI_TraceTimeReg)
            {  tprintf("t=%lf\t", start-LastEventTime);
               LastEventTime = start;
            }
            else
               tprintf("t=%lf\t", start);
         }

         if(MPI_TraceFileLine)
            tprintf("f=%s\tl=%ld\t", DVM_FILE[0], DVM_LINE[0]);
         tprintf("$\n");
      }
   }

   retval = PMPI_Reduce_scatter(sendbuf, recvbuf, recvcnts, datatype,
                                op, c);

   if(debug || trace)
   {
      if(trace)
      {
         tprintf("$ret_MPI_Reduce_scatter\t");

         if(MPI_TraceTime)
         {  finish = dvm_time();

            if(MPI_TraceTimeReg)
            {  tprintf("t=%lf\t", finish-LastEventTime);
               LastEventTime = finish;
            }
            else
               tprintf("t=%lf\t", finish);
         }

         tprintf("$\n");
      }
   }

   return  retval;
}



int  MPI_Scan(void  *sendbuf, void  *recvbuf, int  count,
              MPI_Datatype  datatype, MPI_Op  op, MPI_Comm  comm)

{
   int         retval;
   double      start, finish;
   int         system, trace, debug;
   commInfo   *commInfoPtr;
   MPI_Comm    c;   

   system = (RTS_Call_MPI || DVMCallLevel);
   trace  = (MPI_BotsulaProf && (MPI_TraceLevel || system == 0));
   debug  = (MPI_BotsulaDeb && (MPI_TraceLevel || system == 0));
   retval = (system == 0 && comm == MPI_COMM_WORLD && dvm_OneProcSign == 0);

   if(retval)
      c = DVM_COMM_WORLD;
   else
      c = comm;

   if(debug || trace)
   {
      getcomm(c, &commInfoPtr);

      if(commInfoPtr == NULL)
         epprintf(1 ,__FILE__, __LINE__,
                  "$*** dynamic analyzer err: wrong call MPI_Scan "
                  "(non-existent communicator %u)$\n", comm);

      if(trace)
      {
         if(MPI_SlashOut)
            tprintf("\n");

         tprintf("$call_MPI_Scan\t"
              "sbuf=%lx\trbuf=%lx\tcount=%d\tdtype=%u\t\nsize=%d\top=",
              (uLLng)sendbuf, (uLLng)recvbuf, count, datatype,
              _size(count, datatype));

         _printOp(op);

         tprintf("\tcomm=%u\t\n", c);

         if(MPI_TraceTime)
         {  start = dvm_time();

            if(MPI_TraceTimeReg)
            {  tprintf("t=%lf\t", start-LastEventTime);
               LastEventTime = start;
            }
            else
               tprintf("t=%lf\t", start);
         }

         if(MPI_TraceFileLine)
            tprintf("f=%s\tl=%ld\t", DVM_FILE[0], DVM_LINE[0]);
         tprintf("$\n");
      }
   }

   retval = PMPI_Scan(sendbuf, recvbuf, count, datatype, op, c);

   if(debug || trace)
   {
      if(trace)
      {
         tprintf("$ret_MPI_Scan\t");

         if(MPI_TraceTime)
         {  finish = dvm_time();

            if(MPI_TraceTimeReg)
            {  tprintf("t=%lf\t", finish-LastEventTime);
               LastEventTime = finish;
            }
            else
               tprintf("t=%lf\t", finish);
         }

         tprintf("$\n");
      }
   }

   return  retval;
}



int  MPI_Alltoall(void  *sendbuf, int  sendcount, MPI_Datatype  sendtype,
                  void  *recvbuf, int  recvcount, MPI_Datatype  recvtype,
                  MPI_Comm  comm)
{
   int         retval, i;
   double      start, finish;
   unsigned    checksum;
   int         system, trace, debug, sendsize, recvsize;
   commInfo   *commInfoPtr;
   MPI_Comm    c;   

   system = (RTS_Call_MPI || DVMCallLevel);
   trace  = (MPI_BotsulaProf && (MPI_TraceLevel || system == 0));
   debug  = (MPI_BotsulaDeb && (MPI_TraceLevel || system == 0));
   retval = (system == 0 && comm == MPI_COMM_WORLD && dvm_OneProcSign == 0);

   if(retval)
      c = DVM_COMM_WORLD;
   else
      c = comm;

   if(debug || trace)
   {
      getcomm(c, &commInfoPtr);

      if(commInfoPtr == NULL)
         epprintf(1 ,__FILE__, __LINE__,
                  "$*** dynamic analyzer err: wrong call MPI_Alltoall "
                  "(non-existent communicator %u)$\n", comm);

      if(trace)
      {
         recvsize = _size(recvcount, recvtype);
         sendsize = _extent(sendcount, sendtype);
         checksum = 0;

         for(i=0; i < commInfoPtr->pcount; i++)
             checksum += _checkSum(((char *)sendbuf) + (i*sendsize),
                                   sendcount, sendtype);

         sendsize = _size(sendcount, sendtype);

         if(MPI_SlashOut)
            tprintf("\n");

         tprintf("$call_MPI_Alltoall\t"
                 "sbuf=%lx\tscount=%d\tsdtype=%u\tssize=%d\t\n"
                 "rbuf=%lx\trcount=%d\trdtype=%u\trsize=%d\t\n"
                 "comm=%u\tsum=%u\t\n",
                 (uLLng)sendbuf, sendcount, sendtype, sendsize,
                 (uLLng)recvbuf, recvcount, recvtype, recvsize,
                 c, checksum);

         if(MPI_TraceTime)
         {  start = dvm_time();

            if(MPI_TraceTimeReg)
            {  tprintf("t=%lf\t", start-LastEventTime);
               LastEventTime = start;
            }
            else
               tprintf("t=%lf\t", start);
         }

         if(MPI_TraceFileLine)
            tprintf("f=%s\tl=%ld\t", DVM_FILE[0], DVM_LINE[0]);
         tprintf("$\n");
      }
   }

   retval = PMPI_Alltoall(sendbuf, sendcount, sendtype, recvbuf,
                          recvcount, recvtype, c);

   if(debug || trace)
   {
      if(trace)
      {
         recvsize = _extent(recvcount, recvtype);
         checksum = 0;

         for(i=0; i < commInfoPtr->pcount; i++)
             checksum += _checkSum(((char *)recvbuf) + (i*recvsize),
                                   recvcount, recvtype);

         tprintf("$ret_MPI_Alltoall\tsum=%u\t", checksum);

         if(MPI_TraceTime)
         {  finish = dvm_time();

            if(MPI_TraceTimeReg)
            {  tprintf("t=%lf\t", finish-LastEventTime);
               LastEventTime = finish;
            }
            else
               tprintf("t=%lf\t", finish);
         }

         tprintf("$\n");
      }
   }

   return  retval;
}



int  MPI_Alltoallv(void  *sendbuf, int  *sendcnts, int  *sdispls,
                   MPI_Datatype  sendtype, void  *recvbuf,
                   int  *recvcounts, int  *rdispls,
                   MPI_Datatype  recvtype, MPI_Comm  comm)
{
   int         retval, i, j;
   double      start, finish;
   unsigned    checksum;
   int         system, trace, debug;
   commInfo   *commInfoPtr;
   MPI_Comm    c;   

   system = (RTS_Call_MPI || DVMCallLevel);
   trace  = (MPI_BotsulaProf && (MPI_TraceLevel || system == 0));
   debug  = (MPI_BotsulaDeb && (MPI_TraceLevel || system == 0));
   retval = (system == 0 && comm == MPI_COMM_WORLD && dvm_OneProcSign == 0);

   if(retval)
      c = DVM_COMM_WORLD;
   else
      c = comm;

   if(debug || trace)
   {
      getcomm(c, &commInfoPtr);

      if(commInfoPtr == NULL)
         epprintf(1 ,__FILE__, __LINE__,
                  "$*** dynamic analyzer err: wrong call MPI_Alltoallv "
                  "(non-existent communicator %u)$\n", comm);

      if(trace)
      {
         if(MPI_SlashOut)
            tprintf("\n");

         tprintf("$call_MPI_Alltoallv\t"
                 "sbuf=%lx\tpcount=%d\t\nscounts=\n",
                 commInfoPtr->pcount, (uLLng)sendbuf);

         for(i=0,j=0; i < commInfoPtr->pcount; i++)
         {
            if(j == ListCount)
            {  j = 0;
               tprintf("\n");
            }

            tprintf("%d,", sendcnts[i]);
            j++;
         }

         tprintf("\t\nsdispls=\n");

         for(i=0,j=0; i < commInfoPtr->pcount; i++)
         {
            if(j == ListCount)
            {  j = 0;
               tprintf("\n");
            }

            tprintf("%d,", sdispls[i]);
            j++;
         }

         tprintf("\t\nsdtype=%u\t\nssizes=\n", sendtype);

         for(i=0,j=0; i < commInfoPtr->pcount; i++)
         {
            if(j == ListCount)
            {  j = 0;
               tprintf("\n");
            }

            tprintf("%d,", _size(sendcnts[i], sendtype));
            j++;
         }

         tprintf("\t\nrbuf=%lx\t\nrcounts=\n",
                 (uLLng)recvbuf);

         for(i=0,j=0; i < commInfoPtr->pcount; i++)
         {
            if(j == ListCount)
            {  j = 0;
               tprintf("\n");
            }

            tprintf("%d,", recvcounts[i]);
            j++;
         }

         tprintf("\t\nrdispls=\n");

         for(i=0,j=0; i < commInfoPtr->pcount; i++)
         {
            if(j == ListCount)
            {  j = 0;
               tprintf("\n");
            }

            tprintf("%d,", rdispls[i]);
            j++;
         }

         tprintf("\t\nrdtype=%u\t\nrsizes=\n", recvtype);

         for(i=0,j=0; i < commInfoPtr->pcount; i++)
         {
            if(j == ListCount)
            {  j = 0;
               tprintf("\n");
            }

            tprintf("%d,", _size(recvcounts[i], recvtype));
            j++;
         }
    
         checksum = 0;

         for(i=0; i < commInfoPtr->pcount; i++)
             checksum += _checkSum(((char *)sendbuf) +
                                   (_extent(sdispls[i], sendtype)),
                                   sendcnts[i], sendtype);
         tprintf("\t\ncomm=%u\tsum=%u\t\n", c, checksum);

         if(MPI_TraceTime)
         {  start = dvm_time();

            if(MPI_TraceTimeReg)
            {  tprintf("t=%lf\t", start-LastEventTime);
               LastEventTime = start;
            }
            else
               tprintf("t=%lf\t", start);
         }

         if(MPI_TraceFileLine)
            tprintf("f=%s\tl=%ld\t", DVM_FILE[0], DVM_LINE[0]);
         tprintf("$\n");
      }
   }

   retval = PMPI_Alltoallv(sendbuf, sendcnts, sdispls, sendtype,
                           recvbuf, recvcounts, rdispls, recvtype, c);

   if(debug || trace)
   {
      if(trace)
      {
         checksum = 0;

         for(i=0; i < commInfoPtr->pcount; i++)
             checksum += _checkSum(((char *)recvbuf) +
                                   (_extent(rdispls[i], recvtype)),
                                   recvcounts[i], recvtype);

         tprintf("$ret_MPI_Alltoallv\tsum=%u\t", checksum);

         if(MPI_TraceTime)
         {  finish = dvm_time();

            if(MPI_TraceTimeReg)
            {  tprintf("t=%lf\t", finish-LastEventTime);
               LastEventTime = finish;
            }
            else
               tprintf("t=%lf\t", finish);
         }

         tprintf("$\n");
      }
   }

   return  retval;
}



/* */    /*E0058*/

int  MPI_Comm_dup(MPI_Comm  comm, MPI_Comm  *newcomm)
{
   int         retval;
   double      start, finish;
   int         system, trace, debug, i, j;
   commInfo   *commInfoPtr1, *commInfoPtr2;
   MPI_Comm    c;   

   system = (RTS_Call_MPI || DVMCallLevel);
   trace  = (MPI_BotsulaProf && (MPI_TraceLevel || system == 0));
   debug  = (MPI_BotsulaDeb && (MPI_TraceLevel || system == 0));
   retval = (system == 0 && comm == MPI_COMM_WORLD && dvm_OneProcSign == 0);

   if(retval)
      c = DVM_COMM_WORLD;
   else
      c = comm;

   if(debug || trace)
   {
      getcomm(c, &commInfoPtr1);

      if(commInfoPtr1 == NULL)
         epprintf(1 ,__FILE__, __LINE__,
                  "$*** dynamic analyzer err: wrong call MPI_Comm_dup "
                  "(non-existent communicator %u)$\n", comm);

      if(trace)
      {
        if(MPI_SlashOut)
           tprintf("\n");

        tprintf("$call_MPI_Comm_dup\tcomm=%u\t", c);

        if(MPI_TraceTime)
        {  start = dvm_time();

           if(MPI_TraceTimeReg)
           {  tprintf("t=%lf\t", start-LastEventTime);
              LastEventTime = start;
           }
           else
              tprintf("t=%lf\t", start);
        }

        if(MPI_TraceFileLine)
           tprintf("f=%s\tl=%ld\t", DVM_FILE[0], DVM_LINE[0]);
        tprintf("$\n");
      }
   }

   retval = PMPI_Comm_dup(c, newcomm);

   if(debug || trace)
   {
      /* */    /*E0059*/

      dvm_AllocStruct(commInfo, commInfoPtr2);
      coll_Insert(CommStructColl, commInfoPtr2);
      coll_Insert(CommColl, *newcomm);

      commInfoPtr2->proc   = commInfoPtr1->proc;
      commInfoPtr2->pcount = commInfoPtr1->pcount;
      mac_malloc(commInfoPtr2->plist, int *,
                 commInfoPtr2->pcount*sizeof(int), 0);

      for(i=0; i < commInfoPtr1->pcount; i++)
          commInfoPtr2->plist[i] = commInfoPtr1->plist[i];

      if(trace)
      {
         tprintf("$ret_MPI_Comm_dup\tcomm=%u\tproc=%d\tpcount=%d\t\n"
                 "plist=\n",
                 *newcomm, commInfoPtr2->proc, commInfoPtr2->pcount);

         for(i=0,j=0; i < commInfoPtr2->pcount; i++)
         {
             if(j == ListCount)
             { j = 0;
               tprintf("\n");
             }

             if(i < commInfoPtr2->pcount - 1)
                tprintf("%d,", commInfoPtr2->plist[i]);
             else
                tprintf("%d\t\n", commInfoPtr2->plist[i]);

             j++;
         }

         if(MPI_TraceTime)
         {  finish = dvm_time();

            if(MPI_TraceTimeReg)
            {  tprintf("t=%lf\t", finish-LastEventTime);
               LastEventTime = finish;
            }
            else
               tprintf("t=%lf\t", finish);
         }

         tprintf("$\n");
      }
   }

   return  retval;
}



int  MPI_Comm_create(MPI_Comm  comm, MPI_Group  group,
                     MPI_Comm  *newcomm)
{
   int         retval;
   double      start, finish;
   int         system, trace, debug, i, j;
   commInfo   *commInfoPtr1, *commInfoPtr2;
   MPI_Comm    c;   

   system = (RTS_Call_MPI || DVMCallLevel);
   trace  = (MPI_BotsulaProf && (MPI_TraceLevel || system == 0));
   debug  = (MPI_BotsulaDeb && (MPI_TraceLevel || system == 0));
   retval = (system == 0 && comm == MPI_COMM_WORLD && dvm_OneProcSign == 0);

   if(retval)
      c = DVM_COMM_WORLD;
   else
      c = comm;

   if(debug || trace)
   {
      getcomm(c, &commInfoPtr1);

      if(commInfoPtr1 == NULL)
         epprintf(1 ,__FILE__, __LINE__,
                "$*** dynamic analyzer err: wrong call MPI_Comm_create "
                "(non-existent communicator %u)$\n", comm);

      *newcomm = MPI_COMM_NULL;

      if(trace)
      {
        if(MPI_SlashOut)
           tprintf("\n");

        tprintf("$call_MPI_Comm_create\tcomm=%u\tgroup=%d\t\n",
                c, group);

        if(MPI_TraceTime)
        {  start = dvm_time();

           if(MPI_TraceTimeReg)
           {  tprintf("t=%lf\t", start-LastEventTime);
              LastEventTime = start;
           }
           else
              tprintf("t=%lf\t", start);
        }

        if(MPI_TraceFileLine)
           tprintf("f=%s\tl=%ld\t", DVM_FILE[0], DVM_LINE[0]);
        tprintf("$\n");
      }
   }

   retval = PMPI_Comm_create(c, group, newcomm);

   if(debug || trace)
   {
      /* */    /*E0060*/

      if(*newcomm != MPI_COMM_NULL)
      {
         dvm_AllocStruct(commInfo, commInfoPtr2);
         coll_Insert(CommStructColl, commInfoPtr2);
         coll_Insert(CommColl, *newcomm);

         _commProcs(*newcomm, &commInfoPtr2->pcount,
                    &commInfoPtr2->plist);
         PMPI_Comm_rank(*newcomm, &commInfoPtr2->proc);
      }

      if(trace)
      {
         if(*newcomm != MPI_COMM_NULL)
         {
            tprintf("$ret_MPI_Comm_create\t"
                    "comm=%u\tproc=%d\tpcount=%d\t\nplist=\n",
                    *newcomm, commInfoPtr2->proc, commInfoPtr2->pcount);

            for(i=0,j=0; i < commInfoPtr2->pcount; i++)
            {
                if(j == ListCount)
                { j = 0;
                  tprintf("\n");
                }

                if(i < commInfoPtr2->pcount - 1)
                   tprintf("%d,", commInfoPtr2->plist[i]);
                else
                   tprintf("%d\t\n", commInfoPtr2->plist[i]);

                j++;
            }

            if(MPI_TraceTime)
            {  finish = dvm_time();

               if(MPI_TraceTimeReg)
               {  tprintf("t=%lf\t", finish-LastEventTime);
                  LastEventTime = finish;
               }
               else
                   tprintf("t=%lf\t", finish);
            }

            tprintf("$\n");
         }
         else
         {
            tprintf("$ret_MPI_Comm_create\tcomm=0\t");

            if(MPI_TraceTime)
            {  finish = dvm_time();

               if(MPI_TraceTimeReg)
               {  tprintf("t=%lf\t", finish-LastEventTime);
                  LastEventTime = finish;
               }
               else
                  tprintf("t=%lf\t", finish);
            }

            tprintf("$\n");
         } 
      }
   }

   return  retval;
}



int  MPI_Comm_split(MPI_Comm  comm, int  color, int  key,
                    MPI_Comm  *newcomm)
{
   int         retval;
   double      start, finish;
   int         system, trace, debug, i, j;
   commInfo   *commInfoPtr1, *commInfoPtr2;
   MPI_Comm    c;   

   system = (RTS_Call_MPI || DVMCallLevel);
   trace  = (MPI_BotsulaProf && (MPI_TraceLevel || system == 0));
   debug  = (MPI_BotsulaDeb && (MPI_TraceLevel || system == 0));
   retval = (system == 0 && comm == MPI_COMM_WORLD && dvm_OneProcSign == 0);

   if(retval)
      c = DVM_COMM_WORLD;
   else
      c = comm;

   if(debug || trace)
   {
      getcomm(c, &commInfoPtr1);

      if(commInfoPtr1 == NULL)
         epprintf(1 ,__FILE__, __LINE__,
                "$*** dynamic analyzer err: wrong call MPI_Comm_split "
                "(non-existent communicator %u)$\n", comm);

      *newcomm = MPI_COMM_NULL;

      if(trace)
      {
        if(MPI_SlashOut)
           tprintf("\n");

        tprintf("$call_MPI_Comm_split\tcomm=%u\tcolor=%d\tkey=%d\t\n",
                c, color, key);

        if(MPI_TraceTime)
        {  start = dvm_time();

           if(MPI_TraceTimeReg)
           {  tprintf("t=%lf\t", start-LastEventTime);
              LastEventTime = start;
           }
           else
              tprintf("t=%lf\t", start);
        }

        if(MPI_TraceFileLine)
           tprintf("f=%s\tl=%ld\t", DVM_FILE[0], DVM_LINE[0]);
        tprintf("$\n");
      }
   }

   retval = PMPI_Comm_split(c, color, key, newcomm);

   if(debug || trace)
   {
      /* */    /*E0061*/

      if(*newcomm != MPI_COMM_NULL)
      {
         dvm_AllocStruct(commInfo, commInfoPtr2);
         coll_Insert(CommStructColl, commInfoPtr2);
         coll_Insert(CommColl, *newcomm);

         _commProcs(*newcomm, &commInfoPtr2->pcount,
                    &commInfoPtr2->plist);
         PMPI_Comm_rank(*newcomm, &commInfoPtr2->proc);
      }

      if(trace)
      {
         if(*newcomm != MPI_COMM_NULL)
         {
            tprintf("$ret_MPI_Comm_split\t"
                    "comm=%u\tproc=%d\tpcount=%d\t\nplist=\n",
                    *newcomm, commInfoPtr2->proc, commInfoPtr2->pcount);

            for(i=0,j=0; i < commInfoPtr2->pcount; i++)
            {
                if(j == ListCount)
                { j = 0;
                  tprintf("\n");
                }

                if(i < commInfoPtr2->pcount - 1)
                   tprintf("%d,", commInfoPtr2->plist[i]);
                else
                   tprintf("%d\t\n", commInfoPtr2->plist[i]);

                j++;
            }

            if(MPI_TraceTime)
            {  finish = dvm_time();

               if(MPI_TraceTimeReg)
               {  tprintf("t=%lf\t", finish-LastEventTime);
                  LastEventTime = finish;
               }
               else
                   tprintf("t=%lf\t", finish);
            }

            tprintf("$\n");
         }
         else
         {
            tprintf("$ret_MPI_Comm_split\tcomm=0\t");

            if(MPI_TraceTime)
            {  finish = dvm_time();

               if(MPI_TraceTimeReg)
               {  tprintf("t=%lf\t", finish-LastEventTime);
                  LastEventTime = finish;
               }
               else
                  tprintf("t=%lf\t", finish);
            }

            tprintf("$\n");
         }
      }
   }

   return  retval;
}



int  MPI_Comm_free(MPI_Comm  *commPtr)
{
   int         retval;
   double      start, finish;
   int         system, trace, debug, ind;
   commInfo   *commInfoPtr;
   MPI_Comm    c;   

   system = (RTS_Call_MPI || DVMCallLevel);
   trace  = (MPI_BotsulaProf && (MPI_TraceLevel || system == 0));
   debug  = (MPI_BotsulaDeb && (MPI_TraceLevel || system == 0));
   retval = (system == 0 && *commPtr == MPI_COMM_WORLD && dvm_OneProcSign == 0);

   if(retval)
      c = DVM_COMM_WORLD;
   else
      c = *commPtr;

   if(debug || trace)
   {
      ind = getcomm(c, &commInfoPtr);

      if(c == MPI_COMM_WORLD || c == DVM_COMM_WORLD)
         epprintf(1 ,__FILE__, __LINE__,
                "$*** dynamic analyzer err: wrong call MPI_Comm_free "
                "(destruction of system communicator %u)$\n", *commPtr);

      if(commInfoPtr == NULL)
         epprintf(1 ,__FILE__, __LINE__,
                "$*** dynamic analyzer err: wrong call MPI_Comm_free "
                "(non-existent communicator %u)$\n", *commPtr);

      if(trace)
      {
        if(MPI_SlashOut)
           tprintf("\n");

        tprintf("$call_MPI_Comm_free\tcomm=%u\t", c);

        if(MPI_TraceTime)
        {  start = dvm_time();

           if(MPI_TraceTimeReg)
           {  tprintf("t=%lf\t", start-LastEventTime);
              LastEventTime = start;
           }
           else
              tprintf("t=%lf\t", start);
        }

        if(MPI_TraceFileLine)
           tprintf("f=%s\tl=%ld\t", DVM_FILE[0], DVM_LINE[0]);
        tprintf("$\n");
      }
   }

   retval = PMPI_Comm_free(commPtr);

   if(debug || trace)
   {
      /* */    /*E0062*/

      mac_free((void **)&commInfoPtr->plist);
      coll_AtDelete(CommColl, ind);
      coll_AtDelete(CommStructColl, ind);
      dvm_FreeStruct(commInfoPtr);

      if(trace)
      {
         tprintf("$ret_MPI_Comm_free\t");

         if(MPI_TraceTime)
         {  finish = dvm_time();

            if(MPI_TraceTimeReg)
            {  tprintf("t=%lf\t", finish-LastEventTime);
               LastEventTime = finish;
            }
            else
               tprintf("t=%lf\t", finish);
         } 

         tprintf("$\n");
      }
   }

   return  retval;
}


/* ------------------------------------------------ */    /*E0063*/


int  MPI_Cart_create(MPI_Comm  comm, int  ndims, int  *dims,
                     int  *periods, int  reorder, MPI_Comm  *newcomm)
{
   int         retval;
   double      start, finish;
   int         system, trace, debug, i, j;
   commInfo   *commInfoPtr1, *commInfoPtr2;
   MPI_Comm    c;   

   system = (RTS_Call_MPI || DVMCallLevel);
   trace  = (MPI_BotsulaProf && (MPI_TraceLevel || system == 0));
   debug  = (MPI_BotsulaDeb && (MPI_TraceLevel || system == 0));
   retval = (system == 0 && comm == MPI_COMM_WORLD && dvm_OneProcSign == 0);

   if(retval)
      c = DVM_COMM_WORLD;
   else
      c = comm;

   if(debug || trace)
   {
      getcomm(c, &commInfoPtr1);

      if(commInfoPtr1 == NULL)
         epprintf(1 ,__FILE__, __LINE__,
                "$*** dynamic analyzer err: wrong call MPI_Cart_create "
                "(non-existent communicator %u)$\n", comm);

      *newcomm = MPI_COMM_NULL;

      if(trace)
      {
         if(MPI_SlashOut)
            tprintf("\n");

         tprintf("$call_MPI_Cart_create\t"
                 "comm=%u\tndims=%d\t\ndims=\n", c, ndims);

         for(i=0,j=0; i < ndims; i++)
         {
            if(j == ListCount)
            {  j = 0;
               tprintf("\n");
            }

            tprintf("%d,", dims[i]);
            j++;
         }

         tprintf("\t\nperiods=\n");

         for(i=0,j=0; i < ndims; i++)
         {
            if(j == ListCount)
            {  j = 0;
               tprintf("\n");
            }

            tprintf("%d,", periods[i]);
            j++;
         }

         tprintf("\t\nreorder=%d\t", reorder);

         if(MPI_TraceTime)
         {  start = dvm_time();

            if(MPI_TraceTimeReg)
            {  tprintf("t=%lf\t", start-LastEventTime);
               LastEventTime = start;
            }
            else
               tprintf("t=%lf\t", start);
         }

         if(MPI_TraceFileLine)
            tprintf("f=%s\tl=%ld\t", DVM_FILE[0], DVM_LINE[0]);
         tprintf("$\n");
      }
   }

   retval = PMPI_Cart_create(c, ndims, dims, periods, reorder, newcomm);

   if(debug || trace)
   {
      /* */    /*E0064*/

      if(*newcomm != MPI_COMM_NULL)
      {
         dvm_AllocStruct(commInfo, commInfoPtr2);
         coll_Insert(CommStructColl, commInfoPtr2);
         coll_Insert(CommColl, *newcomm);

         _commProcs(*newcomm, &commInfoPtr2->pcount,
                    &commInfoPtr2->plist);
         PMPI_Comm_rank(*newcomm, &commInfoPtr2->proc);
      }

      if(trace)
      {
         if(*newcomm != MPI_COMM_NULL)
         {
            tprintf("$ret_MPI_Cart_create\t"
                    "comm=%u\tproc=%d\tpcount=%d\t\nplist=\n",
                    *newcomm, commInfoPtr2->proc, commInfoPtr2->pcount);

            for(i=0,j=0; i < commInfoPtr2->pcount; i++)
            {
                if(j == ListCount)
                { j = 0;
                  tprintf("\n");
                }

                if(i < commInfoPtr2->pcount - 1)
                   tprintf("%d,", commInfoPtr2->plist[i]);
                else
                   tprintf("%d\t\n", commInfoPtr2->plist[i]);

                j++;
            }

            if(MPI_TraceTime)
            {  finish = dvm_time();

               if(MPI_TraceTimeReg)
               {  tprintf("t=%lf\t", finish-LastEventTime);
                  LastEventTime = finish;
               }
               else
                  tprintf("t=%lf\t", finish);
            }

            tprintf("$\n");
         }
         else
         {
            tprintf("$ret_MPI_Cart_create\tcomm=0\t");

            if(MPI_TraceTime)
            {  finish = dvm_time();

               if(MPI_TraceTimeReg)
               {  tprintf("t=%lf\t", finish-LastEventTime);
                  LastEventTime = finish;
               }
               else
                  tprintf("t=%lf\t", finish);
            }

            tprintf("$\n");
         }
      }
   }

   return  retval;
}



int  MPI_Cart_sub(MPI_Comm  comm, int  *remain_dims, MPI_Comm  *newcomm)
{
   int         retval;
   double      start, finish;
   int         system, trace, debug, i, j, ndims;
   commInfo   *commInfoPtr1, *commInfoPtr2;
   MPI_Comm    c;   

   system = (RTS_Call_MPI || DVMCallLevel);
   trace  = (MPI_BotsulaProf && (MPI_TraceLevel || system == 0));
   debug  = (MPI_BotsulaDeb && (MPI_TraceLevel || system == 0));
   retval = (system == 0 && comm == MPI_COMM_WORLD && dvm_OneProcSign == 0);

   if(retval)
      c = DVM_COMM_WORLD;
   else
      c = comm;

   if(debug || trace)
   {
      getcomm(c, &commInfoPtr1);

      if(commInfoPtr1 == NULL)
         epprintf(1 ,__FILE__, __LINE__,
                  "$*** dynamic analyzer err: wrong call MPI_Cart_sub "
                  "(non-existent communicator %u)$\n", comm);

      PMPI_Topo_test(comm, &retval);

      if(retval != MPI_CART)
         epprintf(1 ,__FILE__, __LINE__,
                  "$*** dynamic analyzer err: wrong call MPI_Cart_sub "
                  "(communicator topology is not a cartesian structure. "
                  " comm=%u)$\n", comm);

      *newcomm = MPI_COMM_NULL;

      if(trace)
      {
         PMPI_Cartdim_get(comm, &ndims);

         if(MPI_SlashOut)
            tprintf("\n");

         tprintf("$call_MPI_Cart_sub\t"
                 "comm=%u\tndims=%d\t\nrdims=\n", c, ndims);

         for(i=0,j=0; i < ndims; i++)
         {
            if(j == ListCount)
            {  j = 0;
               tprintf("\n");
            }

            tprintf("%d,", remain_dims[i]);
            j++;
         }

         tprintf("\t\n");

         if(MPI_TraceTime)
         {  start = dvm_time();

            if(MPI_TraceTimeReg)
            {  tprintf("t=%lf\t", start-LastEventTime);
               LastEventTime = start;
            }
            else
               tprintf("t=%lf\t", start);
         }

         if(MPI_TraceFileLine)
            tprintf("f=%s\tl=%ld\t", DVM_FILE[0], DVM_LINE[0]);
         tprintf("$\n");
      }
   }

   retval = PMPI_Cart_sub(c, remain_dims, newcomm);

   if(debug || trace)
   {
      /* */    /*E0065*/

      if(*newcomm != MPI_COMM_NULL)
      {
         dvm_AllocStruct(commInfo, commInfoPtr2);
         coll_Insert(CommStructColl, commInfoPtr2);
         coll_Insert(CommColl, *newcomm);

         _commProcs(*newcomm, &commInfoPtr2->pcount,
                    &commInfoPtr2->plist);
         PMPI_Comm_rank(*newcomm, &commInfoPtr2->proc);
      }

      if(trace)
      {
         if(*newcomm != MPI_COMM_NULL)
         {
            tprintf("$ret_MPI_Cart_sub\t"
                    "comm=%u\tproc=%d\tpcount=%d\t\nplist=\n",
                    *newcomm, commInfoPtr2->proc, commInfoPtr2->pcount);

            for(i=0,j=0; i < commInfoPtr2->pcount; i++)
            {
                if(j == ListCount)
                { j = 0;
                  tprintf("\n");
                }

                if(i < commInfoPtr2->pcount - 1)
                   tprintf("%d,", commInfoPtr2->plist[i]);
                else
                   tprintf("%d\t\n", commInfoPtr2->plist[i]);

                j++;
            }

            if(MPI_TraceTime)
            {  finish = dvm_time();

               if(MPI_TraceTimeReg)
               {  tprintf("t=%lf\t", finish-LastEventTime);
                  LastEventTime = finish;
               }
               else
                  tprintf("t=%lf\t", finish);
            }

            tprintf("$\n");
         }
         else
         {
            tprintf("$ret_MPI_Cart_sub\tcomm=0\t");

            if(MPI_TraceTime)
            {  finish = dvm_time();

               if(MPI_TraceTimeReg)
               {  tprintf("t=%lf\t", finish-LastEventTime);
                  LastEventTime = finish;
               }
               else
                  tprintf("t=%lf\t", finish);
            }

            tprintf("$\n");
         }
      }
   }

   return  retval;
}



int  MPI_Graph_create(MPI_Comm  comm, int  nnodes, int  *index,
                      int  *edges, int  reorder, MPI_Comm  *newcomm)
{
   int         retval;
   double      start, finish;
   int         system, trace, debug, i, j;
   commInfo   *commInfoPtr1, *commInfoPtr2;
   MPI_Comm    c;   

   system = (RTS_Call_MPI || DVMCallLevel);
   trace  = (MPI_BotsulaProf && (MPI_TraceLevel || system == 0));
   debug  = (MPI_BotsulaDeb && (MPI_TraceLevel || system == 0));
   retval = (system == 0 && comm == MPI_COMM_WORLD && dvm_OneProcSign == 0);

   if(retval)
      c = DVM_COMM_WORLD;
   else
      c = comm;

   if(debug || trace)
   {
      getcomm(c, &commInfoPtr1);

      if(commInfoPtr1 == NULL)
         epprintf(1 ,__FILE__, __LINE__,
               "$*** dynamic analyzer err: wrong call MPI_Graph_create "
               "(non-existent communicator %u)$\n", comm);

      *newcomm = MPI_COMM_NULL;

      if(trace)
      {
         if(MPI_SlashOut)
            tprintf("\n");

         tprintf("$call_MPI_Graph_create\t"
                 "comm=%u\tnnodes=%d\t\nindex=\n", c, nnodes);

         for(i=0,j=0; i < nnodes; i++)
         {
            if(j == ListCount)
            {  j = 0;
               tprintf("\n");
            }

            tprintf("%d,", index[i]);
            j++;
         }

         tprintf("\t\nedges=\n");

         retval = index[nnodes-1];       /* */    /*E0066*/

         for(i=0,j=0; i < retval; i++)
         {
            if(j == ListCount)
            {  j = 0;
               tprintf("\n");
            }

            tprintf("%d,", edges[i]);
            j++;
         }

         tprintf("\t\nreorder=%d\t", reorder);

         if(MPI_TraceTime)
         {  start = dvm_time();

            if(MPI_TraceTimeReg)
            {  tprintf("t=%lf\t", start-LastEventTime);
               LastEventTime = start;
            }
            else
               tprintf("t=%lf\t", start);
         }

         if(MPI_TraceFileLine)
            tprintf("f=%s\tl=%ld\t", DVM_FILE[0], DVM_LINE[0]);
         tprintf("$\n");
      }
   }

   retval = PMPI_Graph_create(c, nnodes, index, edges, reorder, newcomm);

   if(debug || trace)
   {
      /* */    /*E0067*/

      if(*newcomm != MPI_COMM_NULL)
      {
         dvm_AllocStruct(commInfo, commInfoPtr2);
         coll_Insert(CommStructColl, commInfoPtr2);
         coll_Insert(CommColl, *newcomm);

         _commProcs(*newcomm, &commInfoPtr2->pcount,
                    &commInfoPtr2->plist);
         PMPI_Comm_rank(*newcomm, &commInfoPtr2->proc);
      }

      if(trace)
      {
         if(*newcomm != MPI_COMM_NULL)
         {
            tprintf("$ret_MPI_Graph_create\t"
                    "comm=%u\tproc=%d\tpcount=%d\t\nplist=\n",
                    *newcomm, commInfoPtr2->proc, commInfoPtr2->pcount);

            for(i=0,j=0; i < commInfoPtr2->pcount; i++)
            {
                if(j == ListCount)
                { j = 0;
                  tprintf("\n");
                }

                if(i < commInfoPtr2->pcount - 1)
                   tprintf("%d,", commInfoPtr2->plist[i]);
                else
                   tprintf("%d\t\n", commInfoPtr2->plist[i]);

                j++;
            }

            if(MPI_TraceTime)
            {  finish = dvm_time();

               if(MPI_TraceTimeReg)
               {  tprintf("t=%lf\t", finish-LastEventTime);
                  LastEventTime = finish;
               }
               else
                  tprintf("t=%lf\t", finish);
            }

            tprintf("$\n");
         }
         else
         {
            tprintf("$ret_MPI_Graph_create\tcomm=0\t");

            if(MPI_TraceTime)
            {  finish = dvm_time();

               if(MPI_TraceTimeReg)
               {  tprintf("t=%lf\t", finish-LastEventTime);
                  LastEventTime = finish;
               }
               else
                  tprintf("t=%lf\t", finish);
            }

            tprintf("$\n");
         }
      }
   }

   return  retval;
}



int  MPI_Cart_map(MPI_Comm  comm, int  ndims, int  *dims, int  *periods,
                  int  *newrank)
{
   int         retval;
   double      start, finish;
   int         system, trace, debug, i, j;
   commInfo   *commInfoPtr;
   MPI_Comm    c;   

   system = (RTS_Call_MPI || DVMCallLevel);
   trace  = (MPI_BotsulaProf && (MPI_TraceLevel || system == 0));
   debug  = (MPI_BotsulaDeb && (MPI_TraceLevel || system == 0));
   retval = (system == 0 && comm == MPI_COMM_WORLD && dvm_OneProcSign == 0);

   if(retval)
      c = DVM_COMM_WORLD;
   else
      c = comm;

   if(debug || trace)
   {
      getcomm(c, &commInfoPtr);

      if(commInfoPtr == NULL)
         epprintf(1 ,__FILE__, __LINE__,
                "$*** dynamic analyzer err: wrong call MPI_Cart_map "
                "(non-existent communicator %u)$\n", comm);

      if(trace)
      {
         if(MPI_SlashOut)
            tprintf("\n");

         tprintf("$call_MPI_Cart_map\t"
                 "comm=%u\tndims=%d\t\ndims=\n", c, ndims);

         for(i=0,j=0; i < ndims; i++)
         {
            if(j == ListCount)
            {  j = 0;
               tprintf("\n");
            }

            tprintf("%d,", dims[i]);
            j++;
         }

         tprintf("\t\nperiods=\n");

         for(i=0,j=0; i < ndims; i++)
         {
            if(j == ListCount)
            {  j = 0;
               tprintf("\n");
            }

            tprintf("%d,", periods[i]);
            j++;
         }

         tprintf("\t\n");

         if(MPI_TraceTime)
         {  start = dvm_time();

            if(MPI_TraceTimeReg)
            {  tprintf("t=%lf\t", start-LastEventTime);
               LastEventTime = start;
            }
            else
               tprintf("t=%lf\t", start);
         }

         if(MPI_TraceFileLine)
            tprintf("f=%s\tl=%ld\t", DVM_FILE[0], DVM_LINE[0]);
         tprintf("$\n");
      }
   }

   retval = PMPI_Cart_map(c, ndims, dims, periods, newrank);

   if(debug || trace)
   {
      if(trace)
      {
         tprintf("$ret_MPI_Cart_map\tpcount=%d\t\nplist=\n",
                 commInfoPtr->pcount);

         for(i=0,j=0; i < commInfoPtr->pcount; i++)
         {
            if(j == ListCount)
            { j = 0;
              tprintf("\n");
            }

            if(i < commInfoPtr->pcount - 1)
               tprintf("%d,", newrank[i]);
            else
               tprintf("%d\t\n", newrank[i]);

            j++;
        }

        if(MPI_TraceTime)
        {  finish = dvm_time();

           if(MPI_TraceTimeReg)
           {  tprintf("t=%lf\t", finish-LastEventTime);
              LastEventTime = finish;
           }
           else
              tprintf("t=%lf\t", finish);
        }

        tprintf("$\n");
      }
   }

   return  retval;
}



int  MPI_Graph_map(MPI_Comm  comm, int  nnodes, int  *index,
                   int  *edges, int  *newrank)
{
   int         retval;
   double      start, finish;
   int         system, trace, debug, i, j;
   commInfo   *commInfoPtr;
   MPI_Comm    c;   

   system = (RTS_Call_MPI || DVMCallLevel);
   trace  = (MPI_BotsulaProf && (MPI_TraceLevel || system == 0));
   debug  = (MPI_BotsulaDeb && (MPI_TraceLevel || system == 0));
   retval = (system == 0 && comm == MPI_COMM_WORLD && dvm_OneProcSign == 0);

   if(retval)
      c = DVM_COMM_WORLD;
   else
      c = comm;

   if(debug || trace)
   {
      getcomm(c, &commInfoPtr);

      if(commInfoPtr == NULL)
         epprintf(1 ,__FILE__, __LINE__,
               "$*** dynamic analyzer err: wrong call MPI_Graph_map "
               "(non-existent communicator %u)$\n", comm);

      if(trace)
      {
         if(MPI_SlashOut)
            tprintf("\n");

         tprintf("$call_MPI_Graph_map\t"
                 "comm=%u\tnnodes=%d\t\nindex=\n", c, nnodes);

         for(i=0,j=0; i < nnodes; i++)
         {
            if(j == ListCount)
            {  j = 0;
               tprintf("\n");
            }

            tprintf("%d,", index[i]);
            j++;
         }

         tprintf("\t\nedges=\n");

         retval = index[nnodes-1];       /* */    /*E0068*/

         for(i=0,j=0; i < retval; i++)
         {
            if(j == ListCount)
            {  j = 0;
               tprintf("\n");
            }

            tprintf("%d,", edges[i]);
            j++;
         }

         tprintf("\t\n");

         if(MPI_TraceTime)
         {  start = dvm_time();

            if(MPI_TraceTimeReg)
            {  tprintf("t=%lf\t", start-LastEventTime);
               LastEventTime = start;
            }
            else
               tprintf("t=%lf\t", start);
         }

         if(MPI_TraceFileLine)
            tprintf("f=%s\tl=%ld\t", DVM_FILE[0], DVM_LINE[0]);
         tprintf("$\n");
      }
   }

   retval = PMPI_Graph_map(c, nnodes, index, edges, newrank);

   if(debug || trace)
   {
      if(trace)
      {
         tprintf("$ret_MPI_Graph_map\tpcount=%d\t\nplist=\n",
                 commInfoPtr->pcount);

         for(i=0,j=0; i < commInfoPtr->pcount; i++)
         {
            if(j == ListCount)
            { j = 0;
              tprintf("\n");
            }

            if(i < commInfoPtr->pcount - 1)
               tprintf("%d,", newrank[i]);
            else
               tprintf("%d\t\n", newrank[i]);

            j++;
        }

        if(MPI_TraceTime)
        {  finish = dvm_time();

           if(MPI_TraceTimeReg)
           {  tprintf("t=%lf\t", finish-LastEventTime);
              LastEventTime = finish;
           }
           else
              tprintf("t=%lf\t", finish);
        }

        tprintf("$\n");
      }
   }

   return  retval;
}


/* ------------------------------------------------ */    /*E0069*/


int  MPI_Intercomm_create(MPI_Comm  local_comm, int  local_leader,
                          MPI_Comm  peer_comm, int  remote_leader,
                          int  tag, MPI_Comm  *newintercomm)
{
   int        retval;
   int        system, trace;
   MPI_Comm   c_local, c_peer;   

   system = (RTS_Call_MPI || DVMCallLevel);
   trace  = (MPI_BotsulaProf && (MPI_TraceLevel || system == 0));

   retval = (system == 0 && local_comm == MPI_COMM_WORLD && dvm_OneProcSign == 0);

   if(retval)
      c_local = DVM_COMM_WORLD;
   else
      c_local = local_comm;

   retval = (system == 0 && peer_comm == MPI_COMM_WORLD && dvm_OneProcSign == 0);

   if(retval)
      c_peer = DVM_COMM_WORLD;
   else
      c_peer = peer_comm;

   retval = PMPI_Intercomm_create(c_local, local_leader,
                                  c_peer, remote_leader, tag,
                                  newintercomm);
   return  retval;
}



int  MPI_Intercomm_merge(MPI_Comm  intercomm, int  high,
                         MPI_Comm  *newintracomm)
{
   int        retval;
   int        system, trace;
   MPI_Comm   c;   

   system = (RTS_Call_MPI || DVMCallLevel);
   trace  = (MPI_BotsulaProf && (MPI_TraceLevel || system == 0));
   retval = (system == 0 && intercomm == MPI_COMM_WORLD && dvm_OneProcSign == 0);

   if(retval)
      c = DVM_COMM_WORLD;
   else
      c = intercomm;

   retval = PMPI_Intercomm_merge(c, high, newintracomm);

   return  retval;
}


/* */    /*E0070*/

int  MPI_Comm_size(MPI_Comm  comm, int  *size)
{
   int         retval;
   double      start, finish;
   int         system, trace, debug;
   commInfo   *commInfoPtr;
   MPI_Comm    c;   

   system = (RTS_Call_MPI || DVMCallLevel);
   trace  = (MPI_BotsulaProf && (MPI_TraceLevel || system == 0));
   debug  = (MPI_BotsulaDeb && (MPI_TraceLevel || system == 0));
   retval = (system == 0 && comm == MPI_COMM_WORLD && dvm_OneProcSign == 0);

   if(retval)
      c = DVM_COMM_WORLD;
   else
      c = comm;

   if(debug || trace)
   {
      getcomm(c, &commInfoPtr);

      if(commInfoPtr == NULL)
         epprintf(1 ,__FILE__, __LINE__,
                "$*** dynamic analyzer err: wrong call MPI_Comm_size "
                "(non-existent communicator %u)$\n", comm);

      if(trace && MPI_TraceAll)
      {
        if(MPI_SlashOut)
           tprintf("\n");

        tprintf("$call_MPI_Comm_size\tcomm=%u\t", c);

        if(MPI_TraceTime)
        {  start = dvm_time();

           if(MPI_TraceTimeReg)
           {  tprintf("t=%lf\t", start-LastEventTime);
              LastEventTime = start;
           }
           else
              tprintf("t=%lf\t", start);
        }

        if(MPI_TraceFileLine)
           tprintf("f=%s\tl=%ld\t", DVM_FILE[0], DVM_LINE[0]);
        tprintf("$\n");
      }
   }

   retval = PMPI_Comm_size(c, size);

   if(debug || trace)
   {
      if(trace && MPI_TraceAll)
      { 
         tprintf("$ret_MPI_Comm_free\tsize=%d\t", *size);

         if(MPI_TraceTime)
         {  finish = dvm_time();

            if(MPI_TraceTimeReg)
            {  tprintf("t=%lf\t", finish-LastEventTime);
               LastEventTime = finish;
            }
            else
               tprintf("t=%lf\t", finish);
         }

         tprintf("$\n");
      }
   }

   return  retval;
}



int  MPI_Comm_rank(MPI_Comm  comm, int  *rank)
{
   int         retval;
   double      start, finish;
   int         system, trace, debug;
   commInfo   *commInfoPtr;
   MPI_Comm    c;   

   system = (RTS_Call_MPI || DVMCallLevel);
   trace  = (MPI_BotsulaProf && (MPI_TraceLevel || system == 0));
   debug  = (MPI_BotsulaDeb && (MPI_TraceLevel || system == 0));
   retval = (system == 0 && comm == MPI_COMM_WORLD && dvm_OneProcSign == 0);

   if(retval)
      c = DVM_COMM_WORLD;
   else
      c = comm;

   if(debug || trace)
   {
      getcomm(c, &commInfoPtr);

      if(commInfoPtr == NULL)
         epprintf(1 ,__FILE__, __LINE__,
                "$*** dynamic analyzer err: wrong call MPI_Comm_rank "
                "(non-existent communicator %u)$\n", comm);

      if(trace && MPI_TraceAll)
      {
        if(MPI_SlashOut)
           tprintf("\n");

        tprintf("$call_MPI_Comm_rank\tcomm=%u\t", c);

        if(MPI_TraceTime)
        {  start = dvm_time();

           if(MPI_TraceTimeReg)
           {  tprintf("t=%lf\t", start-LastEventTime);
              LastEventTime = start;
           }
           else
              tprintf("t=%lf\t", start);
        }

        if(MPI_TraceFileLine)
           tprintf("f=%s\tl=%ld\t", DVM_FILE[0], DVM_LINE[0]);
        tprintf("$\n");
      }
   }

   retval = PMPI_Comm_rank(c, rank);

   if(debug || trace)
   {
      if(trace && MPI_TraceAll)
      { 
         tprintf("$ret_MPI_Comm_rank\trank=%d\t", *rank);

         if(MPI_TraceTime)
         {  finish = dvm_time();

            if(MPI_TraceTimeReg)
            {  tprintf("t=%lf\t", finish-LastEventTime);
               LastEventTime = finish;
            }
            else
               tprintf("t=%lf\t", finish);
         }

         tprintf("$\n");
      }
   }

   return  retval;
}


/* */    /*E0071*/

int  MPI_Pack(void  *inbuf, int  incount, MPI_Datatype  datatype,
              void  *outbuf, int  outsize, int  *position,
              MPI_Comm  comm)
{
   int         retval;
   int         system, trace;
   MPI_Comm    c;   

   system = (RTS_Call_MPI || DVMCallLevel);
   trace  = (MPI_BotsulaProf && (MPI_TraceLevel || system == 0));
   retval = (system == 0 && comm == MPI_COMM_WORLD && dvm_OneProcSign == 0);

   if(retval)
      c = DVM_COMM_WORLD;
   else
      c = comm;

   retval = PMPI_Pack(inbuf, incount, datatype, outbuf, outsize,
                      position, c);

   return  retval;
}



int  MPI_Unpack(void  *inbuf, int  insize, int  *position,
                void  *outbuf, int  outcount, MPI_Datatype  datatype,
                MPI_Comm  comm)
{
   int         retval;
   int         system, trace;
   MPI_Comm    c;   

   system = (RTS_Call_MPI || DVMCallLevel);
   trace  = (MPI_BotsulaProf && (MPI_TraceLevel || system == 0));
   retval = (system == 0 && comm == MPI_COMM_WORLD && dvm_OneProcSign == 0);

   if(retval)
      c = DVM_COMM_WORLD;
   else
      c = comm;

   retval = PMPI_Unpack(inbuf, insize, position, outbuf, outcount,
                        datatype, c);

   return  retval;
}



int  MPI_Pack_size(int  incount, MPI_Datatype  datatype, MPI_Comm  comm,
                   int  *size)
{
   int         retval;
   int         system, trace;
   MPI_Comm    c;   

   system = (RTS_Call_MPI || DVMCallLevel);
   trace  = (MPI_BotsulaProf && (MPI_TraceLevel || system == 0));
   retval = (system == 0 && comm == MPI_COMM_WORLD && dvm_OneProcSign == 0);

   if(retval)
      c = DVM_COMM_WORLD;
   else
      c = comm;

   retval = PMPI_Pack_size(incount, datatype, c, size);

   return  retval;
}



int  MPI_Comm_group(MPI_Comm  comm, MPI_Group  *group)
{
   int         retval;
   int         system, trace;
   MPI_Comm    c;   

   system = (RTS_Call_MPI || DVMCallLevel);
   trace  = (MPI_BotsulaProf && (MPI_TraceLevel || system == 0));
   retval = (system == 0 && comm == MPI_COMM_WORLD && dvm_OneProcSign == 0);

   if(retval)
      c = DVM_COMM_WORLD;
   else
      c = comm;

   retval = PMPI_Comm_group(c, group);

   return  retval;
}



int  MPI_Comm_compare(MPI_Comm  comm1, MPI_Comm  comm2, int  *result)
{
   int        retval;
   int        system, trace;
   MPI_Comm   c1, c2;   

   system = (RTS_Call_MPI || DVMCallLevel);
   trace  = (MPI_BotsulaProf && (MPI_TraceLevel || system == 0));

   retval = (system == 0 && comm1 == MPI_COMM_WORLD && dvm_OneProcSign == 0);

   if(retval)
      c1 = DVM_COMM_WORLD;
   else
      c1 = comm1;

   retval = (system == 0 && comm2 == MPI_COMM_WORLD && dvm_OneProcSign == 0);

   if(retval)
      c2 = DVM_COMM_WORLD;
   else
      c2 = comm2;

   retval = PMPI_Comm_compare(c1, c2, result);

   return  retval;
}



int  MPI_Comm_test_inter(MPI_Comm  comm, int  *flag)
{
   int        retval;
   int        system, trace;
   MPI_Comm   c;   

   system = (RTS_Call_MPI || DVMCallLevel);
   trace  = (MPI_BotsulaProf && (MPI_TraceLevel || system == 0));
   retval = (system == 0 && comm == MPI_COMM_WORLD && dvm_OneProcSign == 0);

   if(retval)
      c = DVM_COMM_WORLD;
   else
      c = comm;

   retval = PMPI_Comm_test_inter(c, flag);

   return  retval;
}



int  MPI_Comm_remote_size(MPI_Comm  comm, int  *size)
{
   int        retval;
   int        system, trace;
   MPI_Comm   c;   

   system = (RTS_Call_MPI || DVMCallLevel);
   trace  = (MPI_BotsulaProf && (MPI_TraceLevel || system == 0));
   retval = (system == 0 && comm == MPI_COMM_WORLD && dvm_OneProcSign == 0);

   if(retval && DVM_COMM_WORLD != MPI_COMM_NULL)
      c = DVM_COMM_WORLD;
   else
      c = comm;

   retval = PMPI_Comm_remote_size(c, size);

   return  retval;
}



int  MPI_Comm_remote_group(MPI_Comm  comm, MPI_Group  *group)
{
   int        retval;
   int        system, trace;
   MPI_Comm   c;   

   system = (RTS_Call_MPI || DVMCallLevel);
   trace  = (MPI_BotsulaProf && (MPI_TraceLevel || system == 0));
   retval = (system == 0 && comm == MPI_COMM_WORLD && dvm_OneProcSign == 0);

   if(retval && DVM_COMM_WORLD != MPI_COMM_NULL)
      c = DVM_COMM_WORLD;
   else
      c = comm;

   retval = PMPI_Comm_remote_group(c, group);

   return  retval;
}



int  MPI_Attr_put(MPI_Comm  comm, int  keyval, void  *attribute_val)
{
   int        retval;
   int        system, trace;
   MPI_Comm   c;   

   system = (RTS_Call_MPI || DVMCallLevel);
   trace  = (MPI_BotsulaProf && (MPI_TraceLevel || system == 0));
   retval = (system == 0 && comm == MPI_COMM_WORLD && dvm_OneProcSign == 0);

   if(retval && DVM_COMM_WORLD != MPI_COMM_NULL)
      c = DVM_COMM_WORLD;
   else
      c = comm;

   retval = PMPI_Attr_put(c, keyval, attribute_val);

   return  retval;
}



int  MPI_Attr_get(MPI_Comm  comm, int  keyval, void  *attribute_val,
                  int  *flag)
{
   int        retval;
   int        system, trace;
   MPI_Comm   c;   

   system = (RTS_Call_MPI || DVMCallLevel);
   trace  = (MPI_BotsulaProf && (MPI_TraceLevel || system == 0));
   retval = (system == 0 && comm == MPI_COMM_WORLD && dvm_OneProcSign == 0);

   if(retval && DVM_COMM_WORLD != MPI_COMM_NULL)
      c = DVM_COMM_WORLD;
   else
      c = comm;

   retval = PMPI_Attr_get(c, keyval, attribute_val, flag);

   return  retval;
}



int  MPI_Attr_delete(MPI_Comm  comm, int  keyval)
{
   int        retval;
   int        system, trace;
   MPI_Comm   c;   

   system = (RTS_Call_MPI || DVMCallLevel);
   trace  = (MPI_BotsulaProf && (MPI_TraceLevel || system == 0));
   retval = (system == 0 && comm == MPI_COMM_WORLD && dvm_OneProcSign == 0);

   if(retval && DVM_COMM_WORLD != MPI_COMM_NULL)
      c = DVM_COMM_WORLD;
   else
      c = comm;

   retval = PMPI_Attr_delete(c, keyval);

   return  retval;
}


/* ------------------------------------------------ */    /*E0072*/


int  MPI_Topo_test(MPI_Comm  comm, int  *status)
{
   int        retval;
   int        system, trace;
   MPI_Comm   c;   

   system = (RTS_Call_MPI || DVMCallLevel);
   trace  = (MPI_BotsulaProf && (MPI_TraceLevel || system == 0));
   retval = (system == 0 && comm == MPI_COMM_WORLD && dvm_OneProcSign == 0);

   if(retval && DVM_COMM_WORLD != MPI_COMM_NULL)
      c = DVM_COMM_WORLD;
   else
      c = comm;

   retval = PMPI_Topo_test(c, status);

   return  retval;
}



int  MPI_Graphdims_get(MPI_Comm  comm, int  *nnodes, int  *nedges)
{
   int        retval;
   int        system, trace;
   MPI_Comm   c;   

   system = (RTS_Call_MPI || DVMCallLevel);
   trace  = (MPI_BotsulaProf && (MPI_TraceLevel || system == 0));
   retval = (system == 0 && comm == MPI_COMM_WORLD && dvm_OneProcSign == 0);

   if(retval)
      c = DVM_COMM_WORLD;
   else
      c = comm;

   retval = PMPI_Graphdims_get(c, nnodes, nedges);

   return  retval;
}



int  MPI_Graph_get(MPI_Comm  comm, int  maxindex, int  maxedges,
                   int  *index, int  *edges)
{
   int        retval;
   int        system, trace;
   MPI_Comm   c;   

   system = (RTS_Call_MPI || DVMCallLevel);
   trace  = (MPI_BotsulaProf && (MPI_TraceLevel || system == 0));
   retval = (system == 0 && comm == MPI_COMM_WORLD && dvm_OneProcSign == 0);

   if(retval)
      c = DVM_COMM_WORLD;
   else
      c = comm;

   retval = PMPI_Graph_get(c, maxindex, maxedges, index, edges);

   return  retval;
}



int  MPI_Cartdim_get(MPI_Comm  comm, int  *ndims)
{
   int        retval;
   int        system, trace;
   MPI_Comm   c;   

   system = (RTS_Call_MPI || DVMCallLevel);
   trace  = (MPI_BotsulaProf && (MPI_TraceLevel || system == 0));
   retval = (system == 0 && comm == MPI_COMM_WORLD && dvm_OneProcSign == 0);

   if(retval)
      c = DVM_COMM_WORLD;
   else
      c = comm;

   retval = PMPI_Cartdim_get(c, ndims);

   return  retval;
}



int  MPI_Cart_get(MPI_Comm  comm, int  maxdims, int  *dims,
                  int  *periods, int  *coords)
{
   int        retval;
   int        system, trace;
   MPI_Comm   c;   

   system = (RTS_Call_MPI || DVMCallLevel);
   trace  = (MPI_BotsulaProf && (MPI_TraceLevel || system == 0));
   retval = (system == 0 && comm == MPI_COMM_WORLD && dvm_OneProcSign == 0);

   if(retval)
      c = DVM_COMM_WORLD;
   else
      c = comm;

   retval = PMPI_Cart_get(c, maxdims, dims, periods, coords);

   return  retval;
}



int  MPI_Cart_rank(MPI_Comm  comm, int  *coords, int  *rank)
{
   int        retval;
   int        system, trace;
   MPI_Comm   c;   

   system = (RTS_Call_MPI || DVMCallLevel);
   trace  = (MPI_BotsulaProf && (MPI_TraceLevel || system == 0));
   retval = (system == 0 && comm == MPI_COMM_WORLD && dvm_OneProcSign == 0);

   if(retval)
      c = DVM_COMM_WORLD;
   else
      c = comm;

   retval = PMPI_Cart_rank(c, coords, rank);

   return  retval;
}



int  MPI_Cart_coords(MPI_Comm  comm, int  rank, int  maxdims,
                     int  *coords)
{
   int        retval;
   int        system, trace;
   MPI_Comm   c;   

   system = (RTS_Call_MPI || DVMCallLevel);
   trace  = (MPI_BotsulaProf && (MPI_TraceLevel || system == 0));
   retval = (system == 0 && comm == MPI_COMM_WORLD && dvm_OneProcSign == 0);

   if(retval)
      c = DVM_COMM_WORLD;
   else
      c = comm;

   retval = PMPI_Cart_coords(c, rank, maxdims, coords);

   return  retval;
}



int  MPI_Graph_neighbors_count(MPI_Comm  comm, int  rank,
                               int  *nneighbors)
{
   int        retval;
   int        system, trace;
   MPI_Comm   c;   

   system = (RTS_Call_MPI || DVMCallLevel);
   trace  = (MPI_BotsulaProf && (MPI_TraceLevel || system == 0));
   retval = (system == 0 && comm == MPI_COMM_WORLD && dvm_OneProcSign == 0);

   if(retval)
      c = DVM_COMM_WORLD;
   else
      c = comm;

   retval = PMPI_Graph_neighbors_count(c, rank, nneighbors);

   return  retval;
}



int  MPI_Graph_neighbors(MPI_Comm  comm, int  rank, int  maxneighbors,
                         int  *neighbors)
{
   int        retval;
   int        system, trace;
   MPI_Comm   c;   

   system = (RTS_Call_MPI || DVMCallLevel);
   trace  = (MPI_BotsulaProf && (MPI_TraceLevel || system == 0));
   retval = (system == 0 && comm == MPI_COMM_WORLD && dvm_OneProcSign == 0);

   if(retval)
      c = DVM_COMM_WORLD;
   else
      c = comm;

   retval = PMPI_Graph_neighbors(c, rank, maxneighbors, neighbors);

   return  retval;
}



int  MPI_Cart_shift(MPI_Comm  comm, int  direction, int  disp,
                    int  *rank_source, int  *rank_dest)
{
   int        retval;
   int        system, trace;
   MPI_Comm   c;   

   system = (RTS_Call_MPI || DVMCallLevel);
   trace  = (MPI_BotsulaProf && (MPI_TraceLevel || system == 0));
   retval = (system == 0 && comm == MPI_COMM_WORLD && dvm_OneProcSign == 0);

   if(retval)
      c = DVM_COMM_WORLD;
   else
      c = comm;

   retval = PMPI_Cart_shift(c, direction, disp, rank_source, rank_dest);

   return  retval;
}


/* ------------------------------------------------ */    /*E0073*/


int  MPI_Errhandler_set(MPI_Comm  comm, MPI_Errhandler  errhandler)
{
   int        retval;
   int        system, trace;
   MPI_Comm   c;   

   system = (RTS_Call_MPI || DVMCallLevel);
   trace  = (MPI_BotsulaProf && (MPI_TraceLevel || system == 0));
   retval = (system == 0 && comm == MPI_COMM_WORLD && dvm_OneProcSign == 0);

   if(retval)
      c = DVM_COMM_WORLD;
   else
      c = comm;

   retval = PMPI_Errhandler_set(c, errhandler);

   return  retval;
}



int  MPI_Errhandler_get(MPI_Comm  comm, MPI_Errhandler  *errhandler)
{
   int        retval;
   int        system, trace;
   MPI_Comm   c;   

   system = (RTS_Call_MPI || DVMCallLevel);
   trace  = (MPI_BotsulaProf && (MPI_TraceLevel || system == 0));
   retval = (system == 0 && comm == MPI_COMM_WORLD && dvm_OneProcSign == 0);

   if(retval)
      c = DVM_COMM_WORLD;
   else
      c = comm;

   retval = PMPI_Errhandler_get(c, errhandler);

   return  retval;
}


/* ------------------------------------------------ */    /*E0074*/


/* */    /*E0075*/

int  MPI_NULL_COPY_FN(MPI_Comm  oldcomm, int  keyval, void  *extra_state,
                      void  *attr_in, void  *attr_out, int  *flag)
{
   int        retval;
   int        system, trace;
   MPI_Comm   c;   

   _perror("function PMPI_NULL_COPY_FN does not exist");
   
   system = (RTS_Call_MPI || DVMCallLevel);
   trace  = (MPI_BotsulaProf && (MPI_TraceLevel || system == 0));
   retval = (system == 0 && oldcomm == MPI_COMM_WORLD && dvm_OneProcSign == 0);

   if(retval)
      c = DVM_COMM_WORLD;
   else
      c = oldcomm;

/*
   retval = PMPI_NULL_COPY_FN(c, keyval, extra_state,
                              attr_in, attr_out, flag);
*/    /*e0076*/

   return  retval;
}



int  MPI_NULL_DELETE_FN(MPI_Comm  comm, int  keyval, void  *attr, 
                        void  *extra_state)
{
   int        retval;
   int        system, trace;
   MPI_Comm   c;   

   _perror("function PMPI_NULL_DELETE_FN does not exist");

   system = (RTS_Call_MPI || DVMCallLevel);
   trace  = (MPI_BotsulaProf && (MPI_TraceLevel || system == 0));
   retval = (system == 0 && comm == MPI_COMM_WORLD && dvm_OneProcSign == 0);

   if(retval)
      c = DVM_COMM_WORLD;
   else
      c = comm;

/*
   retval = PMPI_NULL_DELETE_FN(c, keyval, attr, extra_state);
*/    /*e0077*/

   return  retval;
}



int  MPI_DUP_FN(MPI_Comm  comm, int  keyval, void  *extra_state,
                void  *attr_in, void  *attr_out, int  *flag)
{
   int        retval;
   int        system, trace;
   MPI_Comm   c;   

   _perror("function PMPI_DUP_FN does not exist");

   system = (RTS_Call_MPI || DVMCallLevel);
   trace  = (MPI_BotsulaProf && (MPI_TraceLevel || system == 0));
   retval = (system == 0 && comm == MPI_COMM_WORLD && dvm_OneProcSign == 0);

   if(retval)
      c = DVM_COMM_WORLD;
   else
      c = comm;

/*
   retval = PMPI_DUP_FN(c, keyval, extra_state, attr_in, attr_out, flag);
*/    /*e0078*/

   return  retval;
}


/* MPI-2 communicator naming functions */    /*e0079*/

/* */    /*E0080*/

#ifdef _DVM_MPI2_
#ifdef _NT_MPI_

/* */    /*E0081*/

int  MPI_Comm_set_name(MPI_Comm  comm, char  *comm_name)
{
   int        retval;
   int        system, trace;
   MPI_Comm   c;   

#ifdef _NT_MPI_
   _perror("function PMPI_Comm_set_name does not exist");
#endif

   system = (RTS_Call_MPI || DVMCallLevel);
   trace  = (MPI_BotsulaProf && (MPI_TraceLevel || system == 0));
   retval = (system == 0 && comm == MPI_COMM_WORLD && dvm_OneProcSign == 0);

   if(retval && DVM_COMM_WORLD != MPI_COMM_NULL)
      c = DVM_COMM_WORLD;
   else
      c = comm;

#ifndef _NT_MPI_
   retval = PMPI_Comm_set_name(c, comm_name);
#endif

   return  retval;
}



int  MPI_Comm_get_name(MPI_Comm  comm, char  *comm_name, int  *resultlen)
{
   int        retval;
   int        system, trace;
   MPI_Comm   c;   

#ifdef _NT_MPI_
   _perror("function PMPI_Comm_get_name does not exist");
#endif

   system = (RTS_Call_MPI || DVMCallLevel);
   trace  = (MPI_BotsulaProf && (MPI_TraceLevel || system == 0));
   retval = (system == 0 && comm == MPI_COMM_WORLD && dvm_OneProcSign == 0);

   if(retval && DVM_COMM_WORLD != MPI_COMM_NULL)
      c = DVM_COMM_WORLD;
   else
      c = comm;

#ifndef _NT_MPI_
   retval = PMPI_Comm_get_name(c, comm_name, resultlen);
#endif

   return  retval;
}

#endif
#endif


/* */    /*E0082*/

int  MPI_Get_count(MPI_Status  *status, MPI_Datatype  datatype,
                   int  *count)
{
   int   retval;
   int   system, trace;

   system = (RTS_Call_MPI || DVMCallLevel);
   trace  = (MPI_BotsulaProf && (MPI_TraceLevel || system == 0));

   retval = PMPI_Get_count(status, datatype, count);
   return  retval;
}



int  MPI_Buffer_attach(void  *buffer, int  size)
{
   int   retval;
   int   system, trace;

   system = (RTS_Call_MPI || DVMCallLevel);
   trace  = (MPI_BotsulaProf && (MPI_TraceLevel || system == 0));

   retval = PMPI_Buffer_attach(buffer, size);
   return  retval;
}



int  MPI_Buffer_detach(void  *buffer, int  *size)
{
   int   retval;
   int   system, trace;

   system = (RTS_Call_MPI || DVMCallLevel);
   trace  = (MPI_BotsulaProf && (MPI_TraceLevel || system == 0));

   retval = PMPI_Buffer_detach(buffer, size);
   return  retval;
}



int  MPI_Test_cancelled(MPI_Status  *status, int  *flag)
{
   int   retval;
   int   system, trace;

   system = (RTS_Call_MPI || DVMCallLevel);
   trace  = (MPI_BotsulaProf && (MPI_TraceLevel || system == 0));

   retval = PMPI_Test_cancelled(status, flag);
   return  retval;
}



int  MPI_Type_contiguous(int  count, MPI_Datatype  oldtype,
                         MPI_Datatype  *newtype)
{
   int   retval;
   int   system, trace;

   system = (RTS_Call_MPI || DVMCallLevel);
   trace  = (MPI_BotsulaProf && (MPI_TraceLevel || system == 0));

   retval = PMPI_Type_contiguous(count, oldtype, newtype);
   return  retval;
}



int  MPI_Type_vector(int  count, int  blocklength, int  stride, 
                     MPI_Datatype  oldtype, MPI_Datatype  *newtype)
{
   int   retval;
   int   system, trace;

   system = (RTS_Call_MPI || DVMCallLevel);
   trace  = (MPI_BotsulaProf && (MPI_TraceLevel || system == 0));

   retval = PMPI_Type_vector(count, blocklength, stride,
                             oldtype, newtype);
   return  retval;
}



int  MPI_Type_hvector(int  count, int  blocklength, MPI_Aint  stride, 
                      MPI_Datatype  oldtype, MPI_Datatype  *newtype)
{
   int   retval;
   int   system, trace;

   system = (RTS_Call_MPI || DVMCallLevel);
   trace  = (MPI_BotsulaProf && (MPI_TraceLevel || system == 0));

   retval = PMPI_Type_hvector(count, blocklength, stride,
                              oldtype, newtype);
   return  retval;
}



int  MPI_Type_indexed(int  count, int  *array_of_blocklengths, 
                      int  *array_of_displacements,
                      MPI_Datatype  oldtype, MPI_Datatype  *newtype)
{
   int   retval;
   int   system, trace;

   system = (RTS_Call_MPI || DVMCallLevel);
   trace  = (MPI_BotsulaProf && (MPI_TraceLevel || system == 0));

   retval = PMPI_Type_indexed(count, array_of_blocklengths,
                              array_of_displacements, oldtype, newtype);
   return  retval;
}



int  MPI_Type_hindexed(int  count, int  *array_of_blocklengths, 
                       MPI_Aint  *array_of_displacements,
                       MPI_Datatype  oldtype, MPI_Datatype  *newtype)
{
   int   retval;
   int   system, trace;

   system = (RTS_Call_MPI || DVMCallLevel);
   trace  = (MPI_BotsulaProf && (MPI_TraceLevel || system == 0));

   retval = PMPI_Type_hindexed(count, array_of_blocklengths, 
                               array_of_displacements, oldtype, newtype);
   return  retval;
}



int  MPI_Type_struct(int  count, int  *array_of_blocklengths, 
                     MPI_Aint  *array_of_displacements, 
                     MPI_Datatype  *array_of_types,
                     MPI_Datatype  *newtype)
{
   int   retval;
   int   system, trace;

   system = (RTS_Call_MPI || DVMCallLevel);
   trace  = (MPI_BotsulaProf && (MPI_TraceLevel || system == 0));

   retval = PMPI_Type_struct(count, array_of_blocklengths,
                             array_of_displacements, array_of_types,
                             newtype);
   return  retval;
}



int  MPI_Address(void  *location, MPI_Aint  *address)
{
   int   retval;
   int   system, trace;

   system = (RTS_Call_MPI || DVMCallLevel);
   trace  = (MPI_BotsulaProf && (MPI_TraceLevel || system == 0));

   retval = PMPI_Address(location, address);
   return  retval;
}



int  MPI_Type_extent(MPI_Datatype  datatype, MPI_Aint  *extent)
{
   int   retval;
   int   system, trace;

   system = (RTS_Call_MPI || DVMCallLevel);
   trace  = (MPI_BotsulaProf && (MPI_TraceLevel || system == 0));

   retval = PMPI_Type_extent(datatype, extent);
   return  retval;
}



int  MPI_Type_size(MPI_Datatype  datatype, int  *size)
{
   int   retval;
   int   system, trace;

   system = (RTS_Call_MPI || DVMCallLevel);
   trace  = (MPI_BotsulaProf && (MPI_TraceLevel || system == 0));

   retval = PMPI_Type_size(datatype, size);
   return  retval;
}



/* */    /*E0083*/

/*
int  MPI_Type_count(MPI_Datatype  datatype, int  *count)
{
   int   retval;
   int   system, trace;

   system = (RTS_Call_MPI || DVMCallLevel);
   trace  = (MPI_BotsulaProf && (MPI_TraceLevel || system == 0));

   retval = PMPI_Type_count(datatype, count);
   return  retval;
}
*/    /*E0084*/



int  MPI_Type_lb(MPI_Datatype  datatype, MPI_Aint  *displacement)
{
   int   retval;
   int   system, trace;

   system = (RTS_Call_MPI || DVMCallLevel);
   trace  = (MPI_BotsulaProf && (MPI_TraceLevel || system == 0));

   retval = PMPI_Type_lb(datatype, displacement);
   return  retval;
}



int  MPI_Type_ub(MPI_Datatype  datatype, MPI_Aint  *displacement)
{
   int   retval;
   int   system, trace;

   system = (RTS_Call_MPI || DVMCallLevel);
   trace  = (MPI_BotsulaProf && (MPI_TraceLevel || system == 0));

   retval = PMPI_Type_ub(datatype, displacement);
   return  retval;
}



int  MPI_Type_commit(MPI_Datatype  *datatype)
{
   int   retval;
   int   system, trace;

   system = (RTS_Call_MPI || DVMCallLevel);
   trace  = (MPI_BotsulaProf && (MPI_TraceLevel || system == 0));

   retval = PMPI_Type_commit(datatype);
   return  retval;
}



int  MPI_Type_free(MPI_Datatype  *datatype)
{
   int   retval;
   int   system, trace;

   system = (RTS_Call_MPI || DVMCallLevel);
   trace  = (MPI_BotsulaProf && (MPI_TraceLevel || system == 0));

   retval = PMPI_Type_free(datatype);
   return  retval;
}



int  MPI_Get_elements(MPI_Status  *status, MPI_Datatype  datatype,
                      int  *count)
{
   int   retval;
   int   system, trace;

   system = (RTS_Call_MPI || DVMCallLevel);
   trace  = (MPI_BotsulaProf && (MPI_TraceLevel || system == 0));

   retval = PMPI_Get_elements(status, datatype, count);
   return  retval;
}



int  MPI_Op_create(MPI_User_function  *function, int  commute,
                   MPI_Op  *op)
{
   int   retval;
   int   system, trace;

   system = (RTS_Call_MPI || DVMCallLevel);
   trace  = (MPI_BotsulaProf && (MPI_TraceLevel || system == 0));

   retval = PMPI_Op_create(function, commute, op);
   return  retval;
}





int  MPI_Op_free(MPI_Op  *op)
{
   int   retval;
   int   system, trace;

   system = (RTS_Call_MPI || DVMCallLevel);
   trace  = (MPI_BotsulaProf && (MPI_TraceLevel || system == 0));

   retval = PMPI_Op_free(op);
   return  retval;
}



int  MPI_Group_size(MPI_Group  group, int  *size)
{
   int   retval;
   int   system, trace;

   system = (RTS_Call_MPI || DVMCallLevel);
   trace  = (MPI_BotsulaProf && (MPI_TraceLevel || system == 0));

   retval = PMPI_Group_size(group, size);
   return  retval;
}



int  MPI_Group_rank(MPI_Group  group, int  *rank)
{
   int   retval;
   int   system, trace;

   system = (RTS_Call_MPI || DVMCallLevel);
   trace  = (MPI_BotsulaProf && (MPI_TraceLevel || system == 0));

   retval = PMPI_Group_rank(group, rank);
   return  retval;
}



int  MPI_Group_translate_ranks(MPI_Group  group1, int  n, int  *ranks1,
                               MPI_Group  group2, int  *ranks2)
{
   int   retval;
   int   system, trace;

   system = (RTS_Call_MPI || DVMCallLevel);
   trace  = (MPI_BotsulaProf && (MPI_TraceLevel || system == 0));

   retval = PMPI_Group_translate_ranks(group1, n, ranks1,
                                       group2, ranks2);
   return  retval;
}



int  MPI_Group_compare(MPI_Group  group1, MPI_Group  group2,
                       int  *result)
{
   int   retval;
   int   system, trace;

   system = (RTS_Call_MPI || DVMCallLevel);
   trace  = (MPI_BotsulaProf && (MPI_TraceLevel || system == 0));

   retval = PMPI_Group_compare(group1, group2, result);
   return  retval;
}



int  MPI_Group_union(MPI_Group  group1, MPI_Group  group2,
                     MPI_Group  *newgroup)
{
   int   retval;
   int   system, trace;

   system = (RTS_Call_MPI || DVMCallLevel);
   trace  = (MPI_BotsulaProf && (MPI_TraceLevel || system == 0));

   retval = PMPI_Group_union(group1, group2, newgroup);
   return  retval;
}



int  MPI_Group_intersection(MPI_Group  group1, MPI_Group  group2,
                            MPI_Group  *newgroup)
{
   int   retval;
   int   system, trace;

   system = (RTS_Call_MPI || DVMCallLevel);
   trace  = (MPI_BotsulaProf && (MPI_TraceLevel || system == 0));

   retval = PMPI_Group_intersection(group1, group2, newgroup);
   return  retval;
}



int  MPI_Group_difference(MPI_Group  group1, MPI_Group  group2,
                          MPI_Group  *newgroup)
{
   int   retval;
   int   system, trace;

   system = (RTS_Call_MPI || DVMCallLevel);
   trace  = (MPI_BotsulaProf && (MPI_TraceLevel || system == 0));

   retval = PMPI_Group_difference(group1, group2, newgroup);
   return  retval;
}



int  MPI_Group_incl(MPI_Group  group, int  n, int  *ranks,
                    MPI_Group  *newgroup)
{
   int   retval;
   int   system, trace;

   system = (RTS_Call_MPI || DVMCallLevel);
   trace  = (MPI_BotsulaProf && (MPI_TraceLevel || system == 0));

   retval = PMPI_Group_incl(group, n, ranks, newgroup);
   return  retval;
}



int  MPI_Group_excl(MPI_Group  group, int  n, int  *ranks,
                    MPI_Group  *newgroup)
{
   int   retval;
   int   system, trace;

   system = (RTS_Call_MPI || DVMCallLevel);
   trace  = (MPI_BotsulaProf && (MPI_TraceLevel || system == 0));

   retval = PMPI_Group_excl(group, n, ranks, newgroup);
   return  retval;
}



int  MPI_Group_range_incl(MPI_Group  group, int  n, int  ranges[][3],
                          MPI_Group  *newgroup)
{
   int   retval;
   int   system, trace;

   system = (RTS_Call_MPI || DVMCallLevel);
   trace  = (MPI_BotsulaProf && (MPI_TraceLevel || system == 0));

   retval = PMPI_Group_range_incl(group, n, ranges, newgroup);
   return  retval;
}



int  MPI_Group_range_excl(MPI_Group  group, int  n, int  ranges[][3],
                          MPI_Group  *newgroup)
{
   int   retval;
   int   system, trace;

   system = (RTS_Call_MPI || DVMCallLevel);
   trace  = (MPI_BotsulaProf && (MPI_TraceLevel || system == 0));

   retval = PMPI_Group_range_excl(group, n, ranges, newgroup);
   return  retval;
}



int  MPI_Group_free(MPI_Group  *group)
{
   int   retval;
   int   system, trace;

   system = (RTS_Call_MPI || DVMCallLevel);
   trace  = (MPI_BotsulaProf && (MPI_TraceLevel || system == 0));

   retval = PMPI_Group_free(group);
   return  retval;
}



int  MPI_Keyval_create(MPI_Copy_function  *copy_fn,
                       MPI_Delete_function  *delete_fn, 
                       int  *keyval, void  *extra_state)
{
   int   retval;
   int   system, trace;

   system = (RTS_Call_MPI || DVMCallLevel);
   trace  = (MPI_BotsulaProf && (MPI_TraceLevel || system == 0));

   retval = PMPI_Keyval_create(copy_fn, delete_fn, keyval, extra_state);
   return  retval;
}



int  MPI_Keyval_free(int  *keyval)
{
   int   retval;
   int   system, trace;

   system = (RTS_Call_MPI || DVMCallLevel);
   trace  = (MPI_BotsulaProf && (MPI_TraceLevel || system == 0));

   retval = PMPI_Keyval_free(keyval);
   return  retval;
}



int  MPI_Dims_create(int  nnodes, int  ndims, int  *dims)
{
   int   retval;
   int   system, trace;

   system = (RTS_Call_MPI || DVMCallLevel);
   trace  = (MPI_BotsulaProf && (MPI_TraceLevel || system == 0));

   retval = PMPI_Dims_create(nnodes, ndims, dims);
   return  retval;
}



int  MPI_Get_processor_name(char  *name, int  *result_len)
{
   int   retval;
   int   system, trace;

   system = (RTS_Call_MPI || DVMCallLevel);
   trace  = (MPI_BotsulaProf && (MPI_TraceLevel || system == 0));

   retval = PMPI_Get_processor_name(name, result_len);
   return  retval;
}



#ifdef _DVM_MPI2_

int  MPI_Get_version(int  *version, int  *subversion)
{
   int   retval;
   int   system, trace;

   system = (RTS_Call_MPI || DVMCallLevel);
   trace  = (MPI_BotsulaProf && (MPI_TraceLevel || system == 0));

   retval = PMPI_Get_version(version, subversion);
   return  retval;
}

#endif



int  MPI_Errhandler_create(MPI_Handler_function  *function, 
                           MPI_Errhandler  *errhandler)
{
   int   retval;
   int   system, trace;

   system = (RTS_Call_MPI || DVMCallLevel);
   trace  = (MPI_BotsulaProf && (MPI_TraceLevel || system == 0));

   retval = PMPI_Errhandler_create(function, errhandler);
   return  retval;
}



int  MPI_Errhandler_free(MPI_Errhandler  *errhandler)
{
   int   retval;
   int   system, trace;

   system = (RTS_Call_MPI || DVMCallLevel);
   trace  = (MPI_BotsulaProf && (MPI_TraceLevel || system == 0));

   retval = PMPI_Errhandler_free(errhandler);
   return  retval;
}



int  MPI_Error_string(int  errorcode, char  *string, int  *result_len)
{
   int   retval;
   int   system, trace;

   system = (RTS_Call_MPI || DVMCallLevel);
   trace  = (MPI_BotsulaProf && (MPI_TraceLevel || system == 0));

   retval = PMPI_Error_string(errorcode, string, result_len);
   return  retval;
}



int  MPI_Error_class(int  errorcode, int  *errorclass)
{
   int   retval;
   int   system, trace;

   system = (RTS_Call_MPI || DVMCallLevel);
   trace  = (MPI_BotsulaProf && (MPI_TraceLevel || system == 0));

   retval = PMPI_Error_class(errorcode, errorclass);
   return  retval;
}



int  MPI_Pcontrol(const int  level, ... )
{
   int       retval;
   int       system, trace;
   va_list   argptr;

   system = (RTS_Call_MPI || DVMCallLevel);
   trace  = (MPI_BotsulaProf && (MPI_TraceLevel || system == 0));

   va_start(argptr, level);
   retval = PMPI_Pcontrol(level, argptr);
   va_end(argptr);

   return  retval;
}



/* misc2 (MPI2) */    /*E0085*/


#ifdef _DVM_MPI2_

/* */    /*E0086*/

DVMUSERFUN
int  PMPI_Status_f2c(MPI_Fint  *f_status, MPI_Status  *c_status);
DVMUSERFUN
int  PMPI_Status_c2f(MPI_Status  *c_status, MPI_Fint  *f_status);
DVMUSERFUN
int  PMPI_Type_create_indexed_block(int  count, int  blocklength,
                                    int  *array_of_displacements,
                                    MPI_Datatype  oldtype,
                                    MPI_Datatype  *newtype);

/* */    /*E0087*/

/*
int  MPI_Status_f2c(MPI_Fint  *f_status, MPI_Status  *c_status)
{
   int   retval;
   int   system, trace;

   system = (RTS_Call_MPI || DVMCallLevel);
   trace  = (MPI_BotsulaProf && (MPI_TraceLevel || system == 0));

   retval = PMPI_Status_f2c(f_status, c_status);
   return  retval;
}
*/    /*E0088*/



int  MPI_Status_c2f(MPI_Status  *c_status, MPI_Fint  *f_status)
{
   int   retval;
   int   system, trace;

   system = (RTS_Call_MPI || DVMCallLevel);
   trace  = (MPI_BotsulaProf && (MPI_TraceLevel || system == 0));

   retval = PMPI_Status_c2f(c_status, f_status);
   return  retval;
}



int  MPI_Type_create_indexed_block(int  count, int  blocklength,
                                   int  *array_of_displacements,
                                   MPI_Datatype  oldtype,
                                   MPI_Datatype  *newtype)
{
   int   retval;
   int   system, trace;

   system = (RTS_Call_MPI || DVMCallLevel);
   trace  = (MPI_BotsulaProf && (MPI_TraceLevel || system == 0));

   retval = PMPI_Type_create_indexed_block(count, blocklength,
                                           array_of_displacements,
                                           oldtype, newtype);
   return  retval;
}



int  MPI_Type_get_envelope(MPI_Datatype  datatype, int  *num_integers,
                           int  *num_addresses, int  *num_datatypes,
                           int  *combiner)
{
   int   retval;
   int   system, trace;

   system = (RTS_Call_MPI || DVMCallLevel);
   trace  = (MPI_BotsulaProf && (MPI_TraceLevel || system == 0));

   retval = PMPI_Type_get_envelope(datatype, num_integers, num_addresses,
                                   num_datatypes, combiner);
   return  retval;
}



int  MPI_Type_get_contents(MPI_Datatype  datatype, int  max_integers,
                           int  max_addresses, int  max_datatypes,
                           int  *array_of_integers,
                           MPI_Aint  *array_of_addresses,
                           MPI_Datatype  *array_of_datatypes)
{
   int   retval;
   int   system, trace;

   system = (RTS_Call_MPI || DVMCallLevel);
   trace  = (MPI_BotsulaProf && (MPI_TraceLevel || system == 0));

   retval = PMPI_Type_get_contents(datatype, max_integers, max_addresses,
                                   max_datatypes, array_of_integers,
                                   array_of_addresses,
                                   array_of_datatypes);
   return  retval;
}



int  MPI_Type_create_subarray(int  ndims, int  *array_of_sizes,
                              int  *array_of_subsizes,
                              int  *array_of_starts, int  order,
                              MPI_Datatype  oldtype,
                              MPI_Datatype  *newtype)
{
   int   retval;
   int   system, trace;

   system = (RTS_Call_MPI || DVMCallLevel);
   trace  = (MPI_BotsulaProf && (MPI_TraceLevel || system == 0));

   retval = PMPI_Type_create_subarray(ndims, array_of_sizes,
                                      array_of_subsizes, array_of_starts,
                                      order, oldtype, newtype);
   return  retval;
}



int  MPI_Type_create_darray(int  size, int  rank, int  ndims,
                            int  *array_of_gsizes,
                            int  *array_of_distribs,
                            int  *array_of_dargs, int  *array_of_psizes,
                            int  order, MPI_Datatype  oldtype,
                            MPI_Datatype  *newtype)
{
   int   retval;
   int   system, trace;

   system = (RTS_Call_MPI || DVMCallLevel);
   trace  = (MPI_BotsulaProf && (MPI_TraceLevel || system == 0));

   retval = PMPI_Type_create_darray(size, rank, ndims, array_of_gsizes,
                                    array_of_distribs, array_of_dargs,
                                    array_of_psizes, order, oldtype,
                                    newtype);
   return  retval;
}



int  MPI_Info_create(MPI_Info  *info)
{
   int   retval;
   int   system, trace;

   system = (RTS_Call_MPI || DVMCallLevel);
   trace  = (MPI_BotsulaProf && (MPI_TraceLevel || system == 0));

   retval = PMPI_Info_create(info);
   return  retval;
}



int  MPI_Info_set(MPI_Info  info, char  *key, char  *value)
{
   int   retval;
   int   system, trace;

   system = (RTS_Call_MPI || DVMCallLevel);
   trace  = (MPI_BotsulaProf && (MPI_TraceLevel || system == 0));

   retval = PMPI_Info_set(info, key, value);
   return  retval;
}



int  MPI_Info_delete(MPI_Info  info, char  *key)
{
   int   retval;
   int   system, trace;

   system = (RTS_Call_MPI || DVMCallLevel);
   trace  = (MPI_BotsulaProf && (MPI_TraceLevel || system == 0));

   retval = PMPI_Info_delete(info, key);
   return  retval;
}



int  MPI_Info_get(MPI_Info  info, char  *key, int  valuelen,
                  char  *value, int  *flag)
{
   int   retval;
   int   system, trace;

   system = (RTS_Call_MPI || DVMCallLevel);
   trace  = (MPI_BotsulaProf && (MPI_TraceLevel || system == 0));

   retval = PMPI_Info_get(info, key, valuelen, value, flag);
   return  retval;
}



int  MPI_Info_get_valuelen(MPI_Info  info, char  *key, int  *valuelen,
                           int  *flag)
{
   int   retval;
   int   system, trace;

   system = (RTS_Call_MPI || DVMCallLevel);
   trace  = (MPI_BotsulaProf && (MPI_TraceLevel || system == 0));

   retval = PMPI_Info_get_valuelen(info, key, valuelen, flag);
   return  retval;
}



int  MPI_Info_get_nkeys(MPI_Info  info, int  *nkeys)
{
   int   retval;
   int   system, trace;

   system = (RTS_Call_MPI || DVMCallLevel);
   trace  = (MPI_BotsulaProf && (MPI_TraceLevel || system == 0));

   retval = PMPI_Info_get_nkeys(info, nkeys);
   return  retval;
}



int  MPI_Info_get_nthkey(MPI_Info  info, int  n, char  *key)
{
   int   retval;
   int   system, trace;

   system = (RTS_Call_MPI || DVMCallLevel);
   trace  = (MPI_BotsulaProf && (MPI_TraceLevel || system == 0));

   retval = PMPI_Info_get_nthkey(info, n, key);
   return  retval;
}



int  MPI_Info_dup(MPI_Info  info, MPI_Info  *newinfo)
{
   int   retval;
   int   system, trace;

   system = (RTS_Call_MPI || DVMCallLevel);
   trace  = (MPI_BotsulaProf && (MPI_TraceLevel || system == 0));

   retval = PMPI_Info_dup(info, newinfo);
   return  retval;
}



int  MPI_Info_free(MPI_Info  *info)
{
   int   retval;
   int   system, trace;

   system = (RTS_Call_MPI || DVMCallLevel);
   trace  = (MPI_BotsulaProf && (MPI_TraceLevel || system == 0));

   retval = PMPI_Info_free(info);
   return  retval;
}



MPI_Fint  MPI_Info_c2f(MPI_Info  info)
{
   MPI_Fint   retval;
   int        system, trace;

   system = (RTS_Call_MPI || DVMCallLevel);
   trace  = (MPI_BotsulaProf && (MPI_TraceLevel || system == 0));

   retval = PMPI_Info_c2f(info);
   return  retval;
}



MPI_Info  MPI_Info_f2c(MPI_Fint  info)
{
   MPI_Info   retval;
   int        system, trace;

   system = (RTS_Call_MPI || DVMCallLevel);
   trace  = (MPI_BotsulaProf && (MPI_TraceLevel || system == 0));

   retval = PMPI_Info_f2c(info);
   return  retval;
}



/* */    /*E0089*/

DVMUSERFUN
MPI_Fint  PMPI_Request_c2f(MPI_Request  request);

MPI_Fint  MPI_Request_c2f(MPI_Request  request)
{
   MPI_Fint   retval;
   int        system, trace;

   system = (RTS_Call_MPI || DVMCallLevel);
   trace  = (MPI_BotsulaProf && (MPI_TraceLevel || system == 0));

   retval = PMPI_Request_c2f(request);
   return  retval;
}


/* external */    /*E0090*/


int  MPI_Status_set_cancelled(MPI_Status  *status, int  flag)
{
   int   retval;
   int   system, trace;

   system = (RTS_Call_MPI || DVMCallLevel);
   trace  = (MPI_BotsulaProf && (MPI_TraceLevel || system == 0));

   retval = PMPI_Status_set_cancelled(status, flag);
   return  retval;
}



int  MPI_Status_set_elements(MPI_Status  *status, MPI_Datatype  datatype,
                             int  count)
{
   int   retval;
   int   system, trace;

   system = (RTS_Call_MPI || DVMCallLevel);
   trace  = (MPI_BotsulaProf && (MPI_TraceLevel || system == 0));

   retval = PMPI_Status_set_elements(status, datatype, count);
   return  retval;
}

#endif

#endif
#endif


#endif  /* _MPITDS_C_ */    /*E0091*/
