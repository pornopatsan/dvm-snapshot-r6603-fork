@echo off
rem set TAGET=RELEASE
set TAGET=DEBUG
if "%SETVARS%" == "" goto env
goto mak
:env
SET  SETVARSVC=Z:\Program Files\Microsoft Visual Studio\VC98\Bin\vcvars32.bat

rem =================  Visual C++ =======================
rem - Lines added by MS  Visual C++  on 2-18-1999
@CALL "%SETVARSVC%"
rem - End of lines added by Visual C++ 5.0 Setup

rem  ============== Starting cleaning...   ===============
:mak
if "%TAGET%" == "RELEASE" nmake /A /nologo /f makefile.win cleanr
if errorlevel 1 goto err
if "%TAGET%" == "DEBUG" nmake /A /nologo /f makefile.win cleand
if errorlevel 1 goto err
rem  ======== Predictor is successfuly cleaned ===========
echo "****** Predictor is successfuly cleaned. ******"
goto end
:err
echo "*** There were some errors during Predictor cleaning. ***"
:end	
