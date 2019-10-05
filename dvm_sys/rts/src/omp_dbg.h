/**
 * Функции для сбора статистики по нитям
 * 
 * Иван Казаков - KazakovVan@Gmail.com
 */

#ifdef _MS_F_

#define DVMH_PERF_BeforeParallel        dvmh_perf_before_parallel
#define DVMH_PERF_AfterParallel         dvmh_perf_after_parallel
#define DVMH_PERF_BeforeLoop            dvmh_perf_before_loop
#define DVMH_PERF_AfterLoop             dvmh_perf_after_loop
#define DVMH_PERF_BeforeSynchronization dvmh_perf_before_synchro
#define DVMH_PERF_AfterSynchronization  dvmh_perf_after_synchro

#else

#define DVMH_PERF_BeforeParallel        dvmh_perf_before_parallel_
#define DVMH_PERF_AfterParallel         dvmh_perf_after_parallel_
#define DVMH_PERF_BeforeLoop            dvmh_perf_before_loop_
#define DVMH_PERF_AfterLoop             dvmh_perf_after_loop_
#define DVMH_PERF_BeforeSynchronization dvmh_perf_before_synchro_
#define DVMH_PERF_AfterSynchronization  dvmh_perf_after_synchro_

#endif

#ifdef __cplusplus
extern "C" {
#endif

void DVMH_PERF_BeforeParallel(DvmType *LoopRef, DvmType *StatementID, DvmType *ThreadID);
void DVMH_PERF_AfterParallel(DvmType *LoopRef, DvmType *StatementID, DvmType *ThreadID);
void DVMH_PERF_BeforeLoop(DvmType *LoopRef, DvmType *StatementID, DvmType *ThreadID);
void DVMH_PERF_AfterLoop(DvmType *LoopRef, DvmType *StatementID, DvmType *ThreadID);
void DVMH_PERF_BeforeSynchronization(DvmType *LoopRef, DvmType *StatementID, DvmType *ThreadID);
void DVMH_PERF_AfterSynchronization(DvmType *LoopRef, DvmType *StatementID, DvmType *ThreadID);

#ifdef __cplusplus
}
#endif
