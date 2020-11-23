@if exist demopred.1 del demopred.1
@set dvmoutfile=demopred.1

@echo ****** Predictor demo jacobi programs ******
call 1pred c jac1d
call 1pred f jac
call 1pred c jac2d
call 1pred f jacas

@echo ****** Predictor demo gauss programs ******
call 1pred c gauss_c
call 1pred f gausf
call 1pred c gauss_wb
call 1pred f gauswh

@echo ****** Predictor demo redblack programs ******
call 1pred c redb_c
call 1pred f redbf

@echo ****** Predictor demo jacross sor programs ******
call 1pred c jacross
call 1pred f sor

@echo ****** Predictor demo tasks programs ******
call 1pred c tsk
call 1pred c tsk_ra
call 1pred f task2j
call 1pred f tasks
call 1pred f taskst

@echo ****** Predictor demo mgrid programs ******
call 1pred c mgrid

@echo ****** Predictor demo HPF programs ******
call 1pred f gaush .hpf
call 1pred f jach .hpf
call 1pred f redbh .hpf

:exit
@echo ****** end Predictor demo programs ******
   