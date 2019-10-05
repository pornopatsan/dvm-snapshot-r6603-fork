@echo off
call dvm %1 %2%3
if not exist %2.exe goto bad
call dvm runpred %2
if not exist %2.ptr goto bad
if exist %2_2_2.html del %2_2_2.html
call dvm pred 2 2 %2
if not exist %2_2_2.html goto bad
del %2.%1
del %2.exe
del %2.obj
del %2.ptr
del %2_2_2.log
goto exit
:bad
echo ... to abort type "Ctrl+C" ...
pause
:exit
@echo on


