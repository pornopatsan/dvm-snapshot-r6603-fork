#include "FuncCall.h"
#include "ModelStructs.h"
#include "Interval.h"

void FuncCall::biof()
{
	CurrInterval->setIOTrafic();
	++CurrInterval->num_op_io;
}

void FuncCall::tstio()
{
}

void FuncCall::srmem() //??? что делает и что делать!!!
{
	int i;
	double TimeDelta = 0.0;
	long account = 0;
//	double tbyte = currentVM->getTByte(); 
//	double tstart = currentVM->getTStart(); 

    srmem_Info* params=(srmem_Info*) call_params;
	for (i = 0; i < params-> MemoryCount; i++)
		account += params-> LengthArray[i];
    MPSSynchronize(__IO_comm);
    double time_start = CurrProcTime(0);	//__IO_comm

    for (i=0; i < MPSProcCount(); i++) {
		
//		TimeDelta += i * tbyte * account + tstart;
//		AddTime(__IO_comm, currentVM->map(i), TimeDelta);
	}
}

void FuncCall::eiof()
{
	CurrInterval->resetIOTrafic();
}

void FuncCall::ciotime()
{ 
	AddTime(__IO_time, 0, vret_time[0]);
	//grig 	AddTime(__IO_time, 0, ret_time);
	++CurrInterval->num_op_io;
}

void FuncCall::IOTime()
{
    switch(func_id) {
        case biof_ :
			biof();
			break;
        case eiof_ :
			eiof();
			break;
        case srmem_ :
			srmem();
			break;
		default:
			ciotime();

      }

	// calculate times

	RegularTime();  
}
