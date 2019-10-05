#ifndef _RTL_FUN_C_
#define _RTL_FUN_C_
/*****************/    /*E0000*/


/***********************************************************\
* Polling internal sizes of functional allocated processors *
\***********************************************************/    /*E0001*/
 
int rtl_GetCurrentProc(void)
{
  DVMFTimeStart(Event_rtl_GetCurrentProc);

  if(RTL_TRACE)
     dvm_trace(Event_rtl_GetCurrentProc,
               "ProcNumb=%d(%d); ProcId=%ld;\n",
                MPS_CurrentProc,CurrentProcNumber,
                CurrentProcIdent);

  DVMFTimeFinish(Event_rtl_GetCurrentProc);
  return  (DVM_RET, MPS_CurrentProc);
}



int  rtl_GetIOProc(void)
{
  DVMFTimeStart(Event_rtl_GetIOProc);

  if(RTL_TRACE)
     dvm_trace(Event_rtl_GetIOProc,
               "ProcNumb=%d(%d); ProcId=%d\n",
                DVM_IOProc, ProcNumberList[DVM_IOProc],
                ProcIdentList[DVM_IOProc]);

  DVMFTimeFinish(Event_rtl_GetIOProc);
  return  (DVM_RET, DVM_IOProc);
}



int rtl_GetMasterProc(void)
{
  DVMFTimeStart(Event_rtl_GetMasterProc);

  if(RTL_TRACE)
     dvm_trace(Event_rtl_GetMasterProc,"ProcNumb=%d(%d); ProcId=%d;\n",
                                        DVM_MasterProc,
                                        ProcNumberList[DVM_MasterProc],
                                        MasterProcIdent);

  DVMFTimeFinish(Event_rtl_GetMasterProc);
  return  (DVM_RET, DVM_MasterProc);
}



int rtl_GetCentralProc(void)
{
  DVMFTimeStart(Event_rtl_GetCentralProc);

  if(RTL_TRACE)
     dvm_trace(Event_rtl_GetCentralProc,"ProcNumb=%d(%d); ProcId=%d;\n",
                                        DVM_CentralProc,
                                        ProcNumberList[DVM_CentralProc],
                                        ProcIdentList[DVM_CentralProc]);

  DVMFTimeFinish(Event_rtl_GetCentralProc);
  return  (DVM_RET, DVM_CentralProc);
}


/******************************************\
* Function for output to stdout and stderr *
\******************************************/    /*E0002*/

int  rtl_iprintf(char *format,...)
{  int      n = 0;
   va_list  argptr;

   if(_SysInfoPrint)   
   {  if(_SysInfoStdErr)
      {  va_start(argptr, format);
         SYSTEM_RET(n, vfprintf, (stderr, format, argptr))
         va_end(argptr);
      }

      if(_SysInfoStdOut)
      {  va_start(argptr, format);
         SYSTEM_RET(n, vfprintf, (stdout, format, argptr))
         va_end(argptr);
      }

      if(_SysInfoFile)
      {  va_start(argptr, format);
         SYSTEM_RET(n, vfprintf, (SysInfo, format, argptr))
         va_end(argptr);
      }
   }

   return  n;
}


/********************************************************\
* Output functions with prefix equal to processor number *  
\********************************************************/    /*E0003*/ 

int  rtl_printf(char *format,...)
{  int      n = 0, m = 0;
   va_list  argptr;

   if(_SysInfoPrint)   
   {  if(_SysInfoStdErr)
      {
        SYSTEM_RET(n, fprintf, (stderr,
                      "%d(%d): ", MPS_CurrentProc, CurrentProcNumber))
        va_start(argptr, format);
        SYSTEM_RET(m, vfprintf, (stderr, format, argptr))
         va_end(argptr);
      }

      if(_SysInfoStdOut)
      {
        SYSTEM_RET(n, fprintf, (stdout, "%d(%d): ", MPS_CurrentProc,
                                                    CurrentProcNumber))
        va_start(argptr, format);
        SYSTEM_RET(m, vfprintf, (stdout, format, argptr))
         va_end(argptr);
      }

      if(_SysInfoFile)
      {
        SYSTEM_RET(n, fprintf, (SysInfo,
                      "%d(%d): ", MPS_CurrentProc, CurrentProcNumber))
        va_start(argptr, format);
        SYSTEM_RET(m, vfprintf, (SysInfo, format, argptr))
         va_end(argptr);
      }
   }

   return  n+m;
}



int  rtl_mprintf(int  ProcNumber, char *format, ...)
{  va_list  argptr;
   int      n = 0, m = 0;
 
   if(MPS_CurrentProc != ProcNumber)
      return 0;

   if(_SysInfoPrint)
   {  if(_SysInfoStdErr)
      {
        SYSTEM_RET(n, fprintf, (stderr,"%d(%d): ",
                                ProcNumber, ProcNumberList[ProcNumber]))
        va_start(argptr, format);
        SYSTEM_RET(m, vfprintf, (stderr, format, argptr))
        va_end(argptr);
      }

      if(_SysInfoStdOut)
      {
        SYSTEM_RET(n, fprintf, (stdout, "%d(%d): ",
                                ProcNumber, ProcNumberList[ProcNumber]))
        va_start(argptr, format);
        SYSTEM_RET(m, vfprintf, (stdout, format, argptr))
        va_end(argptr);
      }

      if(_SysInfoFile)
      {
        SYSTEM_RET(n, fprintf, (SysInfo,"%d(%d): ",
                                ProcNumber, ProcNumberList[ProcNumber]))
        va_start(argptr, format);
        SYSTEM_RET(m, vfprintf, (SysInfo, format, argptr))
        va_end(argptr);
      }
   }

   return  n+m;
}



int  rtl_fprintf(FILE *stream, char *format, ...)
{  int      n = 0, m = 0;
   va_list  argptr;
   
   SYSTEM_RET(n, fprintf,
                 (stream,"%d(%d): ",
                  MPS_CurrentProc, CurrentProcNumber))
   va_start(argptr, format);
   SYSTEM_RET(m, vfprintf, (stream, format, argptr))
   va_end(argptr);
   return  n+m;
}



int  rtl_mfprintf(int  ProcNumber, FILE *stream, char *format, ...)
{  va_list  argptr;
   int      n = 0, m = 0;

   if(MPS_CurrentProc == ProcNumber)
   {
      SYSTEM_RET(n, fprintf, (stream,"%d(%d): ",
                              ProcNumber, ProcNumberList[ProcNumber]))
      va_start(argptr, format);
      SYSTEM_RET(m, vfprintf, (stream, format, argptr))
      va_end(argptr);
   }

   return  n+m;
}


#endif /* _RTL_FUN_C_ */    /*E0004*/
