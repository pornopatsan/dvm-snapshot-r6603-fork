@echo off
rem if "%1" == "" goto print
set npr=%1
set prname=%2.exe
rem c:\NTMPICH\bin\mpirun -np %npr% %prname%
if "%MPI_HOME%" == "" mpirun -np %npr% %prname%
if not "%MPI_HOME%" == "" %MPI_HOME%\bin\mpirun -np %npr% %prname%
