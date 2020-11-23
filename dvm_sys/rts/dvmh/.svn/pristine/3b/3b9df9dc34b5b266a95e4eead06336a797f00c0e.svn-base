#ifndef DVMHLIB_DEBUG_H
#define DVMHLIB_DEBUG_H

#include "dvmhlib_types.h"

#pragma GCC visibility push(default)
#ifdef __cplusplus
extern "C" {
#endif

DvmType dvmh_dbg_before_write_var_C(DvmType plType, DvmType addr, DvmType handle, char* szOperand);

DvmType dvmh_dbg_read_var_C(DvmType plType, DvmType addr, DvmType handle, char *szOperand);

DvmType dvmh_dbg_after_write_var_C();

DvmType dvmh_dbg_loop_seq_start_C(DvmType no);

DvmType dvmh_dbg_loop_end_C();

DvmType dvmh_dbg_loop_iter_C(DvmType rank, ...);

DvmType dvmh_dbg_loop_par_start_C( DvmType no, DvmType rank, ... );

DvmType dvmh_dbg_loop_create_C(DvmType curRegion, DvmType rank, /* DvmType start, DvmType end, DvmType step */...);

void dvmh_dbg_loop_red_group_create_C(DvmType curLoop);

void dvmh_dbg_loop_global_red_init_C(DvmType curLoop, DvmType redIndex, void *arrayAddr, char *name);

void dvmh_dbg_loop_handler_red_init_C(DvmType curLoop, DvmType redIndex, void *arrayAddr, char *name);

void dvmh_dbg_loop_red_group_delete_C(DvmType curLoop);

#ifdef __cplusplus
}
#endif

#endif /* DVMHLIB_DEBUG_H */

