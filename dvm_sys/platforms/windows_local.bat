:: ****************************************************************************************************
:: ****************************************************************************************************
:: ****************** SET  PARAMETERS  FOR  INSTALLING DVM-SYSTEM HERE ********************************
:: ****************************************************************************************************
:: ****************************************************************************************************
:: ****************************************************************************************************

rem Uncomment MACHINE=x64 setting to compile system in 64-bit mode (only for 64-bit windows)
rem ===================== Set machine configuration ========================================
::set MACHINE=x86
set MACHINE=x64
rem === number of cores 
set DVMH_NUM_THREADS=4
rem === number of GPUs 
set DVMH_NUM_CUDAS=1

rem set for PGI FORTRAN compiler
set MS_F=-D_PGI_F_
rem set for INTEL FORTRAN compiler
::set MS_F=-D_MS_F_


set msys_root=C:\MinGW\msys\1.0
set CC=cl.exe
:: -openmp
set CXX=cl.exe 
::-openmp
set LINKER=link.exe
set FORT=pgfortran.exe
set FFLAGS=-Miface:nomixed_str_len_arg
:: -Minfo=mp
set FLINKER=pgfortran.exe
:: -Minfo=mp

:: set stack size 512 MB
::-F536870912 stack size for INTEL FORTRAN

set MAKE=nmake.exe
set WINLIB=advapi32.lib

rem ========================== One can set NVCC or not =====================================
set NVCC=nvcc.exe -arch=compute_30 -code=sm_30,sm_35,sm_50,sm_60,sm_70 -O3 
if %MACHINE%==x86 set NVCC=%NVCC% --machine=32

rem ========================== Set LLVM PATHS for building C-DVMH ==========================
rem ===== BUILD directory
::set LLVMBuild=C:/Users/~/Desktop/build/

rem ===== SVN checkout directory
::set LLVMPATH=C:/Users/~/Desktop/llvm/

rem ===== Release directory
::set RELEASE=MinSizeRel

::set LIBCLANG=%LLVMBuild%lib/%RELEASE%/
::set BINCLANG=%LLVMBuild%bin/%RELEASE%/
::set LLVMINCLUDE=/I%LLVMBuild%tools/clang/include /I%LLVMPATH%tools/clang/include /I%LLVMPATH%include /I%LLVMBuild%include
::if exist %BINCLANG%clang.exe set CC=%BINCLANG%clang.exe

rem set MPI Paths
set DVMPLATFORM=msmpi
set MPI_HOME=C:\Program Files\Microsoft MPI
set MPI_INC=%MPI_HOME%\Include
set MPI_LIB32=%MPI_HOME%\Lib\x86
set MPI_LIB64=%MPI_HOME%\Lib\x64
set MPI_BIN=%MPI_HOME%\Bin

rem ================== set version of cl.exe compiler (Visual Studio) =======================
rem ================== Visual Studio 2010
::set VSVER=vs2010
rem ================== Visual Studio 2012
::set VSVER=vs2012
rem ================== Visual Studio 2013
::set VSVER=vs2015


rem ===================  Intel Fortran 9.0 ===========================
rem set SETVARSFTN=C:\Program Files\Intel\Compiler\Fortran\9.0\IA32\Bin\ifortvars.bat
rem set ARGSETVARSFTN=

rem ===================  Intel Fortran 9.1 ===========================
rem set SETVARSFTN=C:\Program Files\Intel\Compiler\Fortran\9.1\IA32\Bin\ifortvars.bat
rem set ARGSETVARSFTN=

rem ===================  Intel Fortran 11.1.038 ===========================
rem set SETVARSFTN=C:\Program Files\Intel\Compiler\11.1\038\bin\ia32\ifortvars_ia32.bat
rem set SETVARSFTN=C:\Program Files\Intel\Compiler\11.1\038\bin\ifortvars.bat
rem set ARGSETVARSFTN=ia32

rem ===================  Intel Fortran 14 ===========================
::set SETVARSFTN=%IFORT_COMPILER14%bin\ifortvars.bat
::if %MACHINE%==x86 set ARGSETVARSFTN=ia32 %VSVER%
::if %MACHINE%==x64 set ARGSETVARSFTN=intel64 %VSVER%

rem ===================  PGI Fortran 17.10 ===========================
set SETVARSFTN=C:\Program Files\PGICE\win64\17.10\pgi_env.bat
set ARGSETVARSFTN=


rem =================== C++ .NET 2008_expr ===========================
rem set SETVARSVC=C:\Program Files\Microsoft Visual Studio 9.0\Common7\Tools\vsvars32.bat
rem set ARGSETVARSVC=
rem =================== C++ .NET 2008 ===========================
rem set SETVARSVC=%VS90COMNTOOLS%..\..\VC\vcvarsall.bat
rem set ARGSETVARSVC=%MACHINE%
rem =================== C++ .NET 2010 ===========================
rem set SETVARSVC=%VS100COMNTOOLS%..\..\VC\vcvarsall.bat
rem set ARGSETVARSVC=%MACHINE%
rem =================== C++ .NET 2012 ===========================
rem set SETVARSVC=%VS110COMNTOOLS%..\..\VC\vcvarsall.bat
rem set ARGSETVARSVC=%MACHINE%
rem =================== C++ .NET 2013 ===========================
::set SETVARSVC=%VS120COMNTOOLS%..\..\VC\vcvarsall.bat
::set ARGSETVARSVC=%MACHINE%

rem =================== PERL path ========================
::set PERL=C:\Perl64\bin\perl.exe

:: ****************************************************************************************************
:: ****************************************************************************************************
:: ****************************  END OF PARAMETERS ****************************************************
:: ****************************************************************************************************
:: ****************************************************************************************************
:: ****************************************************************************************************
