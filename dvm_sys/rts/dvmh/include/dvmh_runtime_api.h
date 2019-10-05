#ifndef DVMH_RUNTIME_API_H
#define DVMH_RUNTIME_API_H

#include <stdio.h>

#pragma GCC visibility push(default)
#ifdef __cplusplus
extern "C" {
#endif

/**
 * Барьерная синхронизация в рамках текущей многопроцессорной системы
 */
void dvmh_barrier();

/**
 * @return Количество процессоров всего в текущей многопроцессорной системе
 */
int dvmh_get_total_num_procs();

/**
 * @return Эффективное количество измерений в текущей многопроцессорной системе (хвостовые измерения длины 1 не считаются)
 */
int dvmh_get_num_proc_axes();

/**
 * @param axis Номер оси (нумерация с единицы)
 * @return Количество процессоров в текущей многопроцессорной системе на оси axis
 */
int dvmh_get_num_procs(int axis);

/**
 * @return Время в секундах по настенным часам
 */
double dvmh_wtime();

/* Local I/O functions */
int dvmh_remove_local(const char *filename);
int dvmh_rename_local(const char *oldFN, const char *newFN);
FILE *dvmh_tmpfile_local(void);
char *dvmh_tmpnam_local(char *s);

/* TODO: Add Fortran variants so that they do not clash with these */

#ifdef __cplusplus
}
#endif
#pragma GCC visibility pop

#endif
