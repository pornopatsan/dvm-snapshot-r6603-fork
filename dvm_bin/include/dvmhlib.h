#ifndef DVMHLIB_H
#define DVMHLIB_H

#include "dvmhlib_rename_iface.h"

#include "dvmhlib_types.h"
#include "dvmhlib_const.h"

#ifndef _DVMLIB_H_
typedef DvmType ShadowGroupRef, DAConsistGroupRef, LoopRef, RedRef;
#endif

#pragma GCC visibility push(default)
#ifdef __cplusplus
extern "C" {
#endif

void dvmh_finish_();

void dvmh_get_actual_subvariable_(void *addr, DvmType lowIndex[], DvmType highIndex[]);
void dvmh_get_actual_variable_(void *addr);
void dvmh_get_actual_subarray_(DvmType dvmDesc[], DvmType lowIndex[], DvmType highIndex[]);
void dvmh_get_actual_array_(DvmType dvmDesc[]);
void dvmh_get_actual_all_();

void dvmh_actual_subvariable_(void *addr, DvmType lowIndex[], DvmType highIndex[]);
void dvmh_actual_variable_(void *addr);
void dvmh_actual_subarray_(DvmType dvmDesc[], DvmType lowIndex[], DvmType highIndex[]);
void dvmh_actual_array_(DvmType dvmDesc[]);
void dvmh_actual_all_();

void dvmh_remote_access_(DvmType dvmDesc[]);
void dvmh_shadow_renew_(ShadowGroupRef *group);
void dvmh_redistribute_(DvmType dvmDesc[], DvmType *newValueFlagRef);
void dvmh_realign_(DvmType dvmDesc[], DvmType *newValueFlagRef);

void dvmh_destroy_variable_(void *addr);
void dvmh_destroy_array_(DvmType dvmDesc[]);

void *dvmh_get_device_addr(DvmType *deviceRef, void *variable);
DvmType dvmh_calculate_offset_(DvmType *deviceRef, void *base, void *variable);
void *dvmh_get_natural_base(DvmType *deviceRef, DvmType dvmDesc[]);
void dvmh_fill_header_(DvmType *deviceRef, void *base, DvmType dvmDesc[], DvmType dvmhDesc[]);
void dvmh_fill_header_ex_(DvmType *deviceRef, void *base, DvmType dvmDesc[], DvmType dvmhDesc[], DvmType *outTypeOfTransformation, DvmType extendedParams[]);

DvmType region_create_(DvmType *flagsRef);
void region_register_subarray_(DvmType *regionRef, DvmType *intentRef, DvmType dvmDesc[], DvmType lowIndex[], DvmType highIndex[], DvmType *elemType);
void region_register_array_(DvmType *regionRef, DvmType *intentRef, DvmType dvmDesc[], DvmType *elemType);
void region_register_scalar_(DvmType *regionRef, DvmType *intentRef, void *addr, DvmType *sizeRef, DvmType *varType);
void region_set_name_array_(DvmType *regionRef, DvmType dvmDesc[], const char *name, int nameLength);
void region_set_name_variable_(DvmType *regionRef, void *addr, const char *name, int nameLength);
void region_execute_on_targets_(DvmType *regionRef, DvmType *devicesRef);
void region_handle_consistent_(DvmType *regionRef, DAConsistGroupRef *group);
void region_after_waitrb_(DvmType *regionRef, DvmType dvmDesc[]);
void region_destroy_rb_(DvmType *regionRef, DvmType dvmDesc[]);
void region_end_(DvmType *regionRef);

DvmType loop_create_(DvmType *regionRef, LoopRef *InDvmLoop);
void loop_insred_(DvmType *InDvmhLoop, RedRef *InRedRefPtr);
void loop_across_(DvmType *InDvmhLoop, ShadowGroupRef *oldGroup, ShadowGroupRef *newGroup);
void loop_set_cuda_block_(DvmType *InDvmhLoop, DvmType *InXRef, DvmType *InYRef, DvmType *InZRef);
void loop_shadow_compute_(DvmType *InDvmhLoop, DvmType dvmDesc[]);
void loop_register_handler_(DvmType *InDvmhLoop, DvmType *deviceTypeRef, DvmType *flagsRef, DvmHandlerFunc f, DvmType *basesCount, DvmType *paramCount, ...);
void loop_perform_(DvmType *InDvmhLoop);
DvmType loop_get_device_num_(DvmType *InDvmhLoop);
DvmType loop_has_element_(DvmType *InDvmhLoop, DvmType dvmDesc[], DvmType indexArray[]);
void loop_fill_bounds_(DvmType *InDvmhLoop, DvmType lowIndex[], DvmType highIndex[], DvmType stepIndex[]);
void loop_fill_local_part_(DvmType *InDvmhLoop, DvmType dvmDesc[], DvmType part[]);
void loop_red_init_(DvmType *InDvmhLoop, DvmType *InRedNumRef, void *arrayPtr, void *locPtr);
DvmType loop_get_slot_count_(DvmType *InDvmhLoop);
DvmType loop_get_dependency_mask_(DvmType *InDvmhLoop);
DvmType loop_guess_index_type_(DvmType *InDvmhLoop);
void loop_cuda_register_red(DvmType *InDvmhLoop, DvmType InRedNum, void **ArrayPtr, void **LocPtr);
void loop_cuda_red_init(DvmType *InDvmhLoop, DvmType InRedNum, void **devArrayPtr, void **devLocPtr);
void loop_cuda_red_prepare(DvmType *InDvmhLoop, DvmType InRedNum, DvmType InCount, DvmType InFillFlag);
void *loop_cuda_get_local_part(DvmType *InDvmhLoop, DvmType dvmDesc[], DvmType indexType);
DvmType loop_cuda_autotransform(DvmType *InDvmhLoop, DvmType dvmDesc[]);
void loop_cuda_get_config(DvmType *InDvmhLoop, DvmType InSharedPerThread, DvmType InRegsPerThread, void *InOutThreads, void *OutStream,
        DvmType *OutSharedPerBlock);
DvmType loop_cuda_do(DvmType *InDvmhLoop, void *OutBlocks, void **InOutBlocksInfo, DvmType indexType);
DvmType loop_cuda_get_red_step(DvmType *InDvmhLoop);
void loop_red_finish(DvmType *InDvmhLoop, DvmType InRedNum);
void loop_red_post_(DvmType *InDvmhLoop, DvmType *InRedNumRef, void *arrayPtr, void *locPtr);

void dvmh_change_filled_bounds(DvmType *boundsLow, DvmType *boundsHigh, DvmType *loopSteps, DvmType rank, DvmType depCount, DvmType depMask, DvmType *idxPerm);

DvmType dvmh_register_array_(DvmType dvmDesc[]);
DvmType dvmh_register_scalar_(void *addr, DvmType *sizeRef);
void loop_cuda_rtc_set_lang(DvmType *InDvmhLoop, DvmType lang);
void loop_cuda_rtc_launch(DvmType *InDvmhLoop, const char *kernelName, const char *src, void *blocks, DvmType numPar, ...);
DvmType loop_cuda_get_device_prop(DvmType *InDvmhLoop, DvmType prop);
DvmType dvmh_get_next_stage_(DvmType *dvmhRegRef, LoopRef *loopRef, DvmType *lineNumber, const char *fileName, int len);

#ifdef __cplusplus
}
#endif
#pragma GCC visibility pop

#endif
