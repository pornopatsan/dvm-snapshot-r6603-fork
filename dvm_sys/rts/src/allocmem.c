#ifndef _ALLOCMEM_C_
#define _ALLOCMEM_C_
/******************/    /*E0000*/

void  *rtl_calloc(uLLng  n, uLLng  size, byte  noerr)
{ word    i;
  void   *ptr;
  char   *PTR;
  DvmType    count1, count2;
  uLLng    N, Count;

  N = n + SizeDelta[(n*size) & Msk3]/size + 1;
  N += sizeof(double)/size + 1;

  if(SaveAllocMem == 0 || AllocBuffer == NULL)
  {  SYSTEM_RET(ptr, calloc,( (size_t)N, (size_t)size ))
     if(ptr || noerr)
        return ptr;
     pprintf(2+MultiProcErrReg2,"*** RTS err 200.000: no memory\n");
     epprintf(MultiProcErrReg2,__FILE__,__LINE__,
              "(calloc parameters: n=%ld; size=%ld)\n", n, size);
  }

  Count = 2*BoundSize + N*size;

  if(n == 0)
     epprintf(MultiProcErrReg2,__FILE__,__LINE__,
          "*** RTS err 200.001: invalid calloc parameters\n"
          "(n=%ld; size=%ld)\n", n, size);

  SYSTEM_RET(ptr, calloc, ( (size_t)Count, 1 ))

  if(ptr == NULL)
  {  if(noerr)
        return ptr;
     pprintf(2+MultiProcErrReg2,"*** RTS err 200.002: no memory\n");
     pprintf(2+MultiProcErrReg2,
             "(calloc parameters: n=%ld; size=%ld;\n", n, size);
     check_count(&count1, &count2);
     epprintf(MultiProcErrReg2,__FILE__,__LINE__,
              "allocate counter=%ld; allocate memory=%ld)\n",
              count1, count2);
  }

  check_alloc(ptr, N*size);
  PTR=(char *)ptr;

  for(i=0; i < BoundSize; i++)
      PTR[i] = BoundCode;
  for(i=(word)(BoundSize+N*size); i < Count; i++)
      PTR[i] = BoundCode;

  return (void *)(PTR+BoundSize);
}



void  *rtl_malloc(uLLng  n, byte noerr)
{ word     i;
  void    *ptr;
  char    *PTR;
  DvmType     count1, count2;
  uLLng     N, Count;

  N = n + sizeof(double) + SizeDelta[n & Msk3];

  if(SaveAllocMem == 0 || AllocBuffer == NULL)
  {  SYSTEM_RET(ptr, malloc,( (size_t)N ))
     if(ptr || noerr)
        return ptr;
     pprintf(2+MultiProcErrReg2,"*** RTS err 200.003: no memory\n");
     epprintf(MultiProcErrReg2,__FILE__,__LINE__,
              "(malloc parameter n=%ld)\n", n);
  }

  Count = N + 2*BoundSize;

  if(n == 0)
     epprintf(MultiProcErrReg2,__FILE__,__LINE__,
        "*** RTS err 200.004: invalid malloc parameter (n=%ld)\n", n);

  SYSTEM_RET(ptr, malloc, ( (size_t)Count ))

  if(ptr == NULL)
  {  if(noerr)
        return ptr;
     pprintf(2+MultiProcErrReg2,"*** RTS err 200.005: no memory\n");
     pprintf(2+MultiProcErrReg2,"(malloc parameters: n=%ld;\n", n);
     check_count(&count1, &count2);
     epprintf(MultiProcErrReg2,__FILE__,__LINE__,
	      "allocate counter=%ld; allocate memory=%ld)\n",
	      count1, count2);
  }

  check_alloc(ptr, N);

  PTR = (char *)ptr;

  for(i=0; i < BoundSize; i++)
      PTR[i] = BoundCode;

  for(i=(word)(BoundSize+N); i < Count; i++)
      PTR[i] = BoundCode;

  return (void *)(PTR+BoundSize);
}



void  rtl_free(void **ptr)
{ void  *PTR;

  if(*ptr == NULL)
      return;
  if(SaveAllocMem == 0 || AllocBuffer == NULL)
     SYSTEM(free, (*ptr))
  else
  {  PTR = (void *)((char *)*ptr-BoundSize);
     check_free(PTR);
     SYSTEM(free, (PTR))
  }

  *ptr = NULL;

  return;
}



void  *rtl_realloc(void  *ptr, uLLng  size, byte  noerr)
{ word      i;
  void     *PTR;
  char     *pointer;
  DvmType      count1, count2;
  uLLng      SIZE, Count;

  SIZE = size + sizeof(double) + SizeDelta[size & Msk3];

  if(SaveAllocMem == 0 || AllocBuffer == NULL)
  {  SYSTEM_RET(PTR, realloc, ( ptr,(size_t)SIZE ))
     if(PTR || noerr)
        return PTR;
      pprintf(2+MultiProcErrReg2,"*** RTS err 200.006: no memory\n");
      epprintf(MultiProcErrReg2,__FILE__,__LINE__,
               "(realloc parameters: ptr=%p(%lx); size=%ld)\n",
               ptr, (UDvmType)ptr, size);
  }

  Count = SIZE + 2*BoundSize;  

  if(size == 0)
     epprintf(MultiProcErrReg2,__FILE__,__LINE__,
            "*** RTS err 200.007: invalid realloc parameters\n"
            "(ptr=%p(%lx); size=%ld)\n", ptr, (UDvmType)ptr, size);

  PTR = (void *)((char *)ptr-BoundSize);
  check_free(PTR);
  SYSTEM_RET(PTR, realloc, ( PTR,(size_t)Count ))

  if(PTR == NULL)
  { if(noerr)
       return PTR;
    pprintf(2+MultiProcErrReg2,"*** RTS err 200.008: no memory\n");
    pprintf(2+MultiProcErrReg2,
            "(realloc parameters: ptr=%p(%lx); size=%ld;\n",
            ptr, (UDvmType)ptr, size);
    check_count(&count1, &count2);
    epprintf(MultiProcErrReg2,__FILE__,__LINE__,
             "allocate counter=%ld; allocate memory=%ld)\n",
             count1, count2);
  }

  check_alloc(PTR, SIZE);
  pointer = (char *)PTR;

  for(i=0; i < BoundSize; i++)
      pointer[i] = BoundCode;

  for(i=(word)(size+BoundSize); i < Count; i++)
      pointer[i] = BoundCode;

  return (void *)(pointer+BoundSize);
}


/*:::::::::::::::::::::::::::::::::::::::::::::::::::::*/    /*E0001*/


void  check_count(DvmType *count1, DvmType *count2)
{ int  i;

  *count1 = 0;
  *count2 = 0;

  for(i=0; i < AllocBufSize; i++)
      if(AllocBuffer[i].ptr != (byte *)NULL)
      { *count1 += 1;
        *count2 += AllocBuffer[i].size;
      }

  return;
}  



int  check_bound(byte  *ptr, uLLng  n)
{ word   i;

  for(i=0; i < BoundSize; i++)
      if(ptr[i] != BoundCode)
         return 1;

  for(i=(word)(BoundSize+n); i < 2*BoundSize+n; i++)
     if(ptr[i] != BoundCode)
        return 1;

  return 0;
}



void  check_buf_bound(void)
{ int    i, j, k;
  char  *buf;
  void  *VoidPtr;
 
  for(i=0; i < AllocBufSize; i++)
  {  if(AllocBuffer[i].ptr == (byte *)NULL)
        continue;
     if(check_bound(AllocBuffer[i].ptr, AllocBuffer[i].size) == 0)
        continue;
     
     pprintf(3,"*** RTS err 230.000: wrong boundary\n"
               "addr=%p(%lx)",
               AllocBuffer[i].ptr,(uLLng)AllocBuffer[i].ptr);
     pprintf(3,"boundary code =  %2.2x\n",(unsigned int)BoundCode);

     SYSTEM_RET(VoidPtr, malloc,(64+4*BoundSize))
     buf = (char *)VoidPtr;

     if(buf)
     {  SYSTEM(strcpy, (buf,"left boundary =  "))
        for(j=0,k=13; j < BoundSize; j++,k+=3)
            SYSTEM(sprintf,(&buf[k],
                            "%2.2x ",(byte)(AllocBuffer[i].ptr)[j]))
        buf[k] = '\x00';
        pprintf(3,"%s\n",buf);

        SYSTEM(strcpy, (buf,"right boundary = "))
        for(j=(int)(BoundSize+AllocBuffer[i].size),k=13;
            j < 2*BoundSize+AllocBuffer[i].size; j++,k+=3)
            SYSTEM(sprintf,(&buf[k],
                            "%2.2x ",(byte)(AllocBuffer[i].ptr)[j]))
        buf[k] = '\x00';
        pprintf(3,"%s\n",buf);
     }


     RTS_Call_MPI = 1;

#ifdef _MPI_PROF_TRAN_

     if(1 /*CallDbgCond*/    /*E0002*/ /*EnableTrace && dvm_OneProcSign*/    /*E0003*/)
        SYSTEM(MPI_Finalize, ())
     else
        dvm_exit(1);

#else

     dvm_exit(1);

#endif

  }

  return;
}



void  check_alloc(void  *ptr, uLLng  size)
{ int    i;
  DvmType   count1, count2;

  for(i=0; i < AllocBufSize; i++)
      if(AllocBuffer[i].ptr == (byte *)NULL)
      {  AllocBuffer[i].ptr = (byte *)ptr;
         AllocBuffer[i].size = size;
         break;
      }

  if(i == AllocBufSize)
  {  pprintf(2+MultiProcErrReg2,
             "*** RTS err 230.001: alloc buffer is full\n");
     check_count(&count1, &count2);
     epprintf(MultiProcErrReg2,__FILE__,__LINE__,
              "allocate counter=%ld  allocate memory=%ld\n",
              count1,count2);
  }

   if(!CheckPtr)
      return;   /* pointer value control is on */    /*E0004*/
  if((uLLng)ptr < MinPtr || (uLLng)ptr > MaxPtr)
     epprintf(MultiProcErrReg2,__FILE__,__LINE__,
              "*** RTS err 230.002: invalid alloc ptr=%p(%lx);\n",
              ptr, (uLLng)ptr);

  return;
}



void  check_free(void *ptr)
{ int   i;

  if(CheckFreeMem != 0)
     check_buf_bound();

  for(i=0; i < AllocBufSize; i++)
  {  if(AllocBuffer[i].ptr == (byte *)NULL)
        continue;

     if(AllocBuffer[i].ptr == (byte *)ptr)
     {  AllocBuffer[i].ptr = (byte *)NULL;
        AllocBuffer[i].size = 0;
        break;  
     }
  }

  if(i == AllocBufSize || CheckPtr &&
                          ((uLLng)ptr < MinPtr || (uLLng)ptr > MaxPtr))
  { pprintf(2+MultiProcErrReg2,"BufInd=%d; AllocBufSize=%d;\n",
            i, AllocBufSize);  
    epprintf(MultiProcErrReg2,__FILE__,__LINE__,
             "*** RTS err 230.003: invalid free ptr=%p(%lx);\n",
             ptr, (uLLng)ptr);
  } 

  return;
}



void  dvm_CheckPtr(char  *ptr, uLLng  length)
{
   if(!CheckPtr)
      return;   /*  pointer value control is off */    /*E0005*/
   if((uLLng)ptr >= MinPtr && (uLLng)ptr <= MaxPtr &&
      ((uLLng)ptr+length-1) >= MinPtr &&
      ((uLLng)ptr+length-1) <= MaxPtr)
      return;

   epprintf(MultiProcErrReg2,__FILE__,__LINE__,
           "*** RTS err 230.004: invalid ptr\n"
           "ptr=%lx length=%lx\n",
           (uLLng)ptr, length);
}


#endif  /* _ALLOCMEM_C_ */    /*E0006*/
