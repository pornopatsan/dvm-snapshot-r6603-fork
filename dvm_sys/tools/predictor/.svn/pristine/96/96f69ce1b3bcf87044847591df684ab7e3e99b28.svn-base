#ifndef _INTERVAL_H
#define _INTERVAL_H

#include <limits.h>

#include <fstream>

#include "FuncCall.h"
#include "Processor.h"

enum IntervalType {
	  __IT_MAIN = 0,
	  __IT_SEQ,
	  __IT_PAR,
	  __IT_USER
}; 

#define NO_EXPR 2000000000 

enum PredType{
		_Lost_time,
		_Insuff_parallelism,
    _Insuff_parallelism_usr,		// User insufficient parallelism
    _Insuff_parallelism_sys,		// System Insufficient parallelism 
    _Idle,						// Idle time
    _Communication,				// Communications
    _Synchronization,			// Synchronization
    _Real_synchronization,		// Synchronization
		_Variation,					// Time  variation
    _Overlap,					// Overlap
    _Load_imbalance,				// Load imbalance
		_Execution_time,				// Execution time
		_CPU_time,					// CPU_time_usr + CPU_time_sys
    _CPU_time_usr,				// Usefull processor time			
    _CPU_time_sys,				// Usefull system time			
    _IO_time,

    _IO_comm,					// IO: Communications
    _IO_real_synch,					// IO: Real synch
    _IO_synch,					// IO: Synchronization
    _IO_vary,					// IO: Time variation
    _IO_overlap,					// IO: Overlap

    _Wait_reduction,				// Reduction: Communications
		_Reduction_real_synch,				// Reduction: Real synch
		_Reduction_synch,					// Reduction synchronization
		_Reduction_vary,					// Time variation
		_Reduction_overlap,				// Reduction: Overlap

		_Wait_shadow,				// Shadow: Communications
		_Shadow_real_synch,				// Shadow: Real synch
		_Shadow_synch,					// Shadow synchronization
		_Shadow_vary,					// Time variation
		_Shadow_overlap,				// Shadow: Overlap

    _Remote_access,				// Remote access: Communications
    _Remote_real_synch,				// Remote access: Real synch
		_Remote_vary,				// Remote access: Time variation
		_Remote_synch,				// Remote access: synchronization
		_Remote_overlap,				// Remote access: Overlap

    _Redistribution,				// Redistribution: Communications
    _Redistribution_real_synch,		// Redistribution: Real synch
    _Redistribution_synch,		// Redistribution: synchronization
    _Redistribution_vary,		// Redistribution: time vary
    _Redistribution_overlap,		// Redistribution: Overlap
};


class Interval : public Processor {


	static int		Intervallevel;			// current interval level
	static int		IntervalID;				// current interval ID

    IntervalType	type;					// Interval type
	long			index;
    int				level;					// Interval level
    int				EXE_count;
    int				source_line;
    char *			source_file;
	int				ID;


	//for intelval's tree
    Interval	*	parent_interval;
	int				count;
    Interval	**	nested_intervals;

    Processor	**	Procs;	// processor's vector

    double			Total_time;
    double			Efficiency;
    double			Productive_time;
    double			Productive_CPU_time;
    double			Productive_SYS_time;

public:

	bool			io_trafic;		// start FORTRAN I/O
	int				num_op_io;
	int				num_op_reduct;
	int				num_op_shadow;
	int				num_op_remote;
	int				num_op_redist;
	char			*html_title;
	
	Interval(int arg); // только чтобы отличаться от конструктора по умолчанию
	Interval(int iline = TraceLine::first_line_number, 
				char * ifile = TraceLine::first_file_name, 
					IntervalType itype = __IT_MAIN,
						long index = NO_EXPR,
							Interval * parent_interval = NULL);

	~Interval();

	void		AddTime(TimeType InfoType, int proc_no, double TimeDelta);

	void		AddMPSTime(TimeType InfoType, double TimeDelta);
	//grig
	void        AddMPSTime(TimeType InfoType, std::vector<double> vTimeDelta);
	double      GetEffectiveParameter(); // получить показатель по которому сраниваются характеристики
	//\grig
	void		AddTimeSynchronize(TimeType InfoType, int proc_no, double TimeDelta);
	void		AddTimeVariation(TimeType InfoType, int proc_no, double TimeDelta);
	void		CalcIdleAndImbalance();
	static void Enter(IntervalType int_type, int line, char* file, long index);
	static void Leave();
	void		Integrate();
	void		SaveInFile(std::ofstream& hfile, int up, int next, int pred);
	void		SaveTree(std::ofstream&	hfile, int up, int next, int pred);
	void		setIOTrafic()  { io_trafic = true; }
	void		resetIOTrafic()  { io_trafic = false; }

	//====
	int copy(Interval* from);
	int copy_poss(Interval* from, double p1, double p2);
	double GetProcPred(int proc_no, PredType pred);

	//=***

	friend void CreateHTMLfile();
};

extern Interval * CurrInterval;			// pointer to current interval

inline	void AddTime(TimeType InfoType, int proc_no, double TimeDelta)
	{ CurrInterval->AddTime(InfoType, proc_no, TimeDelta); }

inline	void AddMPSTime(TimeType InfoType, double TimeDelta)
	{ CurrInterval->AddMPSTime(InfoType, TimeDelta); }

//grig
inline	void AddMPSTime(TimeType InfoType, std::vector<double> vTimeDelta)
	{ CurrInterval->AddMPSTime(InfoType, vTimeDelta); }
//\grig

inline	void AddTimeSynchronize(TimeType InfoType, int proc_no, double TimeDelta)
	{ CurrInterval->AddTimeSynchronize(InfoType, proc_no, TimeDelta); }

inline	void AddTimeVariation(TimeType InfoType, int proc_no, double TimeDelta)
	{ CurrInterval->AddTimeVariation(InfoType, proc_no, TimeDelta); }

#endif 
