/**
 * Файл содержит типы и функции для сбора статистики DVMH
 * <b> Файл не полностью совпадает с файлом из RTS! Соотвествующие места помечены как (+) </b>
 * @author Aleksei Shubert <alexei@shubert.ru>
 */

#ifndef DVM_SYS_DVMH_STAT_H
#define DVM_SYS_DVMH_STAT_H

#if defined(_WIN64)
#define __LLP64__ 1
#endif

#if defined(__LLP64__)
typedef long long DvmType;
typedef unsigned long long UDvmType;
#else
typedef long DvmType;
typedef unsigned long UDvmType;
#endif

#define DVMH_EXTENDED_STAT 0  /**< разбор расширенной статистики */

// -- Forward declarations ---------------------------------------------------------------------------------------------

struct _dvmh_stat_header_st;
struct _dvmh_stat_header_gpu_info_st;
struct _dvmh_stat_interval_st;
struct _dvmh_stat_interval_gpu_st;
struct _dvmh_stat_interval_gpu_metric_st;
struct _dvmh_stat_interval_thread_st;

// -- Типы связанные с заголовком статистики
typedef struct _dvmh_stat_header_st          dvmh_stat_header;
typedef struct _dvmh_stat_header_gpu_info_st dvmh_stat_header_gpu_info;

// -- Типы связанные с интервальной статистикой
typedef struct _dvmh_stat_interval_st                  dvmh_stat_interval;
typedef struct _dvmh_stat_interval_gpu_st              dvmh_stat_interval_gpu;
typedef struct _dvmh_stat_interval_gpu_metric_st       dvmh_stat_interval_gpu_metric;
typedef struct _dvmh_stat_interval_thread_st           dvmh_stat_interval_thread;

// -- Constants --------------------------------------------------------------------------------------------------------

// Названия метрик
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
    // --
    DVMH_STAT_METRIC_FORCE_INT
} dvmh_stat_metric_names;

#define DVMH_STAT_SIZE_STR         64
#define DVMH_STAT_METRIC_CNT       DVMH_STAT_METRIC_FORCE_INT
#define DVMH_STAT_MAX_GPU_CNT      8
#define DVMH_STAT_GPU_UNKNOWN      "Unknown"

// -- Global variables -------------------------------------------------------------------------------------------------

static const char *dvmhStatMetricsTitles[DVMH_STAT_METRIC_FORCE_INT] = {
        "Kernel executions",
        "Copy GPU to CPU",
        "Copy CPU to GPU",
        "Copy GPU to GPU",
        "[Shadow] Copy GPU to CPU",
        "[Shadow] Copy CPU to GPU",
        "[Shadow] Copy GPU to GPU",
        "[Remote] Copy GPU to CPU",
        "[Remote] Copy CPU to GPU",
        "[Remote] Copy GPU to GPU",
        "[Redistribution] Copy GPU to CPU",
        "[Redistribution] Copy CPU to GPU",
        "[Redistribution] Copy GPU to GPU",
        "[Region IN] Copy GPU to CPU",
        "[Region IN] Copy CPU to GPU",
        "[Region IN] Copy GPU to GPU",
        "GET_ACTUAL",
        "Loop execution",
        "Data reorganization",
        "Reduction",
        "GPU Runtime compilation"
};

static short dvmhDebug = 0;

// -- Data types -------------------------------------------------------------------------------------------------------

/**
 * Описатель GPU в заголовке статистики
 */
struct _dvmh_stat_header_gpu_info_st {
    unsigned long id;                           /**< идентификатор GPU */
    unsigned char name[DVMH_STAT_SIZE_STR + 1]; /**< текстовое описание GPU */
};

/**
 * Заголовок DVMH статистики
 */
struct _dvmh_stat_header_st {
    unsigned long sizeHeader;
    unsigned long sizeIntervalConstPart;
    unsigned long threadsAmount;
    dvmh_stat_header_gpu_info gpu[DVMH_STAT_MAX_GPU_CNT]; /**< ссылка на массив описателей GPU */
};

/**
 * Характеристика/метрика DVMH
 *
 * Поле `values` не выгружется в файл. Это ведет к избыточному использованию памяти, но упрощает поддержку.
 */
struct _dvmh_stat_interval_gpu_metric_st {
    short hasOwnMeasures;  /**< если собственные измерения */
    short isReduced;       /**< значения метрики редучированы */

#if DVMH_EXTENDED_STAT == 1
    dvmh_stat_interval_gpu_metric_value *values; /**< ссылка на массив значений */
#endif

    UDvmType countMeasures;  /**< количество измерений характеристики */
    double  timeProductive; /**< полезное время */
    double  timeLost;       /**< потерянное время */

    // -- Агрегированные значения (для box-диаграммы)
    double min;    /**< минимальное значение */
    double mean;   /**< среднее */
    double max;    /**< максимальное значение */
    double sum;    /**< сумма значений */

#if DVMH_EXTENDED_STAT == 1
    double q1;     /**< Q1 квантиль */
    double median; /**< медиана */
    double q3;     /**< Q3 квантиль */
#endif
};

/**
 * Интервальное хранилище статистики для одного GPU
 *
 * Необходим, для  автоматического позиционирования в памяти, а не ручному просчету.
 * Ведет к некоторому избыточному использованию памяти, ввиду возможного выравнивания компилятором,
 * но упрощает поддерживаемость кода.
 *
 * Структура избыточна, но введена с целью упрощения понимания кода.
 */
struct _dvmh_stat_interval_gpu_st {
    dvmh_stat_interval_gpu_metric metrics[DVMH_STAT_METRIC_FORCE_INT]; /** статистические метрики */
    // --
    double timeProductive; /**< (+) полезное время */
    double timeLost;       /**< (+) потерянное время */
};

/**
 * Структура для нитей
 */
struct _dvmh_stat_interval_thread_st {
    double user_time;
    double system_time;
};

/**
 * Интервальное хранилище статистики по GPU и нитям
 */
struct _dvmh_stat_interval_st {
    unsigned long mask; /**< маска GPU (если GPU задействован, устанавливаем бит в 1 с соотвествующим номером **/
    bool threadsUsed;

    dvmh_stat_interval_gpu     gpu[DVMH_STAT_MAX_GPU_CNT]; /** хранилище метрик для каждого GPU */
    dvmh_stat_interval_thread* threads;
    // --
    double allGPUTimeProductive; /**< (+) полезное время */
    double allGPUTimeLost;       /**< (+) потерянное время */
   
    double allThreadsUserTime;   /**< (+) полезное время нитей */
    double allThreadsSystemTime; /**< (+) потерянное время нитей */
};

#endif //DVM_SYS_DVMH_STAT_H
