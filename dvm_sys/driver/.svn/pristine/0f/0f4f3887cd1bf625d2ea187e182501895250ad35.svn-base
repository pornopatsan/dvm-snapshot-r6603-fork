set dvmdirW=%~1
echo /%dvmdirW%| %msys_root%\bin\sed s#:##g | %msys_root%\bin\sed s#\\#/#g >tmp1
set /P dvmdirU=<tmp1
del tmp1

rem dvm_settings.sh creation
cd bin
echo #!/bin/sh>dvmtmp

echo.>>dvmtmp
echo export DVMVERS='%DVMVERS%'>>dvmtmp
echo export PLATFORM='%PLATFORM_SAVE%'>>dvmtmp

echo.>>dvmtmp
set DEF_CONV_OPTS=-e2
if not defined NVCC set DEF_CONV_OPTS=%DEF_CONV_OPTS% -noCuda
set FTN_ADD=
if "%MACHINE%" == "x64" set FTN_ADD=-t8
echo export optcconv="$OPTCCONV">>dvmtmp
echo if [ -z "$optcconv" ]; then export optcconv='%DEF_CONV_OPTS%'; fi>>dvmtmp
echo export optfconv="$OPTFCONV">>dvmtmp
echo if [ -z "$optfconv" ]; then export optfconv='%DEF_CONV_OPTS% %FTN_ADD%'; fi>>dvmtmp

echo.>>dvmtmp
echo if [ -z "$PCC" ]; then export PCC='%CC%'; fi>>dvmtmp
echo export PCC="$PCC %MS_F%">>dvmtmp
echo if [ -z "$PCXX" ]; then export PCXX='%CXX%'; fi>>dvmtmp
echo export PCXX="$PCXX %MS_F%">>dvmtmp
echo if [ -z "$PFORT" ]; then export PFORT='%FORT%'; fi>>dvmtmp
if defined NVCC (
    echo if [ -z "$NVCC" ]; then export NVCC='%NVCC%'; fi>>dvmtmp
    echo export NVCC="$NVCC %MS_F%">>dvmtmp
    if defined NVFORT (
        echo export PGI_PATH='%PGI_PATH%'>>dvmtmp
        echo if [ -z "$NVFORT" ]; then export NVFORT='%NVFORT%'; fi>>dvmtmp
    )
)
if defined BINCLANG echo export CLANG='%BINCLANG%clang.exe'>>dvmtmp

echo.>>dvmtmp
echo export PLINKER='%LINKER%'>>dvmtmp
echo export PFLINKER='%FLINKER%'>>dvmtmp
echo export Pld='ld'>>dvmtmp
echo export CUDA_LIB=''>>dvmtmp
echo export ADD_LIBS="$ADD_LIBS %WINLIB%">>dvmtmp

echo.>>dvmtmp
echo export dvmwait="$DVMWAIT">>dvmtmp
echo if [ "$dvmwait" != 0 -a "$dvmwait" != 1 ]; then export dvmwait=0; fi>>dvmtmp

echo.>>dvmtmp
echo export dvmsave="$DVMSAVE">>dvmtmp
echo if [ "$dvmsave" != 0 -a "$dvmsave" != 1 ]; then export dvmsave=0; fi>>dvmtmp
echo export dvmshow="$DVMSHOW">>dvmtmp
echo if [ "$dvmshow" != 0 -a "$dvmshow" != 1 ]; then export dvmshow=0; fi>>dvmtmp

echo.>>dvmtmp
echo export dvmrun='%dvmrun%'>>dvmtmp
echo export flib='%flib%'>>dvmtmp
echo export OPTIONCHAR='-'>>dvmtmp
echo export dvmpar='%dvmdirW%\par\.rel'>>dvmtmp
echo export usrpar=''>>dvmtmp
echo export dvmout='off'>>dvmtmp
echo export Pred_sys='%dvmdirW%\par\Predictor.par'>>dvmtmp
echo export Pred_vis='start'>>dvmtmp
echo export Doc_vis='start'>>dvmtmp
move dvmtmp dvm_settings.sh
cd ..

rem dvm_settings.bat creation
cd bin
echo @Echo Off>tmp

echo.>>tmp
echo set SETVARS=%SETVARS%>>tmp
echo set ARGSETVARS=%ARGSETVARS%>>tmp
echo set SETVARSFTN=%SETVARSFTN%>>tmp
echo set ARGSETVARSFTN=%ARGSETVARSFTN%>>tmp
echo set MPI_BIN=%MPI_BIN%>>tmp
if defined NVCC (
   echo set CUDA_PATH=%CUDA_PATH%>>tmp
)
echo.>>tmp
echo set msys_root=%msys_root%>>tmp
echo set WIN32=1 >>tmp
echo set dvmdir=%dvmdirU%>>tmp
echo set dvmdirW=%dvmdirW%>>tmp
move tmp dvm_settings.bat
cd ..

rem dvm.bat creation
cd user
echo @Echo Off>dvm.bat

echo.>>dvm.bat
echo set dvmdir=%dvmdirW%>>dvm.bat

echo.>>dvm.bat
echo rem --------------- One can customize compiler options:>>dvm.bat
echo rem set "PCC=%CC%" :: C compiler>>dvm.bat
echo rem set "PCXX=%CXX%" :: C++ compiler>>dvm.bat 
echo rem set "PFORT=%FORT%" :: Fortran compiler>>dvm.bat 
if defined NVCC echo rem set "NVCC=%NVCC%" :: NVIDIA CUDA C++ compiler>>dvm.bat
if defined NVFORT echo rem set "NVFORT=%NVFORT%" :: Fortran-CUDA compiler>>dvm.bat

echo.>>dvm.bat
echo rem --------------- One can add libraries (additional linker flags):>>dvm.bat
echo rem set ADD_LIBS=>>dvm.bat

echo.>>dvm.bat
echo rem --------------- DVMH options:>>dvm.bat
echo rem set "DVMH_PPN=" :: Number of processes per node>>dvm.bat
if defined DVMH_NUM_THREADS ( 
    echo set "DVMH_NUM_THREADS=%DVMH_NUM_THREADS%" :: Number of CPU threads per process>>dvm.bat
) else (
    echo set "DVMH_NUM_THREADS=1" :: Number of CPU threads per process>>dvm.bat
)
if defined DVMH_NUM_CUDAS (
    echo set "DVMH_NUM_CUDAS=%DVMH_NUM_CUDAS%" :: Number of GPUs per process>>dvm.bat
) else (
    echo set "DVMH_NUM_CUDAS=0" :: Number of GPUs per process>>dvm.bat
)
echo rem set "DVMH_CPU_PERF=" :: Performance of all cores of CPU per process>>dvm.bat
echo rem set "DVMH_CUDAS_PERF=" :: Performance of each GPU per device>>dvm.bat
echo rem set "DVMH_NO_DIRECT_COPY=0" :: Use standard cudaMemcpy functions instead direct copy with GPU>>dvm.bat
echo rem set "DVMH_SPECIALIZE_RTC=1" :: Use specialization aligorithm to reduce CUDA kernel's resources / or compile kernels during execution without changes>>dvm.bat

echo.>>dvm.bat
echo rem --------------- Debugging options:>>dvm.bat
echo rem set "DVMH_LOGLEVEL=1" :: Levels of debugging: 1 - errors only, 2 - warning, 3 - info, 4 - debug, 5 - trace>>dvm.bat
echo rem set "DVMH_LOGFILE=dvmh_%%d.log" :: Log file name for each process>>dvm.bat
echo rem set "DVMH_COMPARE_DEBUG=0" :: An alternative way to turn comparative debugging mode on>>dvm.bat
echo rem set "dvmsave=0" :: Save convertation results>>dvm.bat
echo rem set "dvmshow=0" :: Show executed commands>>dvm.bat

::echo.>>dvm.bat
::echo rem --- Options below are likely to be removed --->>dvm.bat

::echo.>>dvm.bat
::echo rem --------------- CUDA profiling options:>>dvm.bat
::echo rem set "CUDA_PROFILE=0" :: Enable/disable CUDA profiling>>dvm.bat
::echo rem set "CUDA_PROFILE_CONFIG=cuda.conf" :: File with GPU's metrics>>dvm.bat
::echo rem set "CUDA_PROFILE_LOG=cuda_profile.%%d.%%p" :: Output file name for each process>>dvm.bat
::echo rem set "CUDA_PROFILE_CSV=1" :: Set CSV output format>>dvm.bat

echo.>>dvm.bat
echo call "%%dvmdir%%\bin\dvm_drv.bat" %%*>>dvm.bat
cd ..

rem more predefined presets
rem dvm-mpi.bat creation
cd user
echo @Echo Off>dvmtmp

echo.>>dvmtmp
echo set "DVMH_PPN=" :: Number of processes per node>>dvmtmp
echo set "DVMH_NUM_THREADS=1" :: Number of CPU threads per process>>dvmtmp
echo set "DVMH_NUM_CUDAS=0" :: Number of GPUs per process>>dvmtmp

echo.>>dvmtmp
echo call "%dvmdirW%\bin\dvm_drv.bat" %%*>>dvmtmp
move dvmtmp dvm-mpi.bat
cd ..

rem dvm-omp.bat creation
cd user
echo @Echo Off>dvmtmp

echo.>>dvmtmp
echo set "DVMH_PPN=" :: Number of processes per node>>dvmtmp
echo set "DVMH_NUM_THREADS=" :: Number of CPU threads per process>>dvmtmp
echo set "DVMH_NUM_CUDAS=0" :: Number of GPUs per process>>dvmtmp

echo.>>dvmtmp
echo call "%dvmdirW%\bin\dvm_drv.bat" %%*>>dvmtmp
move dvmtmp dvm-omp.bat
cd ..

if defined NVCC (
    rem dvm-cuda.bat creation
    cd user
    echo @Echo Off>dvmtmp

    echo.>>dvmtmp
    echo set "DVMH_PPN=" :: Number of processes per node>>dvmtmp
    echo set "DVMH_NUM_THREADS=0" :: Number of CPU threads per process>>dvmtmp
    echo set "DVMH_NUM_CUDAS=" :: Number of GPUs per process>>dvmtmp

    echo.>>dvmtmp
    echo call "%dvmdirW%\bin\dvm_drv.bat" %%*>>dvmtmp
    move dvmtmp dvm-cuda.bat
    cd ..
)
