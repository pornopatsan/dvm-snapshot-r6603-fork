@echo off
set DVMVERS=5.0

rem ##################################################
rem ####### PARSE ARGUMENTS AND SETUP PLATFORM #######
rem ##################################################

goto :parse_args

:print_usage
echo.usage:
echo.  %~n0 [OPTIONS]
exit /b 0

:print_help
rem ## print help and exit ##

call :print_usage
echo.
echo.  This script installs DVM-system. It should be executed from DVM-system directory. Files out of this directory will not be modified.
echo.You can specify platform by setting PLATFORM variable in your environment.
echo.
echo.OPTIONS
echo.  /?, --help            Prints this help and exit.
echo.  -p ^<platfrom_name^>    Set PLATFORM=^<platform_name^> for this script.
echo.
exit 0

:wrong_arg
echo.Wrong argument "%1".
call :print_usage
echo.Try %~n0 --help for help.
exit 1

rem ## parse argumetns ##
:parse_args
set key=%1
if not "%key%" == "" set key=%key:/=-%
if .%key% == .--help goto :print_help
if "%key%" == "-?"     goto :print_help
if "%key%" == "-p" (
  if "%2" == "" goto wrong_arg
  set PLATFORM=%2
  shift & shift
  goto :parse_args
)
if not "%key%" == "" goto wrong_arg

rem Set default platform
if not defined PLATFORM set PLATFORM=windows_local
set PLATFORM_SAVE=%PLATFORM%

if exist platforms\%PLATFORM%.bat (
  CALL  platforms\%PLATFORM%.bat
) else (
  echo PLATFORM %PLATFORM%.bat not found
  exit 1 )

rem ##### SET FORTRAN and C compilers #####
call :set_fortran || exit /b 1
if "%VSVER%" == "" (call :set_c || exit /b 1)

rem ##### START dvm install #####
goto start_dvm_install

    :set_fortran
    if not "%SETVARSFTN%" == "" goto iscomp
    call :detect_ifort && goto iscomp

    :iscomp
    if exist "%SETVARSFTN%" (
        CALL "%SETVARSFTN%" %ARGSETVARSFTN%
        exit /b 0
    )
    echo !!! Variable SETVARSFTN = %SETVARSFTN%
    echo !!! File "%SETVARSFTN%" not exist
    echo !!! Fortran compiler is not detected; set variable SETVARSFTN !!!
    exit /b 1

    :set_c
    if not "%SETVARSVC%" == "" goto iscompc
    call :detect_visual_studio && goto iscompc

    :iscompc
    if  exist "%SETVARSVC%" (
       CALL "%SETVARSVC%" %ARGSETVARSVC%
       exit /b 0
    )
    echo !!! Variable SETVARSVC = %SETVARSVC%
    echo !!! File "%SETVARSVC%" not exist
    echo !!! C compiler is not detected; set variable SETVARSVC !!!
    exit /b 1

:start_dvm_install
rem ####### Starting DVM installation #########

if "%DVMPLATFORM%" == "" (    
    echo !!! MPI is not set
    exit /b 1
) else (    
    if not exist "%MPI_INC%\mpi.h" (
        echo !!! MPI %DVMPLATFORM% is not found: file "%MPI_INC%\mpi.h" is not exist 
        exit /b 1
    ) else (
        if "%MACHINE%" == "x86" set MPI_LIB="%MPI_LIB32%\msmpi.lib"
        if "%MACHINE%" == "x64" set MPI_LIB="%MPI_LIB64%\msmpi.lib"
        set OPTCC=
        set OPTCXX=
        set OPTFORT=
        set OPTLINKER=
        set OPTFLINKER=
        set MPSOPT=-D_WIN_MPI_ -D_NT_MPI_ -D_MPI_PROF_EXT_ -D_DVM_STAT_ -DMPICH_IGNORE_CXX_SEEK
        set dvmrun=mpiexec.run
    )
)

echo ============= PLATFORM ================
echo Platform name = %PLATFORM_SAVE%
echo Selected follow components:
echo    Machine           is "%MACHINE%"
echo    MPI               is "%DVMPLATFORM%"
if defined NVCC echo    Nvidia compiler   is "nvcc.exe"
if defined LLVMINCLUDE echo    LLVM              is "clang.exe"
echo    C   compiler      is "%CC%"
echo    C++ compiler      is "%CXX%"
echo    Ifort compiler    is "%FORT%"
echo    CLinker           is "%LINKER%"
echo    FLinker           is "%FLINKER%"
echo =======================================

set CC=%CC% %OPTCC%
set CXX=%CXX% %OPTCXX%
set LINKER=%LINKER% %OPTLINKER%
set FORT=%FORT% %FFLAGS% %OPTFORT%
set FLINKER=%FLINKER% %OPTFLINKER%
set MAKE=%MAKE% /nologo

if not "%dirTRANLIB%" == "" set MPSOPT=%MPSOPT% -D_MPI_PROF_TRAN_
set MPSOPT=%MPSOPT% -D_COPY_FILE_NAME_ -D_DVM_MPI_ -D_DVM_MSC_

rem ####### create empty subdirectories #########
if not exist bin mkdir bin
if not exist include mkdir include
if not exist lib mkdir lib
if not exist par mkdir par

cd cdvm
if not exist obj mkdir obj
cd ..

cd fdvm
if not exist lib mkdir lib
if not exist obj mkdir obj
cd ..

cd driver
if not exist obj mkdir obj
cd ..

cd tools\pppa
if not exist obj mkdir obj
cd ..\..

cd rts
if not exist obj mkdir obj
cd ..

cd lib
SET LIBDIR=%CD%
cd ..
cd bin
SET BINDIR=%CD%
cd ..
cd include
SET INCDIR=%CD%
cd ..

if not exist dvmvers.h (
    echo #define VERS "%DVMVERS%" > dvmvers.h
    echo #define PLATFORM "%DVMPLATFORM%" >> dvmvers.h
)
copy dvmvers.h rts\include\dvmvers.h >nul
copy dvmvers.h tools\pppa\src\dvmvers.h >nul

if defined NVCC (
    if exist %CUDA_PATH%\include\nvrtc.h (
        set HAVE_NVRTC=1
    )
)

set ZLIB=-D_RTS_ZLIB_

rem ######### DVM-system compilation #########
%MAKE% -f makefile.win all
if errorlevel 1 exit /b 1

if exist tools\predictor\Presage\Release\Presage.exe copy tools\predictor\Presage\Release\Presage.exe bin\predictor.exe > nul
if exist tools\predictor\Predictor.par copy tools\predictor\Predictor.par par\Predictor.par >nul
if exist tools\predictor\Trcutil\Release\trcutil.exe copy tools\predictor\Trcutil\Release\trcutil.exe bin\dvmdbgerr.exe > nul
copy rts\dvmh\Win\dll\%MACHINE%\pthreadVC2.dll bin\pthreadVC2.dll >nul
if not exist user\* mkdir user
copy par\usr.par user\usr.par > nul

rem  ######### dvm files creation #########
call driver\src\dvmcreate.bat "%CD%"

if not exist user\dvm.bat (
    echo.
    echo DVM-system is not installed
    exit /b 1
)

if not exist demo\* mkdir demo
cd demo 
if not exist Adi mkdir Adi
if not exist Gauss mkdir Gauss
if not exist Jacobi mkdir Jacobi
if not exist Sor mkdir Sor
if not exist Performance mkdir Performance
if not exist RedBlack mkdir RedBlack
cd ..

copy examples\Adi\*.* demo\Adi\ > nul
copy examples\Gauss\*.* demo\Gauss\ > nul
copy examples\Jacobi\*.* demo\Jacobi\ > nul
copy examples\Sor\*.* demo\Sor\ > nul
copy examples\Performance\*.* demo\Performance\ > nul
copy examples\RedBlack\*.* demo\RedBlack\ > nul

copy user\dvm.bat demo\dvm.bat > nul
copy user\usr.par demo\usr.par > nul

echo call "%SETVARS%" %ARGSETVARS%> ftmp
echo set MPI_HOME=%MPI_HOME%>> ftmp
echo set MPI_LIB=%MPI_LIB%>> ftmp
echo set dvmrun=%dvmrun%>> ftmp
copy ftmp + dvmbin.win dvmbin.bat > nul
del ftmp>nul

echo.
echo DVM-system is successfuly installed
exit 0


rem ##### PROCEDURES #####

:detect_ifort
setlocal EnableDelayedExpansion
set ver=0
for /L %%i in (12, 1, 30) do (
  if defined IFORT_COMPILER%%i (
    set ver=%%i
  )
)
if %ver% equ 0 ( endlocal & exit /b 1 )
set p_ret=!IFORT_COMPILER%ver%!bin\ifortvars.bat
echo Intel Fortran %ver% detected
endlocal & set SETVARSFTN=%p_ret%
if .%MACHINE% == .x86 set ARGSETVARSFTN=ia32 quiet %VSVER%
if .%MACHINE% == .x64 set ARGSETVARSFTN=intel64 quiet %VSVER%
exit /b 0


:detect_visual_studio
setlocal EnableDelayedExpansion
set ver=0
for /L %%i in (9, 1, 30) do (
  if defined VS%%i0COMNTOOLS (
    set ver=%%i
  )
)
if %ver% equ 0 ( endlocal & exit /b 1 )
echo Microsoft Visual Studio %ver% detected
set p_ret=!VS%ver%0COMNTOOLS!\..\..\VC\vcvarsall.bat
endlocal & set SETVARSVC=%p_ret%
set ARGSETVARSVC=%MACHINE%
exit /b 0

:trim_trailing_slash
if .%1 == . goto _end_trim_trailing_slash
setlocal enableDelayedExpansion
if .!%1:~-1! == .\ set %1=!%1:~0,-1!
set val=!%1!
endlocal & set %1=%val%
shift
goto trim_trailing_slash
:_end_trim_trailing_slash
exit /b 0

rem call :is_abs_path "path"
rem sets errorlevel 0 if path is absolute
:is_abs_path
if "%~1" == "%~dpn1" exit /b 0
exit /b 1

rem call :get_parent_dir out_var "path"
rem sets out_var=parent_directory_path
:get_parent_dir
if "%~2" == "" exit /b 1
setlocal enableDelayedExpansion
call :is_abs_path "%~2"
if errorlevel 1 (
  call :_get_parent_dir par_dir "\%~2"
  set par_dir=!par_dir:~3!
) else (
  call :_get_parent_dir par_dir "%~2"
)
endlocal & set %1=%par_dir%
call :trim_trailing_slash %1
exit /b 0

:_get_parent_dir
setlocal
set dir=%~2
if .%dir:~-1% == .\ (
  call :get_parent_dir par_dir "%dir:~0,-1%"
) else set par_dir=%~dp2
endlocal & set %1=%par_dir%
exit /b 0
