
#include <string.h>
#include <assert.h>

#include <fstream>

#include "FuncCall.h"
#include "CallInfoStructs.h"
#include "Vm.h"

using namespace std;

extern ofstream prot;
double grig_time_call=0.0;

// =================================FuncCall =======================================

//------------------------------ CONSTRUCTOR --------------------------------------
FuncCall::FuncCall():
	call_time(0.0),
	ret_time(0.0),
	call_params(NULL),
	source_file(NULL)
{ 
}
 
FuncCall::FuncCall(VectorTraceLine *traceLines) :
	ret_time(0.0),
    call_params(NULL)
{

		int call_info_count = 0;
    char** call_info = NULL; // pointer to string vector with input function params
    int ret_info_count = 0;
    char** ret_info = NULL;  // pointer to string vector  with output function params
	TraceLine	* tl = traceLines->current();

	// 'call_xxxxxx'
	assert(traceLines->current()->line_type == Call_);

    func_id = tl->func_id;
    call_time = tl->func_time;//commented grig  /rootVM->getProcPower();
	grig_time_call+=tl->func_time;
//grig add-on
vcall_time.resize(currentVM->getProcCount());
int k;
for(k=0;k<currentVM->getProcCount();k++)
{
//	printf("VRET[%d of %d]= %f / %f\n",k,currentVM->getProcCount(),tl->func_time,currentVM->getProcPower(k));
  vcall_time[k]=tl->func_time/currentVM->getProcPower(k);
}
//\grig add-on
    source_line = tl->source_line;
    source_file = strdup(tl->source_file);

	// get effective parameters 
	traceLines->next();
	traceLines->GetUnknownLines(call_info_count, call_info);

	//'ret_xxxxxx 
	assert(traceLines->current()->line_type == Ret_);

//grig
	double rettimetemp;
	rettimetemp=traceLines->current()->func_time;
//\grig
	ret_time = rettimetemp; //commented by grig / rootVM->getProcPower();

//grig add-on
//int k;
vret_time.resize(currentVM->getProcCount());
for(k=0;k<currentVM->getProcCount();k++)
{
//  fff=rettimetemp;
  vret_time[k]=rettimetemp/ currentVM->getProcPower(k);
}
//\grig add-on

	traceLines->next();
	traceLines->GetUnknownLines(ret_info_count, ret_info);

	// Only for parameters passing
	TraceCall	trc_call(func_id, source_line, source_file, 
						 call_info_count, call_info, ret_info_count, ret_info);


	// create FuncCall::params
	GetCallParams(trc_call, call_params);

	// free memory
	int i;

	for (i = 0; i < call_info_count; i++)
		delete call_info[i];
	delete call_info;

	for (i = 0; i < ret_info_count; i++)
		delete ret_info[i];
	delete ret_info;
}

// ------------------------------ DESTRUCTOR ------------------------------------- 

FuncCall::~FuncCall()
{
	delete source_file;
    switch (func_id) {

        case binter_ :
        case bsloop_ :
        case bploop_ :
          delete (binter_Info*) call_params;
          break;
        case crtamv_:
          delete (crtamv_Info*) call_params;
          break;
        case blkdiv_ :
          delete (blkdiv_Info*) call_params;
          break;
        case distr_ :
          delete (distr_Info*) call_params;
          break;
        case redis_ :
          delete (redis_Info*) call_params;
          break;
        case crtda_ :
          delete (crtda_Info*) call_params;
          break;
        case align_ :
          delete (align_Info*) call_params;
          break;
        case realn_ :
          delete (realn_Info*) call_params;
          break;
        case arrcpy_ :
          delete (arrcpy_Info*) call_params;
          break;
        case aarrcp_ :
          delete (arrcpy_Info*) call_params;
          break;
        case mappl_ :
          delete (mappl_Info*) call_params;
          break;
        case inssh_ :
          delete (inssh_Info*) call_params;
          break;
        case insshd_ :
          delete (inssh_Info*) call_params;
          break;
        case incsh_ :
          delete (inssh_Info*) call_params;
          break;
        case incshd_ :
          delete (inssh_Info*) call_params;
          break;
        case exfrst_ :
          delete (exfrst_Info*) call_params;
		  break;
        case imlast_ :
          delete (imlast_Info*) call_params;
		  break;
        case einter_ :
          delete (einter_Info*) call_params;
		  break;
        case getam_ :
          delete (getam_Info*) call_params;
		  break;
        case crtps_ :
          delete (crtps_Info*) call_params;
		  break;
        case getps_ :
          delete (getps_Info*) call_params;
		  break;
        case psview_ :
          delete (psview_Info*) call_params;
		  break;
        case delps_ :
          delete (delps_Info*) call_params;
		  break;
        case setelw_ :
          delete (setelw_Info*) call_params;
		  break;
        case getamr_ :
          delete (getamr_Info*) call_params;
		  break;
        case getamv_ :
          delete (getamv_Info*) call_params;
		  break;
        case mapam_ :
          delete (mapam_Info*) call_params;
		  break;
        case runam_ :
          delete (runam_Info*) call_params;
		  break;
        case delamv_ :
          delete (delamv_Info*) call_params;
		  break;
        case delda_ :
          delete (delda_Info*) call_params;
		  break;
        case crtpl_ :
          delete (crtpl_Info*) call_params;
		  break;
        case dopl_ :
          delete (dopl_Info*) call_params;
		  break;
        case endpl_ :
          delete (endpl_Info*) call_params;
		  break;
        case crtrg_ :
          delete (crtrg_Info*) call_params;
		  break;
        case crtred_ :
          delete (crtred_Info*) call_params;
		  break;
        case insred_ :
          delete (insred_Info*) call_params;
		  break;
        case delrg_ :
          delete (delrg_Info*) call_params;
		  break;
        case delred_ :
          delete (delred_Info*) call_params;
		  break;
        case strtrd_ :
          delete (strtrd_Info*) call_params;
		  break;
        case waitrd_ :
          delete (waitrd_Info*) call_params;
		  break;
        case crtshg_ :
          delete (crtshg_Info*) call_params;
		  break;
        case delshg_ :
          delete (delshg_Info*) call_params;
		  break;
        case strtsh_ :
          delete (strtsh_Info*) call_params;
		  break;
        case waitsh_ :
          delete (waitsh_Info*) call_params;
		  break;
        case sendsh_ :
          delete (sendsh_Info*) call_params;
		  break;
        case recvsh_ :
          delete (recvsh_Info*) call_params;
		  break;
        case crtrbl_ :
          delete (crtrbl_Info*) call_params;
		  break;
        case crtrbp_ :
          delete (crtrbp_Info*) call_params;
		  break;
		default:
		   delete call_params;
		   break;
      }
}
