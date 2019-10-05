@echo off
echo ... DVM testing started ...
set Pred_vis_blk=on

set TESTPROG=testcdvm.cdv
echo ... %TESTPROG% testing  ...
if exist test.1 del test.1
set dvmoutfile=test.1
call dvm ctest %TESTPROG%
if exist dvm.err goto exit
echo ... Compare %TESTPROG% results ...
..\bin\dvmdiff test.1 testcdvm_res
if not errorlevel 1 goto endcdvm
echo ... Error in testing %TESTPROG% ...
echo ... to abort type "Ctrl+C" ...
echo ... to continue?
pause
goto fdvm
:endcdvm
if exist comp.err del comp.err
if exist dvmstd.err del dvmstd.err
if exist dvmstd.out del dvmstd.out
if exist test.1 del test.1
echo ... End of testing %TESTPROG% ...

:fdvm
set TESTPROG=testfdvm.fdv
echo ... %TESTPROG% testing ...
if exist test.2 del test.2
set dvmoutfile=test.2
if not exist ..\bin\f_dvm.exe goto nofdvm
if not exist ..\bin\parse.exe goto nofdvm
call dvm ftest testfdvm
if exist dvm.err goto exit
echo ... Compare %TESTPROG% results ...
..\bin\dvmdiff test.2 testfdvm_res
if not errorlevel 1 goto endfdvm
echo ... Error in testing %TESTPROG% ...
echo ... to abort type "Ctrl+C" ...
echo ... to continue?
pause
goto pdvm
:nofdvm
echo ... Fortran DVM is not installed ...
goto pdvm
:endfdvm
if exist comp.err del comp.err
if exist dvmstd.err del dvmstd.err
if exist dvmstd.out del dvmstd.out
if exist config.mpi del config.mpi
if exist test.2 del test.2
echo ... End of testing %TESTPROG% ...

:pdvm
echo ... Predictor testing ....
if exist ..\bin\predictor.exe goto pred
echo ... Predictor is not installed ...
goto exit
:pred
set dvmoutfile=test3
rem call dvm c jac1d
rem call dvm runpred jac1d
call dvm pred 2 2 jac1d
if exist dvm.err goto exit
echo ... Compare Predictor results ...
..\bin\dvmdiff jac1d_2_2.html pred_res
if not errorlevel 1 goto endtest
echo ... Error in testing Predictor ...
goto endtest1
:endtest
echo ... End of testing Predictor ...
echo ... end of testing DVM ...
:endtest1
pause
if exist comp.err del comp.err
if exist dvmstd.err del dvmstd.err
if exist dvmstd.out del dvmstd.out
if exist config.mpi del config.mpi
if exist test.3 del test.3
if exist jac1d_2_2.* del jac1d_2_2.*
:exit
