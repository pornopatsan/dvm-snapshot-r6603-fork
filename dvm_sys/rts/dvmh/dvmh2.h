// TODO: Leave only this include, move comments in the Doxygen format to include/dvmhlib*.h files
#include "include/dvmhlib2.h"

#pragma GCC visibility push(default)
extern "C" {

// Оповещение о местонахождении - имя исходного файла и номер строки в исходном файле. Первый параметр - номер строки. Второй параметр - имя файла.
void dvmh_line_C(DvmType lineNumber, const char fileName[]);
void dvmh_line_(const DvmType *pLineNumber, const DvmType *pFileNameStr);


// Инициализация. Первый параметр - набор флагов инициализации: 1 - фортран, 2 - нет регионов (-noH), 4 - последовательная программа (-s), 8 - будет использоваться OpenMP. Второй параметр - адрес переменной для записи кол-ва аргументов командной строки. Третий параметр - адрес указателя для записи аргументов командной строки.
void dvmh_init_C(DvmType flags, int *pArgc, char ***pArgv);
void dvmh_init2_(const DvmType *pFlags);

// Инициализация для библиотек. Главные отличия: нет параметров командной строки и допускается множественный вызов (срабатывает первый).
void dvmh_init_lib_C(DvmType flags);
void dvmh_init_lib_(const DvmType *pFlags);

// Окончание работы. Первый параметр - код завершения программы.
void dvmh_exit_C(DvmType exitCode);
void dvmh_exit_(const DvmType *pExitCode);


// Создание, распределние, уничтожение распределенных массивов и шаблонов
// Оповещение об объявлении динамически выделяемого массива, заведение информации о нем. Первый параметр - заголовочный массив. Второй параметр - кол-во измерений массива. Третий параметр - тип элемента массива. Далее идут тройки по количеству измерений: размер по измерению, нижняя теневая грань, верхняя теневая грань. Первый размер игнорируется (т.к. он будет выяснен при dvmh_array_alloc_)
void dvmh_array_declare_C(DvmType dvmDesc[], DvmType rank, DvmType typeSize, /* DvmType axisSize, DvmType shadowLow, DvmType shadowHigh */...);

// Создание распределенного массива (до распределения или выравнивания обращаться к нему нельзя). Первый параметр - заголовочный массив. Второй параметр - кол-во измерений. Третий параметр - тип элемента массива. Далее идут тройки по количеству измерений: размер по измерению, нижняя теневая грань, верхняя теневая грань.
void dvmh_array_create_C(DvmType dvmDesc[], DvmType rank, DvmType typeSize, /* DvmType axisSize, DvmType shadowLow, DvmType shadowHigh */...);

// Создание распределенного массива (до распределения или выравнивания обращаться к нему нельзя). Первый параметр - заголовочный массив. Второй параметр - база для индексации. Третий параметр - кол-во измерений. Четвертый параметр - тип элемента массива. Далее идут четверки по количеству измерений: нижний индекс по измерению, верхний индекс по измерению, нижняя теневая грань, верхняя теневая грань.
void dvmh_array_create_(DvmType dvmDesc[], const void *baseAddr, const DvmType *pRank, const DvmType *pTypeSize,
        /* const DvmType *pSpaceLow, const DvmType *pSpaceHigh, const DvmType *pShadowLow, const DvmType *pShadowHigh */...);

// Создание шаблона (до распределения отображать на него нельзя). Первый параметр - заголовочный массив. Второй параметр - кол-во измерений. Третий и последующие по количеству измерений - их размеры.
void dvmh_template_create_C(DvmType dvmDesc[], DvmType rank, /* DvmType axisSize */...);

// Создание шаблона (до распределения отображать на него нельзя). Первый параметр - заголовочный массив. Второй параметр - кол-во измерений. Далее идут пары по количеству измерений: нижний индекс по измерению, верхний индекс по измерению.
void dvmh_template_create_(DvmType dvmDesc[], const DvmType *pRank, /* const DvmType *pSpaceLow, const DvmType *pSpaceHigh */...);

// Оповещение о выделении памяти для динамически выделяемого массива, при этом он окончательно создается  (до распределения или выравнивания обращаться к нему нельзя). Первый параметр - заголовочный массив. Второй параметр - кол-во запрошенных байт.
void dvmh_array_alloc_C(DvmType dvmDesc[], DvmType byteCount);

// Вспомогательная функция для передачи обработчиков. Первый параметр - функция. Второй параметр - количество пользовательских параметров. Третий и последующие - эти самые параметры, которые ВСЕ должны быть совместимы по размеру с типом указателя.
DvmType dvmh_handler_func_C(DvmHandlerFunc handlerFunc, DvmType customParamCount, /* void *param */...);
DvmType dvmh_handler_func_(DvmHandlerFunc handlerFunc, const DvmType *pCustomParamCount, /* void *param */...);

// Вспомогательные функции для задания dervied распределения или добавления поэлементной теневой грани
// Неиспользуемое измерение
DvmType dvmh_derived_rhs_expr_ignore_();
// Выбор одного, конкретного индекса по измерению. Первый параметр - значение константного индекса.
DvmType dvmh_derived_rhs_expr_constant_C(DvmType indexValue);
DvmType dvmh_derived_rhs_expr_constant_(const DvmType *pIndexValue);
// Проход по измерению, возможно, с теневыми гранями. Первый параметр - количество добавляемых теневых граней. Второй и последующие - эти самые теневые грани в виде NULL-terminated const char [].
DvmType dvmh_derived_rhs_expr_scan_C(DvmType shadowCount, /* const char shadowName[] */...);
// То же самое, но строки передаются через dvmh_string().
DvmType dvmh_derived_rhs_expr_scan_(const DvmType *pShadowCount, /* const DvmType *pShadowNameStr */...);
// Агрегация всей правой части правила. Первый параметр - образец (шаблон или распределенный массив). Второй параметр - количество измерений образца. Третий и последующие по количеству измерений образца - описатели выражений правой части, вернутые функциями dvmh_derived_rhs_expr_*.
DvmType dvmh_derived_rhs_C(const DvmType templDesc[], DvmType templRank, /* DvmType derivedRhsExprHelper */...);
DvmType dvmh_derived_rhs_(const DvmType templDesc[], const DvmType *pTemplRank, /* const DvmType *pDerivedRhsExprHelper */...);

// Вспомогательные функции для семейства distribute/redistribute
DvmType dvmh_distribution_replicated_();
// Первый параметр - номер оси многопроцессорной системы.
DvmType dvmh_distribution_block_C(DvmType mpsAxis);
DvmType dvmh_distribution_block_(const DvmType *pMpsAxis);
// Первый параметр - номер оси многопроцессорной системы. Второй параметр - тип элементов массива весов (rt_XXX). Третий параметр - ссылка на массив весов блоков. Четвертый параметр - количество элементов в массиве весов блоков.
DvmType dvmh_distribution_wgtblock_C(DvmType mpsAxis, DvmType elemType, const void *arrayAddr, DvmType elemCount);
DvmType dvmh_distribution_wgtblock_(const DvmType *pMpsAxis, const DvmType *pElemType, const void *arrayAddr, const DvmType *pElemCount);
// Первый параметр - номер оси многопроцессорной системы. Второй параметр - тип элементов массива genblock (rt_XXX). Третий параметр - ссылка на массив genblock.
DvmType dvmh_distribution_genblock_C(DvmType mpsAxis, DvmType elemType, const void *arrayAddr);
DvmType dvmh_distribution_genblock_(const DvmType *pMpsAxis, const DvmType *pElemType, const void *arrayAddr);
// Первый параметр - номер оси многопроцессорной системы. Второй параметр - требуемый делитель размера блока.
DvmType dvmh_distribution_multblock_C(DvmType mpsAxis, DvmType multBlock);
DvmType dvmh_distribution_multblock_(const DvmType *pMpsAxis, const DvmType *pMultBlock);
// Первый параметр - номер оси многопроцессорной системы. Второй параметр - тип элементов массива genblock (rt_XXX, допустим rt_UNKNOWN). Третий параметр - ссылка на массив indirect (в случае rt_UNKNOWN ожидается заголовочный массив распределенного массива).
DvmType dvmh_distribution_indirect_C(DvmType mpsAxis, DvmType elemType, const void *arrayAddr);
DvmType dvmh_distribution_indirect_(const DvmType *pMpsAxis, const DvmType *pElemType, const void *arrayAddr);
// Первый параметр - номер оси многопроцессорной системы. Второй параметр - описание правой части, как вернет dvmh_derived_rhs. Третий параметр - обработчик, считающий размер буфера для индексов (использовать dvmh_handler_func). Четвертый параметр - обработчик, заполняющий буфер индексов (использовать dvmh_handler_func).
DvmType dvmh_distribution_derived_C(DvmType mpsAxis, DvmType derivedRhsHelper, DvmType countingHandlerHelper, DvmType fillingHandlerHelper);
DvmType dvmh_distribution_derived_(const DvmType *pMpsAxis, const DvmType *pDerivedRhsHelper, const DvmType *pCountingHandlerHelper, const DvmType *pFillingHandlerHelper);

// Запрос на получение буфера на count элементов типа DvmType.
DvmType *dvmh_indirect_get_buffer_C(DvmType count);

// Распределение шаблона или распределенного массива. Первый параметр - заголовочный массив. Второй параметр - количество измерений распределяемого шаблона или массива. Третий и последующие по количесту rank - значения, вернутые вспомогательными функциями семейства dvmh_distribution_*.
void dvmh_distribute_C(DvmType dvmDesc[], DvmType rank, /* DvmType distributionHelper */...);
void dvmh_distribute_(DvmType dvmDesc[], const DvmType *pRank, /* const DvmType *pDistributionHelper */...);

// Перераспределение шаблона или распределенного массива, а также всех на них выравненных массивов (напрямую или опосредованно). Первый параметр - заголовочный массив. Второй параметр - количество измерений распределяемого шаблона или массива. Третий и последующие по количесту rank - значения, вернутые вспомогательными функциями семейства dvmh_distribution_*.
void dvmh_redistribute_C(DvmType dvmDesc[], DvmType rank, /* DvmType distributionHelper */...);
void dvmh_redistribute2_(DvmType dvmDesc[], const DvmType *pRank, /* const DvmType *pDistributionHelper */...);

// Вспомогательная функция для задания линейного правила выравнивания для одного измерения образца. Первый параметр - номер измерения (с единицы), -1 означает размножение, 0 выравнивание на константу. Второй параметр - множитель. Третий параметр - слагаемое.
DvmType dvmh_alignment_linear_C(DvmType axis, DvmType multiplier, DvmType summand);
DvmType dvmh_alignment_linear_(const DvmType *pAxis, const DvmType *pMultiplier, const DvmType *pSummand);

// Выравнивание распределенного массива на шаблон или другой распределенный массив. Первый параметр - заголовочный массив выравниваемого массива. Второй параметр - заголовочный массив образца выравнивания. Третий параметр - количество измерений образца. Четвертый и последующие по колчеству измерений образца - значения, вернутые вспомогательными функциями семейства dvmh_alignment_*.
void dvmh_align_C(DvmType dvmDesc[], const DvmType templDesc[], DvmType templRank, /* DvmType alignmentHelper */...);
void dvmh_align_(DvmType dvmDesc[], const DvmType templDesc[], const DvmType *pTemplRank, /* const DvmType *pAlignmentHelper */...);

// Перевыравнивание распределенного массива на шаблон или другой распределенный массив. Первый параметр - заголовочный массив выравниваемого массива. Второй параметр - признак "нового" значения, т.е. отсутствия необходимости сохранить значения элементов перевыравниваемого массива. Третий параметр - заголовочный массив шаблона выравнивания. Четвертый - количество измерений образца. Пятый и последующие по колчеству измерений образца - значения, вернутые вспомогательными функциями семейства dvmh_alignment_*.
void dvmh_realign_C(DvmType dvmDesc[], DvmType newValueFlag, const DvmType templDesc[], DvmType templRank, /* DvmType alignmentHelper */...);
void dvmh_realign2_(DvmType dvmDesc[], const DvmType *pNewValueFlag, const DvmType templDesc[], const DvmType *pTemplRank,
        /* const DvmType *pAlignmentHelper */...);

// Добавление поэлементной теневой грани к шаблону, а также к заданному набору выравненных на него массивов. Первый параметр - заголовочный массив шаблона. Второй параметр - номер оси (с единицы). Третий параметр - описание правой части, как вернет dvmh_derived_rhs. Четвертый параметр - обработчик, заполняющий буфер индексов (использовать dvmh_handler_func). Пятый параметр - имя новой теневой грани. Шестой параметр - количество массивов, в которые надо включить теневую грань. Седьмой и последующие по количеству includeCount - заголовочные массивы этих распределенных массивов.
void dvmh_indirect_shadow_add_C(DvmType dvmDesc[], DvmType axis, DvmType derivedRhsHelper, DvmType countingHandlerHelper, DvmType fillingHandlerHelper,
        const char shadowName[], DvmType includeCount, /* DvmType dvmDesc[] */...);
// То же самое, но строки передаются через dvmh_string().
void dvmh_indirect_shadow_add_(DvmType dvmDesc[], const DvmType *pAxis, const DvmType *pDerivedRhsHelper, const DvmType *pCountingHandlerHelper,
        const DvmType *pFillingHandlerHelper, const DvmType *pShadowNameStr, const DvmType *pIncludeCount, /* DvmType dvmDesc[] */...);

// Команда об освобождении памяти динамически выделяемого массива, при этом массив переходит в состояние как после dvmh_array_declare_ (т.е. может быть сразу передан в dvmh_array_alloc_). Первый параметр - заголовочный массив.
void dvmh_array_free_C(DvmType dvmDesc[]);

// Команда об уничтожении DVMH-объекта. Первый параметр - заголовочный массив.
void dvmh_delete_object_(DvmType dvmDesc[]);

// Оповещение о прекращении существования заголовочного массива. Первый параметр - заголовочный массив.
void dvmh_forget_header_(DvmType dvmDesc[]);


// Работа с обычными переменными
// Запрос указателя на заголовочный массив для обычной переменной, при этом если переменная неизвестная (см. также dvmh_forget_variable), то она становится известной. Первый параметр - адрес переменной (скаляр или массив). Второй параметр - кол-во измерений (0 для скаляра). Третий параметр - тип элемента. Далее следуют по количеству измерений размер каждого из них.
DvmType *dvmh_variable_gen_header_C(const void *addr, DvmType rank, DvmType typeSize, /* DvmType axisSize */...);
// Запрос на заполнение копии заголовочного массива для обычной переменной. Первый параметр - заполняемый заголовочный массив. Второй параметр - база. Третий параметр - адрес переменной (скаляр или массив). Четвертый параметр - кол-во измерений (0 для скаляра). Пятый параметр - тип элемента. Далее следуют пары по количеству измерений: начальный индекс и конечный индекс.
void dvmh_variable_fill_header_(DvmType dvmDesc[], const void *baseAddr, const void *addr, const DvmType *pRank, const DvmType *pTypeSize,
        /* const DvmType *pSpaceLow, const DvmType *pSpaceHigh */...);
// Упрощенный вариант, не заполняет заголовок, а только возвращает его первый элемент. Годится для передачи в другие вызовы из фортрана.
DvmType dvmh_variable_gen_header_(const void *addr, const DvmType *pRank, const DvmType *pTypeSize,
        /* const DvmType *pSpaceLow, const DvmType *pSpaceHigh */...);

// Укороченная версия запроса указателя на заголовочный массив для обычной переменной. Переменная обязана быть известной.
DvmType *dvmh_variable_get_header_C(const void *addr);

// Вход в data регион.
void dvmh_data_enter_C(const void *addr, DvmType size);
void dvmh_data_enter_(const void *addr, const DvmType *pSize);

// Выход из data региона. Второй параметр - признак необходимости сохранения значения переменной.
void dvmh_data_exit_C(const void *addr, DvmType saveFlag);
void dvmh_data_exit_(const void *addr, const DvmType *pSaveFlag);

// Неявный вход для динамически выделяемой переменной
void *dvmh_malloc_C(size_t size);

// Неявный вход для динамически выделяемой переменной
void *dvmh_calloc_C(size_t nmemb, size_t size);

// Неявный выход-вход для динамически выделяемой переменной
void *dvmh_realloc_C(void *ptr, size_t size);

// Неявный вход для динамически выделяемой переменной
char *dvmh_strdup_C(const char *s);

// Неявный выход для динамически выделяемой переменной
void dvmh_free_C(void *ptr);


// Утилиты для работы с массивами
// Вспомогательная функция для задания секции массива
DvmType dvmh_array_slice_C(DvmType rank, /* DvmType start, DvmType end, DvmType step */...);
DvmType dvmh_array_slice_(const DvmType *pRank, /* const DvmType *pStart, const DvmType *pEnd, const DvmType *pStep */...);

// Копирование целиком массива. Любой из аргументов может быть нераспределенным массивом. Конфигурации должны совпадать, в том числе начальные индексы.
void dvmh_array_copy_whole_(const DvmType srcDvmDesc[], DvmType dstDvmDesc[]);

// Общая функция копирования.
void dvmh_array_copy_C(const DvmType srcDvmDesc[], DvmType srcSliceHelper, DvmType dstDvmDesc[], DvmType dstSliceHelper);
void dvmh_array_copy_(const DvmType srcDvmDesc[], DvmType *pSrcSliceHelper, DvmType dstDvmDesc[], DvmType *pDstSliceHelper);


// Доступ к переменным на устройствах
// Запрос на получение естественной базы для массива. Первый параметр - номер устройства (0 = хост-система). Второй параметр - заголовочный массив.
void *dvmh_get_natural_base_C(DvmType deviceNum, const DvmType dvmDesc[]);

// Запрос на получение адреса переменной на устройстве. Первый параметр - номер устройства (0 = хост-система). Второй параметр - переменная.
void *dvmh_get_device_addr_C(DvmType deviceNum, const void *addr);

// Запрос на заполнение заголовка и дополнительных парамтеров для адресации на устройстве. Первый параметр - номер устройства (0 = хост-система). Второй параметр - база. Третий параметр - заголовочный массив. Четвертый параметр - массив, в который будет записан результат. Пятый параметр - массив для записи дополнительных параметров адресации. Возвращает тип трансформации.
// Тип трансформации:
// 0 - нет трансформации. Элемент devHeader[rank] = 1, extendedParams не заполняется
// 1 - перестановка измерений. Элемент devHeader[rank] != 1, extendedParams не заполняется
// 2 - перестановка измерений + диагонализация. Два элемента devHeader зануляются, extendedParams заполняется следующим образом:
//     extendedParams[0] = номер измерения 'x'; // отсчет с единицы, devHeader[extendedParams[0]] = 0
//     extendedParams[1] = начальный индекс по измерению 'x';
//     extendedParams[2] = длина по измерению 'x';
//     extendedParams[3] = номер измерения 'y'; // отсчет с единицы, devHeader[extendedParams[3]] = 0
//     extendedParams[4] = начальный индекс по измерению 'y';
//     extendedParams[5] = длина по измерению 'y';
//     extendedParams[6] = признак направления диагоналей; // 0 - параллельно побочной, 1 - параллельно главной
DvmType dvmh_fill_header_C(DvmType deviceNum, const void *baseAddr, const DvmType dvmDesc[], DvmType devHeader[], DvmType extendedParams[]);
DvmType dvmh_fill_header2_(const DvmType *pDeviceNum, const void *baseAddr, const DvmType dvmDesc[], DvmType devHeader[]);
DvmType dvmh_fill_header_ex2_(const DvmType *pDeviceNum, const void *baseAddr, const DvmType dvmDesc[], DvmType devHeader[], DvmType extendedParams[]);


// Запросы актуализации на хост
// Запрос на актуализацию данных на хост-системе. Первый параметр – скалярная переменная или обычный массив. Второй параметр - количество измерений. Далее идут пары (нижний иднекс, верхний индекс) по количеству измерений. Значение индекса -2147483648 означает открытую границу (то есть до конца конкретного измерения с конкретной стороны имеющегося у этого процесса части пространства массива (локальная часть + теневые грани)).
void dvmh_get_actual_subvariable_C(void *addr, DvmType rank, /* DvmType indexLow, DvmType indexHigh */...);
void dvmh_get_actual_subvariable2_(void *addr, const DvmType *pRank, /* const DvmType *pIndexLow, const DvmType *pIndexHigh */...);

// Запрос на актуализацию данных на хост-системе. Параметр – скалярная переменная или обычный массив.
void dvmh_get_actual_variable2_(void *addr);

// Запрос на актуализацию данных на хост-системе. Первый параметр – заголовочный массив распределенного массива. Второй параметр - количество измерений. Далее идут пары (нижний иднекс, верхний индекс) по количеству измерений. Значение индекса -2147483648 означает открытую границу (то есть до конца конкретного измерения с конкретной стороны имеющегося у этого процесса части пространства массива (локальная часть + теневые грани)).
void dvmh_get_actual_subarray_C(const DvmType dvmDesc[], DvmType rank, /* DvmType indexLow, DvmType indexHigh */...);
void dvmh_get_actual_subarray2_(const DvmType dvmDesc[], const DvmType *pRank, /* const DvmType *pIndexLow, const DvmType *pIndexHigh */...);

// Запрос на актуализацию данных на хост-системе. Первый параметр – заголовочный массив распределенного массива. Распространяется на весь массив целиком (локальная часть + теневые грани).
void dvmh_get_actual_array2_(const DvmType dvmDesc[]);

// Запрос на актуализацию всех данных на хост-системе.
void dvmh_get_actual_all2_();


// Объявление актуальности на хосте
// Объявление актуальности данных на хост-системе. Первый параметр – скалярная переменная или обычный массив. Второй параметр - количество измерений. Далее идут пары (нижний иднекс, верхний индекс) по количеству измерений. Значение индекса -2147483648 означает открытую границу (то есть до конца конкретного измерения с конкретной стороны имеющейся у этого процесса локальной части массива).
void dvmh_actual_subvariable_C(const void *addr, DvmType rank, /* DvmType indexLow, DvmType indexHigh */...);
void dvmh_actual_subvariable2_(const void *addr, const DvmType *pRank, /* const DvmType *pIndexLow, const DvmType *pIndexHigh */...);

// Объявление актуальности данных на хост-системе. Параметр – скалярная переменная или обычный массив.
void dvmh_actual_variable2_(const void *addr);

// Объявление актуальности данных на хост-системе. Первый параметр – заголовочный массив распределенного массива. Второй параметр - количество измерений. Далее идут пары (нижний индекс, верхний индекс) по количеству измерений. Значение индекса -2147483648 означает открытую границу (то есть до конца конкретного измерения с конкретной стороны имеющейся у этого процесса локальной части массива).
void dvmh_actual_subarray_C(const DvmType dvmDesc[], DvmType rank, /* DvmType indexLow, DvmType indexHigh */...);
void dvmh_actual_subarray2_(const DvmType dvmDesc[], const DvmType *pRank, /* const DvmType *pIndexLow, const DvmType *pIndexHigh */...);

// Объявление актуальности данных на хост-системе. Первый параметр – заголовочный массив распределенного массива. Распространяется на всю имеющуюся у этого процесса локальную часть массива.
void dvmh_actual_array2_(const DvmType dvmDesc[]);

// Объявление актуальности всех данных на хост-системе.
void dvmh_actual_all2_();


// Прочие отдельно стоящие директивы и неявные вызовы вне регионов и параллельных циклов
// Обновление теневых граней. Первый параметр - заголовочный массив. Второй параметр - признак обновления уголков. Третий параметр - количество указанных пользователем ширин теневых граней по измерениям. Далее (если указаны) идут пары по количеству измерений массива: обновляемая ширина нижней теневой грани по измерению, обновляемая ширина верхней теневой грани по измерению.
void dvmh_shadow_renew_C(const DvmType dvmDesc[], DvmType cornerFlag, DvmType specifiedRank, /* DvmType shadowLow, DvmType shadowHigh */...);
void dvmh_shadow_renew2_(const DvmType dvmDesc[], const DvmType *pCornerFlag, const DvmType *pSpecifiedRank,
        /* const DvmType *pShadowLow, const DvmType *pShadowHigh */...);

// Обновление поэлементных теневых граней. Первый параметр - заголовочный массив. Второй параметр - номер оси (с единицы). Третий параметр - имя обновляемой теневой грани.
void dvmh_indirect_shadow_renew_C(const DvmType dvmDesc[], DvmType axis, const char shadowName[]);
void dvmh_indirect_shadow_renew_(const DvmType dvmDesc[], const DvmType *pAxis, const DvmType *pShadowNameStr);

// Локализация значений индексоного массива, являющегося ссылкой на поэлементно-распределенное измерение другого массива. Первый параметр - заголовочный массив индексного массива. Второй параметр - заголовочный массив целевого массива. Третий параметр - номер измерения целевого массива.
void dvmh_indirect_localize_C(const DvmType refDvmDesc[], const DvmType targetDvmDesc[], DvmType targetAxis);
void dvmh_indirect_localize_(const DvmType refDvmDesc[], const DvmType targetDvmDesc[], const DvmType *pTargetAxis);

// Разлокализация значений индексного массива, локализованного ранее вызовом dvmh_indirect_localize. Первый параметр - заголовочный массив.
void dvmh_indirect_unlocalize_(const DvmType dvmDesc[]);

// Создание, заполнение, подготовка к использованию буфера удаленных элементов для случая отдельной директивы. Первый параметр - заголовочный массив для буфера удаленных элементов. Второй параметр - заголовочный массив источника. Третий параметр - количество измерений массива. Четвертый и последующие по поличеству измерений массива - значения, вернутые вспомогательными функциями семейства dvmh_alignment_*.
void dvmh_remote_access_C(DvmType rmaDesc[], const DvmType dvmDesc[], DvmType rank, /* DvmType alignmentHelper */...);
// Создание, заполнение, подготовка к использованию буфера удаленных элементов для случая отдельной директивы. Первый параметр - заголовочный массив для буфера удаленных элементов. Второй параметр - база для индексации. Третий параметр - заголовочный массив источника. Четвертый параметр - количество измерений массива. Пятый и последующие по поличеству измерений массива - значения, вернутые вспомогательными функциями семейства dvmh_alignment_*.
void dvmh_remote_access2_(DvmType rmaDesc[], const void *baseAddr, const DvmType dvmDesc[], const DvmType *pRank, /* const DvmType *pAlignmentHelper */...);

// Запрос принадлежности указанного элемента распределенного массива локальной части этого массива в текущем процессе. Первый параметр - заголовочный массив. Второй параметр - количество измерений массива. Третий и последующие по количеству измерений - индексы.
DvmType dvmh_has_element_C(const DvmType dvmDesc[], DvmType rank, /* DvmType index */...);
DvmType dvmh_has_element_(const DvmType dvmDesc[], const DvmType *pRank, /* const DvmType *pIndex */...);

// Запрос линейного индекса для массива по набору его глобальных индексов. Первый параметр - заголовочный массив. Второй параметр - количество измерений массива. Третий и последующие по количеству измерений - индексы.
DvmType dvmh_calc_linear_C(const DvmType dvmDesc[], DvmType rank, /* DvmType globalIndex */...);
DvmType dvmh_calc_linear_(const DvmType dvmDesc[], const DvmType *pRank, /* const DvmType *pGlobalIndex */...);

// Запрос адреса собственного элемента массива по набору его глобальных индексов. NULL если не является собственным. Первый параметр - заголовочный массив. Второй параметр - количество измерений массива. Третий и последующие по количеству измерений - индексы.
void *dvmh_get_own_element_addr_C(const DvmType dvmDesc[], DvmType rank, /* DvmType globalIndex */...);

// Запрос адреса элемента массива по набору его глобальных индексов. NULL если нет на текущем процессоре (ни в локальной части, ни в теневой грани). Первый параметр - заголовочный массив. Второй параметр - количество измерений массива. Третий и последующие по количеству измерений - индексы.
void *dvmh_get_element_addr_C(const DvmType dvmDesc[], DvmType rank, /* DvmType globalIndex */...);


// Регионы
// Создание вычислительного региона. Первый параметр - битовый набор флагов региона (REGION_ASYNC = 1, REGION_COMPARE_DEBUG = 2).
DvmType dvmh_region_create_C(DvmType regionFlags);
DvmType dvmh_region_create_(const DvmType *pRegionFlags);

// Регистрация подмассива в регионе. Первый параметр – описатель региона. Второй параметр – направление использования (INTENT_IN = 1, INTENT_OUT = 2, INTENT_LOCAL = 4, INTENT_INOUT = 3, INTENT_INLOCAL = 5). Третий параметр – заголовочный массив. Четвертый параметр - имя переменной. Пятый параметр - количество измерений массива. Далее идут пары (нижний индекс, верхний индекс) по количеству измерений массива. Значение индекса -2147483648 означает открытую границу (то есть до конца конкретного измерения с конкретной стороны (для in - локальная часть + теневые грани, для out и local - только локальная часть)).
void dvmh_region_register_subarray_C(DvmType curRegion, DvmType intent, const DvmType dvmDesc[], const char varName[], DvmType rank,
        /* DvmType indexLow, DvmType indexHigh */...);
// То же самое, но строки передаются через dvmh_string().
void dvmh_region_register_subarray_(const DvmType *pCurRegion, const DvmType *pIntent, const DvmType dvmDesc[], const DvmType *pVarNameStr,
        const DvmType *pRank, /* const DvmType *pIndexLow, const DvmType *pIndexHigh */...);

// Регистрация массива целиком в регионе. Первый параметр – описатель региона. Второй параметр – направление использования (INTENT_IN = 1, INTENT_OUT = 2, INTENT_LOCAL = 4, INTENT_INOUT = 3, INTENT_INLOCAL = 5). Третий параметр – заголовочный массив. Четвертый параметр - имя переменной. Распространяется на весь массив (для in - локальная часть + теневые грани, для out и local - только локальная часть).
void dvmh_region_register_array_C(DvmType curRegion, DvmType intent, const DvmType dvmDesc[], const char varName[]);
// То же самое, но строки передаются через dvmh_string().
void dvmh_region_register_array_(const DvmType *pCurRegion, const DvmType *pIntent, const DvmType dvmDesc[], const DvmType *pVarNameStr);

// Регистрация скаляра в регионе. Первый параметр – описатель региона. Второй параметр – направление использования (INTENT_IN = 1, INTENT_OUT = 2, INTENT_LOCAL = 4, INTENT_INOUT = 3, INTENT_INLOCAL = 5). Третий параметр – скалярная переменная. Четвертый параметр – тип скаляра. Пятый параметр - имя переменной.
void dvmh_region_register_scalar_C(DvmType curRegion, DvmType intent, const void *addr, DvmType typeSize, const char varName[]);
// То же самое, но строки передаются через dvmh_string().
void dvmh_region_register_scalar_(const DvmType *pCurRegion, const DvmType *pIntent, const void *addr, const DvmType *pTypeSize,
        const DvmType *pVarNameStr);

// Указание целевого набора типов устройств для выполнения региона, завершение регистраций и команда к проведению распределения по устройствам. Первый параметр - описатель региона. Второй параметр – побитовое объединение типов устройств (DEVICE_TYPE_HOST = 1, DEVICE_TYPE_CUDA = 2).
void dvmh_region_execute_on_targets_C(DvmType curRegion, DvmType deviceTypes);
void dvmh_region_execute_on_targets_(const DvmType *pCurRegion, const DvmType *pDeviceTypes);

// Конец региона. Первый параметр - описатель региона.
void dvmh_region_end_C(DvmType curRegion);
void dvmh_region_end_(const DvmType *pCurRegion);


// Параллельные циклы
// Создание параллельного цикла. Первый параметр - описатель региона (0, если не в регионе). Второй параметр - кол-во измерений параллельного цикла (для последовательного участка в регионе ставится нуль). Далее идут тройки по количеству измерений: начальное значение индексной переменной, конечное значение индексной переменной, шаг. Возвращает описатель цикла.
DvmType dvmh_loop_create_C(DvmType curRegion, DvmType rank, /* DvmType start, DvmType end, DvmType step */...);
DvmType dvmh_loop_create_(const DvmType *pCurRegion, const DvmType *pRank, /* const DvmType *pStart, const DvmType *pEnd, const DvmType *pStep */...);

// Задание правила отображения для параллельного цикла. Первый параметр - описатель цикла. Второй параметр - заголовочный массив образца выравнивания. Третий параметр - количество измерений образца. Четвертый и последующие - значения, вернутые вспомогательными функциями семейства dvmh_alignment_*.
void dvmh_loop_map_C(DvmType curLoop, const DvmType templDesc[], DvmType templRank, /* DvmType alignmentHelper */...);
void dvmh_loop_map_(const DvmType *pCurLoop, const DvmType templDesc[], const DvmType *pTemplRank, /* const DvmType *pAlignmentHelper */...);

// Задание указанного пользователем CUDA-блока при цикле. Первый параметр - описатель цикла. Второй параметр - размер блока по оси X. Третий параметр - размер блока по оси Y. Четвертый параметр - размер блока по оси Z.
void dvmh_loop_set_cuda_block_C(DvmType curLoop, DvmType xSize, DvmType ySize, DvmType zSize);
void dvmh_loop_set_cuda_block_(const DvmType *pCurLoop, const DvmType *pXSize, const DvmType *pYSize, const DvmType *pZSize);

// Задание указанного пользователем числа квантов конвейера. Первый параметр - описатель цикла. Второй параметр - количество квантов.
void dvmh_loop_set_stage_C(DvmType curLoop, DvmType stage);
void dvmh_loop_set_stage_(const DvmType *pCurLoop, const DvmType *pStage);

// Добавление редукции в цикл. Первый параметр - описатель цикла. Второй параметр - тип редукции (rf_XXX). Третий параметр - адрес редукционной переменной. Четвертый параметр - тип элемента редукционной переменной (rt_XXX). Пятый параметр - длина редукционного массива. Шестой параметр - адрес LOC-переменной. Седьмо параметр - размер в байтах LOC-переменной.
void dvmh_loop_reduction_C(DvmType curLoop, DvmType redType, void *arrayAddr, DvmType varType, DvmType arrayLength, void *locAddr, DvmType locSize);
void dvmh_loop_reduction_(const DvmType *pCurLoop, const DvmType *pRedType, void *arrayAddr, const DvmType *pVarType, const DvmType *pArrayLength,
        void *locAddr, const DvmType *pLocSize);

// Добавление зависимости по данным в цикл. Первый параметр - описатель цикла. Второй параметр - заголовочный массив. Третий параметр - количество измерений массива. Далее идут пары по количеству измерений массива: читаемая ширина нижней теневой грани, читаемая ширина верхней теневой грани.
void dvmh_loop_across_C(DvmType curLoop, const DvmType dvmDesc[], DvmType rank, /* DvmType shadowLow, DvmType shadowHigh */...);
void dvmh_loop_across_(const DvmType *pCurLoop, const DvmType dvmDesc[], const DvmType *pRank, /* const DvmType *pShadowLow, const DvmType *pShadowHigh */...);

// Указание ширины расширения локальной части витков параллельного цикла клаузой SHADOW_COMPUTE. Первый параметр - описатель цикла. Второй параметр - заголовочный массив, являющийся шаблоном выравнивания для данного цикла. Третий параметр - количество измерений шаблона (если указаны). Далее (если указаны) идут пары по количеству измерений массива: вычисляемая ширина нижней теневой грани по измерению, вычисляемая ширина верхней теневой грани по измерению.
void dvmh_loop_shadow_compute_C(DvmType curLoop, const DvmType templDesc[], DvmType specifiedRank, /* DvmType shadowLow, DvmType shadowHigh */...);
void dvmh_loop_shadow_compute_(const DvmType *pCurLoop, const DvmType templDesc[], const DvmType *pSpecifiedRank,
        /* const DvmType *pShadowLow, const DvmType *pShadowHigh */...);

// Задание изменямых в параллельном цикле массивов для более эффективной обработки указания SHADOW_COMPUTE (необязательно, по умолчанию все массивы региона будут считаться изменяемыми в данном цикле). Первый параметр - описатель цикла. Второй параметр - заголовочный массив изменяемого в цикле массива.
void dvmh_loop_shadow_compute_array_C(DvmType curLoop, const DvmType dvmDesc[]);
void dvmh_loop_shadow_compute_array_(const DvmType *pCurLoop, const DvmType dvmDesc[]);

// Обработка указания CONSISTENT. Первый параметр - описатель цикла. Второй параметр - заголовочный массив. Третий параметр - количество измерений. Четвертый и последующие по количеству измерений - значения, вернутые вспомогательными функциями семейства dvmh_alignment_*.
void dvmh_loop_consistent_C(DvmType curLoop, const DvmType dvmDesc[], DvmType rank, /* DvmType alignmentHelper */...);
void dvmh_loop_consistent_(const DvmType *pCurLoop, const DvmType dvmDesc[], const DvmType *pRank, /* const DvmType *pAlignmentHelper */...);

// Добавление REMOTE_ACCESS в цикл. Первый параметр - описатель цикла. Второй параметр - заголовочный массив источника. Третий параметр - количество измерений. Четвертый и последующие по количеству измерений - значения, вернутые вспомогательными функциями семейства dvmh_alignment_*.
void dvmh_loop_remote_access_C(DvmType curLoop, const DvmType dvmDesc[], DvmType rank, /* DvmType alignmentHelper */...);
void dvmh_loop_remote_access_(const DvmType *pCurLoop, const DvmType dvmDesc[], const DvmType *pRank, /* const DvmType *pAlignmentHelper */...);

// Регистрация обработчика параллельного цикла. Первый параметр - описатель цикла. Второй параметр - тип устройства, для которого подготовлен обработчик (DEVICE_TYPE_HOST = 1, DEVICE_TYPE_CUDA = 2). Третий параметр - тип обработчика (битовый набор флагов HANDLER_TYPE_PARALLEL = 1, HANDLER_TYPE_MASTER = 2). Четвертый параметр - обработчик, исполняющий порцию цикла (использовать dvmh_handler_func).
void dvmh_loop_register_handler_C(DvmType curLoop, DvmType deviceType, DvmType handlerType, DvmType handlerHelper);
void dvmh_loop_register_handler_(const DvmType *pCurLoop, const DvmType *pDeviceType, const DvmType *pHandlerType, const DvmType *pHandlerHelper);

// Команда на выполнение параллельного цикла. Первый параметр - описатель цикла.
void dvmh_loop_perform_C(DvmType curLoop);
void dvmh_loop_perform_(const DvmType *pCurLoop);

// Запрос маски зависимых измерений параллельного цикла. Младший разряд соответствует внутреннему циклу. 1 - есть зависимость, 0 - нет зависимости. Первый параметр - новый локальный описатель цикла, который передается параметром в обработчик.
DvmType dvmh_loop_get_dependency_mask_C(DvmType curLoop);
DvmType dvmh_loop_get_dependency_mask_(const DvmType *pCurLoop);

// Опрос номера устройства, для работы на котором был запущен обработчик. Первый параметр - новый локальный описатель цикла, который передается параметром в обработчик.
DvmType dvmh_loop_get_device_num_C(DvmType curLoop);
DvmType dvmh_loop_get_device_num_(const DvmType *pCurLoop);

// Запрос на проведение автоматической трансформации массива на устройстве для цикла. Первый параметр - новый локальный описатель цикла, который передается параметром в обработчик. Второй параметр - заголовочный массив. Возвращает тип произведенной трансформации: 0 - нет трансформации, 1 - перестановка измерений, 2 - подиагональная трансформация.
DvmType dvmh_loop_autotransform_C(DvmType curLoop, DvmType dvmDesc[]);
DvmType dvmh_loop_autotransform_(const DvmType *pCurLoop, DvmType dvmDesc[]);

// Запрос на получение описания порции цикла для его выполнения в обработчике. Первый параметр - новый локальный описатель цикла, который передается параметром в обработчик. Второй параметр - массив, куда будут записаны начальные значения. Третий параметр - массив, куда будут записаны конечные значения. Четвертый параметр - массив, куда будут записаны шаги (может быть NULL, если шаги не нужны).
void dvmh_loop_fill_bounds_C(DvmType curLoop, DvmType boundsLow[], DvmType boundsHigh[], DvmType loopSteps[]);
void dvmh_loop_fill_bounds_(const DvmType *pCurLoop, DvmType boundsLow[], DvmType boundsHigh[], DvmType loopSteps[]);

// Запрос количества слотов устройства, выделенных для текущего запуска обработчика. Первый параметр - новый локальный описатель цикла, который передается параметром в обработчик.
DvmType dvmh_loop_get_slot_count_C(DvmType curLoop);
DvmType dvmh_loop_get_slot_count_(const DvmType *pCurLoop);

// Запрос на заполнение массива, описывающего локальную часть обрабатываемого сейчас обработчика. Для любых обработчиков на любых устройствах. Первый параметр - новый локальный описатель цикла, которые передается в параметром в обработчик. Второй параметр - заголовочный массив. Третий параметр - массив, в который будут записаны нижние и верхние границы локальной части.
void dvmh_loop_fill_local_part_C(DvmType curLoop, const DvmType dvmDesc[], DvmType part[]);
void dvmh_loop_fill_local_part_(const DvmType *pCurLoop, const DvmType dvmDesc[], DvmType part[]);

// Запрос идентификатора целочисленного типа со знаком (из числа rt_XXX констант), достаточного для вычисления индексных выражений массивов в данном цикле при условии использования естественной (смещение равно нулю) или фактической (адрес начала фактического расположения элементов) базы.
DvmType dvmh_loop_guess_index_type_C(DvmType curLoop);
DvmType dvmh_loop_guess_index_type_(const DvmType *pCurLoop);

// Запрос на заполнение массива в глобальной памяти ГПУ, описывающего локальную часть обрабатываемого сейчас CUDA-обработчика. Первый параметр - новый локальный описатель цикла, который передается параметром в обработчик. Второй параметр - заголовочный массив. Третий параметр - тип индекса (rt_INT, rt_LONG, rt_LLONG).
const void *dvmh_loop_cuda_get_local_part_C(DvmType curLoop, const DvmType dvmDesc[], DvmType indexType);

// Запрос на заполнение заголовочного массива для буфера удаленных элементов. Первый параметр - новый локальный описатель цикла, который передается параметром в обработчик. Второй параметр - номер буфера удаленных элементов (нумерация с единицы). Третий параметр - массив, в который будет помещен заголовочный массив буфера удаленных элементов.
void dvmh_loop_get_remote_buf_C(DvmType curLoop, DvmType rmaIndex, DvmType rmaDesc[]);
void dvmh_loop_get_remote_buf_(const DvmType *pCurLoop, const DvmType *pRmaIndex, DvmType rmaDesc[]);

// Функция для регистрации переменных, в которые будут помещены адреса для редукционной переменной и ее LOC в глобальной памяти ГПУ. Первый параметр – новый локальный описатель цикла, который передается параметром в обработчик. Второй параметр – номер по порядку редукционной функции для цикла (нумерация с единицы). Третий параметр – адрес переменной, в которую будет записан адрес массива редукционных переменных в памяти ГПУ. Четвертый параметр - адрес переменной, в которую будет записан адрес массива LOC-переменных в памяти ГПУ.
void dvmh_loop_cuda_register_red_C(DvmType curLoop, DvmType redIndex, void **arrayAddrPtr, void **locAddrPtr);

// Запрос на заполнение начальными значениями локальных переменных обработчика для проведения редукции. Первый параметр - новый локальный описатель цикла, который передается параметром в обработчик. Второй параметр - номер редукции (с единицы, в порядке dvmh_loop_insred_). Третий параметр - адрес локальной редукционной переменной. Четвертый параметр - адрес локальной LOC-переменной.
void dvmh_loop_red_init_C(DvmType curLoop, DvmType redIndex, void *arrayAddr, void *locAddr);
void dvmh_loop_red_init_(const DvmType *pCurLoop, const DvmType *pRedIndex, void *arrayAddr, void *locAddr);

// Альтернативная функция для запроса на заполнение редукционного массива и сопутствующего ему loc начальными данными, которая создаёт массивы начальных значений в памяти устройства. Первый параметр - новый локальный описатель параллельного DVMH цикла, который передается параметром в обработчик. Второй параметр – номер по порядку редукционной функции для цикла. Третий параметр - указатель на указатель, в который будет записан адрес в памяти устройства для массива начальных значений (допустим NULL - не создавать копию в памяти устройства). Четвертый параметр - указатель на указатель, в который будет записан адрес в памяти устройства для массива начальных значений для loc (допустим NULL - не создавать копию в памяти устройства).
void dvmh_loop_cuda_red_init_C(DvmType curLoop, DvmType redIndex, void **devArrayAddrPtr, void **devLocAddrPtr);

// Функция для запрашивания конфигурации для запуска ядер в данной порции цикла. Первый параметр – новый локальный описатель цикла, который передается параметром в обработчик. Второй параметр – необходимое количество байт разделяемой памяти в расчете на одну нить. Третий параметр – необходимое количество регистров в расчете на одну нить. Четвертый – входной и выходной параметр, который на входе имеет значение блока по-умолчанию (заполненный нулями, если нет предпочтения), а на выходе заполняется выбранным блоком. Пятый и шестой параметры – выходные параметры для задания конфигурации запуска ядер.
void dvmh_loop_cuda_get_config_C(DvmType curLoop, DvmType sharedPerThread, DvmType regsPerThread, void *inOutThreads, void *outStream,
        DvmType *outSharedPerBlock);

// Выделение памяти на заданное количество редукционных переменных (возможно, с инициализацией) для последующего проведения редукции. Первый параметр – новый локальный описатель цикла, который передается параметром в обработчик. Второй параметр – номер по порядку редукционной функции для цикла (нумерация с единицы). Третий параметр – размер (в редукционных переменных) запрашиваемого массива (или двух в случаях MINLOC и MAXLOC). Четвертый параметр - флаг заполнения начальным значением (1 - заполнять, 0 - нет).
void dvmh_loop_cuda_red_prepare_C(DvmType curLoop, DvmType redIndex, DvmType count, DvmType fillFlag);

// Запрос принадлежности указанного элемента локальной части массива, приписанной текущей порции цикла в обработчике. Первый параметр - новый локальный описатель цикла, который передается параметром в обработчик. Второй параметр - заголовочный массив. Третий параметр - количество измерений массива. Четвертый и последующие по количеству измерений массива - индексы.
DvmType dvmh_loop_has_element_C(DvmType curLoop, const DvmType dvmDesc[], DvmType rank, /* DvmType index */...);
DvmType dvmh_loop_has_element_(const DvmType *pCurLoop, const DvmType dvmDesc[], const DvmType *pRank, /* const DvmType *pIndex */...);

// Доделать редукцию, которую ранее зарегистрировали и для которой был запрошен буфер с помощью loop_cuda_red_prepare_. После данного вызова не делать loop_red_post_, так как он выполнится изнутри данного вызова. Первый параметр - новый локальный описатель цикла, который передается параметром в обработчик. Второй параметр - номер по порядку редукционной функции для цикла (нумерация с единицы).
void dvmh_loop_cuda_red_finish_C(DvmType curLoop, DvmType redIndex);

// Возврат результата частичной редукции, произведенной обработчиком. Первый параметр - новый локальный описатель цикла, который передается параметром в обработчик. Второй параметр - номер редукции (с единицы, в порядке dvmh_loop_insred_). Третий параметр - адрес локальной редукционной переменной. Четвертый параметр - адрес локальной LOC-переменной.
void dvmh_loop_red_post_C(DvmType curLoop, DvmType redIndex, const void *arrayAddr, const void *locAddr);
void dvmh_loop_red_post_(const DvmType *pCurLoop, const DvmType *pRedIndex, const void *arrayAddr, const void *locAddr);


// Области видимости - автоматическая подчистка созданных массивов и шаблонов
// Начало новой области видимости
void dvmh_scope_start_();

// Вставка объекта в текущую область видимости
void dvmh_scope_insert_(const DvmType dvmDesc[]);

// Конец текущей области видимости
void dvmh_scope_end_();


// Интервалы для отладки эффективности
// Вход в интервал, охватывающий параллельный цикл.
void dvmh_par_interval_start_C();

// Вход в интервал, охватывающий последовательный цикл.
void dvmh_seq_interval_start_C();

// Выход из интервала, охватывающего цикл.
void dvmh_sp_interval_end_();

// Вход в пользовательский интервал. Параметр - пользовательский идентификатор интервала.
void dvmh_usr_interval_start_C(DvmType userID);
void dvmh_usr_interval_start_(DvmType *pUserID);

// Выход из пользовательского интервала.
void dvmh_usr_interval_end_();


// Функции ввода/вывода для вызова из Фортрана. Поддерживается ограниченно только потоковые (stream) файлы, неформатированные и в перспективе форматированные.
// В случае неуказания опционального параметра типа строка (выражение или переменная), передавать надо 0 (dvm0c0).
// В случае неуказания опционального входного числового параметра, тоже передавать надо 0.
// В случае неуказания опционального выходного числового параметра, тоже передавать надо 0. Тип всех выходных числовых параметров DvmType.
// Открытие файла или изменение режима или переоткрытие ранее открытого юнита. Соответствует оператору OPEN языка Фортран.
void dvmh_ftn_open_(const DvmType *pUnit, const DvmType *pAccessStr, const DvmType *pActionStr, const DvmType *pAsyncStr, const DvmType *pBlankStr,
        const DvmType *pDecimalStr, const DvmType *pDelimStr, const DvmType *pEncodingStr, const DvmType *pFileStr, const DvmType *pFormStr,
        const DvmType *pErrFlagRef, const DvmType *pIOStatRef, const DvmType *pIOMsgStrRef, const DvmType *pNewUnitRef, const DvmType *pPadStr,
        const DvmType *pPositionStr, const DvmType *pRecl, const DvmType *pRoundStr, const DvmType *pSignStr, const DvmType *pStatusStr,
        const DvmType *pDvmModeStr);

// Закрытие файла.
void dvmh_ftn_close_(const DvmType *pUnit, const DvmType *pErrFlagRef, const DvmType *pIOStatRef, const DvmType *pIOMsgStrRef, const DvmType *pStatusStr);

// Проверка подсоединен ли указанный unit через LibDVMH.
DvmType dvmh_ftn_connected_(const DvmType *pUnit, const DvmType *pFailIfYes);

// Неформатированное чтение секции DVM-массива из файла. Восьмой параметр - количество указанных измерений для вырезки. Далее идут параметры парами (нижний иднекс, верхний индекс) по количеству измерений. Значение индекса -2147483648 означает открытую границу (то есть до конца конкретного измерения с конкретной стороны).
void dvmh_ftn_read_unf_(const DvmType *pUnit, const DvmType *pEndFlagRef, const DvmType *pErrFlagRef, const DvmType *pIOStatRef,
        const DvmType *pIOMsgStrRef, const DvmType *pPos, const DvmType dvmDesc[], const DvmType *pSpecifiedRank,
        /* const DvmType *pIndexLow, const DvmType *pIndexHigh */...);

// Неформатированная запись секции DVM-массива в файл. Седьмой параметр - количество указанных измерений для вырезки. Далее идут параметры парами (нижний иднекс, верхний индекс) по количеству измерений. Значение индекса -2147483648 означает открытую границу (то есть до конца конкретного измерения с конкретной стороны).
void dvmh_ftn_write_unf_(const DvmType *pUnit, const DvmType *pErrFlagRef, const DvmType *pIOStatRef, const DvmType *pIOMsgStrRef, const DvmType *pPos,
        const DvmType dvmDesc[], const DvmType *pSpecifiedRank, /* const DvmType *pIndexLow, const DvmType *pIndexHigh */...);

// Обрезание файла по текущей позиции.
void dvmh_ftn_endfile_(const DvmType *pUnit, const DvmType *pErrFlagRef, const DvmType *pIOStatRef, const DvmType *pIOMsgStrRef);

// Установка текущей позиции файла в его начало.
void dvmh_ftn_rewind_(const DvmType *pUnit, const DvmType *pErrFlagRef, const DvmType *pIOStatRef, const DvmType *pIOMsgStrRef);

// Сброс буферов ВВ/ВЫВ.
void dvmh_ftn_flush_(const DvmType *pUnit, const DvmType *pErrFlagRef, const DvmType *pIOStatRef, const DvmType *pIOMsgStrRef);


// Сравнительная отладка
// Трассировка записи нового значения в переменную (вызывается перед записью). Первый параметр - тип переменной, второй параметр - адрес переменной, третий параметр - Handle массива (NULL если переменная не является массиваом), четвертый параметр - имя переменной.
DvmType dvmh_dbg_before_write_var_C(DvmType plType, DvmType addr, DvmType handle, char* szOperand);

// Трассировка записи нового значения в переменную (вызывается после записи).
DvmType dvmh_dbg_after_write_var_C();

// Трассировка чтения значения переменной. Первый параметр - тип переменной, второй параметр - адрес переменной, третий параметр - Handle массива (NULL если переменная не является массиваом), четвертый параметр - имя переменной.
DvmType dvmh_dbg_read_var_C(DvmType plType, DvmType addr, DvmType handle, char *szOperand);

// Трассировка начала последовательного цикла. Параметр - порядковый номер цикла.
DvmType dvmh_dbg_loop_seq_start_C(DvmType no);

// Трассировка начала параллельного цикла. Первый параметр - порядковый номер цикла. Второй параметр - размерность цикла. Третий и последующие параметры - информация об итерационных переменных цикла (нижняя граница, верхняя граница, шаг).
DvmType dvmh_dbg_loop_par_start_C( DvmType no, DvmType rank, ... );

// Трассировка конца цикла.
DvmType dvmh_dbg_loop_end_C();

// Трассировка итерации цикла. Первый параметр - размерность цикла. Второй и последующие параметры - текущие значения итерационных переменных цикла.
DvmType dvmh_dbg_loop_iter_C(DvmType rank, ...);

// Создание параллельного цикла для сравнительной отладки. Первый параметр - описатель региона (0, если не в регионе). Второй параметр - кол-во измерений параллельного цикла (для последовательного участка в регионе ставится нуль). Далее идут тройки по количеству измерений: начальное значение индексной переменной, конечное значение индексной переменной, шаг. Возвращает описатель цикла.
DvmType dvmh_dbg_loop_create_C(DvmType curRegion, DvmType rank, /* DvmType start, DvmType end, DvmType step */...);

// Создание редукционной группы для сравнительной отладки. Параметр - описатель цикла.
void dvmh_dbg_loop_red_group_create_C(DvmType curLoop);

// Инициалицация редукционной переменной основной программы для сравнительной отладки. Первый параметр - описатель цикла. Второй параметр - номер редукции (с единицы). Третий параметр - адрес локальной редукционной переменной. Четвертый параметр - TODO
void dvmh_dbg_loop_global_red_init_C(DvmType curLoop, DvmType redIndex, void *arrayAddr, char *name);

// Инициалицация редукционной переменной обработчика для сравнительной отладки. Первый параметр - описатель цикла. Второй параметр - номер редукции (с единицы). Третий параметр - адрес локальной редукционной переменной. Четвертый параметр - TODO
void dvmh_dbg_loop_handler_red_init_C(DvmType curLoop, DvmType redIndex, void *arrayAddr, char *name);

// Создание отладочной редукционной группы. Параметр - описатель цикла.
void dvmh_dbg_loop_red_group_delete_C(DvmType curLoop);


// Утилиты
// Возвращает адрес переданной переменной
DvmType dvmh_get_addr_(void *pVariable);
// Сохраняет копию строки s длины len во внутреннем буфере и выдает некий номер, который следует передавать в процедуры, принимающие строки в таком виде. Нужна для удобства передачи нескольких строковых параметров из Фортрана за один раз.
DvmType dvmh_string_(const char s[], int len);
// Запоминает ссылку на строковую переменную s длины len и выдает некий номер, который следует передавать в процедуры, принимающие ссылки на строковые переменные в таком виде. Нужна для удобства передачи нескольких строковых переменных из Фортрана за один раз.
DvmType dvmh_string_var_(char s[], int len);
// Перетасовывает три массива boundsLow, boundsHigh и loopSteps так, чтобы сначала шли независимые измерения, а за ними зависимые (снаружи внутрь). depMask - маска зависимостей, как дана loop_get_dependency_mask_.
void dvmh_change_filled_bounds_C(DvmType boundsLow[], DvmType boundsHigh[], DvmType loopSteps[], DvmType rank, DvmType depMask, DvmType idxPerm[]);
// Перетасовывает три массива boundsLow, boundsHigh и loopSteps так, чтобы сначала шли зависимые измерения, а за ними независимые (снаружи внутрь). depMask - маска зависимостей, как дана loop_get_dependency_mask_. Также в отличие от dvmh_change_filled_bounds значения idxPerm начинаются с нуля.
void dvmh_change_filled_bounds2_C(DvmType boundsLow[], DvmType boundsHigh[], DvmType loopSteps[], DvmType rank, DvmType depMask, DvmType idxPerm[]);


// Временные функции
// Возвращает ссылку на LibDVM-заголовочный массив.
DvmType *dvmh_get_dvm_header_C(const DvmType dvmDesc[]);

}
#pragma GCC visibility pop
