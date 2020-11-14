#ifndef DVMHLIB2_H
#define DVMHLIB2_H

#include "dvmhlib2_rename_iface.h"

#include <stdlib.h>

#include "dvmhlib_types.h"
#include "dvmhlib_const.h"
#include "dvmhlib_stdio.h"
#include "dvmhlib_debug.h"
#include "dvmh_runtime_api.h"

#pragma GCC visibility push(default)
#ifdef __cplusplus
extern "C" {
#endif

void dvmh_line_C(DvmType lineNumber, const char fileName[]);
void dvmh_line_(const DvmType *pLineNumber, const DvmType *pFileNameStr);

void dvmh_init_C(DvmType flags, int *pArgc, char ***pArgv);
void dvmh_init2_(const DvmType *pFlags);
void dvmh_init_lib_C(DvmType flags);
void dvmh_init_lib_(const DvmType *pFlags);
void dvmh_exit_C(DvmType exitCode);
void dvmh_exit_(const DvmType *pExitCode);

void dvmh_array_declare_C(DvmType dvmDesc[], DvmType rank, DvmType typeSize, /* DvmType axisSize, DvmType shadowLow, DvmType shadowHigh */...);
void dvmh_array_create_C(DvmType dvmDesc[], DvmType rank, DvmType typeSize, /* DvmType axisSize, DvmType shadowLow, DvmType shadowHigh */...);
void dvmh_array_create_(DvmType dvmDesc[], const void *baseAddr, const DvmType *pRank, const DvmType *pTypeSize,
        /* const DvmType *pSpaceLow, const DvmType *pSpaceHigh, const DvmType *pShadowLow, const DvmType *pShadowHigh */...);
void dvmh_template_create_C(DvmType dvmDesc[], DvmType rank, /* DvmType axisSize */...);
void dvmh_template_create_(DvmType dvmDesc[], const DvmType *pRank, /* const DvmType *pSpaceLow, const DvmType *pSpaceHigh */...);
void dvmh_array_alloc_C(DvmType dvmDesc[], DvmType byteCount);

DvmType dvmh_handler_func_C(DvmHandlerFunc handlerFunc, DvmType customParamCount, /* void *param */...);
DvmType dvmh_handler_func_(DvmHandlerFunc handlerFunc, const DvmType *pCustomParamCount, /* void *param */...);
DvmType dvmh_derived_rhs_expr_ignore_();
DvmType dvmh_derived_rhs_expr_constant_C(DvmType indexValue);
DvmType dvmh_derived_rhs_expr_constant_(const DvmType *pIndexValue);
DvmType dvmh_derived_rhs_expr_scan_C(DvmType shadowCount, /* const char shadowName[] */...);
DvmType dvmh_derived_rhs_expr_scan_(const DvmType *pShadowCount, /* const DvmType *pShadowNameStr */...);
DvmType dvmh_derived_rhs_C(const DvmType templDesc[], DvmType templRank, /* DvmType derivedRhsExprHelper */...);
DvmType dvmh_derived_rhs_(const DvmType templDesc[], const DvmType *pTemplRank, /* const DvmType *pDerivedRhsExprHelper */...);
DvmType dvmh_distribution_replicated_();
DvmType dvmh_distribution_block_C(DvmType mpsAxis);
DvmType dvmh_distribution_block_(const DvmType *pMpsAxis);
DvmType dvmh_distribution_wgtblock_C(DvmType mpsAxis, DvmType elemType, const void *arrayAddr, DvmType elemCount);
DvmType dvmh_distribution_wgtblock_(const DvmType *pMpsAxis, const DvmType *pElemType, const void *arrayAddr, const DvmType *pElemCount);
DvmType dvmh_distribution_genblock_C(DvmType mpsAxis, DvmType elemType, const void *arrayAddr);
DvmType dvmh_distribution_genblock_(const DvmType *pMpsAxis, const DvmType *pElemType, const void *arrayAddr);
DvmType dvmh_distribution_multblock_C(DvmType mpsAxis, DvmType multBlock);
DvmType dvmh_distribution_multblock_(const DvmType *pMpsAxis, const DvmType *pMultBlock);
DvmType dvmh_distribution_indirect_C(DvmType mpsAxis, DvmType elemType, const void *arrayAddr);
DvmType dvmh_distribution_indirect_(const DvmType *pMpsAxis, const DvmType *pElemType, const void *arrayAddr);
DvmType dvmh_distribution_derived_C(DvmType mpsAxis, DvmType derivedRhsHelper, DvmType countingHandlerHelper, DvmType fillingHandlerHelper);
DvmType dvmh_distribution_derived_(const DvmType *pMpsAxis, const DvmType *pDerivedRhsHelper, const DvmType *pCountingHandlerHelper, const DvmType *pFillingHandlerHelper);

DvmType *dvmh_indirect_get_buffer_C(DvmType count);
void dvmh_distribute_C(DvmType dvmDesc[], DvmType rank, /* DvmType distributionHelper */...);
void dvmh_distribute_(DvmType dvmDesc[], const DvmType *pRank, /* const DvmType *pDistributionHelper */...);
void dvmh_redistribute_C(DvmType dvmDesc[], DvmType rank, /* DvmType distributionHelper */...);
void dvmh_redistribute2_(DvmType dvmDesc[], const DvmType *pRank, /* const DvmType *pDistributionHelper */...);
DvmType dvmh_alignment_linear_C(DvmType axis, DvmType multiplier, DvmType summand);
DvmType dvmh_alignment_linear_(const DvmType *pAxis, const DvmType *pMultiplier, const DvmType *pSummand);
void dvmh_align_C(DvmType dvmDesc[], const DvmType templDesc[], DvmType templRank, /* DvmType alignmentHelper */...);
void dvmh_align_(DvmType dvmDesc[], const DvmType templDesc[], const DvmType *pTemplRank, /* const DvmType *pAlignmentHelper */...);
void dvmh_realign_C(DvmType dvmDesc[], DvmType newValueFlag, const DvmType templDesc[], DvmType templRank, /* DvmType alignmentHelper */...);
void dvmh_realign2_(DvmType dvmDesc[], const DvmType *pNewValueFlag, const DvmType templDesc[], const DvmType *pTemplRank,
        /* const DvmType *pAlignmentHelper */...);
void dvmh_indirect_shadow_add_C(DvmType dvmDesc[], DvmType axis, DvmType derivedRhsHelper, DvmType countingHandlerHelper, DvmType fillingHandlerHelper,
        const char shadowName[], DvmType includeCount, /* DvmType dvmDesc[] */...);
void dvmh_indirect_shadow_add_(DvmType dvmDesc[], const DvmType *pAxis, const DvmType *pDerivedRhsHelper, const DvmType *pCountingHandlerHelper,
        const DvmType *pFillingHandlerHelper, const DvmType *pShadowNameStr, const DvmType *pIncludeCount, /* DvmType dvmDesc[] */...);

void dvmh_array_free_C(DvmType dvmDesc[]);
void dvmh_delete_object_(DvmType dvmDesc[]);
void dvmh_forget_header_(DvmType dvmDesc[]);

DvmType *dvmh_variable_gen_header_C(const void *addr, DvmType rank, DvmType typeSize, /* DvmType axisSize */...);
void dvmh_variable_fill_header_(DvmType dvmDesc[], const void *baseAddr, const void *addr, const DvmType *pRank, const DvmType *pTypeSize,
        /* const DvmType *pSpaceLow, const DvmType *pSpaceHigh */...);
DvmType dvmh_variable_gen_header_(const void *addr, const DvmType *pRank, const DvmType *pTypeSize,
        /* const DvmType *pSpaceLow, const DvmType *pSpaceHigh */...);
DvmType *dvmh_variable_get_header_C(const void *addr);
void dvmh_data_enter_C(const void *addr, DvmType size);
void dvmh_data_enter_(const void *addr, const DvmType *pSize);
void dvmh_data_exit_C(const void *addr, DvmType saveFlag);
void dvmh_data_exit_(const void *addr, const DvmType *pSaveFlag);
void *dvmh_malloc_C(size_t size);
void *dvmh_calloc_C(size_t nmemb, size_t size);
void *dvmh_realloc_C(void *ptr, size_t size);
char *dvmh_strdup_C(const char *s);
void dvmh_free_C(void *ptr);

DvmType dvmh_array_slice_C(DvmType rank, /* DvmType start, DvmType end, DvmType step */...);
DvmType dvmh_array_slice_(const DvmType *pRank, /* const DvmType *pStart, const DvmType *pEnd, const DvmType *pStep */...);
void dvmh_array_copy_whole_(const DvmType srcDvmDesc[], DvmType dstDvmDesc[]);
void dvmh_array_copy_C(const DvmType srcDvmDesc[], DvmType srcSliceHelper, DvmType dstDvmDesc[], DvmType dstSliceHelper);
void dvmh_array_copy_(const DvmType srcDvmDesc[], DvmType *pSrcSliceHelper, DvmType dstDvmDesc[], DvmType *pDstSliceHelper);

void *dvmh_get_natural_base_C(DvmType deviceNum, const DvmType dvmDesc[]);
void *dvmh_get_device_addr_C(DvmType deviceNum, const void *addr);
DvmType dvmh_fill_header_C(DvmType deviceNum, const void *baseAddr, const DvmType dvmDesc[], DvmType devHeader[], DvmType extendedParams[]);
DvmType dvmh_fill_header2_(const DvmType *pDeviceNum, const void *baseAddr, const DvmType dvmDesc[], DvmType devHeader[]);
DvmType dvmh_fill_header_ex2_(const DvmType *pDeviceNum, const void *baseAddr, const DvmType dvmDesc[], DvmType devHeader[], DvmType extendedParams[]);

void dvmh_get_actual_subvariable_C(void *addr, DvmType rank, /* DvmType indexLow, DvmType indexHigh */...);
void dvmh_get_actual_subvariable2_(void *addr, const DvmType *pRank, /* const DvmType *pIndexLow, const DvmType *pIndexHigh */...);
void dvmh_get_actual_variable2_(void *addr);
void dvmh_get_actual_subarray_C(const DvmType dvmDesc[], DvmType rank, /* DvmType indexLow, DvmType indexHigh */...);
void dvmh_get_actual_subarray2_(const DvmType dvmDesc[], const DvmType *pRank, /* const DvmType *pIndexLow, const DvmType *pIndexHigh */...);
void dvmh_get_actual_array2_(const DvmType dvmDesc[]);
void dvmh_get_actual_all2_();

void dvmh_actual_subvariable_C(const void *addr, DvmType rank, /* DvmType indexLow, DvmType indexHigh */...);
void dvmh_actual_subvariable2_(const void *addr, const DvmType *pRank, /* const DvmType *pIndexLow, const DvmType *pIndexHigh */...);
void dvmh_actual_variable2_(const void *addr);
void dvmh_actual_subarray_C(const DvmType dvmDesc[], DvmType rank, /* DvmType indexLow, DvmType indexHigh */...);
void dvmh_actual_subarray2_(const DvmType dvmDesc[], const DvmType *pRank, /* const DvmType *pIndexLow, const DvmType *pIndexHigh */...);
void dvmh_actual_array2_(const DvmType dvmDesc[]);
void dvmh_actual_all2_();

void dvmh_shadow_renew_C(const DvmType dvmDesc[], DvmType cornerFlag, DvmType specifiedRank, /* DvmType shadowLow, DvmType shadowHigh */...);
void dvmh_shadow_renew2_(const DvmType dvmDesc[], const DvmType *pCornerFlag, const DvmType *pSpecifiedRank,
        /* const DvmType *pShadowLow, const DvmType *pShadowHigh */...);
void dvmh_indirect_shadow_renew_C(const DvmType dvmDesc[], DvmType axis, const char shadowName[]);
void dvmh_indirect_shadow_renew_(const DvmType dvmDesc[], const DvmType *pAxis, const DvmType *pShadowNameStr);
void dvmh_indirect_localize_C(const DvmType refDvmDesc[], const DvmType targetDvmDesc[], DvmType targetAxis);
void dvmh_indirect_localize_(const DvmType refDvmDesc[], const DvmType targetDvmDesc[], const DvmType *pTargetAxis);
void dvmh_indirect_unlocalize_(const DvmType dvmDesc[]);
void dvmh_remote_access_C(DvmType rmaDesc[], const DvmType dvmDesc[], DvmType rank, /* DvmType alignmentHelper */...);
void dvmh_remote_access2_(DvmType rmaDesc[], const void *baseAddr, const DvmType dvmDesc[], const DvmType *pRank, /* const DvmType *pAlignmentHelper */...);
DvmType dvmh_has_element_C(const DvmType dvmDesc[], DvmType rank, /* DvmType index */...);
DvmType dvmh_has_element_(const DvmType dvmDesc[], const DvmType *pRank, /* const DvmType *pIndex */...);
DvmType dvmh_calc_linear_C(const DvmType dvmDesc[], DvmType rank, /* DvmType globalIndex */...);
DvmType dvmh_calc_linear_(const DvmType dvmDesc[], const DvmType *pRank, /* const DvmType *pGlobalIndex */...);
void *dvmh_get_own_element_addr_C(const DvmType dvmDesc[], DvmType rank, /* DvmType globalIndex */...);
void *dvmh_get_element_addr_C(const DvmType dvmDesc[], DvmType rank, /* DvmType globalIndex */...);

DvmType dvmh_region_create_C(DvmType regionFlags);
DvmType dvmh_region_create_(const DvmType *pRegionFlags);
void dvmh_region_register_subarray_C(DvmType curRegion, DvmType intent, const DvmType dvmDesc[], const char varName[], DvmType rank,
        /* DvmType indexLow, DvmType indexHigh */...);
void dvmh_region_register_subarray_(const DvmType *pCurRegion, const DvmType *pIntent, const DvmType dvmDesc[], const DvmType *pVarNameStr,
        const DvmType *pRank, /* const DvmType *pIndexLow, const DvmType *pIndexHigh */...);
void dvmh_region_register_array_C(DvmType curRegion, DvmType intent, const DvmType dvmDesc[], const char varName[]);
void dvmh_region_register_array_(const DvmType *pCurRegion, const DvmType *pIntent, const DvmType dvmDesc[], const DvmType *pVarNameStr);
void dvmh_region_register_scalar_C(DvmType curRegion, DvmType intent, const void *addr, DvmType typeSize, const char varName[]);
void dvmh_region_register_scalar_(const DvmType *pCurRegion, const DvmType *pIntent, const void *addr, const DvmType *pTypeSize, const DvmType *pVarNameStr);
void dvmh_region_execute_on_targets_C(DvmType curRegion, DvmType deviceTypes);
void dvmh_region_execute_on_targets_(const DvmType *pCurRegion, const DvmType *pDeviceTypes);
void dvmh_region_end_C(DvmType curRegion);
void dvmh_region_end_(const DvmType *pCurRegion);

DvmType dvmh_loop_create_C(DvmType curRegion, DvmType rank, /* DvmType start, DvmType end, DvmType step */...);
DvmType dvmh_loop_create_(const DvmType *pCurRegion, const DvmType *pRank, /* const DvmType *pStart, const DvmType *pEnd, const DvmType *pStep */...);
void dvmh_loop_map_C(DvmType curLoop, const DvmType templDesc[], DvmType templRank, /* DvmType alignmentHelper */...);
void dvmh_loop_map_(const DvmType *pCurLoop, const DvmType templDesc[], const DvmType *pTemplRank, /* const DvmType *pAlignmentHelper */...);
void dvmh_loop_set_cuda_block_C(DvmType curLoop, DvmType xSize, DvmType ySize, DvmType zSize);
void dvmh_loop_set_cuda_block_(const DvmType *pCurLoop, const DvmType *pXSize, const DvmType *pYSize, const DvmType *pZSize);
void dvmh_loop_set_stage_C(DvmType curLoop, DvmType stage);
void dvmh_loop_set_stage_(const DvmType *pCurLoop, const DvmType *pStage);
void dvmh_loop_reduction_C(DvmType curLoop, DvmType redType, void *arrayAddr, DvmType varType, DvmType arrayLength, void *locAddr, DvmType locSize);
void dvmh_loop_reduction_(const DvmType *pCurLoop, const DvmType *pRedType, void *arrayAddr, const DvmType *pVarType, const DvmType *pArrayLength,
        void *locAddr, const DvmType *pLocSize);
void dvmh_loop_across_C(DvmType curLoop, DvmType isOut, const DvmType dvmDesc[], DvmType rank, /* DvmType shadowLow, DvmType shadowHigh */...);
void dvmh_loop_across_(const DvmType *pCurLoop, const DvmType *pIsOut, const DvmType dvmDesc[], const DvmType *pRank, /* const DvmType *pShadowLow, const DvmType *pShadowHigh */...);
void dvmh_loop_shadow_compute_C(DvmType curLoop, const DvmType templDesc[], DvmType specifiedRank, /* DvmType shadowLow, DvmType shadowHigh */...);
void dvmh_loop_shadow_compute_(const DvmType *pCurLoop, const DvmType templDesc[], const DvmType *pSpecifiedRank,
        /* const DvmType *pShadowLow, const DvmType *pShadowHigh */...);
void dvmh_loop_shadow_compute_array_C(DvmType curLoop, const DvmType dvmDesc[]);
void dvmh_loop_shadow_compute_array_(const DvmType *pCurLoop, const DvmType dvmDesc[]);
void dvmh_loop_consistent_C(DvmType curLoop, const DvmType dvmDesc[], DvmType rank, /* DvmType alignmentHelper */...);
void dvmh_loop_consistent_(const DvmType *pCurLoop, const DvmType dvmDesc[], const DvmType *pRank, /* const DvmType *pAlignmentHelper */...);
void dvmh_loop_remote_access_C(DvmType curLoop, const DvmType dvmDesc[], DvmType rank, /* DvmType alignmentHelper */...);
void dvmh_loop_remote_access_(const DvmType *pCurLoop, const DvmType dvmDesc[], const DvmType *pRank, /* const DvmType *pAlignmentHelper */...);
void dvmh_loop_array_correspondence_C(DvmType curLoop, const DvmType dvmDesc[], DvmType rank, /* DvmType loopAxis */...);
void dvmh_loop_array_correspondence_(const DvmType *pCurLoop, const DvmType dvmDesc[], const DvmType *pRank, /* const DvmType *pLoopAxis */...);
void dvmh_loop_register_handler_C(DvmType curLoop, DvmType deviceType, DvmType handlerType, DvmType handlerHelper);
void dvmh_loop_register_handler_(const DvmType *pCurLoop, const DvmType *pDeviceType, const DvmType *pHandlerType, const DvmType *pHandlerHelper);
void dvmh_loop_perform_C(DvmType curLoop);
void dvmh_loop_perform_(const DvmType *pCurLoop);
DvmType dvmh_loop_get_dependency_mask_C(DvmType curLoop);
DvmType dvmh_loop_get_dependency_mask_(const DvmType *pCurLoop);
DvmType dvmh_loop_get_device_num_C(DvmType curLoop);
DvmType dvmh_loop_get_device_num_(const DvmType *pCurLoop);
DvmType dvmh_loop_autotransform_C(DvmType curLoop, DvmType dvmDesc[]);
DvmType dvmh_loop_autotransform_(const DvmType *pCurLoop, DvmType dvmDesc[]);
void dvmh_loop_fill_bounds_C(DvmType curLoop, DvmType boundsLow[], DvmType boundsHigh[], DvmType loopSteps[]);
void dvmh_loop_fill_bounds_(const DvmType *pCurLoop, DvmType boundsLow[], DvmType boundsHigh[], DvmType loopSteps[]);
DvmType dvmh_loop_get_slot_count_C(DvmType curLoop);
DvmType dvmh_loop_get_slot_count_(const DvmType *pCurLoop);
void dvmh_loop_fill_local_part_C(DvmType curLoop, const DvmType dvmDesc[], DvmType part[]);
void dvmh_loop_fill_local_part_(const DvmType *pCurLoop, const DvmType dvmDesc[], DvmType part[]);
DvmType dvmh_loop_guess_index_type_C(DvmType curLoop);
DvmType dvmh_loop_guess_index_type_(const DvmType *pCurLoop);
const void *dvmh_loop_cuda_get_local_part_C(DvmType curLoop, const DvmType dvmDesc[], DvmType indexType);
void dvmh_loop_get_remote_buf_C(DvmType curLoop, DvmType rmaIndex, DvmType rmaDesc[]);
void dvmh_loop_get_remote_buf_(const DvmType *pCurLoop, const DvmType *pRmaIndex, DvmType rmaDesc[]);
void dvmh_loop_cuda_register_red_C(DvmType curLoop, DvmType redIndex, void **arrayAddrPtr, void **locAddrPtr);
void dvmh_loop_red_init_C(DvmType curLoop, DvmType redIndex, void *arrayAddr, void *locAddr);
void dvmh_loop_red_init_(const DvmType *pCurLoop, const DvmType *pRedIndex, void *arrayAddr, void *locAddr);
void dvmh_loop_cuda_red_init_C(DvmType curLoop, DvmType redIndex, void **devArrayAddrPtr, void **devLocAddrPtr);
void dvmh_loop_cuda_get_config_C(DvmType curLoop, DvmType sharedPerThread, DvmType regsPerThread, void *inOutThreads, void *outStream,
        DvmType *outSharedPerBlock);
void dvmh_loop_cuda_red_prepare_C(DvmType curLoop, DvmType redIndex, DvmType count, DvmType fillFlag);
DvmType dvmh_loop_has_element_C(DvmType curLoop, const DvmType dvmDesc[], DvmType rank, /* DvmType index */...);
DvmType dvmh_loop_has_element_(const DvmType *pCurLoop, const DvmType dvmDesc[], const DvmType *pRank, /* const DvmType *pIndex */...);
void dvmh_loop_cuda_red_finish_C(DvmType curLoop, DvmType redIndex);
void dvmh_loop_red_post_C(DvmType curLoop, DvmType redIndex, const void *arrayAddr, const void *locAddr);
void dvmh_loop_red_post_(const DvmType *pCurLoop, const DvmType *pRedIndex, const void *arrayAddr, const void *locAddr);

void dvmh_scope_start_();
void dvmh_scope_insert_(const DvmType dvmDesc[]);
void dvmh_scope_end_();

void dvmh_par_interval_start_C();
void dvmh_seq_interval_start_C();
void dvmh_sp_interval_end_();
void dvmh_usr_interval_start_C(DvmType userID);
void dvmh_usr_interval_start_(DvmType *pUserID);
void dvmh_usr_interval_end_();

DvmType dvmh_get_addr_(void *pVariable);
DvmType dvmh_string_(const char s[], int len);
DvmType dvmh_string_var_(char s[], int len);
void dvmh_change_filled_bounds_C(DvmType boundsLow[], DvmType boundsHigh[], DvmType loopSteps[], DvmType rank, DvmType depMask, DvmType idxPerm[]);
void dvmh_change_filled_bounds2_C(DvmType boundsLow[], DvmType boundsHigh[], DvmType loopSteps[], DvmType rank, DvmType depMask, DvmType idxPerm[]);

DvmType *dvmh_get_dvm_header_C(const DvmType dvmDesc[]);

#ifdef __cplusplus

class DvmhModuleInitializer {
public:
    explicit DvmhModuleInitializer(void (*fInit)(), void (*fFinish)() = 0);
    static void executeInit();
    static void executeFinish();
protected:
    static DvmhModuleInitializer *head;
    DvmhModuleInitializer *next;
    void (*funcInit)();
    void (*funcFinish)();
};

#endif

#ifdef __cplusplus
}
#endif
#pragma GCC visibility pop

#endif
