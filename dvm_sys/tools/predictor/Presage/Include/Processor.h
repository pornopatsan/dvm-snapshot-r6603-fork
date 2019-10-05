#ifndef _PROCESSOR_H
#define _PROCESSOR_H

enum TimeType {
	  __IO_time = 1,
	  __CPU_time,
	  __CPU_time_sys,
	  __CPU_time_usr,
	  __Wait_reduct,
	  __Wait_shadow,
	  __Remote_access,
	  __Remote_overlap,
	  __Redistribute,
	  __IO_comm,
	  __Other_comm,
	  __Insuff_parall_sys,
	  __Insuff_parall_usr,
	  __Synchronize,
		__Variation,
	  __Reduct_overlap,
	  __Shadow_overlap,
};


class Processor {

	friend class Interval;
public:
	double
    Lost_time,
    Insuff_parallelism,			// Insuff_parallelism_usr + Insuff_parallelism_sys
    Insuff_parallelism_usr,		// User insufficient parallelism
    Insuff_parallelism_sys,		// System Insufficient parallelism 
    Idle,						// Idle time
    Communication,				// Communications
    Synchronization,			// Synchronization
    Real_synchronization,		// Synchronization
	Variation,					// Time  variation
    Overlap,					// Overlap
    Load_imbalance,				// Load imbalance
    Execution_time,				// Execution time
	CPU_time,					// CPU_time_usr + CPU_time_sys
    CPU_time_usr,				// Usefull processor time			
    CPU_time_sys,				// Usefull system time			
    IO_time,

    IO_comm,					// IO: Communications
    IO_real_synch,					// IO: Real synch
    IO_synch,					// IO: Synchronization
    IO_vary,					// IO: Time variation
    IO_overlap,					// IO: Overlap

    Wait_reduction,				// Reduction: Communications
		Reduction_real_synch,				// Reduction: Real synch
		Reduction_synch,					// Reduction synchronization
		Reduction_vary,					// Time variation
		Reduction_overlap,				// Reduction: Overlap

		Wait_shadow,				// Shadow: Communications
		Shadow_real_synch,				// Shadow: Real synch
		Shadow_synch,					// Shadow synchronization
		Shadow_vary,					// Time variation
		Shadow_overlap,				// Shadow: Overlap

    Remote_access,				// Remote access: Communications
    Remote_real_synch,				// Remote access: Real synch
		Remote_vary,				// Remote access: Time variation
		Remote_synch,				// Remote access: synchronization
		Remote_overlap,				// Remote access: Overlap

    Redistribution,				// Redistribution: Communications
    Redistribution_real_synch,		// Redistribution: Real synch
    Redistribution_synch,		// Redistribution: synchronization
    Redistribution_vary,		// Redistribution: time vary
    Redistribution_overlap;		// Redistribution: Overlap
		

public:

	Processor();
	~Processor() {}

};

extern void		MPSSynchronize(TimeType InfoType);
extern double *	procElapsedTime;	// processors elapsed times vector
extern int		rootProcCount;		// number of processors in root VM

inline double CurrProcTime(int proc_no) { return procElapsedTime[proc_no]; }

#endif 
