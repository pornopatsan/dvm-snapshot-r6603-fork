@echo off
rem if "%1" == "" goto print
set npr=%1
set prname=%2.exe
set nolocal=1
set mfile=0
set mlist=machinelist
rem set MPICH=H:\Mpich\MP_Mpich\bin
set MPICH=Z:\NTMpich\bin
if not exist machinelist set mlist=%dvmdir%\user\machinelist
if not exist %mlist% set mlist=%MPICH%\machines.txt
if exist %prname% goto startprog
echo Error: File %prname% not exist
rem goto end
:startprog
set arguments=-opf10000 
shift
:beg
shift
if "%1" == "-nolocal" (
	set nolocal=1
	goto beg
)
if "%1" == "-mf"  (
	set mfile=1
	goto beg
)
if "%mfile%" == "1" (
	set mlist=%1
 	set mfile=0
	if not "%1" == "" goto beg
)
set arguments=%arguments% %1
if not "%1" == "" goto beg
if "%nolocal%" == "0" set par_master=-domain IAMREAL -user dvmuser -host LOCALHOST -n %npr% %prname% dvm %arguments%
if "%nolocal%" == "1" set par_master=-domain IAMREAL -user dvmuser -n %npr% -machinefile %mlist% %prname% dvm %arguments%
if "%dvmshow%" == "1" echo %MPICH%\mpiexec %par_master%
call %MPICH%\mpiexec %par_master%
:end
goto finish
:print
echo Use the following command:
echo ______________________________________________________________________
echo dvm run [n1 [n2 [n3 ...]]] program [-nolocal] [-mf machinefile] [args]
echo Where:
echo n1xn2xn3 - matrix of processors
echo -nolocal - run program on availiable machines
echo -mf      - name of machinelist file
echo ______________________________________________________________________
:finish