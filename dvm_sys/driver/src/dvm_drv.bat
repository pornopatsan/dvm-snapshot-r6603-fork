@Echo Off

call "%~dp0\dvm_settings.bat"
call "%~dp0\setvars.bat"

set DVM_ARGS=%*
call "%msys_root%\bin\sh" -c "'%dvmdir%/bin/dvm_drv'"
