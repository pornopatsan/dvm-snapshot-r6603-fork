/**
 * Функции для сбра статистики по нитям
 * 
 * Иван Казаков - KazakovVan@Gmail.com
 */

#include "omp_dbg.h"
#include "dvmh_stat.h"
#ifdef _OPENMP
#include <omp.h>
#endif
#include <stdio.h>

/*-- Для сбора статистики по нитям*/
extern dvmh_stat_interval *dvmhStatisticsIntervalCurrent;

static DvmType MaxThreads = 0;
static dvmh_stat_interval_thread *ThreadsInfo = 0;

#ifdef _OPENMP
double *Timers = 0;

void inline ResetTimer(const DvmType i)
{
    Timers[i] = omp_get_wtime();
}

double inline DeltaTime(const DvmType i)
{
    double currTime = omp_get_wtime();
    double delta = currTime - Timers[i];
    Timers[i] =  currTime;
    return delta;
}
#else
static double Timer = 0;

void inline ResetTimer()
{
    Timer = dvm_time();
}

double inline DeltaTime()
{
    double currTime = dvm_time();
    double delta = currTime - Timer;
    Timer = currTime;
    return delta;
}
#endif

#ifdef __cplusplus
extern "C" {
#endif  

void DVMH_PERF_BeforeParallel(DvmType *LoopRef, DvmType *StatementID, DvmType *ThreadID)
{
#ifdef _OPENMP
    *ThreadID = 0;
    char *buff = (char*) CurrInterPtr;
    if(MaxThreads==0)
        MaxThreads = dvmh_stat_get_threads_amount();
    ThreadsInfo = dvmhStatisticsIntervalCurrent->threads;
    if(Timers==0)
        Timers = dvmh_stat_get_timers_array();
#else
    *ThreadID = 0;
    ThreadsInfo = dvmhStatisticsIntervalCurrent->threads;
#endif
}

void DVMH_PERF_AfterParallel(DvmType *LoopRef, DvmType *StatementID, DvmType *ThreadID)
{
#ifdef _OPENMP
    double CalculationTime;
    DvmType i;
    DvmType ThreadWithMaxTime = 0;
    //Посчитать время на неявной барьерной синхронизацией перед OMP END PARALLEL
    CalculationTime = omp_get_wtime();
    for(i = 1;i < MaxThreads; ++i)
        if(Timers[i] > Timers[ThreadWithMaxTime])
            ThreadWithMaxTime = i;
    for(i = 0; i < MaxThreads; ++i)
        ThreadsInfo[i].system_time += (Timers[ThreadWithMaxTime] - Timers[i]);
    //--
    ThreadsInfo[*ThreadID].system_time += omp_get_wtime() - CalculationTime;
#endif
}

void DVMH_PERF_BeforeLoop(DvmType *LoopRef, DvmType *StatementID, DvmType *ThreadID)
{
#ifdef _OPENMP
    *ThreadID = omp_get_thread_num();
    ResetTimer(*ThreadID);
#else
    ResetTimer();
#endif
}

void DVMH_PERF_AfterLoop(DvmType *LoopRef, DvmType *StatementID, DvmType *ThreadID)
{
#ifdef _OPENMP
    ThreadsInfo[*ThreadID].user_time += DeltaTime(*ThreadID);
#else
    ThreadsInfo[0].user_time += DeltaTime();
#endif
}

void DVMH_PERF_BeforeSynchronization(DvmType *LoopRef, DvmType *StatementID, DvmType *ThreadID)
{
#ifdef _OPENMP
    ThreadsInfo[*ThreadID].user_time += DeltaTime(*ThreadID);
#else
    ThreadsInfo[0].user_time += DeltaTime();
#endif
}

void DVMH_PERF_AfterSynchronization(DvmType *LoopRef, DvmType *StatementID, DvmType *ThreadID)
{
#ifdef _OPENMP
    ThreadsInfo[*ThreadID].system_time += DeltaTime(*ThreadID);
#else
    ThreadsInfo[0].system_time += DeltaTime();
#endif
}

#ifdef __cplusplus
}
#endif
