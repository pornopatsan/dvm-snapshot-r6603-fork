if exist democ.1 del democ.1
set dvmoutfile=democ.1
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
@if not exist dvm.err goto jac2d
@echo ... to abort type "Ctrl+C" ...
@pause

:jac2d
@echo on
call dvm ctest jac2d
@if not exist dvm.err goto gauss_c
@echo ... to abort type "Ctrl+C" ...
@pause

:gauss_c
@echo on
rem ****** DVM demo gauss programs ******
call dvm ctest gauss_c
@if not exist dvm.err goto gauss_wb
@echo ... to abort type "Ctrl+C" ...
@pause

:gauss_wb
@echo on
call dvm ctest gauss_wb
@if not exist dvm.err goto redb_c
@echo ... to abort type "Ctrl+C" ...
@pause

:redb_c
@echo on
rem ****** DVM demo redblack programs ******
call dvm ctest redb_c
@if not exist dvm.err goto jacross
@echo ... to abort type "Ctrl+C" ...
@pause

:jacross
@echo on
rem ****** DVM demo jacross sor programs ******
@echo on
call dvm ctest jacross
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
:tasks
:tasks_t
:mgrid
@echo on
rem ****** DVM demo mgrid programs ******
call dvm ctest mgrid
rem ****** end DVM demo programs ******
 