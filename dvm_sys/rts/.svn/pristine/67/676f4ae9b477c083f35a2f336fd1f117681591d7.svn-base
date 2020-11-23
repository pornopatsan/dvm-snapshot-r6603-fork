/**
 * Файл содержит интерфейсные функции для сбора DVMH статистики.
 * Функции предназначаены для использование в библиотеке DVMH.
 *
 * @link dvmh_stat_metrics.def объявление доступных метрик
 * @link ../src/dvmh_stat.h    реализация функций
 *
 * @author Aleksei Shubert <alexei@shubert.ru>
 */

#ifndef _DVM_SYS_DVMH_STAT_H_
#define _DVM_SYS_DVMH_STAT_H_

#include "compile.def"

/**
 * Метрики DVMH
 * При изменении менять так же ../src/dvmh_stat.h
 **/
typedef enum {
    DVMH_STAT_METRIC_KERNEL_EXEC,
    /* DVMH-CUDA memcpy */
    DVMH_STAT_METRIC_CPY_DTOH,
    DVMH_STAT_METRIC_CPY_HTOD,
    DVMH_STAT_METRIC_CPY_DTOD,
    /* DVMH memcpy */
    DVMH_STAT_METRIC_CPY_SHADOW_DTOH,
    DVMH_STAT_METRIC_CPY_SHADOW_HTOD,
    DVMH_STAT_METRIC_CPY_SHADOW_DTOD,
    DVMH_STAT_METRIC_CPY_REMOTE_DTOH,
    DVMH_STAT_METRIC_CPY_REMOTE_HTOD,
    DVMH_STAT_METRIC_CPY_REMOTE_DTOD,
    DVMH_STAT_METRIC_CPY_REDIST_DTOH,
    DVMH_STAT_METRIC_CPY_REDIST_HTOD,
    DVMH_STAT_METRIC_CPY_REDIST_DTOD,
    DVMH_STAT_METRIC_CPY_IN_REG_DTOH,
    DVMH_STAT_METRIC_CPY_IN_REG_HTOD,
    DVMH_STAT_METRIC_CPY_IN_REG_DTOD,
    DVMH_STAT_METRIC_CPY_GET_ACTUAL,
    /* DVMH loop events */
    DVMH_STAT_METRIC_LOOP_PORTION_TIME,
    /* DVMH utility functions events */
    DVMH_STAT_METRIC_UTIL_ARRAY_TRANSFORMATION,
    DVMH_STAT_METRIC_UTIL_ARRAY_REDUCTION,
    DVMH_STAT_METRIC_UTIL_RTC_COMPILATION,
    DVMH_STAT_METRIC_UTIL_PAGE_LOCK_HOST_MEM,
    // --
    DVMH_STAT_METRIC_FORCE_INT
} dvmh_stat_metric_names;

/**
 * Максимальная длинна строки имени GPU
 *
 * DVMH_STAT_SIZE_STR должно быть кратно 8-ми, чтобы избежать избыточного использования памяти
 * связанного с выравниванием памяти.
 */
#define DVMH_STAT_SIZE_STR         64

/** Максимально возможное число GPU в статистике */
#define DVMH_STAT_MAX_GPU_CNT      8

/**
 * Записать измерение характеристики
 *
 * @param pDvmhStatMetric указатель на метрику DVMH статистики
 * @param timeProductive полезное время (добавляется)
 * @param lostLost       потерянное время (добавляется)
 */
DVMUSERFUN
void dvmh_stat_add_measurement(const int gpuNo, dvmh_stat_metric_names metric, const double value,
                               const double timeProductive, const double timeLost);

/**
 * Установить информацию о GPU
 *
 * @param gpuNo порядковый номер gpu в статистике
 * @param id    внешний идентификатор gpu задаваемый внутри dvmh
 * @param name  наименование графического ускорителя
 */
DVMUSERFUN
void dvmh_stat_set_gpu_info(const int gpuNo, const int id, const char *name);

#endif // _DVM_SYS_DVMH_STAT_H_
