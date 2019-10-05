@echo off
if exist dvmbin.bat (
  del dvmbin.bat
  echo "dvmbin.bat" removed
)
if exist dvmvers.h (
  del dvmvers.h
  echo "dvmvers.h" removed
)

cd cdvm
if "%OS%" == "Windows_NT" (
  if exist obj (
    rmdir /S /Q obj
    echo "cdvm/obj" directory removed
  )
)
if "%OS%" == "" deltree /y obj

cd ..\cdvmh
if exist src\*.obj (
  del src\*.obj
  echo "cdvmh/src/*.obj" removed
)

cd ..\fdvm
if "%OS%" == "Windows_NT" (
  if exist lib (
    rmdir /S /Q lib 
    echo "fdvm/lib" directory removed
  )
)
if "%OS%" == "" deltree /y lib
if "%OS%" == "Windows_NT" (
  if exist obj (
    rmdir /S /Q obj
    echo "fdvm/obj" directory removed
  )
)
if "%OS%" == "" deltree /y obj

cd ..\driver
if "%OS%" == "Windows_NT" (
  if exist obj (
    rmdir /S /Q obj 
    echo "driver/obj" directory removed
  )
)
if "%OS%" == "" deltree /y obj

cd ..\examples
if exist clean.bat call clean.bat
if exist *.obj (
  del *.obj
  echo "examples/obj" directory removed
)

cd ..\tools\pppa
if "%OS%" == "Windows_NT" (
  if exist obj (
    rmdir /S /Q obj  
    echo "tools/pppa/obj" directory removed
  )
)

if "%OS%" == "" deltree /y obj

cd ..\predictor
if "%OS%" == "Windows_NT" (
  if exist Rater\Debug (
    rmdir /S /Q Rater\Debug 
    echo "predictor/Rater/Debug" directory removed
  )
  if exist Presage\Debug (
    rmdir /S /Q Presage\Debug 
    echo "predictor/Presage/Debug" directory removed
  )
  if exist Trcutil\Debug (
    rmdir /S /Q Trcutil\Debug 
    echo "predictor/Trcutil/Debug" directory removed
  )

  
  if exist Rater\Release (
    rmdir /S /Q Rater\Release  
    echo "predictor/Rater/Release" directory removed
  )
  if exist Presage\Release (
    rmdir /S /Q Presage\Release 
    echo "predictor/Presage/Release" directory removed
  )
  if exist Trcutil\Release (
    rmdir /S /Q Trcutil\Release 
    echo "predictor/Trcutil/Release" directory removed
  )  
)

cd ..\Zlib
if "%OS%" == "Windows_NT" (
  if exist Release (
    rmdir /S /Q Release 
    echo "Zlib/Release" directory removed
  )
  if exist Debug (
    rmdir /S /Q Debug 
    echo "Zlib/Debug" directory removed
  )  
)

if "%OS%" == "" (
  deltree /y Rater\Debug 
  deltree /y Presage\Debug 
  deltree /y Trcutil\Debug 
  deltree /y ZLIB\Debug 
  deltree /y Rater\Release
  deltree /y Presage\Release
  deltree /y Trcutil\Release
  deltree /y ZLIB\Release
)


cd ..\..\rts
if "%OS%" == "Windows_NT" (
  if exist obj (
    rmdir /S /Q obj 
    echo "rts/obj" directory removed
  )
)
if "%OS%" == "" deltree /y obj


cd dvmh
if exist *.obj (
  del *.obj
  echo "rts/dvmh/*.obj" removed
)

if exist dynamic_include.h (
  del dynamic_include.h
  echo "rts/dvmh/dynamic_include.h" removed
)
cd ..\..
