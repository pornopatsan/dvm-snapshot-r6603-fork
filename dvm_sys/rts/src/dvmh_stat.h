/**
 * Файл содержит типы и функции для сбора статистики DVMH
 *
 * Переменные окружения:
 * DVMH_STAT_DEBUG         - включает отладку (логирование)
 * DVMH_STAT_VALUES_GATHER - от 0 до 100. Определяем вероятность с которой будет записанно очередное измерение
 *                           характеристики. Когда происходит большое количество замеров характеристик, с целью
 *                           экономии памяти, характеристика рассматривается как случайная величина с нормальным
 *                           распределением, что допустимо с точки зрения мат. статистики. По умолчанию 100.
 *
 * @author Aleksei Shubert <alexei@shubert.ru>
 */

#ifndef DVM_SYS_DVMH_STAT_H
#define DVM_SYS_DVMH_STAT_H

// Interface
#include "../include/dvmh_stat.h"

/** Включить сбор расширенной статистики */
#define DVMH_EXTENDED_STAT 0

// -- Forward declarations ---------------------------------------------------------------------------------------------

struct _dvmh_stat_header_st;
struct _dvmh_stat_header_gpu_info_st;
struct _dvmh_stat_interval_st;
struct _dvmh_stat_interval_gpu_st;
struct _dvmh_stat_interval_gpu_metric_st;
struct _dvmh_stat_interval_gpu_metric_value_st;
struct _dvmh_stat_interval_thread_st;

// -- Типы связанные с заголовком статистики
typedef struct _dvmh_stat_header_st          dvmh_stat_header;
typedef struct _dvmh_stat_header_gpu_info_st dvmh_stat_header_gpu_info;

// -- Типы связанные с интервальной статистикой
typedef struct _dvmh_stat_interval_st                  dvmh_stat_interval;
typedef struct _dvmh_stat_interval_gpu_st              dvmh_stat_interval_gpu;
typedef struct _dvmh_stat_interval_gpu_metric_st       dvmh_stat_interval_gpu_metric;
typedef struct _dvmh_stat_interval_gpu_metric_value_st dvmh_stat_interval_gpu_metric_value;
typedef struct _dvmh_stat_interval_thread_st           dvmh_stat_interval_thread;

// -- Constants --------------------------------------------------------------------------------------------------------

/**
 * Изменение уровня вложенности лога при отладки
 */
typedef enum { DVMH_STAT_LOG_LVL_DOWN, DVMH_STAT_LOG_LVL_UP,  DVMH_STAT_LOG_LVL_THIS} dvmh_stat_log_lvl;

/**
 * Колличество метрик
 */
#define DVMH_STAT_METRIC_CNT   DVMH_STAT_METRIC_FORCE_INT

#ifndef DVMH_STAT_VALUES_GATHER
/**
 * Вероятность с которой значение метрики будет сохранено
 * Необходимо для сбора сверх больших статистик
 */
#define DVMH_STAT_VALUES_GATHER    100
#endif

/**
 * Размер заголовка статистики DVMH.
 */
#define DVMH_STAT_HEADER_SIZE sizeof(dvmh_stat_header)

/**
 * Размер поинтервальной части статистики DVMH
 */
#define DVMH_STAT_INTERVAL_SIZE sizeof(dvmh_stat_interval)
#define DVMH_STAT_INTERVAL_STATIC_SIZE (sizeof(dvmh_stat_interval) - sizeof(dvmh_stat_interval_thread*))

// Режим отладки выводит отладочные сообщения
// #define DVMH_STAT_DEBUG

// -- Data types -------------------------------------------------------------------------------------------------------

/**
 * Описатель GPU в заголовке статистики
 */
struct _dvmh_stat_header_gpu_info_st {
    UDvmType id;                           /**< идентификатор GPU */
    unsigned char name[DVMH_STAT_SIZE_STR + 1]; /**< текстовое описание GPU */
};

/**
 * Заголовок DVMH статистики
 */
struct _dvmh_stat_header_st {
    UDvmType sizeHeader;
    UDvmType sizeIntervalConstPart;
    UDvmType threadsAmount;
    dvmh_stat_header_gpu_info gpu[DVMH_STAT_MAX_GPU_CNT]; /**< ссылка на массив описателей GPU */
};

/**
 * Значения характеристик DVMH (список)
 */
struct _dvmh_stat_interval_gpu_metric_value_st {
    double value; /**< значение */
    dvmh_stat_interval_gpu_metric_value *next; /**< следующий элемент */
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
    dvmh_stat_interval_gpu_metric metrics[DVMH_STAT_METRIC_CNT]; /** статистические метрики */
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
    DvmType    mask; /**< маска GPU (если GPU задействован, устанавливаем бит в 1 с соотвествующим номером **/
    dvmh_stat_interval_gpu gpu[DVMH_STAT_MAX_GPU_CNT]; /** хранилище метрик для каждого GPU */
    dvmh_stat_interval_thread* threads;
};

// -- Functions --------------------------------------------------------------------------------------------------------
/**
 * возвращает количество нитей, для которых будет собрана статистика
 */
UDvmType dvmh_stat_get_threads_amount();

/**
 * Получить указатель на массив с таймерами для нитей.
 */
double* dvmh_stat_get_timers_array();

/**
 * Удалить массив с таймерами нитей
 */
void dvmh_stat_free_timers_array();

/**
 * Инициализация заголовка DVMH статистики в буфере вывода
 */
void dvmh_stat_init_header(void * const pBuffer);

/**
 * Начать сбор статистики DVMH для интервала.
 *
 * @param pBuffer указатель на буфер DVMH статистики
 */
void dvmh_stat_interval_start(void * const pBuffer);

/**
 * Завершить сбор статистики DVMH для интервала.
 *
 * @param pBufferCurrent указатель на буфер DVMH статистики текущего интервала
 * @param pBufferParent  указатель на буфер DVMH статистики родительского интервала
 * @param pOutputBuffer  указатель на буфер в который будет записан текущий интервал
 */
void dvmh_stat_interval_stop(void * const pBufferCurrent, void * const pBufferParent, void * const pOutputBuffer);

/**
 * Загрузить DVMH статистику из буфера в окружение
 *
 * @param pBuffer указатель на буфер DVMH статистики
 */
void dvmh_stat_interval_load(void * const pBuffer);

/**
 * Упаковать заголовк DVMH статистики в буфере
 *
 * Убирает выравнивания памяти созданные компилятором. Необходимо для корректного чтения статистики
 * утилитой PPPA без привязки к архитектуре и компилятору.
 */
void dvmh_stat_pack_header();

/**
 * Вывести строку отладочного лога
 *
 * @param shift  изменение уровня лога.
 * @param format формат вывода
 * @param ...
 */
void dvmh_stat_log(dvmh_stat_log_lvl shift, char  *format, ...);

#endif //DVM_SYS_DVMH_STAT_H
