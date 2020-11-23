#include "Interval.h"
#include "Vm.h"

double	*procElapsedTime;	// processor's elapsed times vector
int		rootProcCount;		// number of processors in root VM

Processor::Processor() :
     Lost_time(0.0),
     Insuff_parallelism(0.0),
     Insuff_parallelism_usr(0.0),
     Insuff_parallelism_sys(0.0),
     Idle(0.0),
     Communication(0.0),
     Synchronization(0.0),
     Real_synchronization(0.0),
	 Variation(0.0),
     Overlap(0.0),
     Load_imbalance(0.0),
     Execution_time(0.0),
     CPU_time(0.0),
     CPU_time_usr(0.0),
     CPU_time_sys(0.0),
     IO_time(0.0),

     IO_comm(0.0),
     IO_real_synch(0.0),
     IO_synch(0.0),
     IO_vary(0.0),
     IO_overlap(0.0),

     Wait_reduction(0.0),
     Reduction_real_synch(0.0),
     Reduction_synch(0.0),
     Reduction_vary(0.0),
     Reduction_overlap(0.0),

     Wait_shadow(0.0),
     Shadow_real_synch(0.0),
     Shadow_synch(0.0),
     Shadow_vary(0.0),
     Shadow_overlap(0.0),

     Remote_access(0.0),
     Remote_real_synch(0.0),
     Remote_synch(0.0),
     Remote_vary(0.0),
     Remote_overlap(0.0),

     Redistribution(0.0),
     Redistribution_real_synch(0.0),
     Redistribution_synch(0.0),
     Redistribution_vary(0.0),
     Redistribution_overlap(0.0)
{
}

void MPSSynchronize(TimeType InfoType)
{ 
	double max_time=0;
    int i;

    for (i = 0; i < MPSProcCount(); i++)
		if (procElapsedTime[currentVM->map(i)] > max_time) 
			max_time = procElapsedTime[currentVM->map(i)];

	for (i = 0; i < MPSProcCount(); i++) {
		AddTimeSynchronize(InfoType, currentVM->map(i), 
			(max_time - procElapsedTime[currentVM->map(i)]));
		AddTime(InfoType, currentVM->map(i), 
			(max_time - procElapsedTime[currentVM->map(i)]));
	}
}

