
@echo off
rem @if "%MPI_HOME%" == "" goto err_no_MPICH

set npr=%1
set prname=%2
if exist %prname% goto startprog
if exist %prname%.exe goto startprog
echo Error: File %prname% or %prname%.exe not exist
rem echo Use  : mpirun.bat -np NP program
goto end
:startprog
if exist %MPI_HOME%\lib\*.dll set path=%path%;%MPI_HOME%\lib;
set arguments=
shift
:beg
shift
set arguments=%arguments% %1
if not "%1" == "" goto beg
set par_master=-n %npr% %prname% -- %arguments%
set par_client=-m LOCALHOST -- %arguments%
:clientcreate
if %npr% LEQ 1 goto startmaster
set /a npr = npr - 1
start /B %prname% %par_client%
if %npr% LEQ 1 goto startmaster
goto clientcreate
:startmaster
%prname% %par_master%
@exit /B 0

:err_no_MPICH
@echo ERROR: MPICH variable is not @set. 
@goto end

:end
