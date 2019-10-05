if "%npr%" == "%nprc%" goto endcfg
set /a nprc=nprc + 1
if "%dvmredirect%" == "0" goto noredir
set redir=-stdout dvm_%nprc%.out -stderr dvm_%nprc%.err
:noredir
for /f %%i in ('cd') do set dir=%%i
set cons=-noconsole
if "%COMPUTERNAME%" == "%1" set cons=
echo %1 %cons% %dir%\%prname% %par_fm% %redir% >> dvm_mpi.cfg 
:endcfg
