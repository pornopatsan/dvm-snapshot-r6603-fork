#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <stdlib.h>

#include <fstream>
#include <float.h>
#include <vector>

#include "Interval.h"
#include "Vm.h"
#include "Ver.h"

#ifdef _UNIX_
#define _strdup strdup
#include "ParseString.h"
#endif

using namespace std;

extern ofstream prot;

extern int			rootProcCount;		// number of processors in root VM
extern double	*	procElapsedTime;	// processor's elapsed times vector
extern VM		*	currentVM;			// pointer to current VM
extern char		*	interval[];			// interval's template

Interval *	CurrInterval			= NULL;	// pointer to current interval
int			Interval::Intervallevel = 0;	// current interval's level
int			Interval::IntervalID	= 0;	// last interval's ID


Interval::Interval(int iline, char * ifile, IntervalType itype, long iindex, Interval * par_int) :
	type(itype),
	index(iindex),
    level(Intervallevel),
    EXE_count(1),
    source_line(iline),
    source_file(strdup(ifile)),
	io_trafic(false),
	ID(++IntervalID),
	num_op_io(0),
	num_op_reduct(0),
	num_op_shadow(0),
	num_op_remote(0),
	num_op_redist(0),
    Total_time(0.0),
    Efficiency(0.0),
    Productive_time(0.0),
    Productive_CPU_time(0.0),
    Productive_SYS_time(0.0),
    parent_interval(par_int),
	count(0),
	html_title(NULL),
    nested_intervals(NULL)
{
	int	i;

	// create prosessor's vector
	Procs = new Processor*[rootProcCount];
	for (i = 0; i < rootProcCount; i++) 
		Procs[i] = new Processor();

	// link with parent
	if (parent_interval != NULL) {
		parent_interval->count++;
		parent_interval->nested_intervals = 
			(Interval**) realloc(parent_interval->nested_intervals, 
				parent_interval->count * sizeof(Interval*));
		parent_interval->nested_intervals[parent_interval->count -1] = this;
	}
}

Interval::Interval(int arg) :
	type(__IT_MAIN),
	index(NO_EXPR),
    level(arg),
    EXE_count(1),
    source_line(0),
	io_trafic(false),
	ID(++IntervalID),
	num_op_io(0),
	num_op_reduct(0),
	num_op_shadow(0),
	num_op_remote(0),
	num_op_redist(0),
    Total_time(0.0),
    Efficiency(0.0),
    Productive_time(0.0),
    Productive_CPU_time(0.0),
    Productive_SYS_time(0.0),
    parent_interval(NULL),
	count(0),
	html_title(NULL),
    nested_intervals(NULL)
{
	int	i;

	source_file=(char*)malloc(sizeof(char)*strlen("no file"));
	strcpy(source_file,"no file");

	// create prosessor's vector
	Procs = new Processor*[rootProcCount];
	for (i = 0; i < rootProcCount; i++) 
		Procs[i] = new Processor();

}


Interval::~Interval()
{
	int i;

	delete source_file;

	for (i = 0; i < rootProcCount; i++) 
		delete Procs[i];
	delete Procs;

	for (i = 0; i < count; i++)
		delete nested_intervals[i];

	delete nested_intervals;
}

int Interval::copy_poss(Interval* from, double p1, double p2)
{	long i;
	for(i=0; i<MPSProcCount(); i++)
	{
		Procs[i]->Lost_time              = Procs[i]->Lost_time * p1              + from->Procs[i]->Lost_time * p2;
		Procs[i]->Execution_time         = Procs[i]->Execution_time * p1         + from->Procs[i]->Execution_time * p2;
		Procs[i]->Insuff_parallelism_usr = Procs[i]->Insuff_parallelism_usr * p1 + from->Procs[i]->Insuff_parallelism_usr * p2;
		Procs[i]->Insuff_parallelism_sys = Procs[i]->Insuff_parallelism_sys * p1 + from->Procs[i]->Insuff_parallelism_sys * p2;
		Procs[i]->Communication	         = Procs[i]->Communication * p1          + from->Procs[i]->Communication * p2;
		Procs[i]->Idle         	         = Procs[i]->Idle * p1                   + from->Procs[i]->Idle * p2;
	}
	return 0;
}


int Interval::copy(Interval* from)
{	copy_poss(from,0,1);
	return 0;
}

double Interval::GetProcPred(int proc_no, PredType pred)
{
	switch (pred)
	{
		case _Lost_time: return Procs[proc_no]->Lost_time;
		case _Insuff_parallelism: return Procs[proc_no]->Insuff_parallelism;
		case _Insuff_parallelism_sys: return Procs[proc_no]->Insuff_parallelism_sys;
		case _Idle: return Procs[proc_no]->Idle;
		case _Communication: return Procs[proc_no]->Communication;
		case _Synchronization: return Procs[proc_no]->Synchronization;
		case _Real_synchronization: return Procs[proc_no]->Real_synchronization;
		case _Variation: return Procs[proc_no]->Variation;
		case _Overlap: return Procs[proc_no]->Overlap;
		case _Load_imbalance: return Procs[proc_no]->Load_imbalance;
		case _Execution_time: return Procs[proc_no]->Execution_time;
		case _CPU_time: return Procs[proc_no]->CPU_time;
		case _CPU_time_usr: return Procs[proc_no]->CPU_time_usr;
		case _CPU_time_sys: return Procs[proc_no]->CPU_time_sys;
		case _IO_time: return Procs[proc_no]->IO_time;
		case _IO_comm: return Procs[proc_no]->IO_comm;
		case _IO_real_synch: return Procs[proc_no]->IO_real_synch;
		case _IO_synch: return Procs[proc_no]->IO_synch;
		case _IO_vary: return Procs[proc_no]->IO_vary;
		case _IO_overlap: return Procs[proc_no]->IO_overlap;
		case _Wait_reduction: return Procs[proc_no]->Wait_reduction;
		case _Reduction_real_synch: return Procs[proc_no]->Reduction_real_synch;
	}

	return 0.0;

}

void Interval::AddTime(TimeType InfoType, int proc_no, double TimeDelta)
{ int i ;
	if (io_trafic) {
//		prot << "proc_no = " << proc_no << ", TimeDelta = " << TimeDelta 
//			<< ", Execution_time = " << Procs[proc_no]->Execution_time << endl; 
		if ((proc_no == 0) && (InfoType != __IO_comm)) {
			InfoType = __IO_time;
		} else if (InfoType != __IO_comm){
			return;
		}
	}
//	printf("ADD TIME [%d] += %f\n",proc_no,TimeDelta);;

//	printf("Exec[%d] %f\n",proc_no,Procs[proc_no]->Execution_time);

	switch (InfoType) {
		case  __IO_time :
          Procs[proc_no]->IO_time+=TimeDelta;
          Procs[proc_no]->Execution_time+=TimeDelta;
          procElapsedTime[proc_no] += TimeDelta;
          break;
        case  __CPU_time_sys :
          Procs[proc_no]->CPU_time_sys+=TimeDelta;
          Procs[proc_no]->CPU_time+=TimeDelta;
          Procs[proc_no]->Execution_time+=TimeDelta;
          procElapsedTime[proc_no] += TimeDelta;
          break;
        case  __CPU_time_usr :
          Procs[proc_no]->CPU_time_usr+=TimeDelta;
          Procs[proc_no]->CPU_time+=TimeDelta;
          Procs[proc_no]->Execution_time+=TimeDelta;
          procElapsedTime[proc_no] += TimeDelta;
          break;
        case  __Wait_reduct :
          Procs[proc_no]->Wait_reduction+=TimeDelta;
          Procs[proc_no]->Communication+=TimeDelta;
          Procs[proc_no]->Lost_time+=TimeDelta;
          Procs[proc_no]->Execution_time+=TimeDelta;
          procElapsedTime[proc_no] += TimeDelta;
//			    for (i = 0; i < MPSProcCount(); i++)	printf("proc[%d].Comm.reduct=%f\n",i,Procs[currentVM->map(i)]->Wait_reduction); printf("\n");
          break;
        case  __Wait_shadow :   
          Procs[proc_no]->Wait_shadow+=TimeDelta;
          Procs[proc_no]->Communication+=TimeDelta;
          Procs[proc_no]->Lost_time+=TimeDelta;
          Procs[proc_no]->Execution_time+=TimeDelta;
          procElapsedTime[proc_no] += TimeDelta;
//			    for (i = 0; i < MPSProcCount(); i++)	printf("proc[%d].Comm.shad=%f\n",i,Procs[currentVM->map(i)]->Wait_shadow); printf("\n");
          break;
        case  __Remote_access :  
          Procs[proc_no]->Remote_access+=TimeDelta;
          Procs[proc_no]->Communication+=TimeDelta;
          Procs[proc_no]->Lost_time+=TimeDelta;
          Procs[proc_no]->Execution_time+=TimeDelta;
          procElapsedTime[proc_no] += TimeDelta;
//			    for (i = 0; i < MPSProcCount(); i++)	printf("proc[%d].Comm.Remote=%f\n",i,Procs[currentVM->map(i)]->Remote_access);

          break;
        case  __Redistribute :  
          Procs[proc_no]->Redistribution+=TimeDelta;
          Procs[proc_no]->Communication+=TimeDelta;
          Procs[proc_no]->Lost_time+=TimeDelta;
          Procs[proc_no]->Execution_time+=TimeDelta;
          procElapsedTime[proc_no] += TimeDelta;
          break;
        case  __IO_comm :  
          Procs[proc_no]->IO_comm+=TimeDelta;
          Procs[proc_no]->Communication+=TimeDelta;
          Procs[proc_no]->Lost_time+=TimeDelta;
          Procs[proc_no]->Execution_time+=TimeDelta;
          procElapsedTime[proc_no] += TimeDelta;
          break;
        case  __Insuff_parall_sys :  
          Procs[proc_no]->Insuff_parallelism_sys+=TimeDelta;
          Procs[proc_no]->Insuff_parallelism+=TimeDelta;
          Procs[proc_no]->Lost_time+=TimeDelta;

          break;
        case  __Insuff_parall_usr :  
          Procs[proc_no]->Insuff_parallelism_usr+=TimeDelta;
          Procs[proc_no]->Insuff_parallelism+=TimeDelta;
          Procs[proc_no]->Lost_time+=TimeDelta;
          break;
        case  __Synchronize :
//					printf("synch %f %f\n",procElapsedTime[proc_no],TimeDelta);
					procElapsedTime[proc_no]=(procElapsedTime[proc_no]>TimeDelta)?procElapsedTime[proc_no]:TimeDelta;
					Procs[proc_no]->Execution_time=procElapsedTime[proc_no];
          break;
        case  __Variation :
//					printf("vary %f %f\n",procElapsedTime[proc_no],TimeDelta);
//					procElapsedTime[proc_no]=(procElapsedTime[proc_no]>TimeDelta)?procElapsedTime[proc_no]:TimeDelta;
					Procs[proc_no]->Variation+=TimeDelta;
          break;
        case  __Remote_overlap :
          Procs[proc_no]->Remote_overlap+=TimeDelta;
          Procs[proc_no]->Overlap+=TimeDelta;
          break;
        case  __Reduct_overlap :
          Procs[proc_no]->Reduction_overlap+=TimeDelta;
          Procs[proc_no]->Overlap+=TimeDelta;
          break;
        case  __Shadow_overlap :
          Procs[proc_no]->Shadow_overlap+=TimeDelta;
          Procs[proc_no]->Overlap+=TimeDelta;
          break;
		default:
		  prot << "Interval::AddTime - unknown time type " << InfoType << endl;
		  exit(EXIT_FAILURE);
	}

//	printf("Procs[%d]->Execution_time=%f\n",proc_no,Procs[proc_no]->Execution_time);
}

void Interval::AddTimeVariation(TimeType InfoType, int proc_no, double TimeDelta)
{
	Procs[proc_no]->Variation+=TimeDelta;

	switch (InfoType) {
		case __Remote_access:
			Procs[proc_no]->Remote_vary += TimeDelta;
			break;
		case __Redistribute:
			Procs[proc_no]->Redistribution_vary += TimeDelta;
			break;
		case __Wait_reduct:
			Procs[proc_no]->Reduction_vary += TimeDelta;
			break;
		case __Wait_shadow:
			Procs[proc_no]->Shadow_vary += TimeDelta;
			break;
		case __IO_comm:
			Procs[proc_no]->IO_vary += TimeDelta;
			break;
	}
}

void Interval::AddTimeSynchronize(TimeType InfoType, int proc_no, double TimeDelta)
{
	Procs[proc_no]->Synchronization+=TimeDelta;
	//====
	if(InfoType!=__Synchronize && InfoType!=__Wait_reduct) 
	{
//		Procs[proc_no]->Real_synchronization+=TimeDelta;
	}
	//=***

	switch (InfoType) {
		case __Remote_access:
			Procs[proc_no]->Remote_synch += TimeDelta;
			break;
		case __Redistribute:
			Procs[proc_no]->Redistribution_synch += TimeDelta;
			break;
		case __Wait_reduct:
//			printf("Red synch %d %f\n",proc_no,TimeDelta);
			Procs[proc_no]->Reduction_synch += TimeDelta;
			break;
		case __Wait_shadow:
			Procs[proc_no]->Shadow_synch += TimeDelta;
			break;
		case __IO_comm:
			Procs[proc_no]->IO_synch += TimeDelta;
			break;

	}
}

void Interval::AddMPSTime(TimeType InfoType, double TimeDelta)
{ 
	int i;
//	printf("Time += %.8f\n",TimeDelta);
	for (i=0; i < MPSProcCount(); i++)
		AddTime(InfoType, currentVM->map(i), TimeDelta);

}

//grig
void        Interval::AddMPSTime(TimeType InfoType, std::vector<double> vTimeDelta)
{
	int i;
	double temp;
	long int_proc;

	for (i=0; i < MPSProcCount(); i++)
	{

		int_proc=currentVM->map(i);
		temp=vTimeDelta[i];
		AddTime(InfoType,currentVM->map(i),vTimeDelta[i]);
	}
}
//\grig






void Interval::CalcIdleAndImbalance()
{ 
	int i;  
    double	max_ExTime=0, 
			max_CPUTimeSys=0,
 			max_CPUTimeUsr=0,
			max_CPUTime=0;

//    for (i = 0; i < MPSProcCount(); i++)	printf("proc[%d].Comm.shad=%f\n",i,Procs[currentVM->map(i)]->Wait_shadow);
//    for (i = 0; i < MPSProcCount(); i++)	printf("proc[%d].Comm.Remote=%f\n",i,Procs[currentVM->map(i)]->Remote_access);



    for (i = 0; i < MPSProcCount(); i++) {

		if (Procs[currentVM->map(i)]->Execution_time > max_ExTime) 
			max_ExTime = Procs[currentVM->map(i)]->Execution_time;

        if (Procs[currentVM->map(i)]->CPU_time_sys > max_CPUTimeSys) 
			max_CPUTimeSys = Procs[currentVM->map(i)]->CPU_time_sys;

        if (Procs[currentVM->map(i)]->CPU_time_usr > max_CPUTimeUsr) 
			max_CPUTimeUsr = Procs[currentVM->map(i)]->CPU_time_usr;
//grig
		if (Procs[currentVM->map(i)]->CPU_time > max_CPUTime) 
			max_CPUTime = Procs[currentVM->map(i)]->CPU_time;
//\grig

	}

    for(i = 0; i < MPSProcCount(); i++) {
		Procs[currentVM->map(i)]->Idle = 
			max_ExTime - Procs[currentVM->map(i)]->Execution_time;
        //Procs[currentVM->map(i)]->Load_imbalance = 
		//	max_CPUTimeSys  - Procs[currentVM->map(i)]->CPU_time_sys;

		//grig
		if(max_CPUTime!=max_CPUTimeSys+max_CPUTimeUsr)
		{

		}
//		printf("max_sys=%f max_usr=%f sys=%f usr=%f\n",max_CPUTimeSys,max_CPUTimeUsr,Procs[currentVM->map(i)]->CPU_time_sys,Procs[currentVM->map(i)]->CPU_time_usr);
		Procs[currentVM->map(i)]->Load_imbalance = 
			max_CPUTimeSys + max_CPUTimeUsr - Procs[currentVM->map(i)]->CPU_time_sys- Procs[currentVM->map(i)]->CPU_time_usr;

		//\grig
	}
}

void Interval::Enter(IntervalType int_type, int line, char* file, long index)
{
	int i;
	Intervallevel++;

    // Searching for interval
    for (i=0; i < CurrInterval->count; i++)
      if ((CurrInterval->nested_intervals[i]->type == int_type) &&
			(CurrInterval->nested_intervals[i]->source_line == line) &&
				(strcmp(CurrInterval->nested_intervals[i]->source_file, file) == 0) &&
					(CurrInterval->nested_intervals[i]->index == index) )
        break;

	if (i >= CurrInterval->count) {
		// Interval not found - create new interval and change current interval
		CurrInterval = new Interval(line, file, int_type, index, CurrInterval);
	} else {
		CurrInterval = CurrInterval->nested_intervals[i];
		CurrInterval->EXE_count++;
	}

}

void Interval::Leave()
{
	CurrInterval->CalcIdleAndImbalance();
	CurrInterval = CurrInterval->parent_interval;
	Intervallevel--;
}

//grig
double      Interval::GetEffectiveParameter()
{
  return this->Execution_time;
}

//\grig


void Interval::Integrate()
{ 
	int		i;
    double	max_ExTime=0;


    // intagrate this interval
    for(i = 0; i < rootProcCount; i++) {

		IO_time				+=	Procs[i]->IO_time;
        CPU_time			+=	Procs[i]->CPU_time;
        CPU_time_sys		+=	Procs[i]->CPU_time_sys;
        CPU_time_usr		+=	Procs[i]->CPU_time_usr;
        Lost_time			+=	Procs[i]->Lost_time;
        Communication		+=	Procs[i]->Communication;

        IO_comm				+=	Procs[i]->IO_comm;
        IO_real_synch	+=	Procs[i]->IO_real_synch;
        IO_synch			+=	Procs[i]->IO_synch;
        IO_vary				+=	Procs[i]->IO_vary;
        IO_overlap		+=	Procs[i]->IO_overlap;

        Wait_reduction		+=	Procs[i]->Wait_reduction;
//		prot << "Wait_reduction=" << Wait_reduction << endl; 
        Reduction_real_synch		+=	Procs[i]->Reduction_real_synch;
        Reduction_synch		+=	Procs[i]->Reduction_synch;
        Reduction_vary		+=	Procs[i]->Reduction_vary;
        Reduction_overlap	+=	Procs[i]->Reduction_overlap;
//		prot << "Reduction_overlap=" << Reduction_overlap << endl; 

        Wait_shadow			+=	Procs[i]->Wait_shadow;
//		prot << "Wait_shadow=" << Wait_shadow << endl; 
        Shadow_real_synch		+=	Procs[i]->Shadow_real_synch;
        Shadow_synch		+=	Procs[i]->Shadow_synch;
        Shadow_vary		+=	Procs[i]->Shadow_vary;
				Shadow_overlap		+=	Procs[i]->Shadow_overlap;
//		prot << "Shadow_overlap=" << Shadow_overlap << endl; 

        Remote_access		+=	Procs[i]->Remote_access;
        Remote_real_synch		+=	Procs[i]->Remote_real_synch;
        Remote_synch		+=	Procs[i]->Remote_synch;
        Remote_vary		+=	Procs[i]->Remote_vary;
        Remote_overlap		+=	Procs[i]->Remote_overlap;

        Redistribution		+=	Procs[i]->Redistribution;
        Redistribution_real_synch+=	Procs[i]->Redistribution_real_synch;
        Redistribution_synch+=	Procs[i]->Redistribution_synch;
        Redistribution_vary+=	Procs[i]->Redistribution_vary;
        Redistribution_overlap+=Procs[i]->Redistribution_overlap;

        Insuff_parallelism	+=	Procs[i]->Insuff_parallelism;
        Insuff_parallelism_sys+=Procs[i]->Insuff_parallelism_sys;
        Insuff_parallelism_usr+=Procs[i]->Insuff_parallelism_usr;
        Synchronization		+=	Procs[i]->Synchronization;
				Variation					+= Procs[i]->Variation;
		Real_synchronization+=	Procs[i]->Real_synchronization;
//		Communication_SYNCH	+=	Procs[i]->Communication_SYNCH;
        Idle				+=	Procs[i]->Idle;
        Load_imbalance		+=	Procs[i]->Load_imbalance;
        Overlap				+=	Procs[i]->Overlap;

        if (max_ExTime < Procs[i]->Execution_time)
			max_ExTime = Procs[i]->Execution_time;
	}

    Lost_time			   += Idle;

    Execution_time		= max_ExTime;

	// Integrate nested intervals

    for (i = 0; i < count; i++) {

		nested_intervals[i]->Integrate();

		num_op_io				+= nested_intervals[i]->num_op_io;
		num_op_reduct			+= nested_intervals[i]->num_op_reduct;
		num_op_shadow			+= nested_intervals[i]->num_op_shadow;
		num_op_remote			+= nested_intervals[i]->num_op_remote;
		num_op_redist			+= nested_intervals[i]->num_op_redist;

		Execution_time			+= nested_intervals[i]->Execution_time;
		IO_time					+= nested_intervals[i]->IO_time;
		CPU_time				+= nested_intervals[i]->CPU_time;
		CPU_time_sys			+= nested_intervals[i]->CPU_time_sys;
		CPU_time_usr			+= nested_intervals[i]->CPU_time_usr;
		Lost_time				+= nested_intervals[i]->Lost_time;
		Communication			+= nested_intervals[i]->Communication;

		IO_comm					+= nested_intervals[i]->IO_comm;
		IO_real_synch				+= nested_intervals[i]->IO_real_synch;
		IO_synch				+= nested_intervals[i]->IO_synch;
		IO_vary				+= nested_intervals[i]->IO_vary;
		IO_overlap				+= nested_intervals[i]->IO_overlap;

		Wait_reduction			+= nested_intervals[i]->Wait_reduction;
		Reduction_synch			+= nested_intervals[i]->Reduction_synch;
		Reduction_overlap		+= nested_intervals[i]->Reduction_overlap;

		Wait_shadow				+= nested_intervals[i]->Wait_shadow;
		Shadow_synch			+= nested_intervals[i]->Shadow_synch;
		Shadow_overlap			+= nested_intervals[i]->Shadow_overlap;

		Remote_access			+= nested_intervals[i]->Remote_access;
		Remote_synch			+= nested_intervals[i]->Remote_synch;
		Remote_overlap			+= nested_intervals[i]->Remote_overlap;

		Redistribution			+= nested_intervals[i]->Redistribution;
		Redistribution_synch	+= nested_intervals[i]->Redistribution_synch;
		Redistribution_overlap	+= nested_intervals[i]->Redistribution_overlap;

		Insuff_parallelism		+= nested_intervals[i]->Insuff_parallelism;
		Insuff_parallelism_sys	+= nested_intervals[i]->Insuff_parallelism_sys;
		Insuff_parallelism_usr	+= nested_intervals[i]->Insuff_parallelism_usr;
		Synchronization			+= nested_intervals[i]->Synchronization;
		Real_synchronization	+= nested_intervals[i]->Real_synchronization;
//		Communication_SYNCH		+= nested_intervals[i]->Communication_SYNCH;
		Idle					+= nested_intervals[i]->Idle;
		Load_imbalance			+= nested_intervals[i]->Load_imbalance;
		Overlap					+= nested_intervals[i]->Overlap;
	
	}

    Total_time			= Execution_time * rootProcCount;
    Productive_CPU_time	= CPU_time_usr - Insuff_parallelism_usr;
    Productive_SYS_time	= CPU_time_sys - Insuff_parallelism_sys;
    Productive_time		= Productive_CPU_time + Productive_SYS_time + IO_time;

//	cout << "Idle = " << Idle << endl;
//	cout << "Lost_time = " << Lost_time << endl;

	if ((Productive_time == 0.0) && (Total_time == 0.0))
		Efficiency = 1.0;
	else if (Total_time == 0.0)
		Efficiency = DBL_MAX; 
	else
		Efficiency = Productive_time / Total_time;
}

// Save interval as part of HTML file

void Interval::SaveInFile(ofstream& hfile, int up, int next, int pred)
{ 
   bool outOn = true;
   int	i = 0,
		j = 0,
		k = 0;
   char idv[64];


	 for (i=0; interval[i] != NULL; i++) {
		if (interval[i][0] == '@') {
			//====
			if (strcmp(interval[i], "@title@") == 0)
			{
				if(html_title!=NULL) hfile << html_title <<" :: ";
				hfile << VER_PRED << endl;
			}
			else 
			//=***
			if (strcmp(interval[i],"@label@") == 0)
				hfile << '"' << _itoa(ID, idv, 10) /*HTML_file*/ << '"' << endl;
			else if (strcmp(interval[i], "@typec@") == 0) {
				switch(type) {
					case __IT_MAIN :
					hfile << "Main" << endl;
					break;
					case __IT_PAR :
					hfile << "Par" << endl;
					break;
					case __IT_SEQ :
					hfile << "Seq" << endl;
					break;
					case __IT_USER :
					hfile << "User" << endl;
					break;
				}
			} else if (strcmp(interval[i], "@levc@") == 0) {
				hfile << level << endl;
			} else if (strcmp(interval[i], "@counc@") == 0) {
				hfile << EXE_count << endl;
			} else if (strcmp(interval[i], "@linec@") == 0) {
				hfile << source_line << endl;
			} else if (strcmp(interval[i], "@exprc@") == 0) {
				if (index != NO_EXPR)
					hfile << index << endl;
			} else if (strcmp(interval[i], "@filec@") == 0) {
				if (source_file != NULL)
					hfile << source_file << endl;
			} else if (strcmp(interval[i], "@proc@") == 0) {
				vector<long>::const_iterator j;
/*				if (currentVM->getMType() == 0)
					hfile << "ethernet ";
				else
					hfile << "transp  ";
*/
/*
				switch (currentVM->getMType()) {
					case mach_ETHERNET :
						hfile << "ethernet ";
						break;
					case mach_TRANSPUTER:
						hfile << "transp  ";
						break;
					case mach_MYRINET:
						hfile << "myrinet ";
						break;
				}
*/
				for (j = rootVM->getSizeArray().begin(); j < rootVM->getSizeArray().end(); j++) {
					if (j != rootVM->getSizeArray().begin()) 
						hfile << 'x';
					hfile << *j;
				}
			} else if (strcmp(interval[i], "@effic@") == 0) {
				hfile << Efficiency << endl;
			} else if (strcmp(interval[i], "@exec@") == 0) {
				hfile << Execution_time << endl;
			} else if (strcmp(interval[i], "@total@") == 0) {
				hfile << Total_time << endl;
			} else if (strcmp(interval[i], "@ptime@") == 0) {
				hfile << Productive_time << endl;
			} else if (strcmp(interval[i], "@ptimec@") == 0) {
				hfile << Productive_CPU_time << endl;
			} else if (strcmp(interval[i], "@ptimes@") == 0) {
				hfile << Productive_SYS_time << endl;
			} else if (strcmp(interval[i], "@ptimei@") == 0) {
				hfile << IO_time << endl;
			} else if (strcmp(interval[i], "@lost@") == 0) {
				hfile << Lost_time << endl;
			} else if (strcmp(interval[i], "@insuf@") == 0) {
				hfile << Insuff_parallelism << endl;
			} else if (strcmp(interval[i], "@iuser@") == 0) {
				hfile << Insuff_parallelism_usr << endl;
			} else if (strcmp(interval[i], "@isyst@") == 0) {
				hfile << Insuff_parallelism_sys << endl;
			} else if (strcmp(interval[i], "@comm@") == 0) {
				hfile << Communication << endl;
			} else if (strcmp(interval[i], "@csyn@") == 0) {
				hfile << Real_synchronization  << endl;	
			} else if (strcmp(interval[i], "@idle@") == 0) {
				hfile << Idle << endl;
			} else if (strcmp(interval[i], "@imbal@") == 0) {
				hfile << Load_imbalance << endl;
			} else if (strcmp(interval[i], "@synch@") == 0) {
				hfile << Synchronization << endl;
			} else if (strcmp(interval[i], "@vary@") == 0) {
				hfile << Variation << endl;			
			} else if (strcmp(interval[i], "@over@") == 0) {
				hfile << Overlap << endl;
			} else if (strcmp(interval[i], "@nopi@") == 0) {
				hfile << num_op_io << endl;
			} else if (strcmp(interval[i], "@comi@") == 0) {
				if (outOn) hfile << IO_comm << endl;
			} else if (strcmp(interval[i], "@rsynchi@") == 0) {
				if (outOn) hfile << IO_real_synch << endl;
			} else if (strcmp(interval[i], "@synchi@") == 0) {
				if (outOn) hfile << IO_synch << endl;
			} else if (strcmp(interval[i], "@varyi@") == 0) {
				if (outOn) hfile << IO_vary << endl;
			} else if (strcmp(interval[i], "@overi@") == 0) {
				if (outOn) hfile << IO_overlap << endl;
			} else if (strcmp(interval[i], "@nopr@") == 0) {
				hfile << num_op_reduct << endl;
			} else if (strcmp(interval[i], "@comr@") == 0) {
				if (outOn) hfile << Wait_reduction << endl;
			} else if (strcmp(interval[i], "@rsynchr@") == 0) {
				if (outOn) hfile << Reduction_real_synch << endl;
			} else if (strcmp(interval[i], "@synchr@") == 0) {
				if (outOn) hfile << Reduction_synch << endl;
			} else if (strcmp(interval[i], "@varyr@") == 0) {
				if (outOn) hfile << Reduction_vary << endl;
			} else if (strcmp(interval[i], "@overr@") == 0) {
				if (outOn) hfile << Reduction_overlap << endl;
			} else if (strcmp(interval[i], "@nops@") == 0) {
				hfile << num_op_shadow << endl;
			} else if (strcmp(interval[i], "@coms@") == 0) {
				if (outOn) hfile << Wait_shadow << endl;
			} else if (strcmp(interval[i], "@rsynchs@") == 0) {
				if (outOn) hfile << Shadow_real_synch << endl;
			} else if (strcmp(interval[i], "@synchs@") == 0) {
				if (outOn) hfile << Shadow_synch << endl;
			} else if (strcmp(interval[i], "@varys@") == 0) {
				if (outOn) hfile << Shadow_vary << endl;
			} else if (strcmp(interval[i], "@overs@") == 0) {
				if (outOn) hfile << Shadow_overlap << endl;
			} else if (strcmp(interval[i], "@nopa@") == 0) {
				hfile << num_op_remote << endl;
			} else if (strcmp(interval[i], "@coma@") == 0) {
				if (outOn) hfile << Remote_access << endl;
			} else if (strcmp(interval[i], "@rsyncha@") == 0) {
				if (outOn) hfile << Remote_real_synch << endl;
			} else if (strcmp(interval[i], "@syncha@") == 0) {
				if (outOn) hfile << Remote_synch << endl;
			} else if (strcmp(interval[i], "@varya@") == 0) {
				if (outOn) hfile << Remote_vary << endl;
			} else if (strcmp(interval[i], "@overa@") == 0) {
				if (outOn) hfile << Remote_overlap << endl;
			} else if (strcmp(interval[i], "@nopd@") == 0) {
				hfile << num_op_redist << endl;
			} else if (strcmp(interval[i], "@comd@") == 0) {
				if (outOn) hfile << Redistribution << endl;
			} else if (strcmp(interval[i], "@rsynchd@") == 0) {
				if (outOn) hfile << Redistribution_real_synch << endl;
			} else if (strcmp(interval[i], "@synchd@") == 0) {
				if (outOn) hfile << Redistribution_synch << endl;
			} else if (strcmp(interval[i], "@varyd@") == 0) {
				if (outOn) hfile << Redistribution_vary << endl;
			} else if (strcmp(interval[i], "@overd@") == 0) {
				if (outOn) hfile << Redistribution_overlap << endl;
			} else if (strcmp(interval[i], "@nestbeg@") == 0) {
				if (count == 0)
					outOn = false;
			} else if (strcmp(interval[i], "@nesteds@") == 0) {
				if (outOn)
					j = i;
			} else if (strcmp(interval[i], "@url@") == 0) {
				if (outOn) {
					hfile << "\"#" << _itoa(nested_intervals[k]->ID, idv, 10) 
						<< '"' << endl;
				}
			} else if (strcmp(interval[i], "@go01@") == 0) {
				if (outOn)
					hfile << k+1 << endl;
			} else if (strcmp(interval[i], "@type@") == 0) {
				if (outOn) {
					switch(nested_intervals[k]->type) {
						case __IT_MAIN :
						hfile << "Main" << endl;
						break;
						case __IT_PAR :
						hfile << "Par" << endl;
						break;
						case __IT_SEQ :
						hfile << "Seq" << endl;
						break;
						case __IT_USER :
						hfile << "User" << endl;
						break;
					}
				}
			} else if (strcmp(interval[i], "@lev@") == 0) {
				if (outOn) {
					hfile << nested_intervals[k]->level << endl;
				}
			} else if (strcmp(interval[i], "@coun@") == 0) {
				if (outOn) {
					hfile << nested_intervals[k]->EXE_count << endl;
				}
			} else if (strcmp(interval[i], "@line@") == 0) {
				if (outOn) {
					hfile << nested_intervals[k]->source_line << endl;
				}
			} else if (strcmp(interval[i], "@expr@") == 0) {
				if ((outOn) && (nested_intervals[k]->index != NO_EXPR))
					hfile << nested_intervals[k]->index << endl;
			} else if (strcmp(interval[i], "@file@") == 0) {
				if (outOn) {
					hfile << nested_intervals[k]->source_file << endl;
				}
			} else if (strcmp(interval[i], "@nestedf@") == 0) {
				if (outOn) {
					k++;
					if (k < count) {
						i = j;
						continue;
					}
				}
			} else if (strcmp(interval[i], "@nestend@") == 0) {
				outOn = true;
			} else if (strcmp(interval[i], "@up@") == 0) {
				hfile << "\"#" << _itoa(up, idv, 10) << '"' << endl;
			} else if (strcmp(interval[i], "@pred@") == 0) {
				hfile << "\"#" << _itoa(pred, idv, 10) << '"' << endl;
			} else if (strcmp(interval[i], "@next@") == 0) {
				hfile << "\"#" << _itoa(next, idv, 10) << '"' << endl;
			} else if (strcmp(interval[i], "@home@") == 0) {
				hfile << "\"#" << _itoa(1, idv, 10) << '"' << endl;
			}
		} else if (outOn) {
			hfile << interval[i] << endl;
//			prot << interval[i] << endl;
		}
	}
	return;

}

// Save all interval's tree as HTML file (recursive)

void Interval::SaveTree(ofstream&	hfile, int up, int next, int pred)
{ 
	int i;

	SaveInFile(hfile, up, next, pred);

	for(i = 0; i < count; i++) 
	{
		int pred = (i == 0) ? ID : nested_intervals[i-1]->ID;
		int next = (i == count - 1) ? ID : nested_intervals[i+1]->ID;
		nested_intervals[i]->SaveTree(hfile, ID, next, pred);
	}
}



