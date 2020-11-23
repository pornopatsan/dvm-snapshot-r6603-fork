@echo off
set npr=%1
set prname=%2.exe
if exist %prname% goto startprog
echo Error: File %prname% not exist
goto end
:startprog
set arguments=-opf10000 
getnumber.exe
set key=%errorlevel%
shift
:beg
shift
set arguments=%arguments% %1
if not "%1" == "" goto beg
rem echo %arguments%

set par_fm=-np %npr% -key %key% dvm %arguments%
set mlist=machinelist
if not exist machinelist set mlist=%dvmdir%\user\machinelist

for /f %%i in (%mlist%) do set /a nlist=nlist + 1
if %npr% LEQ %nlist% goto endnlist
echo Error: Number of processors more then %nlist% (See %mlist%)
goto end
:endnlist

if exist dvm_mpi.cfg del dvm_mpi.cfg
for /f %%i in (%mlist%) do call dvmrun_fm1.bat %%i

echo hmpirun dvm_mpi.cfg
hmpirun dvm_mpi.cfg
:end

