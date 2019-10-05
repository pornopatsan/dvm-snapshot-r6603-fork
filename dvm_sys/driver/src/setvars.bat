if not "%isVARS%" == "" goto :eof
rem Set Fortran environment
call "%SETVARSFTN%" %ARGSETVARSFTN% >nul
rem Set C++ environment
if exist "%SETVARS%" call "%SETVARS%" %ARGSETVARS% >nul 
rem Set PATH to MPI
if defined MPI_BIN set PATH=%MPI_BIN%;%PATH%
if defined CUDA_PATH set PATH=%CUDA_PATH%\bin;%PATH%
set isVARS=yes
