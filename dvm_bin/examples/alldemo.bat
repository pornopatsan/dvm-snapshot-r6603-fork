if exist demo.1 del demo.1
set dvmoutfile=demo.1
set dvm_csdeb=on
set dvm_cpdeb=on
set dvm_fsdeb=on
set dvm_fpdeb=on
set dvm_err=on
set dvm_trc=on
set dvm_ptrc=on
set dvm_red=on
set dvm_dif=on

:jac1d
rem ****** DVM demo jacobi programs ******
call dvm ctest jac1d
@if not exist dvm.err goto jac
@echo ... to abort type "Ctrl+C" ...
@pause

:jac
@echo on
call dvm ftest jac
@if not exist dvm.err goto jac2d
@echo ... to abort type "Ctrl+C" ...
@pause

:jac2d
@echo on
call dvm ctest jac2d
@if not exist dvm.err goto jac_a
@echo ... to abort type "Ctrl+C" ...
@pause

:jac_a
@echo on
call dvm ftest jacas
@if not exist dvm.err goto gauss_c
@echo ... to abort type "Ctrl+C" ...
@pause

:gauss_c
@echo on
rem ****** DVM demo gauss programs ******
call dvm ctest gauss_c
@if not exist dvm.err goto gausf
@echo ... to abort type "Ctrl+C" ...
@pause

:gausf
@echo on
call dvm ftest gausf
@if not exist dvm.err goto gauss_wb
@echo ... to abort type "Ctrl+C" ...
@pause


:gauss_wb
@echo on
call dvm ctest gauss_wb
@if not exist dvm.err goto gauswh
@echo ... to abort type "Ctrl+C" ...
@pause

:gauswh
@echo on
call dvm ftest gauswh
@if not exist dvm.err goto redb_c
@echo ... to abort type "Ctrl+C" ...
@pause

:redb_c
@echo on
rem ****** DVM demo redblack programs ******
call dvm ctest redb_c
@if not exist dvm.err goto redbf
@echo ... to abort type "Ctrl+C" ...
@pause

:redbf
@echo on
call dvm ftest redbf
@if not exist dvm.err goto jacross
@echo ... to abort type "Ctrl+C" ...
@pause

:jacross
@echo on
rem ****** DVM demo jacross sor programs ******
call dvm ctest jacross
@echo on
@if not exist dvm.err goto sor
@echo ... to abort type "Ctrl+C" ...
@pause

:sor
@echo on
call dvm ftest sor
@if not exist dvm.err goto tsk
@echo ... to abort type "Ctrl+C" ...
@pause

:tsk
@echo on
rem ****** DVM demo tasks programs ******
call dvm ctest tsk
@if not exist dvm.err goto tsk_ra
@echo ... to abort type "Ctrl+C" ...
@pause

:tsk_ra
@echo on
call dvm ctest tsk_ra
@if not exist dvm.err goto task2j
@echo ... to abort type "Ctrl+C" ...
@pause

:task2j
@echo on
call dvm ftest task2j
@if not exist dvm.err goto tasks
@echo ... to abort type "Ctrl+C" ...
@pause

:tasks
@echo on
call dvm ftest tasks
@if not exist dvm.err goto taskst
@echo ... to abort type "Ctrl+C" ...
@pause

:taskst
@echo on
call dvm ftest taskst
@if not exist dvm.err goto mgrid
@echo ... to abort type "Ctrl+C" ...
@pause

:mgrid
@echo on
rem ****** DVM demo mgrid programs ******
call dvm ctest mgrid
@if not exist dvm.err goto gaush
@echo ... to abort type "Ctrl+C" ...
@pause

:gaush
@echo on
rem ****** DVM demo HPF programs ******
call dvm ftest gaush.hpf
@if not exist dvm.err goto jach
@echo ... to abort type "Ctrl+C" ...
@pause

:jach
@echo on
call dvm ftest jach.hpf
@if not exist dvm.err goto redbh
@echo ... to abort type "Ctrl+C" ...
@pause

:redbh
@echo on
call dvm ftest redbh.hpf

rem ****** end DVM demo programs ******
   