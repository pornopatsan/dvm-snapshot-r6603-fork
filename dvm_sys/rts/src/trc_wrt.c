
#ifndef _TRC_WRT_C_
#define _TRC_WRT_C_

// includes are needed to save system name, path, etc. into traces
#ifdef _UNIX_
    #include <sys/utsname.h>
    #include <unistd.h>
#else
    #include <windows.h>
    #include <direct.h>
#endif

/*****************/

/*********************************************************************/

DvmType trc_wrt_header(TABLE *Loops, int Level, FILE *hf, int nWriteInfo)
{
    STRUCT_INFO* pLoop;
    dvm_ARRAY_INFO* pArr;
    int i, j;
    int TraceLevel;
    DvmType lSize = 0;
    size_t nLen;
    char *tstr = NULL;
    char pChar[MAX_NUMBER_LENGTH];
    #ifdef _UNIX_
        int    handle;
        struct utsname deb_utsname;
        char   *domain=NULL;
    #else
        TCHAR            HostName[MaxPathSize + 1 ];
        DWORD            HostNameLen = MaxPathSize;
        TCHAR            UserName[MaxPathSize + 1 ];
        DWORD            UNSize = MaxPathSize;
        OSVERSIONINFO    OSVer;
        char            *Plat;
        char             Plat_txt[64];
        SYSTEM_INFO      SysInfo;
    #endif

    if (Level == 0)
    {
        if (nWriteInfo)
        {
            trc_wrt_calctraceinfo();

            SYSTEM_RET(nLen, sprintf, (trc_rdwrt_buff, TraceDescriptions[DSC_FULLSIZE], Trace.Bytes));
            lSize += nLen;
            SYSTEM(fputs, (trc_rdwrt_buff, hf)); Trace.TraceMarker++;

            SYSTEM_RET(nLen, sprintf, (trc_rdwrt_buff, TraceDescriptions[DSC_STRINGCOUNT], Trace.StrCount));
            lSize += nLen;
            SYSTEM(fputs, (trc_rdwrt_buff, hf)); Trace.TraceMarker++;

            SYSTEM_RET(nLen, sprintf, (trc_rdwrt_buff, TraceDescriptions[DSC_MEMORY],
                    Trace.StrCount * Trace.tTrace.ElemSize + Trace.Iters * sizeof(DvmType)));
            lSize += nLen;
            SYSTEM(fputs, (trc_rdwrt_buff, hf)); Trace.TraceMarker++;

            lSize += 1;
            SYSTEM(fputs, ("\n", hf)); Trace.TraceMarker++;
        }
        else
        {
            int nKey;

            SYSTEM_RET(nLen, strlen, (TraceDescriptions[DSC_BEGIN_HEADER]));
            lSize += nLen;
            SYSTEM(fputs, (TraceDescriptions[DSC_BEGIN_HEADER], hf)); Trace.TraceMarker++;

            switch (TraceOptions.TraceLevel)
            {
                case level_CHECKSUM : nKey = N_CHECKSUM; break;
                case level_FULL : nKey = N_FULL; break;
                case level_MODIFY : nKey = N_MODIFY; break;
                case level_MINIMAL : nKey = N_MINIMAL; break;
                case level_NONE : nKey = N_NONE; break;
                default : nKey = N_NONE;
            }


            SYSTEM_RET(nLen, sprintf, (trc_rdwrt_buff, "%s = \"%s\"\n", KeyWords[N_TIME], Trace.TraceTime));
            lSize += nLen;
            SYSTEM(fputs, (trc_rdwrt_buff, hf)); Trace.TraceMarker++;

            SYSTEM_RET(nLen, sprintf, (trc_rdwrt_buff, "%s = \"%s\"\n", KeyWords[N_DEB_VERSION], DEBUGGER_VERSION));
            lSize += nLen;
            SYSTEM(fputs, (trc_rdwrt_buff, hf)); Trace.TraceMarker++;

       #ifndef _UNIX_
          {
             GetComputerName(HostName, &HostNameLen);
             GetUserName(UserName, &UNSize);

             GetSystemInfo(&SysInfo);

             OSVer.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);

             if(GetVersionEx(&OSVer) != 0)
             {
                switch(OSVer.dwPlatformId)
                {
                   case  VER_PLATFORM_WIN32_NT:

                         Plat = "WIN32 NT";
                         break;

                   case  VER_PLATFORM_WIN32s:

                         Plat = "WIN32s";
                         break;

                   case  VER_PLATFORM_WIN32_WINDOWS:

                         Plat = "WIN32 WINDOWS";
                         break;

                   default:

                         sprintf(Plat_txt, "%d\x00", OSVer.dwPlatformId);
                         Plat = Plat_txt;
                         break;
                }

                SYSTEM_RET(nLen, sprintf, (trc_rdwrt_buff, "%s = \"Windows system %-d.%-d, build %-d %s, platform %s\"\n",
                        KeyWords[N_OS],
                        OSVer.dwMajorVersion, OSVer.dwMinorVersion,
                        OSVer.dwBuildNumber, OSVer.szCSDVersion, Plat));
                lSize += nLen;
                SYSTEM(fputs, (trc_rdwrt_buff, hf)); Trace.TraceMarker++;

                SYSTEM_RET(nLen, sprintf, (trc_rdwrt_buff, "%s = \""
                        "OEM Id %d, proc. architecture %d, number of proc. %d, "
                        "proc. type %d, proc. level %d, proc. revision %d\"\n",
                        KeyWords[N_ARCH],
                        SysInfo.dwOemId, SysInfo.wProcessorArchitecture, SysInfo.dwNumberOfProcessors,
                        SysInfo.dwProcessorType, SysInfo.wProcessorLevel,
                        SysInfo.wProcessorRevision));
                lSize += nLen;
                SYSTEM(fputs, (trc_rdwrt_buff, hf)); Trace.TraceMarker++;

                SYSTEM_RET(nLen, sprintf, (trc_rdwrt_buff, "%s = \"%s@%s\"\n",
                        KeyWords[N_USER_HOST],
                        UserName, HostName));
                lSize += nLen;
                SYSTEM(fputs, (trc_rdwrt_buff, hf)); Trace.TraceMarker++;
             }

             SYSTEM_RET(tstr, _getcwd, (trc_filebuff1, MaxPathSize + 1));
          }
       #else

          uname(&deb_utsname);

          #ifdef __USE_GNU
             domain = deb_utsname.domainname;
          #else
             domain = deb_utsname.nodename;
          #endif

                SYSTEM_RET(nLen, sprintf, (trc_rdwrt_buff, "%s = \""
                     "%s release %s (version %s)\"\n",
                        KeyWords[N_OS], deb_utsname.sysname, deb_utsname.release,
                        deb_utsname.version ));
                lSize += nLen;
                SYSTEM(fputs, (trc_rdwrt_buff, hf)); Trace.TraceMarker++;

                SYSTEM_RET(nLen, sprintf, (trc_rdwrt_buff, "%s = \""
                        "Machine %s\"\n", KeyWords[N_ARCH], deb_utsname.machine ));
                lSize += nLen;
                SYSTEM(fputs, (trc_rdwrt_buff, hf)); Trace.TraceMarker++;

                SYSTEM_RET(nLen, sprintf, (trc_rdwrt_buff, "%s = \"%s@%s\"\n",
                        KeyWords[N_USER_HOST],
                        domain, deb_utsname.nodename ));
                lSize += nLen;
                SYSTEM(fputs, (trc_rdwrt_buff, hf)); Trace.TraceMarker++;

                SYSTEM_RET(tstr, getcwd, (trc_filebuff1, MaxPathSize + 1));
       #endif

            if ( tstr != NULL )
            {
                SYSTEM_RET(nLen, sprintf, (trc_rdwrt_buff, "%s = \"%s\"\n", KeyWords[N_WORK_DIR], tstr));
                lSize += nLen;
                SYSTEM(fputs, (trc_rdwrt_buff, hf)); Trace.TraceMarker++;
            }

            SYSTEM_RET(nLen, sprintf, (trc_rdwrt_buff, "%s = \"%s\"\n", KeyWords[N_TASK_NAME], dvm_argv[0]));
            lSize += nLen;
            SYSTEM(fputs, (trc_rdwrt_buff, hf)); Trace.TraceMarker++;

            SYSTEM_RET(nLen, sprintf, (trc_rdwrt_buff, "%s = %s\n", KeyWords[N_MODE], KeyWords[nKey]));
            lSize += nLen;
            SYSTEM(fputs, (trc_rdwrt_buff, hf)); Trace.TraceMarker++;

            SYSTEM_RET(nLen, sprintf, (trc_rdwrt_buff, "%s = %d\n", KeyWords[N_CPUCOUNT], MPS_ProcCount));
            lSize += nLen;
            SYSTEM(fputs, (trc_rdwrt_buff, hf)); Trace.TraceMarker++;

            SYSTEM_RET(nLen, sprintf, (trc_rdwrt_buff, "%s = %d\n", KeyWords[N_EMPTYITER], (int)TraceOptions.WriteEmptyIter));
            lSize += nLen;
            SYSTEM(fputs, (trc_rdwrt_buff, hf)); Trace.TraceMarker++;

            SYSTEM_RET(nLen, sprintf, (trc_rdwrt_buff, "%s = %d\n", KeyWords[N_MULTIDIM_ARRAY], (int)TraceOptions.MultidimensionalArrays));
            lSize += nLen;
            SYSTEM(fputs, (trc_rdwrt_buff, hf)); Trace.TraceMarker++;

            SYSTEM_RET(nLen, sprintf, (trc_rdwrt_buff, "%s = %d\n", KeyWords[N_DEF_ARR_STEP], TraceOptions.DefaultArrayStep));
            lSize += nLen;
            SYSTEM(fputs, (trc_rdwrt_buff, hf)); Trace.TraceMarker++;

            SYSTEM_RET(nLen, sprintf, (trc_rdwrt_buff, "%s = %d\n", KeyWords[N_DEF_ITER_STEP], TraceOptions.DefaultIterStep));
            lSize += nLen;
            SYSTEM(fputs, (trc_rdwrt_buff, hf)); Trace.TraceMarker++;

            if ( Trace.StartPoint == NULL )
            {
                pChar[0] = 0;
            }
            else
            {
                to_string(Trace.StartPoint, pChar);
            }

            SYSTEM_RET(nLen, sprintf, (trc_rdwrt_buff, "%s = %s\n", KeyWords[N_STARTPOINT], pChar));
            lSize += nLen;
            SYSTEM(fputs, (trc_rdwrt_buff, hf)); Trace.TraceMarker++;

            if ( Trace.FinishPoint == NULL )
            {
                pChar[0] = 0;
            }
            else
            {
                to_string(Trace.FinishPoint, pChar);
            }

            SYSTEM_RET(nLen, sprintf, (trc_rdwrt_buff, "%s = %s\n", KeyWords[N_FINISHPOINT], pChar));
            lSize += nLen;
            SYSTEM(fputs, (trc_rdwrt_buff, hf)); Trace.TraceMarker++;

            SYSTEM_RET(nLen, sprintf, (trc_rdwrt_buff, "%s = %d\n", KeyWords[N_IG_LEFT], TraceOptions.Ig_left));
            lSize += nLen;
            SYSTEM(fputs, (trc_rdwrt_buff, hf)); Trace.TraceMarker++;

            SYSTEM_RET(nLen, sprintf, (trc_rdwrt_buff, "%s = %d\n", KeyWords[N_IG_RIGHT], TraceOptions.Ig_right));
            lSize += nLen;
            SYSTEM(fputs, (trc_rdwrt_buff, hf)); Trace.TraceMarker++;

            SYSTEM_RET(nLen, sprintf, (trc_rdwrt_buff, "%s = %d\n", KeyWords[N_ILOC_LEFT], TraceOptions.Iloc_left));
            lSize += nLen;
            SYSTEM(fputs, (trc_rdwrt_buff, hf)); Trace.TraceMarker++;

            SYSTEM_RET(nLen, sprintf, (trc_rdwrt_buff, "%s = %d\n", KeyWords[N_ILOC_RIGHT], TraceOptions.Iloc_right));
            lSize += nLen;
            SYSTEM(fputs, (trc_rdwrt_buff, hf)); Trace.TraceMarker++;

            SYSTEM_RET(nLen, sprintf, (trc_rdwrt_buff, "%s = %d\n", KeyWords[N_IREP_LEFT], TraceOptions.Irep_left));
            lSize += nLen;
            SYSTEM(fputs, (trc_rdwrt_buff, hf)); Trace.TraceMarker++;

            SYSTEM_RET(nLen, sprintf, (trc_rdwrt_buff, "%s = %d\n", KeyWords[N_IREP_RIGHT], TraceOptions.Irep_right));
            lSize += nLen;
            SYSTEM(fputs, (trc_rdwrt_buff, hf)); Trace.TraceMarker++;

            SYSTEM_RET(nLen, sprintf, (trc_rdwrt_buff, "%s = %d\n", KeyWords[N_LOCITERWIDTH], TraceOptions.LocIterWidth));
            lSize += nLen;
            SYSTEM(fputs, (trc_rdwrt_buff, hf)); Trace.TraceMarker++;

            SYSTEM_RET(nLen, sprintf, (trc_rdwrt_buff, "%s = %d\n", KeyWords[N_REPITERWIDTH], TraceOptions.RepIterWidth));
            lSize += nLen;
            SYSTEM(fputs, (trc_rdwrt_buff, hf)); Trace.TraceMarker++;

            SYSTEM_RET(nLen, sprintf, (trc_rdwrt_buff, "%s = %d\n", KeyWords[N_CALC_CHECKSUM], TraceOptions.CalcChecksums));
            lSize += nLen;
            SYSTEM(fputs, (trc_rdwrt_buff, hf)); Trace.TraceMarker++;
        }

        if (TraceOptions.drarr)
        {

            for (i = 0; i < table_Count(&Trace.tArrays); ++i)
            {
                pArr = table_At(dvm_ARRAY_INFO, &Trace.tArrays, i);
                if (!pArr->bIsRemoteBuffer) /* It's distributed array */
                {
                    switch (pArr->eTraceLevel)
                    {
                        case level_FULL : TraceLevel = N_FULL; break;
                        case level_MODIFY : TraceLevel = N_MODIFY; break;
                        case level_MINIMAL : TraceLevel = N_MINIMAL; break;
                        case level_NONE : TraceLevel = N_NONE; break;
                        default : TraceLevel = N_EMPTY; break;
                    }
                    SYSTEM_RET(nLen, sprintf, (trc_rdwrt_buff, "%s: \"%s\" (%d) [%d] {\"%s\", %lu, %d, %d} = %s", KeyWords[N_ARRAY],
                        pArr->szOperand, (int)pArr->lElemType, (int)(pArr->bRank), pArr->szFile, pArr->ulLine, pArr->iNumber, (int)(pArr->bIsDistr),
                        KeyWords[TraceLevel]));
                    lSize += nLen;
                    SYSTEM(fputs, (trc_rdwrt_buff, hf));

                    for (j = 0; j < 1 || (j < pArr->bRank &&
                        TraceOptions.MultidimensionalArrays); ++j)
                    {
                        if (pArr->rgLimit[j].Lower != MAXLONG ||
                            pArr->rgLimit[j].Upper != MAXLONG ||
                            pArr->rgLimit[j].Step != MAXLONG )
                        {
                            SYSTEM_RET(nLen, sprintf, (trc_rdwrt_buff, ", (%d:", j));
                            lSize += nLen;
                            SYSTEM(fputs, (trc_rdwrt_buff, hf));

                            if (pArr->rgLimit[j].Lower != MAXLONG)
                            {
                                SYSTEM_RET(nLen, sprintf, (trc_rdwrt_buff, "%ld", pArr->rgLimit[j].Lower));
                                lSize += nLen;
                                SYSTEM(fputs, (trc_rdwrt_buff, hf));
                            }

                            lSize += 1;
                            SYSTEM(fputs, (",", hf));

                            if (pArr->rgLimit[j].Upper != MAXLONG)
                            {
                                SYSTEM_RET(nLen, sprintf, (trc_rdwrt_buff, "%ld", pArr->rgLimit[j].Upper));
                                lSize += nLen;
                                SYSTEM(fputs, (trc_rdwrt_buff, hf));
                            }

                            lSize += 1;
                            SYSTEM(fputs, (",", hf));

                            if (pArr->rgLimit[j].Step != MAXLONG)
                            {
                                SYSTEM_RET(nLen, sprintf, (trc_rdwrt_buff, "%ld", pArr->rgLimit[j].Step));
                                lSize += nLen;
                                SYSTEM(fputs, (trc_rdwrt_buff, hf));
                            }

                            lSize += 1;
                            SYSTEM(fputs, (")", hf));
                        }
                    }

                    if (nWriteInfo)
                    {
                        /* Write special comment with array limits */

                        lSize += 3;
                        SYSTEM(fputs, (" # ", hf));
                        if (TraceOptions.MultidimensionalArrays)
                        {
                            for (j = 0; j < pArr->bRank; ++j)
                            {
                                SYSTEM_RET(nLen, sprintf, (trc_rdwrt_buff, ", (%d:0,%ld,1)", j, pArr->rgSize[j] - 1));
                                lSize += nLen;
                                SYSTEM(fputs, (trc_rdwrt_buff, hf));
                            }
                        }
                        else
                        {
                            DvmType lFullSize = 1;
                            for (j = 0; j < pArr->bRank; ++j)
                            {
                                lFullSize *= pArr->rgSize[j];
                            }
                            SYSTEM_RET(nLen, sprintf, (trc_rdwrt_buff, ", (%d:0,%ld,1)", 0, lFullSize - 1));
                            lSize += nLen;
                            SYSTEM(fputs, (trc_rdwrt_buff, hf));
                        }
                        lSize += 1;
                        SYSTEM(fputs, ("\n", hf)); Trace.TraceMarker++;

                        if ( TraceOptions.TraceLevel != level_CHECKSUM )
                        {
                            SYSTEM_RET(nLen, sprintf, (trc_rdwrt_buff, TraceDescriptions[DSC_READ], pArr->ulRead));
                            lSize += nLen;
                            SYSTEM(fputs, (trc_rdwrt_buff, hf)); Trace.TraceMarker++;

                            SYSTEM_RET(nLen, sprintf, (trc_rdwrt_buff, TraceDescriptions[DSC_WRITE], pArr->ulWrite));
                            lSize += nLen;
                            SYSTEM(fputs, (trc_rdwrt_buff, hf)); Trace.TraceMarker++;
                        }

                        SYSTEM_RET(nLen, sprintf, (trc_rdwrt_buff, TraceDescriptions[DSC_MEMORY],
                            (pArr->ulRead + pArr->ulWrite) * Trace.tTrace.ElemSize +
                                sizeof(dvm_ARRAY_INFO)));
                        lSize += nLen;
                        SYSTEM(fputs, (trc_rdwrt_buff, hf)); Trace.TraceMarker++;
                        lSize += 1;
                        SYSTEM(fputs, ("\n", hf)); Trace.TraceMarker++;
                    }
                    else
                    {
                        lSize += 1;
                        SYSTEM(fputs, ("\n", hf)); Trace.TraceMarker++;
                    }
                }
            }
        }
    }

    for( i = 0; i < table_Count(Loops); i++ )
    {
        int nKey;
        pLoop = table_At(STRUCT_INFO, Loops, i);

        /* L: <No>( <Parent> ) [<Rank>] {<File>, <Line>} = Level, (dim:min,max,step), ... */
        lSize += trc_wrt_indent( hf, Level );
        switch (pLoop->Type)
        {
            case 0 : nKey = N_SLOOP; break;
            case 1 : nKey = N_PLOOP; break;
            case 2 : nKey = N_TASKREGION; break;
            default : nKey = N_SLOOP;
        }
        SYSTEM_RET(nLen, sprintf, (trc_rdwrt_buff, "%s: %d(", KeyWords[nKey], (int)(pLoop->No)));
        lSize += nLen;
        SYSTEM(fputs, (trc_rdwrt_buff, hf));

        if( pLoop->pParent )
        {
            SYSTEM_RET(nLen, sprintf, (trc_rdwrt_buff, "%d", (int)(pLoop->pParent->No) ));
            lSize += nLen;
            SYSTEM(fputs, (trc_rdwrt_buff, hf));
        }
        switch( pLoop->TraceLevel )
        {
            case level_FULL : TraceLevel = N_FULL; break;
            case level_MODIFY : TraceLevel = N_MODIFY; break;
            case level_MINIMAL : TraceLevel = N_MINIMAL; break;
            case level_NONE : TraceLevel = N_NONE; break;
            default : TraceLevel = N_EMPTY; break;
        }
        SYSTEM_RET(nLen, sprintf, (trc_rdwrt_buff, ") [%d] {\"%s\", %lu} = %s", (int)(pLoop->Rank), pLoop->File,
                pLoop->Line, KeyWords[TraceLevel]));
        lSize += nLen;
        SYSTEM(fputs, (trc_rdwrt_buff, hf));

        for( j = 0; j < pLoop->Rank; j++ )
        {
            if( pLoop->Limit[j].Lower != MAXLONG ||
                pLoop->Limit[j].Upper != MAXLONG ||
                pLoop->Limit[j].Step != MAXLONG )
            {
                SYSTEM_RET(nLen, sprintf, (trc_rdwrt_buff, ", (%d:", j ));
                lSize += nLen;
                SYSTEM(fputs, (trc_rdwrt_buff, hf));

                if( pLoop->Limit[j].Lower != MAXLONG )
                {
                    SYSTEM_RET(nLen, sprintf, (trc_rdwrt_buff, "%ld", pLoop->Limit[j].Lower ));
                    lSize += nLen;
                    SYSTEM(fputs, (trc_rdwrt_buff, hf));
                }

                lSize += 1;
                SYSTEM(fputs, (",", hf));

                if( pLoop->Limit[j].Upper != MAXLONG )
                {
                    SYSTEM_RET(nLen, sprintf, (trc_rdwrt_buff, "%ld", pLoop->Limit[j].Upper ));
                    lSize += nLen;
                    SYSTEM(fputs, (trc_rdwrt_buff, hf));
                }

                lSize += 1;
                SYSTEM(fputs, (",", hf));

                if( pLoop->Limit[j].Step != MAXLONG )
                {
                    SYSTEM_RET(nLen, sprintf, (trc_rdwrt_buff, "%ld", pLoop->Limit[j].Step ));
                    lSize += nLen;
                    SYSTEM(fputs, (trc_rdwrt_buff, hf));
                }

                lSize += 1;
                SYSTEM(fputs, (")", hf));
            }
        }
        if( nWriteInfo )
        {
            /* Write special comment with loop variables  */

            lSize += 3;
            SYSTEM(fputs, (" # ", hf));
            for( j = 0; j < pLoop->Rank; j++ )
            {
                if( pLoop->Common[j].Lower != MAXLONG ||
                    pLoop->Common[j].Upper != MAXLONG ||
                    pLoop->Common[j].Step != MAXLONG )
                {
                    SYSTEM_RET(nLen, sprintf, (trc_rdwrt_buff, ", (%d:", j ));
                    lSize += nLen;
                    SYSTEM(fputs, (trc_rdwrt_buff, hf));

                    if( pLoop->Common[j].Lower != MAXLONG )
                    {
                        SYSTEM_RET(nLen, sprintf, (trc_rdwrt_buff, "%ld", pLoop->Common[j].Lower ));
                        lSize += nLen;
                        SYSTEM(fputs, (trc_rdwrt_buff, hf));
                    }

                    lSize += 1;
                    SYSTEM(fputs, (",", hf));

                    if( pLoop->Common[j].Upper != MAXLONG )
                    {
                        SYSTEM_RET(nLen, sprintf, (trc_rdwrt_buff, "%ld", pLoop->Common[j].Upper ));
                        lSize += nLen;
                        SYSTEM(fputs, (trc_rdwrt_buff, hf));
                    }

                    lSize += 1;
                    SYSTEM(fputs, (",", hf));

                    if( pLoop->Common[j].Step != MAXLONG )
                    {
                        SYSTEM_RET(nLen, sprintf, (trc_rdwrt_buff, "%ld", pLoop->Common[j].Step ));
                        lSize += nLen;
                        SYSTEM(fputs, (trc_rdwrt_buff, hf));
                    }

                    lSize += 1;
                    SYSTEM(fputs, (")", hf));
                }
            }
            lSize += 1;
            SYSTEM(fputs, ("\n", hf)); Trace.TraceMarker++;

            lSize += trc_wrt_indent(hf, Level + 1);
            SYSTEM_RET(nLen, sprintf, (trc_rdwrt_buff, TraceDescriptions[DSC_FULLSIZE], pLoop->Bytes));
            lSize += nLen;
            SYSTEM(fputs, (trc_rdwrt_buff, hf)); Trace.TraceMarker++;

            lSize += trc_wrt_indent(hf, Level + 1);
            SYSTEM_RET(nLen, sprintf, (trc_rdwrt_buff, TraceDescriptions[DSC_STRINGCOUNT], pLoop->StrCount));
            lSize += nLen;
            SYSTEM(fputs, (trc_rdwrt_buff, hf)); Trace.TraceMarker++;

            lSize += trc_wrt_indent(hf, Level + 1);
            SYSTEM_RET(nLen, sprintf, (trc_rdwrt_buff, TraceDescriptions[DSC_ITERCOUNT], pLoop->Iters));
            lSize += nLen;
            SYSTEM(fputs, (trc_rdwrt_buff, hf)); Trace.TraceMarker++;

            lSize += trc_wrt_indent( hf, Level+1 );
            SYSTEM_RET(nLen, sprintf, (trc_rdwrt_buff, TraceDescriptions[DSC_MEMORY],
                    pLoop->StrCount * Trace.tTrace.ElemSize + pLoop->Iters * sizeof(DvmType)));
            lSize += nLen;
            SYSTEM(fputs, (trc_rdwrt_buff, hf)); Trace.TraceMarker++;
        }
        else
        {
            lSize += 1;
            SYSTEM(fputs, ("\n", hf));  Trace.TraceMarker++;
        }

        lSize += trc_wrt_header( &(pLoop->tChildren), Level + 1, hf, nWriteInfo );

        lSize += trc_wrt_indent( hf, Level );
        SYSTEM_RET(nLen, sprintf, (trc_rdwrt_buff, "%s: %d", KeyWords[N_END_LOOP], (int)(pLoop->No) ));

        /* print array access information */
        nLen += list_to_header_string(trc_rdwrt_buff+nLen, &(pLoop->ArrList));

        nLen += sprintf(trc_rdwrt_buff+nLen, "\n");

        lSize += nLen;
        SYSTEM(fputs, (trc_rdwrt_buff, hf)); Trace.TraceMarker++;
    }

    if (Level == 0)
    {
        SYSTEM_RET(nLen, strlen, (KeyWords[N_END_HEADER]));
        lSize += nLen;
        SYSTEM(fputs, (KeyWords[N_END_HEADER], hf));
        lSize += 1;
        SYSTEM(fputs, ("\n", hf)); Trace.TraceMarker++;
        if (!nWriteInfo)
        {
            SYSTEM_RET(nLen, strlen, (TraceDescriptions[DSC_END_HEADER]));
            lSize += nLen;
            SYSTEM(fputs, (TraceDescriptions[DSC_END_HEADER], hf)); Trace.TraceMarker++;
        }
    }

    //SYSTEM(fflush, (hf));

    #ifdef _UNIX_
       SYSTEM_RET(handle, fileno, (hf))
       //SYSTEM(fsync, (handle))
    #endif

    return lSize;
}

void trc_wrt_codecoverage(FILE *hf)
{
    int i, j, first;

    DBG_ASSERT(__FILE__, __LINE__, EnableCodeCoverage && !Trace.CodeCovWritten && Trace.CovInfo);

    SYSTEM(fputs, ("\n", hf));

    for (i=0; (i < MaxSourceFileCount) && (Trace.CovInfo->AccessInfoSizes[i] != 0); i++)
    {
        SYSTEM(sprintf, (trc_rdwrt_buff, "FILE=%s;\nLINES=", Trace.CovInfo->FileNames[i]));
        SYSTEM(fputs, (trc_rdwrt_buff, hf));

        first = 1;
        for (j=0; j<Trace.CovInfo->AccessInfoSizes[i]; j++)
        {
            if ( Trace.CovInfo->LinesAccessInfo[i][j] )
            {
                if ( first )
                {
                    SYSTEM(sprintf, (trc_rdwrt_buff, "%d", j+1));
                    first = 0;
                }
                else
                    SYSTEM(sprintf, (trc_rdwrt_buff, ",%d", j+1));

                SYSTEM(fputs, (trc_rdwrt_buff, hf));
            }
        }
        SYSTEM(fputs, (";\n", hf));
    }

    Trace.CodeCovWritten = 1;
}

#define MARK_RECORDS \
    if ( !(Trace.TraceMarker++ % 10) )\
    {\
        SYSTEM(sprintf, (error_message, "#\t%lu\n", Trace.TraceMarker++));\
        SYSTEM(fputs, (error_message, hf));\
    }


void trc_wrt_trace( FILE *hf, TRACE_WRITE_REASON reason )
{
    DvmType i;
    byte  *RecordType;

    Trace.Level = 0;

    Trace.TraceMarker++;
//    unfinished implementation attempt to write line counting records in trace
//    SYSTEM(sprintf, (trc_rdwrt_buff, "#\t%lu\n", Trace.TraceMarker));
//    SYSTEM(fputs, (trc_rdwrt_buff, hf));

    DvmType writingTraceSize = table_Count(&Trace.tTrace);
    if (reason == TRACE_DYN_MEMORY_LIMIT) {
        writingTraceSize = (int)(writingTraceSize * 0.95);
    }

    for( i = 0l; (i < writingTraceSize) && EnableTrace; i++ )
    {
        RecordType = table_At( byte, &Trace.tTrace, i );
        switch( *RecordType )
        {
            case trc_ITER :
                trc_wrt_iter( hf, (ITERATION *)RecordType );
                break;
            case trc_STRUCTBEG :
                trc_wrt_beginstruct( hf, (STRUCT_BEGIN *)RecordType );
                Trace.Level++;
                break;
            case trc_STRUCTEND :
                Trace.Level--;
                trc_wrt_endstruct( hf, (STRUCT_END *)RecordType );
                break;
            case trc_PREWRITEVAR :
                trc_wrt_prewritevar( hf, (VARIABLE *)RecordType );
                break;
            case trc_POSTWRITEVAR :
                trc_wrt_postwritevar( hf, (VARIABLE *)RecordType );
                break;
            case trc_READVAR :
                trc_wrt_readvar( hf, (VARIABLE *)RecordType );
                break;
            case trc_REDUCTVAR :
                trc_wrt_reductvar( hf, (VARIABLE *)RecordType );
                break;
            case trc_SKIP :
                trc_wrt_skip( hf, (SKIP *)RecordType );
                break;
            case trc_CHUNK :
                trc_wrt_chunk( hf, (CHUNK *)RecordType );
                break;
        }
    }
    if ( EnableTrace )
    {
        trc_wrt_end( hf );
    }
}

size_t trc_wrt_beginstruct( FILE *hf, STRUCT_BEGIN *loop )
{
    int TraceLevel;
    size_t Res;
    size_t Len;
    int nKey, i;
    char tmp[MAX_NUMBER_LENGTH];
    #ifdef _UNIX_
       int handle;
    #endif

    Res = Len = TraceOptions.SpaceIndent * Trace.Level;
    memset( trc_rdwrt_buff, ' ', Len );
    trc_rdwrt_buff[ Len ] = 0;

    switch (loop->pCnfgInfo->Type)
    {
        case 0 : nKey = N_SLOOP; break;
        case 1 : nKey = N_PLOOP; break;
        case 2 : nKey = N_TASKREGION; break;
        default : nKey = N_SLOOP;
    }
    SYSTEM_RET(Len, sprintf, (trc_rdwrt_buff + Res, "%s: %d(", KeyWords[nKey], (int)(loop->pCnfgInfo->No)));
    Res += Len;
    if( loop->pCnfgInfo->pParent )
    {
        SYSTEM_RET(Len, sprintf,( trc_rdwrt_buff + Res, "%d", (int)(loop->pCnfgInfo->pParent->No) ));
        Res += Len;
    }

    switch( loop->pCnfgInfo->TraceLevel )
    {
        case level_FULL : TraceLevel = N_FULL; break;
        case level_MODIFY : TraceLevel = N_MODIFY; break;
        case level_MINIMAL : TraceLevel = N_MINIMAL; break;
        case level_NONE : TraceLevel = N_NONE; break;
        default : TraceLevel = N_EMPTY; break;
    }

    SYSTEM_RET(Len, sprintf, (trc_rdwrt_buff + Res, ") [%d]", (int)(loop->pCnfgInfo->Rank)));
    Res += Len;

    if ( loop->pChunkSet )
    {
        DBG_ASSERT(__FILE__, __LINE__, loop->pChunkSet[0].Chunks);

        for( i = 0; i < loop->pChunkSet[0].Chunks[0].Rank; i++ )
        {
            SYSTEM_RET(Len, sprintf, (trc_rdwrt_buff + Res, " (%ld, %ld, %ld)",
                    loop->pChunkSet[0].Chunks[0].Set[i].Lower,
                    loop->pChunkSet[0].Chunks[0].Set[i].Upper,
                    loop->pChunkSet[0].Chunks[0].Set[i].Step ));

            Res += Len;
        }
    }

    SYSTEM_RET(Len, sprintf, (trc_rdwrt_buff + Res, "; {\"%s\", %lu}", loop->File, loop->Line));
    Res += Len;

    if ( loop->num != NULL )
    {
        SYSTEM_RET(Len, sprintf, (trc_rdwrt_buff + Res, ", %s\n", to_string(loop->num, tmp)));
        Res += Len;
    }
    else
    {
        SYSTEM_RET(Len, sprintf, (trc_rdwrt_buff + Res, "\n"));
        Res += Len;
    }

    if ( loop->pChunkSet )
    {
       if ( loop->pChunkSet[0].Size == 2 )
       {
            if ( TraceOptions.SpaceIndent * Trace.Level == 0 )
                SYSTEM_RET(Len, sprintf, (trc_rdwrt_buff + Res, "LOCAL:"))
            else
                SYSTEM_RET(Len, sprintf, (trc_rdwrt_buff + Res, "%*cLOCAL:",
                        TraceOptions.SpaceIndent * Trace.Level, ' '));
            Res += Len;

            for( i = 0; i < loop->pChunkSet[0].Chunks[1].Rank; i++ )
            {
                SYSTEM_RET(Len, sprintf, (trc_rdwrt_buff + Res, " (%ld, %ld, %ld)",
                        loop->pChunkSet[0].Chunks[1].Set[i].Lower,
                        loop->pChunkSet[0].Chunks[1].Set[i].Upper,
                        loop->pChunkSet[0].Chunks[1].Set[i].Step ));

                Res += Len;
            }

            SYSTEM_RET(Len, sprintf, (trc_rdwrt_buff + Res, "\n"));
            Res += Len;
       }
       else
       {
            DBG_ASSERT(__FILE__, __LINE__, loop->pChunkSet[0].Size == 1);

            if ( TraceOptions.SpaceIndent * Trace.Level == 0 )
                SYSTEM_RET(Len, sprintf, (trc_rdwrt_buff + Res, "LOCAL: NONE\n"))
            else
                SYSTEM_RET(Len, sprintf, (trc_rdwrt_buff + Res, "%*cLOCAL: NONE\n",
                        TraceOptions.SpaceIndent * Trace.Level, ' '));
            Res += Len;
       }
    }

    if( hf )
    {
        CurrCmpTraceFileSize += Res;
        if (CurrCmpTraceFileSize > MaxTraceFileSize)
        {
            pprintf(3, "*** RTS err: CMPTRACE: Trace file size exceeds the limit %d\n", MaxTraceFileSize);
            EnableTrace = 0;
        }

        SYSTEM( fputs,( trc_rdwrt_buff, hf ) );
        //SYSTEM( fflush, (hf) );

        #ifdef _UNIX_
           SYSTEM_RET(handle, fileno, (hf))
           //SYSTEM(fsync, (handle))
        #endif
    }

    return Res;
}

size_t trc_wrt_endstruct( FILE *hf, STRUCT_END *loop )
{
    size_t Res;
    size_t Len;
    int j, k;
    dvm_ARRAY_INFO* pArr;
    char tmp[MAX_NUMBER_LENGTH];
    #ifdef _UNIX_
       int handle;
    #endif

    Res = Len = TraceOptions.SpaceIndent * Trace.Level;
    memset( trc_rdwrt_buff, ' ', Len );
    trc_rdwrt_buff[ Len ] = 0;

    SYSTEM_RET(Len, sprintf,( trc_rdwrt_buff + Res, "%s: %d; {\"%s\", %lu}", KeyWords[N_END_LOOP], (int)(loop->pCnfgInfo->No), loop->File, loop->Line ));
    Res += Len;

    if ( loop->num != NULL )
    {
        SYSTEM_RET(Len, sprintf, (trc_rdwrt_buff + Res, ", %s\n", to_string(loop->num, tmp)));
        Res += Len;
    }
    else
    {
        SYSTEM_RET(Len, sprintf, (trc_rdwrt_buff + Res, "\n"));
        Res += Len;
    }

    for (j = 0; j < loop->csSize; j++)
    {
        for (k=0; k<TraceOptions.SpaceIndent*(Trace.Level+1); k++) trc_rdwrt_buff[Res++]=' ';

        pArr = table_At(dvm_ARRAY_INFO, &Trace.tArrays, loop->checksums[j].lArrNo);
        Res += sprintf(trc_rdwrt_buff + Res, "CS(\"%s\", \"%s\", %ld, %d, \"%s\")", pArr->szOperand,
                        pArr->szFile, pArr->ulLine, pArr->iNumber,
                        (loop->checksums[j].accType==1)?"r":((loop->checksums[j].accType==2)?"w":"rw"));
        if (loop->checksums[j].errCode == 1)
                Res += sprintf(trc_rdwrt_buff + Res, "=\"%.*G\"\n", Trace.DoublePrecision, loop->checksums[j].sum);
        else
                Res += sprintf(trc_rdwrt_buff + Res, " FAILED\n");
    }

    if( hf )
    {
        CurrCmpTraceFileSize += Res;
        if (CurrCmpTraceFileSize > MaxTraceFileSize)
        {
            pprintf(3, "*** RTS err: CMPTRACE: Trace file size exceeds the limit %d\n", MaxTraceFileSize);
            EnableTrace = 0;
        }

        SYSTEM( fputs,( trc_rdwrt_buff, hf ) );
        //SYSTEM( fflush, (hf) );

        #ifdef _UNIX_
           SYSTEM_RET(handle, fileno, (hf))
           //SYSTEM(fsync, (handle))
        #endif
    }

    return Res;
}

size_t trc_wrt_chunk(FILE *hf, CHUNK *pChunk)
{
    size_t Res;
    size_t Len;
    int i;

    #ifdef _UNIX_
       int handle;
    #endif

    Res = Len = TraceOptions.SpaceIndent * Trace.Level;
    memset( trc_rdwrt_buff, ' ', Len );
    trc_rdwrt_buff[ Len ] = 0;

    SYSTEM_RET(Len, sprintf,( trc_rdwrt_buff + Res, "%s:", KeyWords[N_CHUNK]));
    Res += Len;

    for( i = 0; i < pChunk->block.Rank; i++ )
    {
        SYSTEM_RET(Len, sprintf, (trc_rdwrt_buff + Res, " (%ld, %ld, %ld)",
                                    pChunk->block.Set[i].Lower,
                                    pChunk->block.Set[i].Upper,
                                    pChunk->block.Set[i].Step ));
        Res += Len;
    }

    SYSTEM_RET(Len, sprintf, (trc_rdwrt_buff + Res, " VTR=%d\n", pChunk->block.vtr));
    Res += Len;

    if( hf )
    {
        CurrCmpTraceFileSize += Res;
        if (CurrCmpTraceFileSize > MaxTraceFileSize)
        {
            pprintf(3, "*** RTS err: CMPTRACE: Trace file size exceeds the limit %d\n", MaxTraceFileSize);
            EnableTrace = 0;
        }

        SYSTEM( fputs,( trc_rdwrt_buff, hf ) );
        //SYSTEM( fflush, (hf) );

        #ifdef _UNIX_
           SYSTEM_RET(handle, fileno, (hf))
           //SYSTEM(fsync, (handle))
        #endif
    }

    return Res;
}

size_t trc_wrt_iter( FILE *hf, ITERATION *Iter )
{
    short i;
    size_t Res;
    size_t Len;

    #ifdef _UNIX_
       int handle;
    #endif

    Res = Len = TraceOptions.SpaceIndent * Trace.Level;
    memset( trc_rdwrt_buff, ' ', Len );
    trc_rdwrt_buff[ Len ] = 0;

    SYSTEM_RET(Len, sprintf,( trc_rdwrt_buff + Res, "%s: %ld, (%ld", KeyWords[N_ITERATION], Iter->LI, Iter->Index[0] ));
    Res += Len;

    for( i = 1; i < Iter->Rank; i++ )
    {
        SYSTEM_RET(Len, sprintf,( trc_rdwrt_buff + Res, ",%ld", Iter->Index[i] ));
        Res += Len;
    }
    SYSTEM_RET(Len, sprintf,( trc_rdwrt_buff + Res, ")\n" ));
    Res += Len;

    if( hf )
    {
        CurrCmpTraceFileSize += Res;
        if (CurrCmpTraceFileSize > MaxTraceFileSize)
        {

            pprintf(3, "*** RTS err: CMPTRACE: Trace file size exceeds the limit %d\n", MaxTraceFileSize);
            EnableTrace = 0;
        }

        SYSTEM( fputs,( trc_rdwrt_buff, hf ) );
        //SYSTEM( fflush, (hf) );

        #ifdef _UNIX_
           SYSTEM_RET(handle, fileno, (hf))
           //SYSTEM(fsync, (handle))
        #endif
    }

    return Res;
}

size_t trc_wrt_readvar( FILE *hf, VARIABLE *Var )
{
    return trc_wrt_variable( hf, Var, Var->Reduct ? N_R_READ : N_READ );
}

size_t trc_wrt_prewritevar( FILE *hf, VARIABLE *Var )
{
    return trc_wrt_variable( hf, Var, Var->Reduct ? N_R_PRE_WRITE : N_PRE_WRITE );
}

size_t trc_wrt_postwritevar( FILE *hf, VARIABLE *Var )
{
    return trc_wrt_variable( hf, Var, Var->Reduct ? N_R_POST_WRITE : N_POST_WRITE );
}

size_t trc_wrt_reductvar( FILE *hf, VARIABLE *Var )
{
    size_t Res;
    size_t Len;

    #ifdef _UNIX_
       int handle;
    #endif

    Res = Len = TraceOptions.SpaceIndent * Trace.Level;
    memset( trc_rdwrt_buff, ' ', Len );
    trc_rdwrt_buff[ Len ] = 0;

    SYSTEM_RET(Len, sprintf,( trc_rdwrt_buff + Res, "%s: [%ld]", KeyWords[N_REDUCT], Var->vType ));
    Res += Len;

    if ( dbg_isNAN_Val(&Var->val, Var->vType) )
    {
        SYSTEM_RET(Len, sprintf, (trc_rdwrt_buff + Res, "#NAN"));
    }
    else
    {
        switch( Var->vType )
        {
        case rt_INT :
        case rt_LOGICAL :
            SYSTEM_RET(Len, sprintf, (trc_rdwrt_buff + Res, " %d", Var->val._int));
            break;
        case rt_LONG :
            SYSTEM_RET(Len, sprintf, (trc_rdwrt_buff + Res, " %ld", Var->val._long));
            break;
        case rt_LLONG:
            SYSTEM_RET(Len, sprintf, (trc_rdwrt_buff + Res, " %lld", Var->val._longlong));
            break;
        case rt_FLOAT :
            SYSTEM_RET(Len, sprintf, (trc_rdwrt_buff + Res, " %.*G", Trace.FloatPrecision, Var->val._float));
            break;
        case rt_DOUBLE :
            SYSTEM_RET(Len, sprintf,( trc_rdwrt_buff + Res, " %.*lG", Trace.DoublePrecision, Var->val._double));
            break;
        case rt_FLOAT_COMPLEX :
            SYSTEM_RET(Len, sprintf, (trc_rdwrt_buff + Res, " (%.*G,%.*G)",
                Trace.FloatPrecision, Var->val._complex_float[0],
                Trace.FloatPrecision, Var->val._complex_float[1]));
            break;
        case rt_DOUBLE_COMPLEX :
            SYSTEM_RET(Len, sprintf, (trc_rdwrt_buff + Res, " (%.*lG,%.*lG)",
                Trace.DoublePrecision, Var->val._complex_double[0],
                Trace.DoublePrecision, Var->val._complex_double[1]));
            break;
        default:
            Len = 0;
        }
    }

    Res += Len;

    SYSTEM_RET(Len, sprintf,( trc_rdwrt_buff + Res, "; {\"%s\", %lu}\n", Var->File, Var->Line ));
    Res += Len;

    if( hf )
    {
        CurrCmpTraceFileSize += Res;
        if (CurrCmpTraceFileSize > MaxTraceFileSize)
        {

            pprintf(3, "*** RTS err: CMPTRACE: Trace file size exceeds the limit %d\n", MaxTraceFileSize);
            EnableTrace = 0;
        }

        SYSTEM( fputs,( trc_rdwrt_buff, hf ) );
        //SYSTEM( fflush, (hf) );

        #ifdef _UNIX_
           SYSTEM_RET(handle, fileno, (hf))
           //SYSTEM(fsync, (handle))
        #endif
    }

    return Res;
}

size_t trc_wrt_variable( FILE *hf, VARIABLE *Var, int iType )
{
    size_t Res;
    size_t Len;

    #ifdef _UNIX_
       int handle;
    #endif

    Res = Len = TraceOptions.SpaceIndent * Trace.Level;
    memset( trc_rdwrt_buff, ' ', Len );
    trc_rdwrt_buff[ Len ] = 0;

    SYSTEM_RET(Len, sprintf,( trc_rdwrt_buff + Res, "%s: [%ld] \"%s\"", KeyWords[iType], Var->vType, Var->Operand ));
    Res += Len;
    if( iType != N_PRE_WRITE && iType != N_R_PRE_WRITE )
    {
        if ( dbg_isNAN_Val(&Var->val, Var->vType) )
        {
            SYSTEM_RET(Len, sprintf, (trc_rdwrt_buff + Res, "#NAN"));
        }
        else
        {
            switch( Var->vType )
            {
            case rt_INT :
            case rt_LOGICAL :
                SYSTEM_RET(Len, sprintf, (trc_rdwrt_buff + Res, " = %d", Var->val._int));
                break;
            case rt_LONG :
                SYSTEM_RET(Len, sprintf, (trc_rdwrt_buff + Res, " = %ld", Var->val._long));
                break;
            case rt_LLONG:
                SYSTEM_RET(Len, sprintf, (trc_rdwrt_buff + Res, " = %lld", Var->val._longlong));
                break;
            case rt_FLOAT :
                SYSTEM_RET(Len, sprintf, (trc_rdwrt_buff + Res, " = %.*G", Trace.FloatPrecision, Var->val._float));
                break;
            case rt_DOUBLE :
                SYSTEM_RET(Len, sprintf,( trc_rdwrt_buff + Res, " = %.*lG", Trace.DoublePrecision, Var->val._double));
                break;
            case rt_FLOAT_COMPLEX :
                SYSTEM_RET(Len, sprintf, (trc_rdwrt_buff + Res, " = (%.*G,%.*G)",
                    Trace.FloatPrecision, Var->val._complex_float[0],
                    Trace.FloatPrecision, Var->val._complex_float[1]));
                break;
            case rt_DOUBLE_COMPLEX :
                SYSTEM_RET(Len, sprintf, (trc_rdwrt_buff + Res, " = (%.*lG,%.*lG)",
                    Trace.DoublePrecision, Var->val._complex_double[0],
                    Trace.DoublePrecision, Var->val._complex_double[1]));
                break;
            default: Len = 0;
            }
        }
        Res += Len;
    }

    SYSTEM_RET(Len, sprintf,( trc_rdwrt_buff + Res, "; {\"%s\", %lu}\n", Var->File, Var->Line ));
    Res += Len;

    if( hf )
    {
        CurrCmpTraceFileSize += Res;
        if (CurrCmpTraceFileSize > MaxTraceFileSize)
        {

            pprintf(3, "*** RTS err: CMPTRACE: Trace file size exceeds the limit %d\n", MaxTraceFileSize);
            EnableTrace = 0;
        }

        SYSTEM( fputs,( trc_rdwrt_buff, hf ) );
        //SYSTEM( fflush, (hf) );

        #ifdef _UNIX_
           SYSTEM_RET(handle, fileno, (hf))
           //SYSTEM(fsync, (handle))
        #endif
    }

    return Res;
}

size_t trc_wrt_skip( FILE *hf, SKIP *Skip )
{
    size_t Res;
    size_t Len;

    #ifdef _UNIX_
       int handle;
    #endif

    Res = Len = TraceOptions.SpaceIndent * Trace.Level;
    memset( trc_rdwrt_buff, ' ', Len );
    trc_rdwrt_buff[ Len ] = 0;

    SYSTEM_RET(Len, sprintf,( trc_rdwrt_buff + Res, "%s: {\"%s\", %lu}\n", KeyWords[N_SKIP], Skip->File, Skip->Line ));
    Res += Len;

    if( hf )
    {
        CurrCmpTraceFileSize += Res;
        if (CurrCmpTraceFileSize > MaxTraceFileSize)
        {
            pprintf(3, "*** RTS err: CMPTRACE: Trace file size exceeds the limit %d\n", MaxTraceFileSize);
            EnableTrace = 0;
        }

        SYSTEM( fputs,( trc_rdwrt_buff, hf ) );
        //SYSTEM( fflush, (hf) );

        #ifdef _UNIX_
           SYSTEM_RET(handle, fileno, (hf))
           //SYSTEM(fsync, (handle))
        #endif
    }

    return Res;
}

void trc_wrt_end( FILE *hf )
{
    size_t Res;
    size_t Len;

    #ifdef _UNIX_
       int handle;
    #endif

    Res = Len = TraceOptions.SpaceIndent * Trace.Level;
    memset( trc_rdwrt_buff, ' ', Len );
    trc_rdwrt_buff[ Len ] = 0;

    SYSTEM_RET(Len, sprintf,( trc_rdwrt_buff + Res, "%s\n", KeyWords[N_END_TRACE]));
    Res += Len;

    if( hf )
    {
        CurrCmpTraceFileSize += Res;
        if (CurrCmpTraceFileSize > MaxTraceFileSize)
        {
            pprintf(3, "*** RTS err: CMPTRACE: Trace file size exceeds the limit %d\n", MaxTraceFileSize);
            EnableTrace = 0;
        }

        SYSTEM( fputs,( trc_rdwrt_buff, hf ) );
        //SYSTEM( fflush, (hf) );

        #ifdef _UNIX_
           SYSTEM_RET(handle, fileno, (hf))
           //SYSTEM(fsync, (handle))
        #endif
    }

    return ;;
}

size_t trc_wrt_indent( FILE *hf, int Level )
{
    int i;
    for( i = 0; i < TraceOptions.SpaceIndent * Level; i++ )
        SYSTEM(fputs,( " ", hf ));

    return TraceOptions.SpaceIndent * Level;
}

void trc_wrt_calctraceinfo(void)
{
    int i;
    STRUCT_INFO* Loop;

    #ifdef WIN32
        /* '\n' under Windows is 2 bytes */
        Trace.Bytes += Trace.StrCount;
    #endif

    for( i = 0; i < table_Count(&Trace.tStructs); i++ )
    {
        Loop = table_At(STRUCT_INFO, &Trace.tStructs, i );
        trc_wrt_calcinfo( Loop );
        Trace.Bytes += Loop->Bytes;
        Trace.StrCount += Loop->StrCount;
        Trace.Iters += Loop->Iters;
    }
}

void trc_wrt_calcinfo( STRUCT_INFO* Loop )
{
    int i;
    STRUCT_INFO *Child;

    #ifdef WIN32
        /* '\n' under Windows is 2 bytes */
        Loop->Bytes += Loop->StrCount;
    #endif

    for( i = 0; i < table_Count(&Loop->tChildren); i++ )
    {
        Child = table_At(STRUCT_INFO, &(Loop->tChildren), i);
        trc_wrt_calcinfo( Child );
        Loop->Bytes += Child->Bytes;
        Loop->StrCount += Child->StrCount;
    }
}

#endif /* _TRC_WRT_C_ */
