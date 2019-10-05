# Microsoft Developer Studio Generated NMAKE File, Based on Presage.dsp
!IF "$(CFG)" == ""
CFG=Presage - Win32 Debug
!MESSAGE No configuration specified. Defaulting to Presage - Win32 Debug.
!ENDIF 
 
!IF "$(CFG)" != "Presage - Win32 Release" && "$(CFG)" != "Presage - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "Presage.mak" CFG="Presage - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "Presage - Win32 Release" (based on "Win32 (x86) Console Application")
!MESSAGE "Presage - Win32 Debug" (based on "Win32 (x86) Console Application")
!MESSAGE 
!ERROR An invalid configuration is specified.
!ENDIF 

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE 
NULL=nul
!ENDIF 

!IF  "$(CFG)" == "Presage - Win32 Release"

OUTDIR=.\Release
INTDIR=.\Release
# Begin Custom Macros
OutDir=.\Release
# End Custom Macros

ALL : "$(OUTDIR)\Presage.exe" "$(OUTDIR)\Presage.bsc"


CLEAN :
	-@erase "$(INTDIR)\CallParams.obj"
	-@erase "$(INTDIR)\CallParams.sbr"
	-@erase "$(INTDIR)\Event.obj"
	-@erase "$(INTDIR)\Event.sbr"
	-@erase "$(INTDIR)\FuncCall.obj"
	-@erase "$(INTDIR)\FuncCall.sbr"
	-@erase "$(INTDIR)\Interval.obj"
	-@erase "$(INTDIR)\Interval.sbr"
	-@erase "$(INTDIR)\IntervalTemplate.obj"
	-@erase "$(INTDIR)\IntervalTemplate.sbr"
	-@erase "$(INTDIR)\ModelDArray.obj"
	-@erase "$(INTDIR)\ModelDArray.sbr"
	-@erase "$(INTDIR)\ModelInterval.obj"
	-@erase "$(INTDIR)\ModelInterval.sbr"
	-@erase "$(INTDIR)\ModelIO.obj"
	-@erase "$(INTDIR)\ModelIO.sbr"
	-@erase "$(INTDIR)\ModelMPS_AM.obj"
	-@erase "$(INTDIR)\ModelParLoop.obj"
	-@erase "$(INTDIR)\ModelParLoop.sbr"
	-@erase "$(INTDIR)\ModelReduct.obj"
	-@erase "$(INTDIR)\ModelReduct.sbr"
	-@erase "$(INTDIR)\ModelRegular.obj"
	-@erase "$(INTDIR)\ModelRegular.sbr"
	-@erase "$(INTDIR)\ModelRemAccess.obj"
	-@erase "$(INTDIR)\ModelRemAccess.sbr"
	-@erase "$(INTDIR)\ModelShadow.obj"
	-@erase "$(INTDIR)\ModelShadow.sbr"
	-@erase "$(INTDIR)\ParseString.obj"
	-@erase "$(INTDIR)\ParseString.sbr"
	-@erase "$(INTDIR)\Predictor.obj"
	-@erase "$(INTDIR)\Processor.obj"
	-@erase "$(INTDIR)\Processor.sbr"
	-@erase "$(INTDIR)\PS.obj"
	-@erase "$(INTDIR)\PS.sbr"
	-@erase "$(INTDIR)\TraceLine.obj"
	-@erase "$(INTDIR)\TraceLine.sbr"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(OUTDIR)\Presage.bsc"
	-@erase "$(OUTDIR)\Presage.exe"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

F90=df.exe
CPP=cl.exe
#CPP_PROJ=/nologo /ML /W3 /GX /O2 /I "../Presage/Include" /I "../Rater/Include" /I "../../Zlib/Include" /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /FR"$(INTDIR)\\" /Fp"$(INTDIR)\Presage.pch"  /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
CPP_PROJ=/nologo /EHsc /I "../Presage/Include" /I "../Rater/Include" /I "../../Zlib/Include" /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /FR"$(INTDIR)\\" /Fp"$(INTDIR)\Presage.pch" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

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
BSC32_FLAGS=/nologo /o"$(OUTDIR)\Presage.bsc" 
BSC32_SBRS= \
	"$(INTDIR)\CallParams.sbr" \
	"$(INTDIR)\Event.sbr" \
	"$(INTDIR)\FuncCall.sbr" \
	"$(INTDIR)\Interval.sbr" \
	"$(INTDIR)\IntervalTemplate.sbr" \
	"$(INTDIR)\ModelDArray.sbr" \
	"$(INTDIR)\ModelInterval.sbr" \
	"$(INTDIR)\ModelIO.sbr" \
	"$(INTDIR)\ModelParLoop.sbr" \
	"$(INTDIR)\ModelReduct.sbr" \
	"$(INTDIR)\ModelRegular.sbr" \
	"$(INTDIR)\ModelRemAccess.sbr" \
	"$(INTDIR)\ModelShadow.sbr" \
	"$(INTDIR)\ParseString.sbr" \
	"$(INTDIR)\Processor.sbr" \
	"$(INTDIR)\PS.sbr" \
	"$(INTDIR)\TraceLine.sbr"

"$(OUTDIR)\Presage.bsc" : "$(OUTDIR)" $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib ../Rater/Release/Rater.lib ../../Zlib/Release/Zlib.lib /nologo /subsystem:console /incremental:no /pdb:"$(OUTDIR)\Presage.pdb" /out:"$(OUTDIR)\Presage.exe" 
LINK32_OBJS= \
	"$(INTDIR)\CallParams.obj" \
	"$(INTDIR)\Event.obj" \
	"$(INTDIR)\FuncCall.obj" \
	"$(INTDIR)\Interval.obj" \
	"$(INTDIR)\IntervalTemplate.obj" \
	"$(INTDIR)\ModelDArray.obj" \
	"$(INTDIR)\ModelInterval.obj" \
	"$(INTDIR)\ModelIO.obj" \
	"$(INTDIR)\ModelMPS_AM.obj" \
	"$(INTDIR)\ModelParLoop.obj" \
	"$(INTDIR)\ModelReduct.obj" \
	"$(INTDIR)\ModelRegular.obj" \
	"$(INTDIR)\ModelRemAccess.obj" \
	"$(INTDIR)\ModelShadow.obj" \
	"$(INTDIR)\ParseString.obj" \
	"$(INTDIR)\Predictor.obj" \
	"$(INTDIR)\Processor.obj" \
	"$(INTDIR)\PS.obj" \
	"$(INTDIR)\TraceLine.obj"

"$(OUTDIR)\Presage.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "Presage - Win32 Debug"

OUTDIR=.\Debug
INTDIR=.\Debug
# Begin Custom Macros
OutDir=.\Debug
# End Custom Macros

ALL : "$(OUTDIR)\Presage.exe" "$(OUTDIR)\Presage.bsc"


CLEAN :
	-@erase "$(INTDIR)\CallParams.obj"
	-@erase "$(INTDIR)\CallParams.sbr"
	-@erase "$(INTDIR)\Event.obj"
	-@erase "$(INTDIR)\Event.sbr"
	-@erase "$(INTDIR)\FuncCall.obj"
	-@erase "$(INTDIR)\FuncCall.sbr"
	-@erase "$(INTDIR)\Interval.obj"
	-@erase "$(INTDIR)\Interval.sbr"
	-@erase "$(INTDIR)\IntervalTemplate.obj"
	-@erase "$(INTDIR)\IntervalTemplate.sbr"
	-@erase "$(INTDIR)\ModelDArray.obj"
	-@erase "$(INTDIR)\ModelDArray.sbr"
	-@erase "$(INTDIR)\ModelInterval.obj"
	-@erase "$(INTDIR)\ModelInterval.sbr"
	-@erase "$(INTDIR)\ModelIO.obj"
	-@erase "$(INTDIR)\ModelIO.sbr"
	-@erase "$(INTDIR)\ModelMPS_AM.obj"
	-@erase "$(INTDIR)\ModelParLoop.obj"
	-@erase "$(INTDIR)\ModelParLoop.sbr"
	-@erase "$(INTDIR)\ModelReduct.obj"
	-@erase "$(INTDIR)\ModelReduct.sbr"
	-@erase "$(INTDIR)\ModelRegular.obj"
	-@erase "$(INTDIR)\ModelRegular.sbr"
	-@erase "$(INTDIR)\ModelRemAccess.obj"
	-@erase "$(INTDIR)\ModelRemAccess.sbr"
	-@erase "$(INTDIR)\ModelShadow.obj"
	-@erase "$(INTDIR)\ModelShadow.sbr"
	-@erase "$(INTDIR)\ParseString.obj"
	-@erase "$(INTDIR)\ParseString.sbr"
	-@erase "$(INTDIR)\Predictor.obj"
	-@erase "$(INTDIR)\Processor.obj"
	-@erase "$(INTDIR)\Processor.sbr"
	-@erase "$(INTDIR)\PS.obj"
	-@erase "$(INTDIR)\TraceLine.obj"
	-@erase "$(INTDIR)\TraceLine.sbr"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(OUTDIR)\Presage.bsc"
	-@erase "$(OUTDIR)\Presage.exe"
	-@erase "$(OUTDIR)\Presage.ilk"
	-@erase "$(OUTDIR)\Presage.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

F90=df.exe
CPP=cl.exe
#CPP_PROJ=/nologo /MLd /W3 /GX /ZI /Od /I "../Presage/Include" /I "../Rater/Include" /I "../../Zlib/Include" /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /D "P_DEBUG" /FR"$(INTDIR)\\" /Fp"$(INTDIR)\Presage.pch"  /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
CPP_PROJ=/nologo /EHsc /I "../Presage/Include" /I "../Rater/Include" /I "../../Zlib/Include" /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /D "P_DEBUG" /FR"$(INTDIR)\\" /Fp"$(INTDIR)\Presage.pch" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

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
BSC32_FLAGS=/nologo /o"$(OUTDIR)\Presage.bsc" 
BSC32_SBRS= \
	"$(INTDIR)\CallParams.sbr" \
	"$(INTDIR)\Event.sbr" \
	"$(INTDIR)\FuncCall.sbr" \
	"$(INTDIR)\Interval.sbr" \
	"$(INTDIR)\IntervalTemplate.sbr" \
	"$(INTDIR)\ModelDArray.sbr" \
	"$(INTDIR)\ModelInterval.sbr" \
	"$(INTDIR)\ModelIO.sbr" \
	"$(INTDIR)\ModelParLoop.sbr" \
	"$(INTDIR)\ModelReduct.sbr" \
	"$(INTDIR)\ModelRegular.sbr" \
	"$(INTDIR)\ModelRemAccess.sbr" \
	"$(INTDIR)\ModelShadow.sbr" \
	"$(INTDIR)\ParseString.sbr" \
	"$(INTDIR)\Processor.sbr" \
	"$(INTDIR)\TraceLine.sbr"

"$(OUTDIR)\Presage.bsc" : "$(OUTDIR)" $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib ../Rater/Debug/Rater.lib ../../Zlib/Debug/Zlib.lib /nologo /subsystem:console /incremental:yes /pdb:"$(OUTDIR)\Presage.pdb" /debug /out:"$(OUTDIR)\Presage.exe" /pdbtype:sept 
LINK32_OBJS= \
	"$(INTDIR)\CallParams.obj" \
	"$(INTDIR)\Event.obj" \
	"$(INTDIR)\FuncCall.obj" \
	"$(INTDIR)\Interval.obj" \
	"$(INTDIR)\IntervalTemplate.obj" \
	"$(INTDIR)\ModelDArray.obj" \
	"$(INTDIR)\ModelInterval.obj" \
	"$(INTDIR)\ModelIO.obj" \
	"$(INTDIR)\ModelMPS_AM.obj" \
	"$(INTDIR)\ModelParLoop.obj" \
	"$(INTDIR)\ModelReduct.obj" \
	"$(INTDIR)\ModelRegular.obj" \
	"$(INTDIR)\ModelRemAccess.obj" \
	"$(INTDIR)\ModelShadow.obj" \
	"$(INTDIR)\ParseString.obj" \
	"$(INTDIR)\Predictor.obj" \
	"$(INTDIR)\Processor.obj" \
	"$(INTDIR)\PS.obj" \
	"$(INTDIR)\TraceLine.obj"

"$(OUTDIR)\Presage.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ENDIF 


!IF "$(NO_EXTERNAL_DEPS)" != "1"
!IF EXISTS("Presage.dep")
!INCLUDE "Presage.dep"
!ELSE 
!MESSAGE Warning: cannot find "Presage.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "Presage - Win32 Release" || "$(CFG)" == "Presage - Win32 Debug"
SOURCE=.\Src\CallParams.cpp

"$(INTDIR)\CallParams.obj"	"$(INTDIR)\CallParams.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\Src\Event.cpp

"$(INTDIR)\Event.obj"	"$(INTDIR)\Event.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\Src\FuncCall.cpp

"$(INTDIR)\FuncCall.obj"	"$(INTDIR)\FuncCall.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\Src\Interval.cpp

"$(INTDIR)\Interval.obj"	"$(INTDIR)\Interval.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\Src\IntervalTemplate.cpp

"$(INTDIR)\IntervalTemplate.obj"	"$(INTDIR)\IntervalTemplate.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\Src\ModelDArray.cpp

"$(INTDIR)\ModelDArray.obj"	"$(INTDIR)\ModelDArray.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\Src\ModelInterval.cpp

"$(INTDIR)\ModelInterval.obj"	"$(INTDIR)\ModelInterval.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\Src\ModelIO.cpp

"$(INTDIR)\ModelIO.obj"	"$(INTDIR)\ModelIO.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\Src\ModelMPS_AM.cpp

!IF  "$(CFG)" == "Presage - Win32 Release"

CPP_SWITCHES=/nologo /EHsc /O2 /I "../Presage/Include" /I "../Rater/Include" /I "../../Zlib/Include" /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /Fp"$(INTDIR)\Presage.pch"  /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\ModelMPS_AM.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "Presage - Win32 Debug"

CPP_SWITCHES=/nologo /EHsc /Zi /Od /I "../Presage/Include" /I "../Rater/Include" /I "../../Zlib/Include" /D "WIN32" /D "_CONSOLE" /D "_MBCS" /D "P_DEBUG" /Fp"$(INTDIR)\Presage.pch"  /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\ModelMPS_AM.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=.\Src\ModelParLoop.cpp

!IF  "$(CFG)" == "Presage - Win32 Release"

CPP_SWITCHES=/nologo /EHsc /O2 /I "../Presage/Include" /I "../Rater/Include" /I "../../Zlib/Include" /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /FR"$(INTDIR)\\" /Fp"$(INTDIR)\Presage.pch"  /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\ModelParLoop.obj"	"$(INTDIR)\ModelParLoop.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "Presage - Win32 Debug"

CPP_SWITCHES=/nologo /EHsc /Zi /Od /I "../Presage/Include" /I "../Rater/Include" /I "../../Zlib/Include" /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /D "P_DEBUG" /FR"$(INTDIR)\\" /Fp"$(INTDIR)\Presage.pch"  /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\ModelParLoop.obj"	"$(INTDIR)\ModelParLoop.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=.\Src\ModelReduct.cpp

"$(INTDIR)\ModelReduct.obj"	"$(INTDIR)\ModelReduct.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\Src\ModelRegular.cpp

"$(INTDIR)\ModelRegular.obj"	"$(INTDIR)\ModelRegular.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\Src\ModelRemAccess.cpp

"$(INTDIR)\ModelRemAccess.obj"	"$(INTDIR)\ModelRemAccess.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\Src\ModelShadow.cpp

"$(INTDIR)\ModelShadow.obj"	"$(INTDIR)\ModelShadow.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\Src\ParseString.cpp

"$(INTDIR)\ParseString.obj"	"$(INTDIR)\ParseString.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\Src\Predictor.cpp

!IF  "$(CFG)" == "Presage - Win32 Release"

CPP_SWITCHES=/nologo /EHsc /O2 /I "../Presage/Include" /I "../Rater/Include" /I "../../Zlib/Include" /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /Fp"$(INTDIR)\Presage.pch"  /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\Predictor.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "Presage - Win32 Debug"

CPP_SWITCHES=/nologo /EHsc /Zi /Od /I "../Presage/Include" /I "../Rater/Include" /I "../../Zlib/Include" /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /D "P_DEBUG" /Fp"$(INTDIR)\Presage.pch"  /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\Predictor.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=.\Src\Processor.cpp

"$(INTDIR)\Processor.obj"	"$(INTDIR)\Processor.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\Src\PS.cpp

!IF  "$(CFG)" == "Presage - Win32 Release"

CPP_SWITCHES=/nologo /EHsc /O2 /I "../Presage/Include" /I "../Rater/Include" /I "../../Zlib/Include" /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /FR"$(INTDIR)\\" /Fp"$(INTDIR)\Presage.pch"  /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\PS.obj"	"$(INTDIR)\PS.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "Presage - Win32 Debug"

CPP_SWITCHES=/nologo /EHsc /Zd /Od /I "../Presage/Include" /I "../Rater/Include" /I "../../Zlib/Include" /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /D "P_DEBUG" /Fp"$(INTDIR)\Presage.pch"  /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\PS.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=.\Src\TraceLine.cpp

"$(INTDIR)\TraceLine.obj"	"$(INTDIR)\TraceLine.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)



!ENDIF 

