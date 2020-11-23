#if !defined( __STRALL_H )
#define __STRALL_H
#if defined (_STATFILE_)
#include "sysstat.h"
#endif

// Константы для длин типов
#define SZSH  sizeof(short)
#define SZL   sizeof(DvmType)
#define SZINT sizeof(int)
#define SZD   sizeof(double)
#define SZV   sizeof(void*)
#define STM   (3 * StatGrpCount * StatGrpCount)
#define DVM_STAT_INTERVAL_SIZE (sizeof(struct inter_short) + sizeof(struct inter_long) + sizeof(struct inter_void) + sizeof(struct inter_double))

// Подключение типов для статистикки dvmh
#include "dvmh_stat.h"

enum typecollect {INOUT=1,SINOUT,WINOUT,NINOUT,REDUC,SREDUC,WREDUC,NREDUC,
SHAD,SSHAD,WSHAD,NSHAD,RACC,SRACC,WRACC,NRACC,REDISTR,SREDISTR,WREDISTR,
NREDISTR}; /* new operation insert before REDISTR */
#define QCOLLECT NREDISTR  /* 4 type for 5 collective operation */

enum typefrag {PREFIX=QCOLLECT,SEQ,PAR,USER};
#if !defined (_STATFILE_)

// -- Заголовок буфера вывода статистики -------------------------------------------------------------------------------

/**
 * Группа характеристик описания заголовка буфера вывода статистики
 *
 * @see tvms_ch
 */
struct vms_const {
	unsigned char reverse[2]; /**< признак, что информация собрана не на рабочей станции */
	unsigned char szsh   [2]; /**< размер переменной типа short */
	unsigned char szl    [2]; /**< размер переменной типа long */
	unsigned char szv    [2]; /**< размер переменной типа void* */
	unsigned char szd    [2]; /**< размер переменной типа double */
};

/**
 * Группа характеристик описания заголовка буфера вывода статистики
 *
 * @see tvms_ch
 */
struct vms_short {
	unsigned char rank     [SZSH]; /**< ранг матрицы */
	unsigned char maxnlev  [SZSH]; /**< максимальный номер уровня вложенности */
	unsigned char smallbuff[SZSH]; /**< признак того, что во время выполнения не хватило места в файле */
	unsigned char lvers    [SZSH]; /**< длина строки версии dvm, платформы и имени процессора */
};

/**
 * Группа характеристик описания заголовка буфера вывода статистики
 *
 * @see tvms_ch
 */
struct vms_long{
	unsigned char proccount[SZL]; /**< количество процессоров */
	unsigned char mpstype  [SZL]; /**< тип передачи сообщений */
	unsigned char ioproc   [SZL]; /**< номер процессора ввода–вывода */
	unsigned char qfrag    [SZL]; /**< количество интервалов */
	unsigned char lbuf     [SZL]; /**< размер буфера */
	unsigned char linter   [SZL]; /**< длина в байтах записей всех интервалов */
	unsigned char lsynchro [SZL]; /**< длина в байтах записей синхронизационных времен */
};

/**
 * Группа характеристик описания заголовка буфера вывода статистики
 *
 * @see tvms_ch
 */
struct vms_void{
	unsigned char pbuffer[SZV]; /**< начало буфера */
};

/**
 * Группа характеристик описания заголовка буфера вывода статистики
 *
 * @see tvms_ch
 */
struct vms_double{
	unsigned char proctime[SZD]; /**< processor work time */
};

/**
 * Структура описывающая заголовок буфера вывода статистики
 * if change here chahge statread.h
 */
typedef struct tvms_ch {
	struct vms_const  shc; /**< группа параметров константного типа  */
	struct vms_short  sh;  /**< группа параметров типа short  */
	struct vms_long   l;   /**< группа параметров типа long   */
	struct vms_void   v;   /**< группа параметров типа void*  */
	struct vms_double d;   /**< группа параметров типа double */
} *pvms_ch;

// -- Статистические интервалы -----------------------------------------------------------------------------------------

/**
 * Группа характеристик описания статистического интервала
 *
 * @see pinter_ch
 */
struct inter_short {
	unsigned char nlev[SZSH]; /**< номер уровня вложенности */
	unsigned char type[SZSH]; /**< тип интервала */
};

/**
 * Группа характеристик описания статистического интервала
 *
 * @see pinter_ch
 */
struct inter_long {
	unsigned char nline        [SZL]; /**< начало фрагмента (номер строки пользовательской программы) */
	unsigned char nline_end    [SZL]; /**< конец фрагмента (номер строки пользовательской программы) */
	unsigned char valvar       [SZL]; /**< пользовательский идентификатор интервала */
	unsigned char qproc        [SZL]; /**< количество процессоров, на которых выполнялся интервал */
	unsigned char ninter       [SZL]; /**< порядковый номер интервала */
	unsigned char SendCallCount[SZL]; /**< */
	unsigned char RecvCallCount[SZL]; /**< */
};

/**
 * Группа характеристик описания статистического интервала
 *
 * @see pinter_ch
 */
struct inter_void{
	unsigned char up        [SZV]; /**< ссылка на родительский интервал */
	unsigned char next      [SZV]; /**< ссылка на следующего соседа по уровню */
	unsigned char down      [SZV]; /**< ссылка на первый дочерний интервал */
	unsigned char ptimes    [SZV]; /**< ссылка на массив времен */
};

/**
 * Группа характеристик описания статистического интервала
 *
 * @see pinter_ch
 */
struct inter_double{
	unsigned char nenter         [SZD]; /**< число вхождений в интервал */
	unsigned char SendCallTime   [SZD]; /**< */
	unsigned char MinSendCallTime[SZD]; /**< */
	unsigned char MaxSendCallTime[SZD]; /**< */
	unsigned char RecvCallTime   [SZD]; /**< */
	unsigned char MinRecvCallTime[SZD]; /**< */
	unsigned char MaxRecvCallTime[SZD]; /**< */
	unsigned char times          [STM][SZD]; /**< матрица времен */
};

/**
 * Структура описывающая статистические интервалы
 *
 * Элемсенты разделены по подгруппам разделенные по типу.
 * If change here change treeinter.h
 */
typedef struct tinter_ch {
	struct inter_short  sh; /**< группа параметров типа short  */
	struct inter_long   l;  /**< группа параметров типа long   */
	struct inter_void   v;  /**< группа параметров типа void*  */
	struct inter_double d;  /**< группа параметров типа double */
	// --
	unsigned char dvmhStat[DVMH_STAT_INTERVAL_STATIC_SIZE]; /**< статистика dvmh */
} *pinter_ch;

// -- Структура времени рассинхронизации -------------------------------------------------------------------------------

struct syn_short{
	unsigned char nitem[SZSH];
};
struct syn_long{
	unsigned char ninter[SZL];
};
struct syn_void{
	unsigned char pgrp[SZV];
};
struct syn_double{
	unsigned char time[SZD];
};
/* if change here change potensyn.h */
typedef struct tsyn_ch {
       struct syn_short sh;
	   struct syn_long l;
	   struct syn_void v;
	   struct syn_double d;
}*psyn_ch;
#define CPYMEM(to,from)\
 memcpy(&(to),&(from),sizeof(to));
#define CPYMEMC(to,from)\
	smfrom=0;\
        stcond = sizeof(from)>sizeof(to);\
	if (stcond && toright==1) smfrom=sizeof(from)-sizeof(to);\
	memcpy(&(to),(unsigned char *)(&(from))+smfrom,sizeof(to));
#else
#define min(a,b) (((a) <(b)) ? (a):(b))
#define MAKESHORT(p,nm,nmfirst)\
	(&(p->sh.nm)-&(p->sh.nmfirst))*szsh
#define MAKELONG(p,nm,nmfirst,q_short)\
	q_short*szsh+(&(p->l.nm)-&(p->l.nmfirst))*szl
#define MAKEVOID(p,nm,nmfirst,q_short,q_long)\
	q_short*szsh+q_long*szl+(&(p->v.nm)-&(p->v.nmfirst))*szv
#define MAKEDOUBLE(p,nm,nmfirst,q_short,q_long,q_void)\
	q_short*szsh+q_long*szl+q_void*szv+(&(p->d.nm)-&(p->d.nmfirst))*szd
#define MAKEDOUBLEA(p,nm,nmfirst,q_short,q_long,q_void,a)\
	a=q_short*szsh+q_long*szl+q_void*szv+(&(p->d.nm)-&(p->d.nmfirst))*szd;
	
#define CPYMEM(to,pfrom,sz_var)\
{\
	int sz_to;\
	int smfrom=0,smto=0,mmin;\
	sz_to=sizeof(to);\
	mmin=min(sz_to,sz_var);\
	if (sz_to!=sz_var) {\
		if (sz_to>sz_var) {\
			if (toright==1)	smto=sz_to-sz_var;\
		} else {\
			if (toright==1) {\
				if (reverse==1) smfrom=sz_var-sz_to;\
			} else {\
				if (reverse!=1) smfrom=sz_var-sz_to;\
			}\
		}\
	}\
	if (reverse==1) {\
		memcpy((unsigned char *)(&(to))+smto,pfrom+smfrom,mmin);\
	} else {\
		int imcpy;\
		for (imcpy=mmin-1;imcpy>=0;imcpy--) {\
			*((unsigned char*)(&(to))+imcpy+smto)=\
				*(pfrom+smfrom+mmin-1-imcpy);\
		}\
	}\
}
#endif
#endif
