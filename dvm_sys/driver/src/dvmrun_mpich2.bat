@echo off
rem if "%1" == "" goto print
set npr=%1
set prname=%2
if exist %prname% goto startprog
if exist %prname%.exe goto startprog
echo Error: File %prname% not exist
rem goto end
:startprog
set arguments=-opf10000 
shift
:beg
shift
set arguments=%arguments% %1
if not "%1" == "" goto beg
if "%dvmshow%"=="1" echo call %MPI_HOME%\Bin\mpiexec -n %npr% -localonly -genvlist PATH %prname% %arguments%
call %MPI_HOME%\Bin\mpiexec -n %npr% -localonly -genvlist PATH %prname% %arguments%
:end
goto finish
:print
echo Use the following command:
echo ______________________________________________________________________
echo dvm run [n1 [n2 [n3 ...]]] program [args]
echo Where:
echo n1xn2xn3 - matrix of processors
echo ______________________________________________________________________
:finish