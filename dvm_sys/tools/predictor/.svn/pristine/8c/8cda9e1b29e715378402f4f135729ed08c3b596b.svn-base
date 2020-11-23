#include <stdlib.h>
#include <assert.h>

#include <fstream>
#include <vector>

#include "ModelStructs.h"
#include "FuncCall.h"
#include "CallInfoStructs.h"
#include "Interval.h"

using namespace std;

extern ofstream prot;

void FuncCall::RegularTime()
{

	//grig
	AddMPSTime(__CPU_time_usr, vcall_time);
	//\grig
	

	if (ret_time !=0.0)
	{
		//grig AddMPSTime(__CPU_time_sys, ret_time);
		//grig
		AddMPSTime(__CPU_time_sys, vret_time);
		//grig

	}
 
	//grig
	int k;
	vector<double> tempret,tempcall;
	tempcall.resize(0);
	tempret.resize(0);

	for(k=0;k<vret_time.size();k++)
		tempret.push_back(vret_time[k]*((double) MPSProcCount()-1.0) / (double) MPSProcCount());

	for(k=0;k<vcall_time.size();k++)
		tempcall.push_back(vcall_time[k] * ((double) MPSProcCount()-1.0) / (double) MPSProcCount());
	AddMPSTime(__Insuff_parall_sys,tempret  );
    AddMPSTime(__Insuff_parall_usr,tempcall );
	//\grig

}

void FuncCall::UnknownTime()
{ 
	RegularTime();
}

