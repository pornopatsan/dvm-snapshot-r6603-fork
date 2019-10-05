
#ifndef _DEBPAR_C_
#define _DEBPAR_C_
/****************/


void  DynControlParSet(void)
{
  Parameter(DbgInfoPrint,byte); /* flag on information messages
                                   of dynamic control system
                                   and trace */

  Parameter(DynDebugPrintStatistics,byte); /* Print statistics */
  Parameter(DynDebugMemoryLimit, DvmType);     /* Memory limit for
                                              dynamic debugger */
  Parameter(DynDebugExecutionTimeLimit, DvmType); /* execution time limit
                                                 for dynamic debugger */

  Parameter(EnableTrace,byte);  /* mode of manual reduction
                                   calculations */
  Parameter(EnableCodeCoverage,byte); /* defines if code coverage gathering
                                         is turned on or off */
  Parameter(DelUsrTrace,byte);  /* delete old files with
                                   tarcing information */
  Parameter(ManualReductCalc,byte); /* mode of manual reduction
                                       calculations */

  Parameter(TraceOptions.FileLoopInfo,char);   /* file  with
                                                  loop descriptions */
  Parameter(TraceOptions.ErrorFile,char);      /* file for error
                                                  diagnostics */
  Parameter(TraceOptions.TracePath,char);      /* path for trace
                                                  files */
  Parameter(TraceOptions.InputTracePrefix,char);  /* input trace
                                                     prefix */
  Parameter(TraceOptions.OutputTracePrefix,char); /* output trace
                                                     prefix */
  Parameter(TraceOptions.Ext,char);            /* file extension
                                                  for processor
                                                  trace */
  Parameter(TraceOptions.Exp,double);          /* accuracy of floating
                                                  numbers comparison */
  Parameter(TraceOptions.RelCompareMin,double);/* parameter which limits
                                                  relative comparison of small numbers */
  Parameter(TraceOptions.ExpIsAbsolute, byte); /* flag, defines the type of accuracy to use
                                                  1 - absolute, 0 - relative accuracy */
  Parameter(TraceOptions.StrictCompare, byte); /* flag, defines how the values are gonna be compared
                                                  1 - use Exp as threshold, 0 - no threshold */
  Parameter(TraceOptions.TraceMode,int);       /* trace mode */
  Parameter(TraceOptions.TraceLevel,int);      /* depth of trace */
  Parameter(TraceOptions.SpaceIndent,int);     /* indent for
                                                  the next level */
  Parameter(TraceOptions.ErrorToScreen,byte);  /* error diagnostics output
                                                  to the screen */
  Parameter(TraceOptions.WrtHeaderProc,int);   /* processor, generating file
                                                  for the loop descriptions */
  Parameter(TraceOptions.TableTraceSize,int);
  Parameter(TraceOptions.HashIterIndex,int);
  Parameter(TraceOptions.HashIterSize,int);
  Parameter(TraceOptions.ReductHashIndexSize,int);
  Parameter(TraceOptions.ReductHashTableSize,int);
  Parameter(TraceOptions.ReductVarTableSize,int);

  Parameter(TraceOptions.SaveThroughExec,byte);/* save trace during execution */
  Parameter(TraceOptions.WriteEmptyIter,byte); /* trace empty iterations */
  Parameter(TraceOptions.AppendErrorFile,byte);/* append error diagnostics
                                                  into existing file */
  Parameter(TraceOptions.MaxErrors,int);       /* max number of output
                                                  error diagnostics */
  Parameter(TraceOptions.MultidimensionalArrays,byte);
  Parameter(TraceOptions.drarr,byte);
  Parameter(TraceOptions.DefaultArrayStep,int);
  Parameter(TraceOptions.DefaultIterStep,int);

  Parameter(TraceOptions.ChecksumMode,byte);   /* defines the coverage
                                                  of arrays for
                                                  calculating checksums */
  Parameter(TraceOptions.CalcChecksums, byte); /* defines if in addition to usual trace level
						  checksums will be calculated */
  Parameter(TraceOptions.ChecksumBinary, byte); /* defines if checksums are calculated by binary addition */
  Parameter(TraceOptions.SeqLdivParContextOnly,byte);
                                            /* defines if seq. loops will be divided in blocks
                                               only if they are nested in parallel loop or always */
  Parameter(TraceOptions.ChecksumDisarrOnly,byte);  /* defines if checksums are
                                                       calculated for disarrays only */
  Parameter(TraceOptions.TrapArraysAnyway,byte); /* defines whether to trap access to arrays if header read or not */

  Parameter(TraceOptions.StartPoint,char);      /* dynamic point from which to start verbose trace */
  Parameter(TraceOptions.FinishPoint,char);     /* dynamic point from which to finish verbose trace */

  Parameter(TraceOptions.SaveArrayFilename,char);    /* file to contain array elements when saved */
  Parameter(TraceOptions.SaveArrayID,char);          /* ID of an array to be saved */
  Parameter(TraceOptions.Ig_left, int);  /* defines a set of global first */
  Parameter(TraceOptions.Ig_right, int); /* and last parallel loop's iterations */
  Parameter(TraceOptions.Iloc_left, int);  /* defines a set of local first */
  Parameter(TraceOptions.Iloc_right, int); /* and last parallel loop's iterations */
  Parameter(TraceOptions.Irep_left, int);  /* number of first */
  Parameter(TraceOptions.Irep_right, int); /* and last sequential loops iterations */
  Parameter(TraceOptions.SubstRedResults, byte); /* substitute reduction results with the results from the trace */
  Parameter(TraceOptions.SubstAllResults, byte); /* substitute all write operations results with the results from the trace */
  Parameter(TraceOptions.DisableRedArrays, byte);  /* disable reduction array's debugging */
  Parameter(TraceOptions.SRCLocCompareMode, byte); /* comparison mode for code locations */

  Parameter(TraceOptions.LocIterWidth, int);  /* defines a set of local traced iterations of par. loops */
  Parameter(TraceOptions.GlobIterWidth, int); /* defines a set of global traced iterations of par. loops */
  Parameter(TraceOptions.RepIterWidth, int);  /* defines a set of traced iterations of seq. loops */
  Parameter(TraceOptions.IterTraceMode, int); /* controls VTR value in programs compiled with VTR */
  Parameter(TraceOptions.SetCoreSizeMax, byte); /* set core file size to maximum allowed (Unix only) */
  Parameter(TraceOptions.EnableNANChecks, byte);   /* indicates if runtime checks for NotANumber values are performed */
  Parameter(TraceOptions.MrgTracePrint, byte);   /* mode of printing merged trace */

  Parameter(EnableDynControl,byte);
  Parameter(HashMethod,byte);                  /* used Hash function */
  Parameter(HashOffsetValue,byte);

  Parameter(DebugOptions.ErrorFile,char);
  Parameter(DebugOptions.HashIndexSize,int);
  Parameter(DebugOptions.HashTableSize,int);
  Parameter(DebugOptions.VarTableSize,int);
  Parameter(DebugOptions.ErrorToScreen,byte);
  Parameter(DebugOptions.AppendErrorFile,byte);/* append error diagnostics
                                                  into existing file */
  Parameter(DebugOptions.MaxErrors,int);       /* max number of output
                                                  error diagnostics */

  Parameter(DebugOptions.CheckVarInitialization,byte);
  Parameter(DebugOptions.CheckDisArrInitialization,byte);
  Parameter(DebugOptions.CheckReductionAccess,byte);
  Parameter(DebugOptions.CheckRemoteBufferAccess,byte);
  Parameter(DebugOptions.CheckDisArrLimits,byte);
  Parameter(DebugOptions.CheckDataDependence,byte);
  Parameter(DebugOptions.CheckVarReadOnly,byte);
  Parameter(DebugOptions.CheckDisArrEdgeExchange,byte);
  Parameter(DebugOptions.CheckDisArrSequentialAccess,byte);
  Parameter(DebugOptions.CheckDisArrLocalElm,byte);

  return;
}


#endif   /* _DEBPAR_C_ */
