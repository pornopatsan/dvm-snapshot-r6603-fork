# Microsoft Developer Studio Generated NMAKE File, Based on Rater.dsp
!IF "$(CFG)" == ""
CFG=Rater - Win32 Debug
!MESSAGE No configuration specified. Defaulting to Rater - Win32 Debug.
!ENDIF 
 
!IF "$(CFG)" != "Rater - Win32 Release" && "$(CFG)" != "Rater - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "Rater.mak" CFG="Rater - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "Rater - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "Rater - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 
!ERROR An invalid configuration is specified.
!ENDIF 

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE 
NULL=nul
!ENDIF 

!IF  "$(CFG)" == "Rater - Win32 Release"

OUTDIR=.\Release
INTDIR=.\Release
# Begin Custom Macros
OutDir=.\Release
# End Custom Macros

ALL : "$(OUTDIR)\Rater.lib"


CLEAN :
	-@erase "$(INTDIR)\AlignAxis.obj"
	-@erase "$(INTDIR)\AMView.obj"
	-@erase "$(INTDIR)\BGroup.obj"
	-@erase "$(INTDIR)\Block.obj"
	-@erase "$(INTDIR)\CommCost.obj"
	-@erase "$(INTDIR)\DArray.obj"
	-@erase "$(INTDIR)\DimBound.obj"
	-@erase "$(INTDIR)\DistAxis.obj"
	-@erase "$(INTDIR)\intersection.obj"
	-@erase "$(INTDIR)\LoopBlock.obj"
	-@erase "$(INTDIR)\LoopLS.obj"
	-@erase "$(INTDIR)\Ls.obj"
	-@erase "$(INTDIR)\ParLoop.obj"
	-@erase "$(INTDIR)\RedGroup.obj"
	-@erase "$(INTDIR)\RedVar.obj"
	-@erase "$(INTDIR)\RemAccessBuf.obj"
	-@erase "$(INTDIR)\Space.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\Vm.obj"
	-@erase "$(OUTDIR)\Rater.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

F90=df.exe
CPP=cl.exe
CPP_PROJ=/nologo /O2 /I "Include" /I "../Presage/include" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /Fp"$(INTDIR)\Rater.pch"  /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

.c{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.c{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

RSC=rc.exe
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\Rater.bsc" 
BSC32_SBRS= \
	
LIB32=link.exe -lib
LIB32_FLAGS=/nologo /out:"$(OUTDIR)\Rater.lib" 
LIB32_OBJS= \
	"$(INTDIR)\AlignAxis.obj" \
	"$(INTDIR)\AMView.obj" \
	"$(INTDIR)\BGroup.obj" \
	"$(INTDIR)\Block.obj" \
	"$(INTDIR)\CommCost.obj" \
	"$(INTDIR)\DArray.obj" \
	"$(INTDIR)\DimBound.obj" \
	"$(INTDIR)\DistAxis.obj" \
	"$(INTDIR)\intersection.obj" \
	"$(INTDIR)\LoopBlock.obj" \
	"$(INTDIR)\LoopLS.obj" \
	"$(INTDIR)\Ls.obj" \
	"$(INTDIR)\ParLoop.obj" \
	"$(INTDIR)\RedGroup.obj" \
	"$(INTDIR)\RedVar.obj" \
	"$(INTDIR)\RemAccessBuf.obj" \
	"$(INTDIR)\Space.obj" \
	"$(INTDIR)\Vm.obj"

"$(OUTDIR)\Rater.lib" : "$(OUTDIR)" $(DEF_FILE) $(LIB32_OBJS)
    $(LIB32) @<<
  $(LIB32_FLAGS) $(DEF_FLAGS) $(LIB32_OBJS)
<<

!ELSEIF  "$(CFG)" == "Rater - Win32 Debug"

OUTDIR=.\Debug
INTDIR=.\Debug
# Begin Custom Macros
OutDir=.\Debug
# End Custom Macros

ALL : "$(OUTDIR)\Rater.lib" "$(OUTDIR)\Rater.bsc"


CLEAN :
	-@erase "$(INTDIR)\AlignAxis.obj"
	-@erase "$(INTDIR)\AlignAxis.sbr"
	-@erase "$(INTDIR)\AMView.obj"
	-@erase "$(INTDIR)\AMView.sbr"
	-@erase "$(INTDIR)\BGroup.obj"
	-@erase "$(INTDIR)\BGroup.sbr"
	-@erase "$(INTDIR)\Block.obj"
	-@erase "$(INTDIR)\Block.sbr"
	-@erase "$(INTDIR)\CommCost.obj"
	-@erase "$(INTDIR)\CommCost.sbr"
	-@erase "$(INTDIR)\DArray.obj"
	-@erase "$(INTDIR)\DArray.sbr"
	-@erase "$(INTDIR)\DimBound.obj"
	-@erase "$(INTDIR)\DimBound.sbr"
	-@erase "$(INTDIR)\DistAxis.obj"
	-@erase "$(INTDIR)\DistAxis.sbr"
	-@erase "$(INTDIR)\intersection.obj"
	-@erase "$(INTDIR)\intersection.sbr"
	-@erase "$(INTDIR)\LoopBlock.obj"
	-@erase "$(INTDIR)\LoopBlock.sbr"
	-@erase "$(INTDIR)\LoopLS.obj"
	-@erase "$(INTDIR)\LoopLS.sbr"
	-@erase "$(INTDIR)\Ls.obj"
	-@erase "$(INTDIR)\Ls.sbr"
	-@erase "$(INTDIR)\ParLoop.obj"
	-@erase "$(INTDIR)\ParLoop.sbr"
	-@erase "$(INTDIR)\RedGroup.obj"
	-@erase "$(INTDIR)\RedGroup.sbr"
	-@erase "$(INTDIR)\RedVar.obj"
	-@erase "$(INTDIR)\RedVar.sbr"
	-@erase "$(INTDIR)\RemAccessBuf.obj"
	-@erase "$(INTDIR)\RemAccessBuf.sbr"
	-@erase "$(INTDIR)\Space.obj"
	-@erase "$(INTDIR)\Space.sbr"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\Vm.obj"
	-@erase "$(INTDIR)\Vm.sbr"
	-@erase "$(OUTDIR)\Rater.bsc"
	-@erase "$(OUTDIR)\Rater.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

F90=df.exe
CPP=cl.exe
CPP_PROJ=/nologo /Z7 /Od /I "Include" /I "../Presage/include" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "P_DEBUG" /D "_TIME_TRACE_" /FR"$(INTDIR)\\" /Fp"$(INTDIR)\Rater.pch"  /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

.c{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.c{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

RSC=rc.exe
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\Rater.bsc" 
BSC32_SBRS= \
	"$(INTDIR)\AlignAxis.sbr" \
	"$(INTDIR)\AMView.sbr" \
	"$(INTDIR)\BGroup.sbr" \
	"$(INTDIR)\Block.sbr" \
	"$(INTDIR)\CommCost.sbr" \
	"$(INTDIR)\DArray.sbr" \
	"$(INTDIR)\DimBound.sbr" \
	"$(INTDIR)\DistAxis.sbr" \
	"$(INTDIR)\intersection.sbr" \
	"$(INTDIR)\LoopBlock.sbr" \
	"$(INTDIR)\LoopLS.sbr" \
	"$(INTDIR)\Ls.sbr" \
	"$(INTDIR)\ParLoop.sbr" \
	"$(INTDIR)\RedGroup.sbr" \
	"$(INTDIR)\RedVar.sbr" \
	"$(INTDIR)\RemAccessBuf.sbr" \
	"$(INTDIR)\Space.sbr" \
	"$(INTDIR)\Vm.sbr"

"$(OUTDIR)\Rater.bsc" : "$(OUTDIR)" $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LIB32=link.exe -lib
LIB32_FLAGS=/nologo /out:"$(OUTDIR)\Rater.lib" 
LIB32_OBJS= \
	"$(INTDIR)\AlignAxis.obj" \
	"$(INTDIR)\AMView.obj" \
	"$(INTDIR)\BGroup.obj" \
	"$(INTDIR)\Block.obj" \
	"$(INTDIR)\CommCost.obj" \
	"$(INTDIR)\DArray.obj" \
	"$(INTDIR)\DimBound.obj" \
	"$(INTDIR)\DistAxis.obj" \
	"$(INTDIR)\intersection.obj" \
	"$(INTDIR)\LoopBlock.obj" \
	"$(INTDIR)\LoopLS.obj" \
	"$(INTDIR)\Ls.obj" \
	"$(INTDIR)\ParLoop.obj" \
	"$(INTDIR)\RedGroup.obj" \
	"$(INTDIR)\RedVar.obj" \
	"$(INTDIR)\RemAccessBuf.obj" \
	"$(INTDIR)\Space.obj" \
	"$(INTDIR)\Vm.obj"

"$(OUTDIR)\Rater.lib" : "$(OUTDIR)" $(DEF_FILE) $(LIB32_OBJS)
    $(LIB32) @<<
  $(LIB32_FLAGS) $(DEF_FLAGS) $(LIB32_OBJS)
<<

!ENDIF 


!IF "$(NO_EXTERNAL_DEPS)" != "1"
!IF EXISTS("Rater.dep")
!INCLUDE "Rater.dep"
!ELSE 
!MESSAGE Warning: cannot find "Rater.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "Rater - Win32 Release" || "$(CFG)" == "Rater - Win32 Debug"
SOURCE=.\Src\AlignAxis.cpp

!IF  "$(CFG)" == "Rater - Win32 Release"


"$(INTDIR)\AlignAxis.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Rater - Win32 Debug"


"$(INTDIR)\AlignAxis.obj"	"$(INTDIR)\AlignAxis.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\Src\AMView.cpp

!IF  "$(CFG)" == "Rater - Win32 Release"


"$(INTDIR)\AMView.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Rater - Win32 Debug"


"$(INTDIR)\AMView.obj"	"$(INTDIR)\AMView.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\Src\BGroup.cpp

!IF  "$(CFG)" == "Rater - Win32 Release"


"$(INTDIR)\BGroup.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Rater - Win32 Debug"


"$(INTDIR)\BGroup.obj"	"$(INTDIR)\BGroup.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\Src\Block.cpp

!IF  "$(CFG)" == "Rater - Win32 Release"


"$(INTDIR)\Block.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Rater - Win32 Debug"


"$(INTDIR)\Block.obj"	"$(INTDIR)\Block.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\Src\CommCost.cpp

!IF  "$(CFG)" == "Rater - Win32 Release"

CPP_SWITCHES=/nologo /O2 /I "Include" /I "../Presage/include" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /Fp"$(INTDIR)\Rater.pch"  /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\CommCost.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "Rater - Win32 Debug"

CPP_SWITCHES=/nologo /Z7 /Od /I "Include" /I "../Presage/include" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "P_DEBUG" /D "_TIME_TRACE_" /FR"$(INTDIR)\\" /Fp"$(INTDIR)\Rater.pch"  /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\CommCost.obj"	"$(INTDIR)\CommCost.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=.\Src\DArray.cpp

!IF  "$(CFG)" == "Rater - Win32 Release"


"$(INTDIR)\DArray.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Rater - Win32 Debug"


"$(INTDIR)\DArray.obj"	"$(INTDIR)\DArray.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\Src\DimBound.cpp

!IF  "$(CFG)" == "Rater - Win32 Release"


"$(INTDIR)\DimBound.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Rater - Win32 Debug"


"$(INTDIR)\DimBound.obj"	"$(INTDIR)\DimBound.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\Src\DistAxis.cpp

!IF  "$(CFG)" == "Rater - Win32 Release"


"$(INTDIR)\DistAxis.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Rater - Win32 Debug"


"$(INTDIR)\DistAxis.obj"	"$(INTDIR)\DistAxis.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\Src\intersection.cpp

!IF  "$(CFG)" == "Rater - Win32 Release"


"$(INTDIR)\intersection.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Rater - Win32 Debug"


"$(INTDIR)\intersection.obj"	"$(INTDIR)\intersection.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\Src\LoopBlock.cpp

!IF  "$(CFG)" == "Rater - Win32 Release"


"$(INTDIR)\LoopBlock.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Rater - Win32 Debug"


"$(INTDIR)\LoopBlock.obj"	"$(INTDIR)\LoopBlock.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\Src\LoopLS.cpp

!IF  "$(CFG)" == "Rater - Win32 Release"


"$(INTDIR)\LoopLS.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Rater - Win32 Debug"


"$(INTDIR)\LoopLS.obj"	"$(INTDIR)\LoopLS.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\Src\Ls.cpp

!IF  "$(CFG)" == "Rater - Win32 Release"


"$(INTDIR)\Ls.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Rater - Win32 Debug"


"$(INTDIR)\Ls.obj"	"$(INTDIR)\Ls.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\Src\ParLoop.cpp

!IF  "$(CFG)" == "Rater - Win32 Release"


"$(INTDIR)\ParLoop.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Rater - Win32 Debug"


"$(INTDIR)\ParLoop.obj"	"$(INTDIR)\ParLoop.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\Src\RedGroup.cpp

!IF  "$(CFG)" == "Rater - Win32 Release"


"$(INTDIR)\RedGroup.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Rater - Win32 Debug"


"$(INTDIR)\RedGroup.obj"	"$(INTDIR)\RedGroup.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\Src\RedVar.cpp

!IF  "$(CFG)" == "Rater - Win32 Release"


"$(INTDIR)\RedVar.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Rater - Win32 Debug"


"$(INTDIR)\RedVar.obj"	"$(INTDIR)\RedVar.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\Src\RemAccessBuf.cpp

!IF  "$(CFG)" == "Rater - Win32 Release"


"$(INTDIR)\RemAccessBuf.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Rater - Win32 Debug"


"$(INTDIR)\RemAccessBuf.obj"	"$(INTDIR)\RemAccessBuf.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\Src\Space.cpp

!IF  "$(CFG)" == "Rater - Win32 Release"


"$(INTDIR)\Space.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Rater - Win32 Debug"


"$(INTDIR)\Space.obj"	"$(INTDIR)\Space.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\Src\Vm.cpp

!IF  "$(CFG)" == "Rater - Win32 Release"


"$(INTDIR)\Vm.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Rater - Win32 Debug"


"$(INTDIR)\Vm.obj"	"$(INTDIR)\Vm.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 


!ENDIF 

