/**
 * Файл содержит типы и функции для сбора статистики DVMH
 *
 * @author Aleksei Shubert <alexei@shubert.ru>
 */

#include "dvmh_stat.h"
#include "system.def"
#include "system.typ"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <float.h>
#ifdef _OPENMP      
#include <omp.h>
#endif

/** Размер буфера для строки лога отладки */
#define DVMH_STAT_LOG_BUF_SIZE 256

/** Размер уровня отступа строки в отладочном логе */
#define DVMH_STAT_LOG_SHIFT_OFFSET 3

// -- External  variables -------------------------------------------------------------------------------------------------
/**
 * Интервальная статистика DVMH текущего интервала
 */
dvmh_stat_interval *dvmhStatisticsIntervalCurrent;

// -- Internal  variables -------------------------------------------------------------------------------------------------

/**
 * Заголовок DVMH статистики текущего процесса
 */
static dvmh_stat_header *dvmhStatisticsHeader;

/**
 * Масив таймеров для нитей
 */
static double *TimersArray = 0;


/** Уровень вложенности информации в логе. Определяет сколько отступов от края делать.*/
static unsigned int dvmhLogShift = 0;

// -- Internal functions declaration------------------------------------------------------------------------------------

/**
 * Инициализация описателя GPU в заголовке
 *
 * @param pDvmhStatGpu указатель на описатель GPU
 */
static void dvmh_stat_init_gpu_info(dvmh_stat_header_gpu_info * const pDvmhStatGpu);

/**
 * Инициализация метрики
 *
 * @param pDvmhStatMetric указатель на метрику DVMH статистики
 */
static void dvmh_stat_init_metric(dvmh_stat_interval_gpu_metric * const pDvmhStatMetric);

#if DVMH_EXTENDED_STAT == 1
/**
 * Очистить память занимаемую значениями метрики
 *
 * @param pDvmhStatMetric указатель на метрику DVMH статистики
 */
static void dvmh_stat_clean_values(dvmh_stat_interval_gpu_metric * const pDvmhStatMetric);

/**
 * Редуцировать значения до статистической модели
 *
 * @param pDvmhStatMetric указатель на метрику DVMH статистики
 */
static void dvmh_stat_reduce_values(dvmh_stat_interval_gpu_metric * const pDvmhStatMetric);
#endif

/**
 * Записать значение метрики
 *
 * @param pDvmhStatMetric указатель на метрику DVMH статистики
 * @param timeProductive полезное время
 * @param lostLost       потерянное время
 */
static void dvmh_stat_add_value(dvmh_stat_interval_gpu_metric * const pDvmhStatMetric, const double value,
                                const double timeProductive, const double timeLost);

/**
 * Распространить статистику к родителю
 *
 * @param child  dvmh статистика интервала
 * @param parent dvmh статистика родительского интервала
 */
static void dvmh_stat_bubble(const dvmh_stat_interval * const child, dvmh_stat_interval * const parent);

/**
 * Упаковать интрвальную статистику DVMH в буфере
 *
 * Убирает выравнивания памяти созданные компилятором. Необходимо для корректного чтения статистики
 * утилитой PPPA без привязки к архитектуре и компилятору.
 *
 * @param pOutputBuffer     буфер в который записываем интервальную статистику
 * @param pDvmhStatInterval ссылка на интервальную статистику
 */
static void dvmh_stat_pack_interval(void * const pOutputBuffer, dvmh_stat_interval * const pDvmhStatInterval);

/**
 * Упаковать интрвальную метрику
 *
 * @param pBuffer         (in/out)смещение в буфере записи
 * @param pDvmhStatMetric метрика
 */
static void dvmh_stat_pack_metric(unsigned char ** pBuffer, dvmh_stat_interval_gpu_metric * const pDvmhStatMetric);

static void dvmh_stat_read_env() {

}

// -- Interface functions ----------------------------------------------------------------------------------------------

UDvmType dvmh_stat_get_threads_amount()
{
#ifdef _OPENMP        
        return omp_get_max_threads();
#else
        return 1;
#endif
}

double* dvmh_stat_get_timers_array()
{
	if(TimersArray==0)
		TimersArray = (double*) malloc(dvmh_stat_get_threads_amount() * sizeof(double));
	return TimersArray;
}

void dvmh_stat_free_timers_array()
{
	if(TimersArray != 0)
	{
		free(TimersArray);
		TimersArray = 0;
	}
}

void dvmh_stat_init_header(void * const pBuffer) {
    unsigned int i;

    dvmh_stat_log(DVMH_STAT_LOG_LVL_DOWN, "dvmh_stat_init_header (start)");

    dvmhStatisticsHeader       = (dvmh_stat_header *) pBuffer;

    // Запомниаем размеры структур с отсупами, для дальнейшей корректной распаковки в PPPA
    dvmhStatisticsHeader->sizeHeader   = DVMH_STAT_HEADER_SIZE;
    dvmhStatisticsHeader->sizeIntervalConstPart = DVMH_STAT_INTERVAL_STATIC_SIZE;// dvmh_STAT_INTERVAL_SIZE;
    /*В этот момент omp_get_max_threads ещё не готова к работе*/
    //dvmhStatisticsHeader->threadsAmount = dvmh_stat_get_threads_amount();
    dvmhStatisticsHeader->threadsAmount = 0;

    for (i = 0; i < DVMH_STAT_MAX_GPU_CNT; ++i)
        dvmh_stat_init_gpu_info(& dvmhStatisticsHeader->gpu[i]);

    dvmh_stat_log(DVMH_STAT_LOG_LVL_THIS, "dvmh_stat_init_header (info) Size: %d", DVMH_STAT_HEADER_SIZE);
    dvmh_stat_log(DVMH_STAT_LOG_LVL_THIS, "dvmh_stat_init_header (info) Size(dvmh_stat_header): %d",  dvmhStatisticsHeader->sizeHeader);
    dvmh_stat_log(DVMH_STAT_LOG_LVL_THIS, "dvmh_stat_init_header (info) Size(dvmh_stat_header_gpu_info): %d", sizeof(dvmh_stat_header_gpu_info));
    dvmh_stat_log(DVMH_STAT_LOG_LVL_THIS, "dvmh_stat_init_header (info) Size(dvmh_stat_interval): %d", dvmhStatisticsHeader->sizeIntervalConstPart);

    dvmh_stat_log(DVMH_STAT_LOG_LVL_UP, "dvmh_stat_init_header (stop)");
}

void dvmh_stat_interval_start(void * const pBuffer) {
    unsigned int i, j;
    dvmh_stat_interval_gpu        *pDvmhStatGpu;
    dvmh_stat_interval_gpu_metric *pDvmhStatMetric;

    dvmh_stat_log(DVMH_STAT_LOG_LVL_DOWN, "dvmh_stat_interval_start (start)");

    dvmh_stat_log(DVMH_STAT_LOG_LVL_THIS, "dvmh_stat_interval_start (info) Static size: %d", DVMH_STAT_INTERVAL_STATIC_SIZE);
    dvmh_stat_log(DVMH_STAT_LOG_LVL_THIS, "dvmh_stat_interval_start (info) Size(dvmh_stat_interval): %d", sizeof(dvmh_stat_interval));
    dvmh_stat_log(DVMH_STAT_LOG_LVL_THIS, "dvmh_stat_interval_start (info) Size(dvmh_stat_interval:mask): %d", sizeof(dvmhStatisticsIntervalCurrent->mask));
    dvmh_stat_log(DVMH_STAT_LOG_LVL_THIS, "dvmh_stat_interval_start (info) Size(dvmh_stat_interval:gpu) : %d", sizeof(dvmhStatisticsIntervalCurrent->gpu));
    dvmh_stat_log(DVMH_STAT_LOG_LVL_THIS, "dvmh_stat_interval_start (info) Size(dvmh_stat_interval_gpu): %d", sizeof(dvmh_stat_interval_gpu));
    dvmh_stat_log(DVMH_STAT_LOG_LVL_THIS, "dvmh_stat_interval_start (info) Size(dvmh_stat_interval_gpu:metrics): %d", sizeof(pDvmhStatGpu->metrics));
    dvmh_stat_log(DVMH_STAT_LOG_LVL_THIS, "dvmh_stat_interval_start (info) Size(dvmh_stat_interval_gpu_metric): %d", sizeof(dvmh_stat_interval_gpu_metric));
    dvmh_stat_log(DVMH_STAT_LOG_LVL_THIS, "dvmh_stat_interval_start (info) Size(dvmh_stat_interval_gpu_metric:hasOwnMeasures): %d", sizeof(pDvmhStatMetric->hasOwnMeasures));
    dvmh_stat_log(DVMH_STAT_LOG_LVL_THIS, "dvmh_stat_interval_start (info) Size(dvmh_stat_interval_gpu_metric:isReduced)     : %d", sizeof(pDvmhStatMetric->isReduced));
    #if DVMH_EXTENDED_STAT == 1
    dvmh_stat_log(DVMH_STAT_LOG_LVL_THIS, "dvmh_stat_interval_start (info) Size(dvmh_stat_interval_gpu_metric:values)        : %d", sizeof(pDvmhStatMetric->values));
    #endif
    dvmh_stat_log(DVMH_STAT_LOG_LVL_THIS, "dvmh_stat_interval_start (info) Size(dvmh_stat_interval_gpu_metric:countMeasures) : %d", sizeof(pDvmhStatMetric->countMeasures));
    dvmh_stat_log(DVMH_STAT_LOG_LVL_THIS, "dvmh_stat_interval_start (info) Size(dvmh_stat_interval_gpu_metric:timeProductive): %d", sizeof(pDvmhStatMetric->timeProductive));
    dvmh_stat_log(DVMH_STAT_LOG_LVL_THIS, "dvmh_stat_interval_start (info) Size(dvmh_stat_interval_gpu_metric:timeLost)      : %d", sizeof(pDvmhStatMetric->timeLost));
    dvmh_stat_log(DVMH_STAT_LOG_LVL_THIS, "dvmh_stat_interval_start (info) Size(dvmh_stat_interval_gpu_metric:min)           : %d", sizeof(pDvmhStatMetric->min));
    dvmh_stat_log(DVMH_STAT_LOG_LVL_THIS, "dvmh_stat_interval_start (info) Size(dvmh_stat_interval_gpu_metric:mean)          : %d", sizeof(pDvmhStatMetric->mean));
    dvmh_stat_log(DVMH_STAT_LOG_LVL_THIS, "dvmh_stat_interval_start (info) Size(dvmh_stat_interval_gpu_metric:max)           : %d", sizeof(pDvmhStatMetric->max));
    dvmh_stat_log(DVMH_STAT_LOG_LVL_THIS, "dvmh_stat_interval_start (info) Size(dvmh_stat_interval_gpu_metric:sum)           : %d", sizeof(pDvmhStatMetric->sum));
    #if DVMH_EXTENDED_STAT == 1
    dvmh_stat_log(DVMH_STAT_LOG_LVL_THIS, "dvmh_stat_interval_start (info) Size(dvmh_stat_interval_gpu_metric:q1)            : %d", sizeof(pDvmhStatMetric->q1));
    dvmh_stat_log(DVMH_STAT_LOG_LVL_THIS, "dvmh_stat_interval_start (info) Size(dvmh_stat_interval_gpu_metric:median)        : %d", sizeof(pDvmhStatMetric->median));
    dvmh_stat_log(DVMH_STAT_LOG_LVL_THIS, "dvmh_stat_interval_start (info) Size(dvmh_stat_interval_gpu_metric:q3)            : %d", sizeof(pDvmhStatMetric->q3));
    #endif


    // Инициализация заголовка
    * (dvmh_stat_interval **) pBuffer = NULL;
    dvmh_stat_interval_load(pBuffer);

    dvmh_stat_log(DVMH_STAT_LOG_LVL_THIS, "dvmh_stat_interval_start (info) dvmhStatisticsIntervalCurrent: %p", dvmhStatisticsIntervalCurrent);

    dvmhStatisticsIntervalCurrent->mask = 0L;

    // Инициализируем метрики и gpu
    for (i = 0; i < DVMH_STAT_MAX_GPU_CNT; ++i) {
        pDvmhStatGpu = & (dvmhStatisticsIntervalCurrent->gpu[i]);

        dvmh_stat_log(DVMH_STAT_LOG_LVL_THIS, "dvmh_stat_interval_start (info) gpu#%d: %p", i, pDvmhStatGpu);
        for (j = 0; j < DVMH_STAT_METRIC_CNT; ++j) {
            pDvmhStatMetric = & (pDvmhStatGpu->metrics[j]);
            dvmh_stat_log(DVMH_STAT_LOG_LVL_THIS, "dvmh_stat_interval_start (info) gpu#%d:metric#%d: %p", i, j, pDvmhStatMetric);
            dvmh_stat_init_metric(pDvmhStatMetric);
        }
    }
    //Инициализация статистики нитей
    UDvmType threadsAmount = dvmh_stat_get_threads_amount();
    for (i = 0; i < threadsAmount; ++i)
    {
        dvmhStatisticsIntervalCurrent->threads[i].user_time = 0.0;
        dvmhStatisticsIntervalCurrent->threads[i].system_time = 0.0;
    }

    dvmh_stat_log(DVMH_STAT_LOG_LVL_UP, "dvmh_stat_interval_start (stop)");
}

void dvmh_stat_interval_load(void * const pBuffer) {
    dvmh_stat_log(DVMH_STAT_LOG_LVL_DOWN, "dvmh_stat_interval_load (start)");

    if (!pBuffer) {
        dvmhStatisticsIntervalCurrent = 0;
        dvmh_stat_log(DVMH_STAT_LOG_LVL_UP, "dvmh_stat_interval_load (stop:1)");
        return;
    }

    dvmhStatisticsIntervalCurrent = * (dvmh_stat_interval **) pBuffer;

    if (!dvmhStatisticsIntervalCurrent) {
        dvmhStatisticsIntervalCurrent = (dvmh_stat_interval *) dvm_getmemnoerr(sizeof(dvmh_stat_interval));
        * (dvmh_stat_interval **) pBuffer = dvmhStatisticsIntervalCurrent;
        dvmhStatisticsIntervalCurrent->threads =
		 (dvmh_stat_interval_thread*) dvm_getmemnoerr(sizeof(dvmh_stat_interval_thread) * dvmh_stat_get_threads_amount());

        dvmh_stat_log(DVMH_STAT_LOG_LVL_THIS, "dvmh_stat_interval_load (info) Allocate interval storage [%p]", dvmhStatisticsIntervalCurrent);
    }
    dvmh_stat_log(DVMH_STAT_LOG_LVL_THIS, "dvmh_stat_interval_load (info) Loaded interval [%p]",dvmhStatisticsIntervalCurrent);

    dvmh_stat_log(DVMH_STAT_LOG_LVL_UP, "dvmh_stat_interval_load (stop)");
}

void dvmh_stat_interval_stop(void * const pBufferCurrent, void * const pBufferParent, void * const pOutputBuffer) {
    unsigned int i, j;
    dvmh_stat_interval_gpu        *pDvmhStatGpu;
    dvmh_stat_interval_gpu_metric *pDvmhStatMetric;
    dvmh_stat_interval            *pDvmhIntervalCurrent;
    dvmh_stat_interval            *pDvmhIntervalParent;

    dvmh_stat_log(DVMH_STAT_LOG_LVL_DOWN, "dvmh_stat_interval_stop (start) (%p)", pBufferCurrent);

    dvmh_stat_log(DVMH_STAT_LOG_LVL_THIS, "dvmh_stat_interval_stop (info) [%p|%p -> %p]", pBufferParent, pBufferCurrent, pOutputBuffer);

    if (pBufferCurrent) {
        dvmh_stat_interval_load(pBufferCurrent);
        pDvmhIntervalCurrent = dvmhStatisticsIntervalCurrent;

        dvmh_stat_interval_load(pBufferParent);
        pDvmhIntervalParent = dvmhStatisticsIntervalCurrent;
    }else {
        dvmh_stat_log(DVMH_STAT_LOG_LVL_UP, "dvmh_stat_interval_stop (stop:1)");
        return;
    }

    dvmh_stat_log(DVMH_STAT_LOG_LVL_THIS, "dvmh_stat_interval_stop (info) pDvmhIntervalCurrent: %p", pDvmhIntervalCurrent);
    dvmh_stat_log(DVMH_STAT_LOG_LVL_THIS, "dvmh_stat_interval_stop (info) pDvmhIntervalParent : %p", pDvmhIntervalParent);

    if (!pDvmhIntervalCurrent) {
        dvmh_stat_log(DVMH_STAT_LOG_LVL_UP, "dvmh_stat_interval_stop (stop:2)");
        return;
    }

    dvmh_stat_log(DVMH_STAT_LOG_LVL_THIS, "dvmh_stat_interval_stop (info) gpu-mask: %d", pDvmhIntervalCurrent->mask);
    for (i = 0; i < DVMH_STAT_MAX_GPU_CNT; ++i) {
        pDvmhStatGpu = & (pDvmhIntervalCurrent->gpu[i]);

        dvmh_stat_log(DVMH_STAT_LOG_LVL_THIS, "dvmh_stat_interval_stop (info) gpu#%d: %p", i, pDvmhStatGpu);
        for (j = 0; j < DVMH_STAT_METRIC_CNT; ++j) {
            pDvmhStatMetric = & (pDvmhStatGpu->metrics[j]);

            dvmh_stat_log(DVMH_STAT_LOG_LVL_THIS, "dvmh_stat_interval_stop (info) gpu#%d:metric#%d: %p", i, j, pDvmhStatMetric);
            #if DVMH_EXTENDED_STAT == 1
            dvmh_stat_reduce_values(pDvmhStatMetric);
            dvmh_stat_clean_values(pDvmhStatMetric);
            #else
            if (pDvmhStatMetric->countMeasures > 0)
                pDvmhStatMetric->isReduced = 1;
            else
                pDvmhStatMetric->isReduced = 0;
            #endif
        }
    }

    dvmh_stat_bubble(pDvmhIntervalCurrent, pDvmhIntervalParent);
    dvmh_stat_pack_interval(pOutputBuffer, pDvmhIntervalCurrent);

    dvm_FreeStruct(pDvmhIntervalCurrent);

    dvmh_stat_log(DVMH_STAT_LOG_LVL_UP, "dvmh_stat_interval_stop (stop)");
}

void dvmh_stat_add_measurement(const int gpuNo, dvmh_stat_metric_names metric, const double value,
                               const double timeProductive, const double timeLost)
{
#ifdef _F_TIME_
    dvmh_stat_interval_gpu        *pDvmhStatGpu;
    dvmh_stat_interval_gpu_metric *pDvmhStatMetric;

    dvmh_stat_log(DVMH_STAT_LOG_LVL_DOWN, "dvmh_stat_add_measurement (start)");

    if(!IsExpend) {
        dvmh_stat_log(DVMH_STAT_LOG_LVL_UP, "dvmh_stat_add_measurement (stop:1)");
        return;
    }

    if (gpuNo >= DVMH_STAT_MAX_GPU_CNT || metric >= DVMH_STAT_METRIC_CNT || !dvmhStatisticsIntervalCurrent) {
        dvmh_stat_log(DVMH_STAT_LOG_LVL_UP, "dvmh_stat_add_measurement (stop:2)");
        return;
    }

    dvmh_stat_log(DVMH_STAT_LOG_LVL_THIS, "dvmh_stat_add_measurement (info) Gpu#%d Metric#%d: %.4lf (%.4lf, %.4lf)",
        gpuNo, metric, value, timeProductive,timeLost );

    pDvmhStatGpu    = & (dvmhStatisticsIntervalCurrent->gpu[gpuNo]);
    pDvmhStatMetric = & (pDvmhStatGpu->metrics[metric]);

    // Указываем что GPU был использован
    // XXX: correct << operation: if 64x -> long long(1 ) << gpuNo;
    dvmhStatisticsIntervalCurrent->mask |= 1 << gpuNo;

    dvmh_stat_add_value(pDvmhStatMetric, value, timeProductive, timeLost);

    dvmh_stat_log(DVMH_STAT_LOG_LVL_UP, "dvmh_stat_add_measurement (stop)");
#endif
}

void dvmh_stat_set_gpu_info(const int gpuNo, const int id, const char *name) {
#ifdef _F_TIME_
    dvmh_stat_header_gpu_info *pDvmhStatGpuInfo;

    dvmh_stat_log(DVMH_STAT_LOG_LVL_DOWN, "dvmh_stat_set_gpu_info (start)");

    if (gpuNo >= DVMH_STAT_MAX_GPU_CNT || !dvmhStatisticsHeader) {
        dvmh_stat_log(DVMH_STAT_LOG_LVL_UP, "dvmh_stat_set_gpu_info (stop:1)");
        return;
    }

    pDvmhStatGpuInfo = & (dvmhStatisticsHeader->gpu[gpuNo]);
    pDvmhStatGpuInfo->id = id;

    strncpy((char*)(pDvmhStatGpuInfo->name), name, DVMH_STAT_SIZE_STR);
    pDvmhStatGpuInfo->name[DVMH_STAT_SIZE_STR - 1] = '\0';

    dvmh_stat_log(DVMH_STAT_LOG_LVL_THIS, "dvmh_stat_set_gpu_info (info) gpu  = %d", gpuNo);
    dvmh_stat_log(DVMH_STAT_LOG_LVL_THIS, "dvmh_stat_set_gpu_info (info) id   = %d", pDvmhStatGpuInfo->id);
    dvmh_stat_log(DVMH_STAT_LOG_LVL_THIS, "dvmh_stat_set_gpu_info (info) name = %s", pDvmhStatGpuInfo->name);

    dvmh_stat_log(DVMH_STAT_LOG_LVL_UP, "dvmh_stat_set_gpu_info (stop)");
#endif
}

void dvmh_stat_pack_header() {
    dvmh_stat_header_gpu_info *pDvmhGpuInfo;
    unsigned char *pBufferShift;
    unsigned int i, j;

    dvmh_stat_log(DVMH_STAT_LOG_LVL_DOWN, "dvmh_stat_pack_header (start)");

    if (!dvmhStatisticsHeader) {
        dvmh_stat_log(DVMH_STAT_LOG_LVL_UP, "dvmh_stat_pack_header (stop:1)");
        return;
    }

    pBufferShift = (unsigned char *) dvmhStatisticsHeader;

    // Пакуем собственную информацию
    CPYMEM(*(UDvmType *)pBufferShift, dvmhStatisticsHeader->sizeHeader);
    dvmh_stat_log(DVMH_STAT_LOG_LVL_THIS, "dvmh_stat_pack_header (info) sizeHeader: %lu", *(UDvmType *)pBufferShift);
    pBufferShift += sizeof(UDvmType);

    CPYMEM(*(UDvmType *)pBufferShift, dvmhStatisticsHeader->sizeIntervalConstPart);
    dvmh_stat_log(DVMH_STAT_LOG_LVL_THIS, "dvmh_stat_pack_header (info) sizeIntervalConstPart: %lu", *(UDvmType *)pBufferShift);
    pBufferShift += sizeof(UDvmType);
    
    dvmhStatisticsHeader->threadsAmount = dvmh_stat_get_threads_amount();
    CPYMEM(*(UDvmType *)pBufferShift, dvmhStatisticsHeader->threadsAmount);
    dvmh_stat_log(DVMH_STAT_LOG_LVL_THIS, "dvmh_stat_pack_header (info) sizeIntervalConstPart: %lu", *(UDvmType *)pBufferShift);
    pBufferShift += sizeof(UDvmType);

    // Пакуем информацию о GPU
    for (i = 0; i < DVMH_STAT_MAX_GPU_CNT; ++i) {
        pDvmhGpuInfo = &dvmhStatisticsHeader->gpu[i];

        dvmh_stat_log(DVMH_STAT_LOG_LVL_THIS, "dvmh_stat_pack_header (info) gpu#%d", i);

        CPYMEM(*(UDvmType *)pBufferShift, pDvmhGpuInfo->id);
        pBufferShift += sizeof(UDvmType);
        for (j = 0; j <= DVMH_STAT_SIZE_STR; ++j) {
            CPYMEM(*(unsigned char *) pBufferShift, pDvmhGpuInfo->name[j]);
            pBufferShift += sizeof(unsigned char);
        }
    }

    dvmh_stat_log(DVMH_STAT_LOG_LVL_THIS, "dvmh_stat_pack_header (info) Size: %lu -> %lu", DVMH_STAT_HEADER_SIZE, (UDvmType)pBufferShift - (UDvmType)dvmhStatisticsHeader);
    dvmh_stat_log(DVMH_STAT_LOG_LVL_UP, "dvmh_stat_pack_header (stop)");
}

inline void dvmh_stat_log(dvmh_stat_log_lvl shift, char *format,  ...) {
#ifdef DVMH_STAT_DEBUG
    va_list args;

    // Уменьшаем уровень лога
    if (shift == DVMH_STAT_LOG_LVL_UP)
        dvmhLogShift -= DVMH_STAT_LOG_SHIFT_OFFSET;

    printf("%*s", dvmhLogShift, "");

    // Увеличиваем уровень лога
    if (shift == DVMH_STAT_LOG_LVL_DOWN)
        dvmhLogShift += DVMH_STAT_LOG_SHIFT_OFFSET;

    va_start (args, format);
    vfprintf(stdout, format, args);
    va_end (args);

    printf("\n");

    fflush(stdout);
#endif
}


// -- Internal functions -----------------------------------------------------------------------------------------------
// +
static void dvmh_stat_init_gpu_info(dvmh_stat_header_gpu_info * const pDvmhStatGpu) {
    dvmh_stat_log(DVMH_STAT_LOG_LVL_DOWN, "dvmh_stat_init_gpu_info (start)");

    if (!pDvmhStatGpu) {
        epprintf(MultiProcErrReg2, DVM_FILE[0], DVM_LINE[0],
                 "Statistics: internal error, bad pointer to gpu info in dvmh statistics, data may be broken.\n");
        return;
    }

    pDvmhStatGpu->id = -1;
    pDvmhStatGpu->name[0] = '\0';

    dvmh_stat_log(DVMH_STAT_LOG_LVL_UP, "dvmh_stat_init_gpu_info (stop)");
}

// +
static void dvmh_stat_init_metric(dvmh_stat_interval_gpu_metric * const pDvmhStatMetric) {
    dvmh_stat_log(DVMH_STAT_LOG_LVL_DOWN, "dvmh_stat_init_metric (start)");

    if (!pDvmhStatMetric) {
        epprintf(MultiProcErrReg2, DVM_FILE[0], DVM_LINE[0],
                 "Statistics: internal error, bad pointer to gpu metric in dvmh statistics, data may be broken.\n");
        return;
    }

    pDvmhStatMetric->hasOwnMeasures = 0;
    pDvmhStatMetric->countMeasures  = 0;
    #if DVMH_EXTENDED_STAT == 1
    pDvmhStatMetric->values         = NULL;
    #endif
    pDvmhStatMetric->isReduced      = 0;
    pDvmhStatMetric->timeLost       = -1.0f; // если отрицательное значение, вывод времени в PPPA не будет производится
    pDvmhStatMetric->timeProductive = -1.0f; // если отрицательное значение, вывод времени в PPPA не будет производится

    // Редуцируемые поля
    pDvmhStatMetric->min    = FLT_MAX;
    pDvmhStatMetric->mean   = 0.f;
    pDvmhStatMetric->max    = 0.f;
    pDvmhStatMetric->sum    = 0.f;
    #if DVMH_EXTENDED_STAT == 1
    pDvmhStatMetric->q1     = 0.f;
    pDvmhStatMetric->median = 0.f;
    pDvmhStatMetric->q3     = 0.f;
    #endif

    dvmh_stat_log(DVMH_STAT_LOG_LVL_UP, "dvmh_stat_interval_gpu_metric (stop)");
}

// +
static void dvmh_stat_bubble(const dvmh_stat_interval * const child, dvmh_stat_interval * const parent) {
    DvmType i, j, mask;
    dvmh_stat_interval_gpu              *gpuParent;
    const dvmh_stat_interval_gpu        *gpuChild;
    dvmh_stat_interval_gpu_metric       *metricParent;
    const dvmh_stat_interval_gpu_metric *metricChild;

    dvmh_stat_log(DVMH_STAT_LOG_LVL_DOWN, "dvmh_stat_bubble (start) [%p -> %p]", child, parent);

    if (!child || !parent || child == parent) {
        dvmh_stat_log(DVMH_STAT_LOG_LVL_UP, "dvmh_stat_bubble (stop:1)");
        return;
    }

    mask = child->mask;
    parent->mask |= child->mask;

    dvmh_stat_log(DVMH_STAT_LOG_LVL_THIS, "dvmh_stat_bubble (info) Mask #%lu", mask);
    for (i = 0; i < DVMH_STAT_MAX_GPU_CNT; ++i) {
        if (!((mask >> i) & 1)) continue;

        gpuParent = & (parent->gpu[i]);
        gpuChild  = & (child->gpu[i]);

        dvmh_stat_log(DVMH_STAT_LOG_LVL_THIS, "dvmh_stat_bubble (info) Gpu #%d", i);

        // Пропагируем информацию о метриках
        for (j = 0; j < DVMH_STAT_METRIC_CNT; ++j) {
            metricParent = & (gpuParent->metrics[j]);
            metricChild  = & (gpuChild->metrics[j]);

            dvmh_stat_log(DVMH_STAT_LOG_LVL_THIS, "dvmh_stat_bubble (info) Metric #%d", j);

            if (!metricChild->isReduced) continue;

            metricParent->isReduced = 1;

            if (metricChild->timeProductive > 0.f) {
                if (metricParent->timeProductive < 0.f) metricParent->timeProductive = 0.f;
                metricParent->timeProductive += metricChild->timeProductive;
            }

            if (metricChild->timeLost > 0.f) {
                if (metricParent->timeLost < 0.f) metricParent->timeLost = 0.f;
                metricParent->timeLost += metricChild->timeLost;
            }

            metricParent->sum            += metricChild->sum;
            if (metricParent->countMeasures + metricChild->countMeasures <= 0)
                metricParent->mean = 0.0;
            else
                metricParent->mean =
                    (metricParent->mean * metricParent->countMeasures + metricChild->mean * metricChild->countMeasures) /
                    (metricParent->countMeasures + metricChild->countMeasures);
            metricParent->countMeasures  += metricChild->countMeasures;

            if (metricParent->max < metricChild->max) metricParent->max = metricChild->max;
            if (metricParent->min > metricChild->min) metricParent->min = metricChild->min;

            dvmh_stat_log(DVMH_STAT_LOG_LVL_THIS, "dvmh_stat_bubble (info) min    = %.4f", metricParent->min);
            dvmh_stat_log(DVMH_STAT_LOG_LVL_THIS, "dvmh_stat_bubble (info) max    = %.4f", metricParent->max);
            dvmh_stat_log(DVMH_STAT_LOG_LVL_THIS, "dvmh_stat_bubble (info) mean   = %.4f", metricParent->mean);
            dvmh_stat_log(DVMH_STAT_LOG_LVL_THIS, "dvmh_stat_bubble (info) sum    = %.4f", metricParent->sum);
            #if DVMH_EXTENDED_STAT == 1
            dvmh_stat_log(DVMH_STAT_LOG_LVL_THIS, "dvmh_stat_bubble (info) q1     = %.4f", metricParent->q1);
            dvmh_stat_log(DVMH_STAT_LOG_LVL_THIS, "dvmh_stat_bubble (info) median = %.4f", metricParent->median);
            dvmh_stat_log(DVMH_STAT_LOG_LVL_THIS, "dvmh_stat_bubble (info) q3     = %.4f", metricParent->q3);
            #endif
        }
    }
    //Для нитей
    UDvmType threadsAmount = dvmh_stat_get_threads_amount();
    for(i = 0; i < threadsAmount; ++i)
    {
        parent->threads[i].user_time += child->threads[i].user_time;
        parent->threads[i].system_time += child->threads[i].system_time;
    }
    dvmh_stat_log(DVMH_STAT_LOG_LVL_THIS, "dvmh_stat_bubble (info) thread's info is propagateted", i);

    dvmh_stat_log(DVMH_STAT_LOG_LVL_UP, "dvmh_stat_bubble (stop)");
}

#if DVMH_EXTENDED_STAT == 1
static void dvmh_stat_clean_values(dvmh_stat_interval_gpu_metric * const pDvmhStatMetric) {
    dvmh_stat_interval_gpu_metric_value *pValue, *pValueNext;

    dvmh_stat_log(DVMH_STAT_LOG_LVL_DOWN, "dvmh_stat_clean_values (start)");

    if (!pDvmhStatMetric) return;

    pValue = pDvmhStatMetric->values;
    pDvmhStatMetric->values = 0;

    while (pValue) {
        dvmh_stat_log(DVMH_STAT_LOG_LVL_THIS, "dvmh_stat_clean_values (info) value: %0.4lf", pValue->value);

        pValueNext = pValue->next;
        dvm_freemem((void **) &pValue);
        pValue = pValueNext;
    }

    dvmh_stat_log(DVMH_STAT_LOG_LVL_UP, "dvmh_stat_clean_values (stop)");
}
#endif

#if DVMH_EXTENDED_STAT == 1
static void dvmh_stat_reduce_values(dvmh_stat_interval_gpu_metric * const pDvmhStatMetric) {
    dvmh_stat_interval_gpu_metric_value *pValue;
    double value, min, max, sum, mean, median, q1, q3, redCount, valCount;
    int i = 0, n, nQ1, nMedian, nQ3, isQ1Odd, isMedianOdd, isQ3Odd;

    dvmh_stat_log(DVMH_STAT_LOG_LVL_DOWN, "dvmh_stat_reduce_values (start) (%p)", pDvmhStatMetric);

    if (pDvmhStatMetric == 0 || pDvmhStatMetric->values == 0) {
        dvmh_stat_log(DVMH_STAT_LOG_LVL_UP, "dvmh_stat_reduce_values (stop:1)");
        return;
    }

    pValue = pDvmhStatMetric->values;

    if (pValue != 0) {
        min = pValue->value;
        max = pValue->value;
    }

    n           = pDvmhStatMetric->countMeasures;
    nMedian     = n / 2;
    isMedianOdd = n % 2 == 0;
    if (nMedian < 1) nMedian = 1;
    nQ1         = nMedian / 2;
    isQ1Odd     = nMedian % 2 == 0;
    if (nQ1 < 1) nQ1 = 1;
    nQ3         = nMedian + (n - nMedian) / 2;
    isQ3Odd     = (n - nMedian) % 2 == 0;
    if (nQ3 < 1) nQ3 = 1;
    valCount    = 0;

    dvmh_stat_log(DVMH_STAT_LOG_LVL_THIS, "dvmh_stat_reduce_values (info) hasOwnMeasures: %d", pDvmhStatMetric->hasOwnMeasures);
    dvmh_stat_log(DVMH_STAT_LOG_LVL_THIS, "dvmh_stat_reduce_values (info) N          : %d", n);
    dvmh_stat_log(DVMH_STAT_LOG_LVL_THIS, "dvmh_stat_reduce_values (info) nMedian    : %d", nMedian);
    dvmh_stat_log(DVMH_STAT_LOG_LVL_THIS, "dvmh_stat_reduce_values (info) isMedianOdd: %d", isMedianOdd);
    dvmh_stat_log(DVMH_STAT_LOG_LVL_THIS, "dvmh_stat_reduce_values (info) nQ1        : %d", nQ1);
    dvmh_stat_log(DVMH_STAT_LOG_LVL_THIS, "dvmh_stat_reduce_values (info) isQ1Odd    : %d", isQ1Odd);
    dvmh_stat_log(DVMH_STAT_LOG_LVL_THIS, "dvmh_stat_reduce_values (info) nQ3        : %d", nQ3);
    dvmh_stat_log(DVMH_STAT_LOG_LVL_THIS, "dvmh_stat_reduce_values (info) isQ3Odd    : %d", isQ3Odd);

    sum    = 0.f;
    mean   = 0.f;
    median = 0.f;
    q1     = 0.f;
    q3     = 0.f;

    sum = 0;
    while (pValue) {
        value = pValue->value;
        i++;

        if (min > value) min = value;
        if (max < value) max = value;

        sum += value;

        if (i == nQ1) q1 = value;
        if ((i == nQ1 + 1) && isQ1Odd) q1 = (q1 + value) / 2;

        if (i == nMedian) median = value;
        if ((i == nMedian + 1) && isMedianOdd) median = (median + value) / 2;

        if (i == nQ3) q3 = value;
        if ((i == nQ3 + 1) && isQ3Odd) q3 = (q3 + value) / 2;

        dvmh_stat_log(DVMH_STAT_LOG_LVL_THIS, "dvmh_stat_reduce_values (info) Value#%d: %.4f", i, value);
        dvmh_stat_log(DVMH_STAT_LOG_LVL_THIS, "dvmh_stat_reduce_values (info) q1     = %.4f", q1);
        dvmh_stat_log(DVMH_STAT_LOG_LVL_THIS, "dvmh_stat_reduce_values (info) median = %.4f", median);
        dvmh_stat_log(DVMH_STAT_LOG_LVL_THIS, "dvmh_stat_reduce_values (info) q3     = %.4f", q3);
        dvmh_stat_log(DVMH_STAT_LOG_LVL_THIS, "dvmh_stat_reduce_values (info) i == nQ1    : %d", (i == nQ1));
        dvmh_stat_log(DVMH_STAT_LOG_LVL_THIS, "dvmh_stat_reduce_values (info) i == nMedian: %d", (i == nMedian));
        dvmh_stat_log(DVMH_STAT_LOG_LVL_THIS, "dvmh_stat_reduce_values (info) i == nQ3    : %d", (i == nQ3));

        pValue = pValue->next;
        valCount++;
    }

    if (valCount) mean = sum / valCount;


    // Запись значение
    if (pDvmhStatMetric->isReduced) {
        if (pDvmhStatMetric->min > min) pDvmhStatMetric->min = min;
        if (pDvmhStatMetric->max < max) pDvmhStatMetric->max = max;
        redCount = n - valCount;
        pDvmhStatMetric->mean =
                (pDvmhStatMetric->mean * redCount + mean * valCount) / n;
        pDvmhStatMetric->sum += sum;
    }else {
        pDvmhStatMetric->min = min;
        pDvmhStatMetric->max = max;
        pDvmhStatMetric->sum = sum;
        pDvmhStatMetric->mean = mean;
        pDvmhStatMetric->isReduced = 1;
    }

    pDvmhStatMetric->q1 = q1;
    pDvmhStatMetric->median = median;
    pDvmhStatMetric->q3 = q3;

    dvmh_stat_log(DVMH_STAT_LOG_LVL_THIS, "dvmh_stat_reduce_values (info) ----");
    dvmh_stat_log(DVMH_STAT_LOG_LVL_THIS, "dvmh_stat_reduce_values (info) min    = %.4f", pDvmhStatMetric->min);
    dvmh_stat_log(DVMH_STAT_LOG_LVL_THIS, "dvmh_stat_reduce_values (info) max    = %.4f", pDvmhStatMetric->max);
    dvmh_stat_log(DVMH_STAT_LOG_LVL_THIS, "dvmh_stat_reduce_values (info) mean   = %.4f", pDvmhStatMetric->mean);
    dvmh_stat_log(DVMH_STAT_LOG_LVL_THIS, "dvmh_stat_reduce_values (info) sum    = %.4f", pDvmhStatMetric->sum);
    dvmh_stat_log(DVMH_STAT_LOG_LVL_THIS, "dvmh_stat_reduce_values (info) q1     = %.4f", pDvmhStatMetric->q1);
    dvmh_stat_log(DVMH_STAT_LOG_LVL_THIS, "dvmh_stat_reduce_values (info) median = %.4f", pDvmhStatMetric->median);
    dvmh_stat_log(DVMH_STAT_LOG_LVL_THIS, "dvmh_stat_reduce_values (info) q3     = %.4f", pDvmhStatMetric->q3);

    dvmh_stat_log(DVMH_STAT_LOG_LVL_UP, "dvmh_stat_reduce_values (stop)");
}
#endif

// +
static void dvmh_stat_add_value(dvmh_stat_interval_gpu_metric * const pDvmhStatMetric, const double value,
                                const double timeProductive, const double timeLost)
{
    #if DVMH_EXTENDED_STAT == 1
    dvmh_stat_interval_gpu_metric_value *pValueNew, *pValueCurr, *pValuePrev;
    #endif

    dvmh_stat_log(DVMH_STAT_LOG_LVL_DOWN, "dvmh_stat_add_value (start)");

    if (!pDvmhStatMetric || rand() % 100 >= DVMH_STAT_VALUES_GATHER) {
        dvmh_stat_log(DVMH_STAT_LOG_LVL_UP, "dvmh_stat_add_value (stop:dropped by rand)");
        return;
    }

    pDvmhStatMetric->hasOwnMeasures = 1;
    pDvmhStatMetric->countMeasures++;

    if (pDvmhStatMetric->timeProductive < 0) pDvmhStatMetric->timeProductive = 0.f;
    if (pDvmhStatMetric->timeLost       < 0) pDvmhStatMetric->timeLost       = 0.f;

    pDvmhStatMetric->timeProductive += timeProductive;
    pDvmhStatMetric->timeLost       += timeLost;

    #if DVMH_EXTENDED_STAT == 1
    pValueCurr = pDvmhStatMetric->values;
    pValuePrev = 0;
    while (pValueCurr) {
        if (pValueCurr->value > value) break;
        pValuePrev = pValueCurr;
        pValueCurr = pValueCurr->next;
    }

    pValueNew = (dvmh_stat_interval_gpu_metric_value *) dvm_getmemnoerr(sizeof(dvmh_stat_interval_gpu_metric_value));
    if (!pValueNew) {
        epprintf(MultiProcErrReg2, DVM_FILE[0], DVM_LINE[0],
                 "Statistics:not enough memory for dvmh metric value, data were not wrote to the file\n");
        return;
    }

    pValueNew->value = value;
    pValueNew->next  = pValueCurr;
    if (pValuePrev)
        pValuePrev->next = pValueNew;
    else
        pDvmhStatMetric->values = pValueNew;
    #else
    if (value < pDvmhStatMetric->min) pDvmhStatMetric->min = value;
    if (value > pDvmhStatMetric->max) pDvmhStatMetric->max = value;
    pDvmhStatMetric->sum  += value;
    if (pDvmhStatMetric->countMeasures <= 0)
        pDvmhStatMetric->mean = 0.0;
    else
        pDvmhStatMetric->mean = (pDvmhStatMetric->mean * (pDvmhStatMetric->countMeasures - 1) + value)
                            / pDvmhStatMetric->countMeasures;
    #endif

    dvmh_stat_log(DVMH_STAT_LOG_LVL_THIS, "dvmh_stat_add_value (info) prod  = %.4f", pDvmhStatMetric->timeProductive);
    dvmh_stat_log(DVMH_STAT_LOG_LVL_THIS, "dvmh_stat_add_value (info) lost  = %.4f", pDvmhStatMetric->timeLost);
    dvmh_stat_log(DVMH_STAT_LOG_LVL_THIS, "dvmh_stat_add_value (info) value = %.4f", value);
    #if DVMH_EXTENDED_STAT != 1
    dvmh_stat_log(DVMH_STAT_LOG_LVL_THIS, "dvmh_stat_add_value (info) min   = %.4f", pDvmhStatMetric->min);
    dvmh_stat_log(DVMH_STAT_LOG_LVL_THIS, "dvmh_stat_add_value (info) mean  = %.4f", pDvmhStatMetric->mean);
    dvmh_stat_log(DVMH_STAT_LOG_LVL_THIS, "dvmh_stat_add_value (info) max   = %.4f", pDvmhStatMetric->max);
    #endif

    dvmh_stat_log(DVMH_STAT_LOG_LVL_UP, "dvmh_stat_add_value (stop)");
}

// +
static void dvmh_stat_pack_interval(void * const pOutputBuffer, dvmh_stat_interval * const pDvmhStatInterval) {
    unsigned int i, j;
    unsigned char *pBuffer;
    dvmh_stat_interval_gpu        *pDvmhStatGpu;
    dvmh_stat_interval_gpu_metric *pDvmhStatMetric;

    dvmh_stat_log(DVMH_STAT_LOG_LVL_DOWN, "dvmh_stat_pack_interval (start)");

    dvmh_stat_log(DVMH_STAT_LOG_LVL_THIS, "dvmh_stat_pack_interval (info) [%p -> %p]", pDvmhStatInterval, pOutputBuffer);

    pBuffer = (unsigned char *) pOutputBuffer;

    CPYMEM(*(UDvmType *)pBuffer, pDvmhStatInterval->mask);
    pBuffer += sizeof(UDvmType);

    for (i = 0; i < DVMH_STAT_MAX_GPU_CNT; ++i) {
        pDvmhStatGpu = & (pDvmhStatInterval->gpu[i]);

        dvmh_stat_log(DVMH_STAT_LOG_LVL_THIS, "dvmh_stat_pack_interval (info) gpu#%d: %p", i, pDvmhStatGpu);
        for (j = 0; j < DVMH_STAT_METRIC_CNT; ++j) {
            pDvmhStatMetric = & (pDvmhStatGpu->metrics[j]);
            dvmh_stat_log(DVMH_STAT_LOG_LVL_THIS, "dvmh_stat_pack_interval (info) gpu#%d:metric#%d: %p", i, j, pDvmhStatMetric);
            dvmh_stat_pack_metric(&pBuffer, pDvmhStatMetric);

            if (i == 0 && j == 0)
                dvmh_stat_log(DVMH_STAT_LOG_LVL_THIS, "dvmh_stat_pack_interval (info) Size(metric): %lu -> %lu", sizeof(dvmh_stat_interval_gpu_metric), (UDvmType)pBuffer - (UDvmType)pOutputBuffer);
        }

        if (i == 0)
            dvmh_stat_log(DVMH_STAT_LOG_LVL_THIS, "dvmh_stat_pack_interval (info) Size(gpu): %lu -> %lu", sizeof(dvmh_stat_interval_gpu), (UDvmType)pBuffer - (UDvmType)pOutputBuffer);
    }
    unsigned long threadsAmount = dvmh_stat_get_threads_amount();
    for(i=0;i<threadsAmount;++i)
    {
    	CPYMEM(*(double *)pBuffer, pDvmhStatInterval->threads[i].user_time);
    	pBuffer += sizeof(double);
    	CPYMEM(*(double *)pBuffer, pDvmhStatInterval->threads[i].system_time);
    	pBuffer += sizeof(double);
    }

    dvmh_stat_log(DVMH_STAT_LOG_LVL_THIS, "dvmh_stat_pack_interval (info) Size: %lu -> %lu", DVMH_STAT_INTERVAL_SIZE, (UDvmType)pBuffer - (UDvmType)pOutputBuffer);

    dvmh_stat_log(DVMH_STAT_LOG_LVL_UP, "dvmh_stat_pack_interval (stop)");
}

// +
static void dvmh_stat_pack_metric(unsigned char ** pBuffer, dvmh_stat_interval_gpu_metric * const pDvmhStatMetric) {
    dvmh_stat_log(DVMH_STAT_LOG_LVL_DOWN, "dvmh_stat_pack_metric (start)");

    CPYMEM( * (short *) *pBuffer, pDvmhStatMetric->isReduced);
    *pBuffer += sizeof(short);

    CPYMEM( * (short *) *pBuffer, pDvmhStatMetric->hasOwnMeasures);
    *pBuffer += sizeof(short);

    CPYMEM( * (UDvmType *) *pBuffer, pDvmhStatMetric->countMeasures);
    *pBuffer += sizeof(UDvmType);

    CPYMEM( * (double *) *pBuffer, pDvmhStatMetric->timeProductive);
    *pBuffer += sizeof(double);

    CPYMEM( * (double *) *pBuffer, pDvmhStatMetric->timeLost);
    *pBuffer += sizeof(double);

    CPYMEM( * (double *) *pBuffer, pDvmhStatMetric->min);
    *pBuffer += sizeof(double);

    CPYMEM( * (double *) *pBuffer, pDvmhStatMetric->mean);
    *pBuffer += sizeof(double);

    CPYMEM( * (double *) *pBuffer, pDvmhStatMetric->max);
    *pBuffer += sizeof(double);

    CPYMEM( * (double *) *pBuffer, pDvmhStatMetric->sum);
    *pBuffer += sizeof(double);

#if DVMH_EXTENDED_STAT == 1
    CPYMEM( * (double *) *pBuffer, pDvmhStatMetric->q1);
    *pBuffer += sizeof(double);

    CPYMEM( * (double *) *pBuffer, pDvmhStatMetric->median);
    *pBuffer += sizeof(double);

    CPYMEM( * (double *) *pBuffer, pDvmhStatMetric->q3);
    *pBuffer += sizeof(double);
#endif

    dvmh_stat_log(DVMH_STAT_LOG_LVL_UP, "dvmh_stat_pack_metric (stop)");
}
