@echo off
set Pred_vis_blk=on
if exist testcdvm_res del testcdvm_res
set dvmoutfile=testcdvm_res
call dvm ctest testcdvm
if exist dvm.err goto exit
if exist testfdvm_res del testfdvm_res 
set dvmoutfile=testfdvm_res
if not exist ..\bin\f_dvm.exe goto pp1
if not exist ..\bin\parse.exe goto pp1
call dvm ftest testfdvm
if exist dvm.err goto exit
goto pp
:pp1
echo ... Fortran DVM is not installed ...
:pp
if exist ..\bin\predictor.exe goto pred
echo ... Predictor is not installed ...
goto exit
:pred
set dvmoutfile=
call dvm c jac1d
call dvm runpred jac1d
call dvm pred 2 2 jac1d
if exist dvm.err goto exit
copy jac1d_2_2.html pred_res
echo ... end of installing alltest DVM ...
pause
if exist comp.err del comp.err
if exist testcdvm.c del testcdvm.c
if exist testfdvm.f del testfdvm.f
if exist dvmstd.err del dvmstd.err
if exist dvmstd.out del dvmstd.out
if exist config.mpi del config.mpi
if exist test.3 del test.3
if exist jac1d_2_2.* del jac1d_2_2.*
if exist jac1d.c del jac1d.c
if exist jac1d.obj del jac1d.obj
if exist jac1d.exe del jac1d.exe
if exist *.btr del *.btr

if exist *.dat del *.dat
:exit
