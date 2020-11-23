@echo off

if "%OS%" == "Windows_NT" goto WNT
deltree /y bin
deltree /y demo
deltree /y include
deltree /y lib
deltree /y obj
deltree /y par
deltree /y user
deltree /y doc
goto rest

:WNT
if exist bin ( 
  rmdir /S /Q bin 
  echo "bin" directory removed
)
if exist demo (
  rmdir /S /Q demo 
  echo "demo" directory removed
)
if exist include (
  rmdir /S /Q include
  echo "include" directory removed
)
if exist lib (
  rmdir /S /Q lib 
  echo "lib" directory removed
)
if exist obj (
  rmdir /S /Q obj 
  echo "obj" directory removed
)
if exist par (
  rmdir /S /Q par 
  echo "par" directory removed
)
if exist user (
  rmdir /S /Q user 
  echo "user" directory removed
)
if exist doc (
  rmdir /S /Q doc
  echo "doc" directory removed
)
:rest
call dvmclean.bat
