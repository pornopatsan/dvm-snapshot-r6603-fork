/**
 * Файл содержит основные вункции для инициализации процесса сбора статистики
 * и для работы с интервалами
 */

#ifndef _STATIST_C_
#define _STATIST_C_

#include "statist.h"
#include "interval.h"
#include "system.def"
#include "system.typ"
#include "strall.h"
#include "dvmvers.h"

// dvmh статистика
#include "dvmh_stat.h"

static UDvmType offsetInter; /**< смещение на информацию об интервалах */
static UDvmType offsetSync; /**< смещение на данные о рассинхронизациях */
static UDvmType nInterIterator; /**< итератор порядковых номеров интервалов */

/* -- Управление списоком интервалов */
static pinter_ch pInterInfoCurr;    /**< указатель на текущий интервал в списке*/
static pinter_ch pInterInfoRoot;    /**< указатель на начало списка интервалов */
static pinter_ch pInterInfoSibling; /**< указатель на последний дочерний интервал */

static pvms_ch pBufferHeader; /**< структура общей информации, записываемой в начале каждого буфера. */
static psyn_ch pSyncInfo;     /**< структура времени рассинхронизации */

static UDvmType sizeMemOverheadInter;   /**< память требуемая сверх размера буфера, для хранения треб. статистики */
static UDvmType sizeMemOverheadSync;    /**< память требуемая сверх размера буфера, для сохранения информации о рассинхронизациях */
static UDvmType isStatBufferClosed = 1; /**< закрыт ли буфер сбора статистики? */
static UDvmType nDepthLvlOverhead;      /**< текущая глубина вложенности достигнутая сверх ограничений по детализации */

static UDvmType sizeInterTimes;    /**< размер матрицы времен интервала */
static UDvmType sizeBufferHeaders; /**< размер заголовка буфера с доп. полями зависящих от архитектуры */

static short nInterMaxLevel; /**< Максимальный уровень вложенности интервалов */

/* -- Окружение для автоматически создаваеммых интервалов для коллективных операций */
extern short UserInter;
extern DvmType  NLine_coll; /**< Номер строки исходного кода начала интервала */

#ifdef _DVM_MPI_
extern char   ProcName[];  /**< processor name */
extern int    ProcNameLen; /**< length of processor name */
extern double ProcTime;    /**< processor work time */
#endif

/**
 * -- Interface functions ----------------------------------------------------------------------------------------------
 */

/**
 * Создать статистический интервал
 *
 * Для корректного построения дерева интервалов,
 * необходимо предварительно вызвать {@see FindInter()}
 *
 * @param type            тип интервала
 * @param userDescriptor  пользовательский идентификатор интервала
 */
void CreateInter(int type, DvmType userDescriptor) {
    int i, j; // Итерационные переменные

    UDvmType sizeInterInfo; /**< размер описателя интервала */

    pinter_ch  pInterval; /**< статистический интервал */
    void *pIntervalChild; /**< указатель на вложенный интервал */

    short nLvl;  /**< текущий уровень вложенности интервалов */
    short sType; /**< тип интервала приведенный к short */

    char *pInterTimes; /**< Матрица времен для нового интервала */
    s_SendRecvTimes  *pSendRecvTimes; /**< структура хранящая информацию о коллективных операциях */

    /* Переменные для подстановки */
    double         d0     = 0.0;  /**< число вхождений в интервал (заглушка для копирования) */
    UDvmType  ul0    = 0;    /**< количество процессоров, на которых выполнялся интервал  (заглушка) */
    UDvmType  pNull  = 0;    /**< заклушка для пустых указтелей */

    dvmh_stat_log(DVMH_STAT_LOG_LVL_DOWN, "CreateInter (start) #%d", userDescriptor);

    // Если буфер сбора статистики закрыт ничего не делаем.
    if (isStatBufferClosed == 1) {
        dvmh_stat_log(DVMH_STAT_LOG_LVL_UP, "CreateInter (stop:1)");
        return;
    }

    // Определяем уровень текущей вложенности интервалов
    if (pInterInfoCurr != NULL) {
        CPYMEM(nLvl, pInterInfoCurr->sh.nlev);
        nLvl++;
    } else {
        nLvl = 0;
    }

    dvmh_stat_log(DVMH_STAT_LOG_LVL_THIS, "CreateInter (info) Lvl: %d", nLvl);

    // Если уровень вложенности для детализации статистики превышен - пропускаем
    if (nLvl > MaxIntervalLevel) {
        nDepthLvlOverhead++;
        dvmh_stat_log(DVMH_STAT_LOG_LVL_UP, "CreateInter (stop:2)");
        return;
    }

    // Обновляем максимальный уровень вложенности
    if (nInterMaxLevel < nLvl) nInterMaxLevel = nLvl;

    // Заполняем окружение для автоматически сгенерированных интервалов для коллективных операций
    if (type == USER && IntermediateIntervalsLevel == nLvl + 1) {
        UserInter = 1;
        NLine_coll = DVM_LINE[0];
    }


    // Определяем размер интервала при записи в буфер (смещение на интервал)
    // DVM_FILE - массив имен файлов исходного кода

    sizeInterInfo = DVM_STAT_INTERVAL_SIZE + DVMH_STAT_INTERVAL_STATIC_SIZE +
			 dvmh_stat_get_threads_amount() * sizeof(dvmh_stat_interval_thread) + strlen(DVM_FILE[0]) + 1;
    //sizeInterInfo = sizeof(*pInterval) + strlen(DVM_FILE[0]) + 1;
    //sizeInterInfo = sizeof(*pInterval) - sizeof(dvmh_stat_interval_thread*) + ThreadsSize + strlen(DVM_FILE[0]) + 1;

    // dvmh_stat_log(DVMH_STAT_LOG_LVL_THIS, "CreateInter (info) Interval size          : %d", sizeof(*pInterval));
    // dvmh_stat_log(DVMH_STAT_LOG_LVL_THIS, "CreateInter (info) Interval size(d)       : %d", sizeof(pInterval->d));
    // dvmh_stat_log(DVMH_STAT_LOG_LVL_THIS, "CreateInter (info) Interval size(l)       : %d", sizeof(pInterval->l));
    // dvmh_stat_log(DVMH_STAT_LOG_LVL_THIS, "CreateInter (info) Interval size(sh)      : %d", sizeof(pInterval->sh));
    // dvmh_stat_log(DVMH_STAT_LOG_LVL_THIS, "CreateInter (info) Interval size(v)       : %d", sizeof(pInterval->v));
    // dvmh_stat_log(DVMH_STAT_LOG_LVL_THIS, "CreateInter (info) Interval size(dvmhStat): %d", sizeof(pInterval->dvmhStat));

    // Если невозможно поместить новый интервал в буфер, создаем интервал без записи в буфер
    // StatBufLength - длина буфера для сбора статистики в байтах для одного процессора
    if (((DvmType) (StatBufLength - sizeBufferHeaders) < 0)
        || (offsetInter + sizeInterInfo > StatBufLength - sizeBufferHeaders))
    {
        // Выделяем память под интервал вне буфера
        pInterval = (pinter_ch) dvm_getmemnoerr(sizeInterInfo);
        if (pInterval == NULL) {
            FreeInter();
            epprintf(MultiProcErrReg2, DVM_FILE[0], DVM_LINE[0],
                     "Statistics:not enough memory for interval, data were not wrote to the file\n"); fflush(stdout);
            return;
        }

        // Обновляем количество памяти требуемой дополнительно
        sizeMemOverheadInter = sizeMemOverheadInter + sizeInterInfo;
        sizeInterInfo = 0;
    } else {
        // Определения смещения для записи информации о рассинхронизациях
        while (offsetInter + sizeInterInfo > StatBufLength - offsetSync - sizeBufferHeaders) {
            offsetSync = offsetSync - sizeof(*pSyncInfo);
            sizeMemOverheadSync = sizeMemOverheadSync + sizeof(*pSyncInfo);
        }

        // Выделяем интервал из буфера
        pInterval = (pinter_ch) ((char *) StatBufPtr + sizeBufferHeaders + offsetInter);
    }

    // Обновляем смещение относительно начала буфера
    offsetInter = offsetInter + sizeInterInfo;

    // Конвертируем тип интервала
    sType = (short) type;

    // Выделяем память для матрици времен интервалов
    pInterTimes = (char *) dvm_getmemnoerr(sizeInterTimes);
    if (pInterTimes == NULL) {
        FreeInter();
        epprintf(MultiProcErrReg2, DVM_FILE[0], DVM_LINE[0],
                 "Statistics:not enough memory for interval, data were not wrote to the file\n"); fflush(stdout);
        return;
    }

    // Обновляем глобальный указатель на матрицу времен текущего интервала
    CurrInterPtr = (s_GRPTIMES(*)[StatGrpCount]) pInterTimes;

    // Зануляем матрицу
    for (i = 0; i < StatGrpCount; i++) {
        for (j = 0; j < StatGrpCount; j++) {
            CurrInterPtr[i][j].CallCount   = 0.0;
            CurrInterPtr[i][j].ProductTime = 0.0;
            CurrInterPtr[i][j].LostTime    = 0.0;
        }
    }

    // Инициализируем сбор статистики DVMH
    dvmh_stat_interval_start(pInterval->dvmhStat);

    // Увеличиваем итератор порядковых номеров
    nInterIterator++;

    // Заполняем информацию о интервале
    CPYMEM(pInterval->sh.type    , sType);         // тип
    CPYMEM(pInterval->sh.nlev    , nLvl);          // уровень вложенности
    CPYMEM(pInterval->l.qproc    , ul0);           // число процессов
    CPYMEM(pInterval->d.nenter   , d0);            // кол-во вхождений
    CPYMEM(pInterval->l.nline    , DVM_LINE[0]);   // строка кода
    CPYMEM(pInterval->l.valvar   , userDescriptor);// пользовательский идентификатор интервала
    CPYMEM(pInterval->l.ninter   , nInterIterator);// порядковый номер интервала
    CPYMEM(pInterval->v.ptimes   , pInterTimes);   // матрица времен
    CPYMEM(pInterval->v.up       , pNull);         // ссылка на родительский интервал
    CPYMEM(pInterval->v.down     , pNull);         // ссылка на дочерний интервал
    CPYMEM(pInterval->v.next     , pNull);         // ссылка на следующего соседа по уровню

    // Устанавлиеваем отношение с родительским интервалом
    if (pInterInfoCurr) {
        // dvmh_stat_log(DVMH_STAT_LOG_LVL_THIS, "CreateInter (info) v.up = %p", pInterInfoCurr);
        dvmh_stat_log(DVMH_STAT_LOG_LVL_THIS, "CreateInter (info) Parent: %d", pInterInfoCurr->l.valvar);

        CPYMEM(pInterval->v.up, pInterInfoCurr);
        CPYMEM(pIntervalChild, pInterInfoCurr->v.down);
        if (!pIntervalChild)
            CPYMEM(pInterInfoCurr->v.down, pInterval);
    } else {
        // dvmh_stat_log(DVMH_STAT_LOG_LVL_THIS, "CreateInter (info) v.up(defualt) = %p", pInterval->v.up);
        // dvmh_stat_log(DVMH_STAT_LOG_LVL_THIS, "CreateInter (info) interval      = %p", pInterval);
    }

    // Если интервал не является первым для своего родителя связываем его с последним дочерним интервалом родителя
    if (pInterInfoSibling)
        CPYMEM(pInterInfoSibling->v.next, pInterval);
    pInterInfoSibling = 0;

    // Выделяем часть матрици характеристик, отвечающую за коллективные операции
    pSendRecvTimes = (s_SendRecvTimes *)&(CurrInterPtr[StatGrpCount - 1][StatGrpCount]);

    // Инициализируем информацию о коллективных операциях
    pSendRecvTimes->SendCallTime    = 0.0;
    pSendRecvTimes->MinSendCallTime = 0.0;
    pSendRecvTimes->MaxSendCallTime = 0.0;
    pSendRecvTimes->SendCallCount   = 0;
    pSendRecvTimes->RecvCallTime    = 0.0;
    pSendRecvTimes->MinRecvCallTime = 0.0;
    pSendRecvTimes->MaxRecvCallTime = 0.0;
    pSendRecvTimes->RecvCallCount   = 0;

    // Записываем в буфер после интервала имя пользовательского исходного файла
    if (strlen(DVM_FILE[0]) != 0)
        //strcpy((char *) pInterval + sizeof(*pInterval) , DVM_FILE[0]);
        strcpy((char *) pInterval + sizeInterInfo - (strlen(DVM_FILE[0]) + 1), DVM_FILE[0]);
    else
        //*((char *) (pInterval) + sizeof(*pInterval)) = '\0';
        *((char *) (pInterval) + sizeInterInfo - 1) = '\0';


    // Обновляем глобальный указатель на текущий интервал
    pInterInfoCurr = pInterval;
    if (pInterInfoRoot == NULL) pInterInfoRoot = pInterInfoCurr;

    dvmh_stat_log(DVMH_STAT_LOG_LVL_UP, "CreateInter (stop:2)");

    return;
}

/**
 * Найти и восстановить интервал внутри текущего активного интервала
 * - Устанавливает глобальные переменные на текущий интервал и матрицу характеристик
 * - Поиск производится в потомках текущего активного интервала, так как после закрытия
 *   интервала текущим активным становится родитель.
 *
 * @param type            тип интервала
 * @param userDescriptor  пользовательский идентификатор интервала
 *
 * @return 1 - найден, 0 - не найден
 */
int FindInter(int type, DvmType userDescriptor) {
    pinter_ch     pInterval;             /**< статистический интервал */
    UDvmType nSourceLine;           /**< номер строки в пользовательском коде */
    DvmType          userDescriptorCurrent; /**< пользовательский идентификатор интервала*/
    short         sType;                 /**< тип интервала */
    short         nLvl;                  /**< уровень вложенности */

    dvmh_stat_log(DVMH_STAT_LOG_LVL_DOWN, "FindInter (start)");

    userDescriptorCurrent = -1;

    // Если буфер сбора статистики закрыт - интервал не найден.
    if (isStatBufferClosed == 1) {
        dvmh_stat_log(DVMH_STAT_LOG_LVL_UP, "FindInter (stop:1)");
        return (0);
    }

    // Обнуляем указатель на последний дочерний интервал
    pInterInfoSibling = NULL;

    // Если список интервалов пуст - интервал не найден.
    if (pInterInfoCurr == NULL) {
        dvmh_stat_log(DVMH_STAT_LOG_LVL_UP, "FindInter (stop:2)");
        return (0);
    }

    // Выполняем барьерную синхронизацию
    if (IntervalBarrier == 1) (RTL_CALL, bsynch_());

    // проверяем наличие потомков
    CPYMEM(pInterval, pInterInfoCurr->v.down);
    if (pInterval == NULL) {
        dvmh_stat_log(DVMH_STAT_LOG_LVL_UP, "FindInter (stop:3)");
        return (0);
    }

    // Ищем
    while (pInterval != NULL) {
        CPYMEM(nSourceLine          , pInterval->l.nline);
        CPYMEM(userDescriptorCurrent, pInterval->l.valvar);
        CPYMEM(sType                , pInterval->sh.type);

	UDvmType sizeInter = DVM_STAT_INTERVAL_SIZE + DVMH_STAT_INTERVAL_STATIC_SIZE + 
				 dvmh_stat_get_threads_amount() * sizeof(dvmh_stat_interval_thread);

        if ((nSourceLine == DVM_LINE[0])
            //&& (strcmp((char *) pInterval + sizeof(*pInterval), DVM_FILE[0]) == 0)
            && (strcmp((char *) pInterval + sizeInter, DVM_FILE[0]) == 0)
            //&& (strcmp((char *) pInterval + sizeInterInfo - (strlen(DVM_FILE[0]) + 1), DVM_FILE[0]) == 0)
            && (userDescriptorCurrent == userDescriptor)
            && ((short) type == sType))
        {
            // Подменяем глобальный указатель на текущий интервал и матрицу характеристик
            CPYMEM(CurrInterPtr, pInterval->v.ptimes);
            pInterInfoCurr = pInterval;

            // Восстановление окружения статистики DVMH выбранного интервала
            dvmh_stat_interval_load(pInterval->dvmhStat);

            // Заполняем окружение для автоматически сгенерированных интервалов для коллективных операций
            CPYMEM(nLvl, pInterInfoCurr->sh.nlev);
            if (type == USER && IntermediateIntervalsLevel == nLvl + 1) {
                UserInter = 1;
                NLine_coll = DVM_LINE[0];
            }

            dvmh_stat_log(DVMH_STAT_LOG_LVL_UP, "FindInter (stop:+) Interval: %ld", userDescriptorCurrent);
            return (1);
        }

        pInterInfoSibling = pInterval;
        CPYMEM(pInterval, pInterval->v.next);
    }

    dvmh_stat_log(DVMH_STAT_LOG_LVL_UP, "FindInter (stop:-) Sibling: %ld", userDescriptorCurrent);
    return (0);
}

/**
 * Завершить текущий интервал
 *
 * @param nSrcLine номер строки исходного кода начала интервала (используется для верификации)
 */
void EndInter(DvmType nSrcLine) {
    DvmType   nSourceLine; /**< номер строки в пользовательском коде */
    DvmType   nProcessors; /**< количество процессоров, на которых выполнялся интервал */
    double nHits;       /**< число вхождений в интервал */
    DvmType   userDescriptor;

    UDvmType ul0 = 0; /**< константа для копирования нуля (заглушка) */

    dvmh_stat_log(DVMH_STAT_LOG_LVL_DOWN, "EndInter (start)");

    // Если буфер сбора статистики закрыт ничего не делаем.
    if (isStatBufferClosed == 1) {
        dvmh_stat_log(DVMH_STAT_LOG_LVL_UP, "EndInter (stop:1)");
        return;
    }

    // Если список интервалов пуст - интервал не найден.
    if (pInterInfoCurr == NULL) {
        pprintf(2 + MultiProcErrReg2,
                "Statistics:number of end of interval > number of begin of "
                        "interval, data were not wrote to the file\n"); fflush(stdout);
        isStatBufferClosed = 1;
        return;
    }

    CPYMEM(userDescriptor, pInterInfoCurr->l.valvar);
    dvmh_stat_log(DVMH_STAT_LOG_LVL_THIS, "EndInter (info) #%ld", userDescriptor);

    // Если превышен уровень детализации, изменяем степень превышения
    if (nDepthLvlOverhead > 0) {
        nDepthLvlOverhead--;

        dvmh_stat_log(DVMH_STAT_LOG_LVL_UP, "EndInter (stop:2)");
        return;
    }

    // Проверяем, корректный ли интервал пытаются закрыть
    CPYMEM(nSourceLine, pInterInfoCurr->l.nline);
    if (nSourceLine != nSrcLine) {
        pprintf(2 + MultiProcErrReg2,
                "Statistics:end of interval nline=%ld, name=%s, no end "
                        "nline=%ld name=%s, data were not wrote to the file\n",
                nSrcLine, DVM_FILE[0], nSourceLine, (char *) pInterInfoCurr + sizeof(*pInterInfoCurr));

        isStatBufferClosed = 1;
        return;
    }

    // Нормализуем число вхождений в интервал
    CPYMEM(nHits, pInterInfoCurr->d.nenter);
    if (nHits == 0.0) { /* first EndInter */
        CPYMEM(pInterInfoCurr->l.qproc, CurrEnvProcCount);
        nHits = 1. / CurrEnvProcCount;
        CPYMEM(pInterInfoCurr->d.nenter, nHits);
    } else {
        nHits = nHits + 1. / CurrEnvProcCount;
        CPYMEM(pInterInfoCurr->d.nenter, nHits);
        CPYMEM(nProcessors, pInterInfoCurr->l.qproc);
        if (nProcessors != CurrEnvProcCount)
            CPYMEM(pInterInfoCurr->l.qproc, ul0);
    }

    // Заполняем информацию строке исходного кода где заканчивается интервал
    CPYMEM(pInterInfoCurr->l.nline_end, DVM_LINE[0]);

    dvmh_stat_log(DVMH_STAT_LOG_LVL_THIS, "EndInter (info) pInterInfoCurr: %p", pInterInfoCurr);

    // Возвращем глобальный указатель на текущий интервал к родителю
    CPYMEM(pInterInfoCurr, pInterInfoCurr->v.up);

    dvmh_stat_log(DVMH_STAT_LOG_LVL_THIS, "EndInter (info) pParent: %p", pInterInfoCurr);

    // Если родитель существовал, восстанавливаем глобальную переменную матрици характеристик
    // и окружение DVMH статистики
    if (pInterInfoCurr != NULL) {
        CPYMEM(CurrInterPtr, pInterInfoCurr->v.ptimes);
        dvmh_stat_interval_load(pInterInfoCurr->dvmhStat);
    } else dvmh_stat_interval_load(0);

    dvmh_stat_log(DVMH_STAT_LOG_LVL_UP, "EndInter (stop)");
    return;
}

/**
 *  Рекурсивно удалить статистическую информацию о интервалах из памяти
 *  и закрыть DVMH статистику
 */
static void FreeIntervalRecursive (pinter_ch pInterval, pinter_ch pParent) {
    s_GRPTIMES(*pInterTimes)[StatGrpCount]; /**< ссылка на матрицу времен интервала */
    pinter_ch pChild; /**< потомок текущего статистического интервала */
    pinter_ch pNext;  /**< сосед справа текущего статистического интервала */
    DvmType  userDescriptor;
    DvmType  userDescriptorParent;

    dvmh_stat_log(DVMH_STAT_LOG_LVL_DOWN, "FreeIntervalRecursive (start)");

    if (pInterval == NULL) {
        dvmh_stat_log(DVMH_STAT_LOG_LVL_UP, "FreeIntervalRecursive (stop:1)");
        return;
    }

    CPYMEM(userDescriptor, pInterval->l.valvar);
    userDescriptorParent = -1;
    if (pParent != NULL) CPYMEM(userDescriptorParent, pParent->l.valvar);

    dvmh_stat_log(DVMH_STAT_LOG_LVL_THIS, "FreeIntervalRecursive (info) Interval #%lu (Parent: %lu)",
                  userDescriptor, userDescriptorParent
    );

    // Закрываем следующий интервал того же уровня
    CPYMEM(pNext, pInterval->v.next);
    if (pNext) FreeIntervalRecursive(pNext, pParent);

    // Закрываем дочерний интервал
    CPYMEM(pChild, pInterval->v.down);
    if (pChild) FreeIntervalRecursive(pChild, pInterval);

    // Удаляем матрицу времен
    CPYMEM(pInterTimes, pInterval->v.ptimes)pInterTimes;
    dvm_freemem((void **) (&pInterTimes));

    // Закрываем DVMH статистику
    dvmh_stat_interval_stop(
            pInterval->dvmhStat,
            pParent != NULL ? pParent->dvmhStat : NULL,
            &pInterval->d + 1);

    // Если интервал объявлен вне буфера вывода, то удаляем его вручную
    if ((char *) pInterval < StatBufPtr + sizeBufferHeaders
        || (char *) pInterval > StatBufPtr + sizeBufferHeaders + offsetInter)
    {
        dvm_freemem((void **) (&pInterval));
    }


    dvmh_stat_log(DVMH_STAT_LOG_LVL_UP, "FreeIntervalRecursive (stop)");
}

/**
 * Удалить статистическую информацию о интервалах из памяти
 * и закрыть DVMH статистику
 */
void FreeInter(void) {
    dvmh_stat_log(DVMH_STAT_LOG_LVL_DOWN, "FreeInter (start)");

    FreeIntervalRecursive(pInterInfoRoot, NULL);

    dvmh_stat_log(DVMH_STAT_LOG_LVL_UP, "FreeInter (stop)");
}


/**
 * -- Internal functions -----------------------------------------------------------------------------------------------
 */

/**
 * Инициализация сбора статистики
 */
void stat_init(void) {
    UDvmType i;
    double d0 = 0.0;

    char *pBufferHeaderEnd; /**< Указатель на конец заголовка буфера */
    short sMSRank = 0;      /**< rank of processor system */
    short size;             /**< переменная для хранения размера типа */
    short lenVersionString; /**< длина строки версии dvm, платформы и имени процессора */

    char *pch;

    /* for CPYMEMC */
    int smfrom, toright=0, stcond;
    // --

    dvmh_stat_log(DVMH_STAT_LOG_LVL_DOWN, "stat_init (start)");

    pInterInfoRoot       = NULL;
    pInterInfoCurr       = NULL;
    nInterIterator       = 0;
    pBufferHeader        = (pvms_ch) StatBufPtr;
    sizeInterTimes       = MISize;
    nInterMaxLevel       = 0;
    nDepthLvlOverhead    = 0;
    offsetInter          = 0;
    offsetSync           = 0;
    sizeMemOverheadInter = 0;
    sizeMemOverheadSync  = 0;
    isStatBufferClosed   = 0;
    pBufferHeaderEnd     = NULL;

    // Определяем размер дополнительной информации
#ifdef _DVM_MPI_
	sizeBufferHeaders = sizeof(*pBufferHeader) + VMSRank * sizeof(VMSSize[0]) + strlen(VERS) +
			  strlen(PLATFORM) + 3 + ProcNameLen;
#else
    sizeBufferHeaders = sizeof(*pBufferHeader) + VMSRank * sizeof(VMSSize[0]) + strlen(VERS) +
              strlen(PLATFORM) + 2;
#endif

    // Инициализация буфера вывода нулями
    for (i = 0; i < StatBufLength; i++) StatBufPtr[i] = '\0';

    // Дополняем заголовком DVMH статистики
    dvmh_stat_init_header(StatBufPtr + sizeBufferHeaders);
    sizeBufferHeaders += DVMH_STAT_HEADER_SIZE;

    dvmh_stat_log(DVMH_STAT_LOG_LVL_THIS, "stat_init (info) sizeBufferHeaders: %lu", sizeBufferHeaders);

    // Инициализация cтруктуры времени рассинхронизации
    pSyncInfo = (psyn_ch) (StatBufPtr + StatBufLength - sizeof(*pSyncInfo));

    // Если размер заголовка больше самого буфера не пишем заголовок
    if (sizeBufferHeaders > StatBufLength) pBufferHeader = NULL;

    // Заполняем заголовок
    if (pBufferHeader != NULL) {
        size = 1;
        pch=(char *)&(size);
        if (pch[0]==0) toright=1; /* sizeof(short)!=2 */
        CPYMEMC(pBufferHeader->shc.reverse, size); // признак, что информация собрана не на рабочей станции

        CPYMEM(pBufferHeader->l.proccount, ProcCount);  // количество процессоров
        CPYMEM(pBufferHeader->l.mpstype  , MPS_TYPE);   // тип передачи сообщений
        CPYMEM(pBufferHeader->l.ioproc   , MPS_IOProc); // номер процессора ввода–вывода
        CPYMEM(pBufferHeader->v.pbuffer  , StatBufPtr); // начало буфера

        // Записываем размер типов
        size = SZSH;
        CPYMEMC(pBufferHeader->shc.szsh, size);
        size = SZL;
        CPYMEMC(pBufferHeader->shc.szl, size);
        size = SZV;
        CPYMEMC(pBufferHeader->shc.szv, size);
        size = SZD;
        CPYMEMC(pBufferHeader->shc.szd, size);

        // Store rank of processor system
        sMSRank = (short) VMSRank;
        CPYMEM(pBufferHeader->sh.rank, sMSRank);

#ifdef _DVM_MPI_
		lenVersionString=(short)(strlen(VERS)+strlen(PLATFORM)+3+ProcNameLen);
		CPYMEM(pBufferHeader->d.proctime, ProcTime);
#else
        lenVersionString = (short) (strlen(VERS) + strlen(PLATFORM) + 2);
        CPYMEM(pBufferHeader->d.proctime, d0);
#endif
        // Длинна версии
        CPYMEM(pBufferHeader->sh.lvers, lenVersionString);
    } else {
        dvmh_stat_log(DVMH_STAT_LOG_LVL_UP, "stat_init (stop:1)");
        return;
    }

    // Инициализируем указатель на конец заголовка
    pBufferHeaderEnd = StatBufPtr + sizeof(*pBufferHeader);

    // Записываем матрицу процессоров после заголовка
    for (i = 0; i < VMSRank; i++) {
        if (pBufferHeaderEnd != NULL) {
            memcpy(pBufferHeaderEnd, &VMSSize[i], sizeof(VMSSize[0]));
            pBufferHeaderEnd = pBufferHeaderEnd + sizeof(VMSSize[0]);
        }
    }
    // Копируем информацию о версии и платформе
    strcpy(pBufferHeaderEnd, VERS);
    pBufferHeaderEnd = pBufferHeaderEnd + strlen(VERS) + 1;
    strcpy(pBufferHeaderEnd, PLATFORM);
    pBufferHeaderEnd = pBufferHeaderEnd + strlen(PLATFORM) + 1;
#ifdef _DVM_MPI_
    if (ProcNameLen>0) strcpy(pBufferHeaderEnd,ProcName);
#endif

    dvmh_stat_log(DVMH_STAT_LOG_LVL_UP, "stat_init (stop)");

    return;
}

/**
 * Завершение сбора статистики
 */
void stat_done(void) {
    int i, j;

    UDvmType sizeBufEmptySpace; /**< свободное место в буфере вывода */
    UDvmType offsetInterTimes;  /**< смещение относительно линеаризованной матрици времен */

    DvmType nSourceLine; /**< номер строки в пользовательском коде */

    DvmType  isFirstInterval          = 1; /**< является ли первым интервалом в списке */
    short isInsufficientBufferSize = 0; /**< есть ли недостаток буфера */
    short nLvl;                         /**< текущий уровень вложенности интервалов */
    short sType;                        /**< тип интервала приведенный к short */

    unsigned char *pBufBegin; /**< указатель на начало буфера вывода */
    unsigned char *pBufEnd;   /**< указатель на конец буфера вывода */
    unsigned char *pBufPos;   /**< указатель на позицию внутри буфера вывода */

    s_GRPTIMES(*pInterTimes)[StatGrpCount]; /**< ссылка на матрицу времен интервала */
    pinter_ch        pInterval;             /**< статистический интервал */
    s_SendRecvTimes *pSendRecvTimes;        /**< структура хранящая информацию о коллективных операциях */

    dvmh_stat_log(DVMH_STAT_LOG_LVL_DOWN, "stat_done (start)");


    // Если буфер сбора статистики закрыт ничего не делаем.
    if (isStatBufferClosed == 1) {
        FreeInter(); /* was error */
        dvmh_stat_log(DVMH_STAT_LOG_LVL_UP, "stat_done (stop:1)");
    }

    // Завершаем все открытые интервалы
    while (pInterInfoCurr != NULL) {
        CPYMEM(nSourceLine, pInterInfoCurr->l.nline);
        CPYMEM(sType      , pInterInfoCurr->sh.type);
        CPYMEM(nLvl       , pInterInfoCurr->sh.nlev);

        // Проверяем целостность
        if (nLvl != 0 && isFirstInterval == 1) {
            pprintf(2 + MultiProcErrReg2,
                    "Statistics warning:In the interval used return or goto, "
                            "times may be incorrect, name=%s nline=%ld\n",
                    (char *) pInterInfoCurr + sizeof(*pInterInfoCurr), nSourceLine);
        } isFirstInterval = 0;

        // Завершаем интервал
        isFirstInterval = 2;
        DVM_FILE[0] = (char *) pInterInfoCurr + sizeof(*pInterInfoCurr);
        if (sType == USER)
            (RTL_CALL, einter_(&isFirstInterval, &nSourceLine));
        else
            (RTL_CALL, enloop_(&isFirstInterval, &nSourceLine));
    }

    //Удаляем массив таймеров для нитей, если он был.
    dvmh_stat_free_timers_array();

    // Вычисление неиспользованного места в буфере
    if ((DvmType) (StatBufLength - sizeBufferHeaders) < 0)
        sizeBufEmptySpace = sizeBufferHeaders - StatBufLength;
    else
        sizeBufEmptySpace = StatBufLength - offsetInter - offsetSync - sizeBufferHeaders;

    // Сообщаем о переполнении буфера свзянном с информацией об интервалах
    if (isStatBufferClosed == 0
        && ((sizeMemOverheadInter > 0) || ((DvmType) (StatBufLength - sizeBufferHeaders) < 0)))
    {
        pprintf(2 + MultiProcErrReg2,
                "Statistics:StatBufLength=%ld, increase buffer's size by %ld "
                        "bytes, data were not wrote to the file\n",
                StatBufLength, sizeMemOverheadInter + sizeMemOverheadSync - sizeBufEmptySpace);
        FreeInter();
    }

    // Сообщаем о переполнении буфера свзянном с информацией о рассинхронизациях и коллективных операциях
    if (sizeMemOverheadSync > 0 && isStatBufferClosed == 0) {
        isInsufficientBufferSize = 1;

        pprintf(2 + MultiProcErrReg2,
                "Statistics:StatBufLength=%ld, not enough memory for times of "
                        "collective operations, increase buffer's size by %ld bytes,\n",

                StatBufLength, sizeMemOverheadSync - sizeBufEmptySpace);
        pprintf(2 + MultiProcErrReg2,
                " only part of times of collective operations and intervals were wrote to the file\n"); fflush(stdout);
    }

    // При наличии заголовка - заполняем его
    if (pBufferHeader != NULL) {
        CPYMEM(pBufferHeader->l.linter    , offsetInter);
        CPYMEM(pBufferHeader->l.lsynchro  , offsetSync);
        CPYMEM(pBufferHeader->l.lbuf      , StatBufLength);
        CPYMEM(pBufferHeader->l.qfrag     , nInterIterator);
        CPYMEM(pBufferHeader->sh.maxnlev  , nInterMaxLevel);
        CPYMEM(pBufferHeader->sh.smallbuff, isInsufficientBufferSize);

        // Пакуем DVMH заголовк
        dvmh_stat_pack_header();
    }

    // Еще раз проверяем, не возникли ли ошибки при закрытии интервалов. Если да - прекращаем работу.
    if (isStatBufferClosed == 1) {
        dvmh_stat_log(DVMH_STAT_LOG_LVL_UP, "stat_done (stop:2)");
        return;
    }


    // Копируем в буфер данные матриц времен и поля коллективных операций
    pBufBegin = (unsigned char *) (StatBufPtr + sizeBufferHeaders);
    pBufPos   = (unsigned char *) pInterInfoRoot;
    pBufEnd   = (unsigned char *) (StatBufPtr + sizeBufferHeaders + offsetInter);
    while (pBufPos >= pBufBegin && pBufPos < pBufEnd) {
        pInterval = (pinter_ch) pBufPos;
        CPYMEM(pInterTimes, pInterval->v.ptimes)pInterTimes;
        offsetInterTimes = 0;
        for (i = 0; i < StatGrpCount; i++) {
            for (j = 0; j < StatGrpCount; j++) {
                CPYMEM(pInterval->d.times[offsetInterTimes]    , pInterTimes[i][j].CallCount  );
                CPYMEM(pInterval->d.times[offsetInterTimes + 1], pInterTimes[i][j].ProductTime);
                CPYMEM(pInterval->d.times[offsetInterTimes + 2], pInterTimes[i][j].LostTime   );
                offsetInterTimes = offsetInterTimes + 3;
            }
        }
        //pBufPos = pBufPos + strlen((char *) pBufPos + sizeof(*pInterval)) + 1 + sizeof(*pInterval);
	
	UDvmType sizeInter = DVM_STAT_INTERVAL_SIZE + DVMH_STAT_INTERVAL_STATIC_SIZE + 
				 dvmh_stat_get_threads_amount() * sizeof(dvmh_stat_interval_thread);

        pBufPos = pBufPos + sizeInter + strlen((char*)pBufPos + sizeInter) + 1;

        pSendRecvTimes = (s_SendRecvTimes *) & pInterTimes[StatGrpCount - 1][StatGrpCount];

        // Копируем информацию о коллективных операциях
        CPYMEM(pInterval->d.SendCallTime   , pSendRecvTimes->SendCallTime);
        CPYMEM(pInterval->d.MinSendCallTime, pSendRecvTimes->MinSendCallTime);
        CPYMEM(pInterval->d.MaxSendCallTime, pSendRecvTimes->MaxSendCallTime);
        CPYMEM(pInterval->l.SendCallCount  , pSendRecvTimes->SendCallCount);
        CPYMEM(pInterval->d.RecvCallTime   , pSendRecvTimes->RecvCallTime);
        CPYMEM(pInterval->d.MinRecvCallTime, pSendRecvTimes->MinRecvCallTime);
        CPYMEM(pInterval->d.MaxRecvCallTime, pSendRecvTimes->MaxRecvCallTime);
        CPYMEM(pInterval->l.RecvCallCount  , pSendRecvTimes->RecvCallCount);
    }

    FreeInter();

    dvmh_stat_log(DVMH_STAT_LOG_LVL_UP, "stat_done (stop)");
    return;
}

/****************************\
* Synchronization operations *
\****************************/

void stat_event(int numbevent) {
    short nitem;
    DvmType nint;
    if (isStatBufferClosed == 1) return;
    nitem = IsStat[numbevent];
    if (nitem <= 0 || nitem > QCOLLECT + QCOLLECT) {
        pprintf(2 + MultiProcErrReg2,
                "Statistics:incorrect ngrp=%d numbevent=%d, programmer's error\n",
                nitem, numbevent);
        return;
    }

/* Operation from operation or operation from fortran input/output */

    if ((DVMCallLevel != 0 && nitem != SSHAD && nitem != SSHAD + QCOLLECT && nitem != WSHAD &&
         nitem != WSHAD + QCOLLECT) || CurrOperFix != 0)
        return;
    CPYMEM(nint, pInterInfoCurr->l.ninter);

/*
pprintf (2+MultiProcErrReg2,
        "Statevent numbevent=%d nint=%d nitem=%i grp=%d nline=%ld\n",
        numbevent,nint, nitem,StatObjectRef,DVM_LINE[0]);
*/

    if (nitem > NINOUT && nitem < QCOLLECT && UserInter > 0) {

/* create collective interval */
        eiinter_();
        biinter_(nitem);
/*EndInter(NLine_coll);
if (FindInter(nitem,Fic_index)==0) CreateInter(nitem,Fic_index);
NLine_coll=DVM_LINE[0];*/
    }

/* Don't write into the file call_StartOperation */

    if ((nitem & 3) == 2 && nitem <= QCOLLECT) {
        return;
    }

/* Synchronization operations */

    if (((DvmType) (StatBufLength - offsetInter - sizeBufferHeaders) < 0) ||
        (offsetSync + sizeof(*pSyncInfo) > StatBufLength - offsetInter - sizeBufferHeaders)) {
        sizeMemOverheadSync = sizeMemOverheadSync + sizeof(*pSyncInfo);
    }

    if (sizeMemOverheadSync == 0) {
        CPYMEM(nint, pInterInfoCurr->l.ninter);
        CPYMEM(pSyncInfo->l.ninter, nint);
        CPYMEM(pSyncInfo->sh.nitem, nitem);
        CPYMEM(pSyncInfo->d.time, Curr_dvm_time);
        CPYMEM(pSyncInfo->v.pgrp, StatObjectRef);
        offsetSync = offsetSync + sizeof(*pSyncInfo);
        pSyncInfo = (psyn_ch) (StatBufPtr + StatBufLength - sizeof(*pSyncInfo) - offsetSync);
    }

    return;
}

#endif   /* _STATIST_C_ */
