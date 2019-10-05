if "%1" == "" goto remove
if exist dvm.bat     del dvm.bat
if exist usr.par     del usr.par
:remove
if exist cdvm_msg    del cdvm_msg
if exist config.mpi  del config.mpi
if exist current.par del current.par
if exist demo.*      del demo.* 
if exist dvm.err     del dvm.err
if exist error.*     del error.*
if exist sts         del sts
if exist *.c         del *.c
if exist *.dep       del *.dep
if exist *.exe       del *.exe
rem if exist *.e??       del *.e??
if exist *.f         del *.f
if exist *.lst       del *.lst
if exist *.obj       del *.obj
if exist *.out       del *.out
if exist *.proj      del *.proj
if exist *.tr*       del *.tr*
