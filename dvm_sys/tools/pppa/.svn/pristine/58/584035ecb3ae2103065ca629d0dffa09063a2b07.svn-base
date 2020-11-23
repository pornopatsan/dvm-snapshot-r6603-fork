#ifndef _INTER_H
#define _INTER_H

#include "sysstat.h"
#include "strall.h"
#include "dvmh_stat.h"

enum typegrp {
	COM,
	RCOM,
	SYN,
	VAR,
	OVERLAP,
	CALL
};

enum typetimeim {
	CALLSMT,
	LOSTMT,
	PRODMT
};

enum typetime {
	LOST,
	INSUFUSR,
	INSUF,
	IDLE,
	SUMCOM,
	SUMRCOM,
	SUMSYN,
	SUMVAR,
	SUMOVERLAP,
	IMB,
	EXEC,
	CPUUSR,
	CPU,
	IOTIME,
	START,
	DVMH_THREADS_USER_TIME,
	DVMH_THREADS_SYSTEM_TIME,
	DVMH_GPU_TIME_PRODUCTIVE, 
	DVMH_GPU_TIME_LOST, 
	PROC,
	ITER
};

enum typecom {
	IO,
	RD,
	SH,
	RA,
	RED
};
//5 collective operation. new operation insert before RED
//if insert new time don't forget insert text in the statread.h
//don't insert new time between SUMCOM...SUMOVERLAP


/** Описатель интервала */
typedef struct tident {
        typefrag      t;          // тип интервала
		short         nlev;       // номер уровня, вложенности
        char         *pname;      // имя исходного файла, где задан интервал
        long          expr;       // значение выражения.
        unsigned long nline;      // номер строки исходного файла
        unsigned long nline_end;  // ???
        unsigned long proc;       // количество процессоров, на которых выполнялся интервал
        double        nenter;     // число вхождений в интервал        
} ident;


typedef struct	{	
		double	SendCallTime;
		double	MinSendCallTime;
		double	MaxSendCallTime;
		long	SendCallCount;
		double	RecvCallTime;
		double	MinRecvCallTime;
		double	MaxRecvCallTime;
		long	RecvCallCount;
} s_SendRecvTimes;


class CInter {
	public:
		/**
		 * Конструктор интервала
		 *
	     * @param pt   				указатель на массив времен, переписанный из файла
	     * @param ps 				??? 
	     * @param id   				индентификатор интервала
	     * @param nint 				номер интервала
	     * @param iIM 				??? (для отладки)
	     * @param jIM 				??? (для отладки)
	     * @param sore 				??? (для отладки)
	     * @param dvmhStatInterval  указатель на DVMH-статистику по интервалу
		 */
		CInter(
			s_GRPTIMES          (*pt)[StatGrpCount],
			s_SendRecvTimes     ps,
			ident               id,
			unsigned long       nint,
			int 				iIM,
			int 				jIM,
			short               sore, 
			dvmh_stat_interval *dvmhStatInterval
		);

		~CInter(void);

		/**  
		 * Эти функции-члены добавляют к ранее посчитанным или записывают новые значения временных 
		 * характеристик. Первая функция AddTime и WriteTime предназначены для работы с массивом mgen, 
		 * первый параметр - это номер индекса, а второй само значение. Вторая функция AddTime 
		 * предназначена для работы с остальными массивами, параметр t1 служит для выбора массива, 
		 * параметр t2 – значение индекса массива, а val - значение. 
		 */
		void AddTime(typetime t2, double val);
		void WriteTime(typetime t2, double val);
		void AddTime(typegrp t1, typecom t2, double val);

        /** 
         * Эти функции-члены читают значения временных характеристик, значения их параметров такие же, 
         * как и для записи, только последний параметр передается ссылкой.
         */
		void ReadTime(typetime t2,double &val);				
		void ReadTime(typegrp t1,typecom t2,double &val);
		void ReadTime(typetimeim t1,int t2,double &val);
		
		/**
		 * Сравнивает идентификатор интервала с другого процессора с идентификатором текущего интервала, 
		 * параметр р - указатель на идентификатор интервала. В случае совпадения идентификаторов по всем 
		 * элементам структуры возвращает 1, в противном случае – 0.
		 */
		int CompIdent(ident *p);
		
		/**
		 * Устанавливает указатель равным адресу идентификатора интервала.
		 */
		void ReadIdent(ident **p);
		
		/** 
		 * Эта функция-член суммирует значения временных характеристик интервала со значениями интервала 
		 * более высокого уровня. Параметр р – указатель на интервал более высокого уровня.
		 */
		void SumInter(CInter *p);

		// -- Открытые параметры 
		unsigned long ninter;                 // номер интервала	
		dvmh_stat_interval *dvmhStatInterval; // DVMH статитстика

	private:
		ident  idint;  // описатель интервала

		double mgen    [ITER + 1]; // массив времен, для выдачи характеристик по процессорам

		double mcom    [RED + 1];  // массив времен передачи сообщений в коллективных операциях
		double mrcom   [RED + 1];  // массив времен реальной рассинхронизации
		double msyn    [RED + 1];  // массив времен рассинхронизации
		double mvar    [RED + 1];  // массив разброса времен
		double moverlap[RED + 1];  // массив времен перекрытия операций
		double mcall   [RED + 1];  // количество вызовов коллективных операций
		
		double lost[StatGrpCount];
		double prod[StatGrpCount];
		double calls[StatGrpCount];

		double	SendCallTime;
		double	MinSendCallTime;
		double	MaxSendCallTime;
		long	SendCallCount;
		double	RecvCallTime;
		double	MinRecvCallTime;
		double	MaxRecvCallTime;
		long	RecvCallCount;		
};

#endif
