#if !defined( __INTERVAL_H )
#define __INTERVAL_H
#define Fic_index 2000000000

#ifndef __callstd
#define __callstd
#endif

/**
 * Создать пользовательский интервал
 *
 *
 * @param nfrag	порядковый номер цикла для идентификации парных команд
 * @param nline	номер строки в исходном файле
 * @param val	значение выражения в пользовательском интервале
 */
DVMUSERFUN void __callstd binter_(DvmType *nfrag, DvmType *index);

/**
 * Завершить пользовательский интервал
 *
 *
 * @param nfrag	порядковый номер цикла для идентификации парных команд
 * @param nline	номер строки в исходном файле
 */
DVMUSERFUN void __callstd einter_(DvmType *nfrag, DvmType *nline);

/**
 * Создать коллективный интервал
 *
 *
 * @param nitem тип интервала коллективной опперации
 */
DVMUSERFUN void __callstd biinter_(short nitem);

/**
 * Закрыть текущий коллективный интервал
 */
DVMUSERFUN void __callstd eiinter_(void);

/**
 * Создать последовательный интервал для цикла
 *
 *
 * @param nfrag	порядковый номер цикла для идентификации парных команд
 */
DVMUSERFUN void __callstd bsloop_(DvmType *nfrag);

/**
 * Создать праллельный интервал для цикла
 *
 *
 * @param nfrag	порядковый номер цикла для идентификации парных команд
 */
DVMUSERFUN void __callstd bploop_(DvmType *nfrag);

/**
 * Закрыть текущий последовательный и праллельный интервал для цикла
 *
 *
 * @param nfrag	порядковый номер цикла для идентификации парных команд
 * @param nline	номер строки в исходном файле
 */
DVMUSERFUN void __callstd enloop_(DvmType *nfrag, DvmType *nline);

#endif
