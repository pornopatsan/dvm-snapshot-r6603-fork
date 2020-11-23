#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <limits.h>

#include <fstream>

#include "CallInfoStructs.h"
#include "ParseString.h"
#include "Ver.h"
#include "TraceLine.h"
#include "Interval.h"
//====
#include <stdio.h>
//=***

#ifdef _UNIX_
#define _strdup strdup
#endif

using namespace std;

static Event global_func_id;
static int global_source_line;
static char* global_source_file;
//====
//int ShdWid[10];
long TraceProcNum;
//=***

extern ofstream prot;

TraceCall::TraceCall(Event p_func_id, int p_source_line, char* p_source_file, int p_call_info_count,
					 char** p_call_info, int p_ret_info_count, char** p_ret_info) :
	func_id(p_func_id), 
	source_line(p_source_line),
	source_file(p_source_file),
	call_info_count(p_call_info_count),
	call_info(p_call_info),
	ret_info_count(p_ret_info_count),
	ret_info(p_ret_info)
{
}

// Service functions headers 

long* GetArrayParam(char* param_name, char** str_list, int str_count);
double* GetArrayParamD(char* param_name, char** str_list, int str_count);
unsigned long* GetVectorParam(char* param_name, char** str_list, int str_count);

// Internal routines 

Event func_id;

static int arrayRank;	// modified by "GetArrayParam"

void ParamNotFoundErr(char* param)
{ 
	cerr << "ERROR [" << global_source_file << ':' << global_source_line << ':' << global_func_id << "] : '" << param << "' parameter missing" << endl;
	prot << "ERROR [" << global_source_file << ':' << global_source_line << ':' << global_func_id << "] : '" << param << "' parameter missing" << endl;
	abort();
}


/* InfoStruct functions */

IDOnlyInfo* Get_IDOnlyInfo(char* param_name, char** str_list, int str_count, int base)
{ 
	long tmp_l;

	if (ParamFromStrArr(param_name, tmp_l, str_list, str_count, base)) {
		IDOnlyInfo* tmp = new IDOnlyInfo;
		assert(tmp != NULL);
        tmp->ID = tmp_l;
        return tmp;
    } else {
		ParamNotFoundErr(param_name);
        return NULL;
    }
}

binter_Info* Get_binter_Info(TraceCall& trc_call)
{
	binter_Info* tmp = new binter_Info;
	assert(tmp != NULL);

    tmp->line=trc_call.source_line;
    tmp->file=_strdup(trc_call.source_file);
	tmp->index = 0;
	if (!ParamFromStrArr("index", tmp->index, trc_call.call_info, 
		trc_call.call_info_count, 10))
		tmp->index = NO_EXPR;

    return tmp;
}

//-------------------------------------------- Get_getps_Info ------------------------------------------

getps_Info*	Get_getps_Info(TraceCall& trc_call)
{
	getps_Info*	tmp = new getps_Info;
	bool error = false;

    if (!ParamFromStrArr("AMRef", tmp->AMRef, trc_call.call_info, 
		trc_call.call_info_count, 16)) {
        ParamNotFoundErr("AMRef");
		error = true;
    } else if (!ParamFromStrArr("PSRef", tmp->PSRef, trc_call.ret_info, 
		trc_call.ret_info_count, 16)) {
        ParamNotFoundErr("PSRef");
		error = true;
	}

	if (error) {
		delete tmp;
		tmp = NULL;
	}

	return tmp;

}

//-------------------------------------------- Get_crtps_Info ------------------------------------------

crtps_Info*	Get_crtps_Info(TraceCall& trc_call)
{
	crtps_Info* tmp = new crtps_Info;
	long *AInitIndexArray = NULL;
	long *ALastIndexArray = NULL;
	bool error = false;

    if (!ParamFromStrArr("PSRef", tmp->PSRefParent, trc_call.call_info, 
		trc_call.call_info_count, 16)) {
        ParamNotFoundErr("PSRef");
		error = true;
    } else if (!ParamFromStrArr("StaticSign", tmp->StaticSign, trc_call.call_info, 
		trc_call.call_info_count, 10)) {
        ParamNotFoundErr("StaticSign");
		error = true;
    } else if ((AInitIndexArray = GetArrayParam("InitIndexArray", trc_call.call_info, trc_call.call_info_count)) == NULL) {
        ParamNotFoundErr("InitIndexArray");
		error = true;
    } else if ((ALastIndexArray = GetArrayParam("LastIndexArray", trc_call.call_info, trc_call.call_info_count)) == NULL) {
        ParamNotFoundErr("LastIndexArray");
		error = true;
    } else if (!ParamFromStrArr("PSRef", tmp->PSRef, trc_call.ret_info, 
		trc_call.ret_info_count, 16)) {
        ParamNotFoundErr("PSRef");
		error = true;
	}

	if (error) {
		delete [] AInitIndexArray;
		delete [] ALastIndexArray;
		delete tmp;
		tmp = NULL;
	} else {
		tmp->InitIndexArray.resize(arrayRank);
		tmp->LastIndexArray.resize(arrayRank);
		for (int i=0; i < arrayRank; i++) {
			tmp->InitIndexArray[i] = AInitIndexArray[i];
			tmp->LastIndexArray[i] = ALastIndexArray[i];
		}
		delete [] AInitIndexArray;
		delete [] ALastIndexArray;
	}

    return tmp;
}

//-------------------------------------------- Get_psview_Info ------------------------------------------

psview_Info*	Get_psview_Info(TraceCall& trc_call)
{
	psview_Info* tmp = new psview_Info;
	long *ASizeArray = NULL;
	bool error = false;

    if (!ParamFromStrArr("PSRef", tmp->PSRefParent, trc_call.call_info, 
		trc_call.call_info_count, 16)) {
        ParamNotFoundErr("PSRef");
		error = true;
    } else if (!ParamFromStrArr("Rank", tmp->Rank, trc_call.call_info, 
		trc_call.call_info_count, 10)) {
        ParamNotFoundErr("Rank");
		error = true;
    } else if (!ParamFromStrArr("StaticSign", tmp->StaticSign, trc_call.call_info, 
		trc_call.call_info_count, 10)) {
        ParamNotFoundErr("StaticSign");
		error = true;
    } else if ((ASizeArray = GetArrayParam("SizeArray", trc_call.call_info, trc_call.call_info_count)) == NULL) {
        ParamNotFoundErr("SizeArray");
		error = true;
    } else if (!ParamFromStrArr("PSRef", tmp->PSRef, trc_call.ret_info, 
		trc_call.ret_info_count, 16)) {
        ParamNotFoundErr("PSRef");
		error = true;
	}

	if (error) {
		delete [] ASizeArray;
		delete tmp;
		tmp = NULL;
	} else {
		tmp->SizeArray.resize(arrayRank);
		for (int i=0; i < arrayRank; i++)
			tmp->SizeArray[i] = ASizeArray[i];
		delete [] ASizeArray;
	}

    return tmp;
}

//-------------------------------------------- Get_setelw_Info ------------------------------------------
/*
	long				PSRef;		// 16
	long				AMViewRef;	// 16
	long				AddrNumber;	// 10
	std::vector<long>	WeightNumber;
	// length = sun i = [0,AddrNumber-1] WeightNumber[i]  
	std:vector<double>	LoadWeight;
*/
setelw_Info*	Get_setelw_Info(TraceCall& trc_call)
{
	setelw_Info* tmp = new setelw_Info;
	long *AWeightNumber = NULL;
	double *ALoadWeight = NULL;
	bool error = false;

	if (!ParamFromStrArr("PSRef", tmp->PSRef, trc_call.call_info, trc_call.call_info_count, 16)) {
        ParamNotFoundErr("PSRef");
		error = true;
    } else if (!ParamFromStrArr("AMViewRef", tmp->AMViewRef, trc_call.call_info, trc_call.call_info_count, 16)) {
        ParamNotFoundErr("AMViewRef");
		error = true;
    } else if (!ParamFromStrArr("AddrNumber", tmp->AddrNumber, trc_call.call_info, trc_call.call_info_count, 10)) {
        ParamNotFoundErr("AddrNumber");
		error = true;
    }
	//grig/* 
	else if ((AWeightNumber = GetArrayParam("WeightNumber", trc_call.call_info, trc_call.call_info_count)) == NULL) {
        ParamNotFoundErr("WeightNumber");
		error = true;
    } else if ((ALoadWeight = (double *) GetArrayParamD("LoadWeight", trc_call.call_info, trc_call.call_info_count)) == NULL) {
        ParamNotFoundErr("LoadWeight");
		error = true;
    }
	//grig*/ 
	
	if (error) {
		delete [] AWeightNumber;
		delete [] ALoadWeight;
		delete tmp;
		tmp = NULL;
	} else {

		tmp->WeightNumber.resize(arrayRank);
		tmp->LoadWeight.resize(arrayRank);		// arrayRank
		for (int i=0; i < arrayRank; i++) {
			tmp->WeightNumber[i] = AWeightNumber[i];
			tmp->LoadWeight[i] = ALoadWeight[i];
		}
		delete [] AWeightNumber;
		delete [] ALoadWeight;
	}

    return tmp;

}

//-------------------------------------------- Get_getamr_Info ------------------------------------------

getamr_Info*	Get_getamr_Info(TraceCall& trc_call)
{
	getamr_Info* tmp = new getamr_Info;
	long *AIndexArray = NULL;
	bool error = false;

    if (!ParamFromStrArr("AMViewRef", tmp->AMViewRef, trc_call.call_info, 
		trc_call.call_info_count, 16)) {
        ParamNotFoundErr("AMViewRef");
		error = true;
    } else if ((AIndexArray = GetArrayParam("IndexArray", trc_call.call_info, trc_call.call_info_count)) == NULL) {
        ParamNotFoundErr("IndexArray");
		error = true;
    } else if (!ParamFromStrArr("AMRef", tmp->AMRef, trc_call.ret_info, 
		trc_call.ret_info_count, 16)) {
        ParamNotFoundErr("AMRef");
		error = true;
	}

	if (error) {
		delete [] AIndexArray;
		delete tmp;
		tmp = NULL;
	} else {
		tmp->IndexArray.resize(arrayRank);
		for (int i=0; i < arrayRank; i++)
			tmp->IndexArray[i] = AIndexArray[i];
		delete [] AIndexArray;
	}

    return tmp;
}

//-------------------------------------------- Get_getamv_Info ------------------------------------------

getamv_Info*	Get_getamv_Info(TraceCall& trc_call)
{
	getamv_Info* tmp = new getamv_Info;
	bool error = false;

    if (!ParamFromStrArr("ArrayHeader", tmp->ArrayHeader, trc_call.call_info, 
		trc_call.call_info_count, 16)) {
        ParamNotFoundErr("ArrayHeader");
		error = true;
    } else if (!ParamFromStrArr("AMViewRef", tmp->AMViewRef, trc_call.ret_info, 
		trc_call.ret_info_count, 16)) {
        ParamNotFoundErr("AMViewRef");
		error = true;
	}

	if (error) {
		delete tmp;
		tmp = NULL;
	}

    return tmp;
}

//-------------------------------------------- Get_getamv_Info ------------------------------------------

mapam_Info*	Get_mapam_Info(TraceCall& trc_call)
{
	mapam_Info* tmp = new mapam_Info;
	bool error = false;

    if (!ParamFromStrArr("AMRef", tmp->AMRef, trc_call.call_info, 
		trc_call.call_info_count, 16)) {
        ParamNotFoundErr("AMRef");
		error = true;
    } else if (!ParamFromStrArr("PSRef", tmp->PSRef, trc_call.call_info, 
		trc_call.call_info_count, 16)) {
        ParamNotFoundErr("PSRef");
		error = true;
	}

	if (error) {
		delete tmp;
		tmp = NULL;
	}

    return tmp;
}


//-------------------------------------------- Get_crtamv_Info ------------------------------------------

crtamv_Info* Get_crtamv_Info(TraceCall& trc_call)
{ 
	crtamv_Info* tmp = new crtamv_Info;
	long *ASizeArray = NULL;
	long rank =0;
	bool error = false;

    if (!ParamFromStrArr("AMRef", tmp->AM_ID, trc_call.call_info, 
		trc_call.call_info_count, 16)) {
        ParamNotFoundErr("AMRef");
		error = true;
    } else if (!ParamFromStrArr("AMViewRef", tmp->ID, trc_call.ret_info, 
		trc_call.ret_info_count, 16)) {
        ParamNotFoundErr("AMViewRef");
		error = true;
	} else if (!ParamFromStrArr("StaticSign", tmp->StaticSign, trc_call.call_info,
		trc_call.call_info_count, 10)) {
		ParamNotFoundErr("StaticSign");
		error = true;
    } else if (!ParamFromStrArr("Rank", rank, trc_call.call_info, 
		trc_call.call_info_count, 10)) {
        ParamNotFoundErr("Rank");
		error = true;
    } else if ((ASizeArray = GetArrayParam("SizeArray", trc_call.call_info, trc_call.call_info_count)) == NULL) {
        ParamNotFoundErr("SizeArray");
		error = true;
	}

	if (error) {
		delete [] ASizeArray;
		delete tmp;
		tmp = NULL;
	} else {
		tmp->SizeArray.resize(rank);
		for (int i=0; i < rank; i++)
			tmp->SizeArray[i] = ASizeArray[i];
		delete [] ASizeArray;
	}

    return tmp;
}

//====
//-------------------------------------------- Get_blkdiv_Info ------------------------------------------

blkdiv_Info* Get_blkdiv_Info(TraceCall& trc_call)
{ 	
	blkdiv_Info* tmp = new blkdiv_Info;
	long *AxisArray = NULL;
	long rank = 0;
	bool error = false;

	if (!ParamFromStrArr("AMViewRef", tmp->ID, trc_call.call_info, trc_call.call_info_count, 16)) {
		ParamNotFoundErr("AMViewRef");
		error = true;
	} else if (!ParamFromStrArr("AMVAxisCount", rank, trc_call.call_info, trc_call.call_info_count, 10)) {
		ParamNotFoundErr("AMVAxisCount");
		error = true;
	} else if (rank > 0) { 
		if ((AxisArray=GetArrayParam("AMVAxisDiv", trc_call.call_info, trc_call.call_info_count)) == NULL) {
			ParamNotFoundErr("AMVAxisDiv");
			error = true;
		}
	}

	if (error) {
		delete tmp;
		tmp = NULL;
		delete [] AxisArray;
	} else {
		tmp->AMVAxisDiv.resize(rank);
		for (int i=0; i < rank; i++) {
			tmp->AMVAxisDiv[i] = AxisArray[i];
		}
		delete [] AxisArray;
	}

	return tmp;
}

//=***

//-------------------------------------------- Get_distr_Info ------------------------------------------

distr_Info* Get_distr_Info(TraceCall& trc_call)
{ 
	distr_Info* tmp = new distr_Info;
	long *AxisArray = NULL;
	long *DistrParamArray = NULL;
	long rank = 0;
	bool error = false;

    if (!ParamFromStrArr("AMViewRef", tmp->ID, trc_call.call_info, trc_call.call_info_count, 16)) {
        ParamNotFoundErr("AMViewRef");
		error = true;
	} else if (!ParamFromStrArr("PSRef", tmp->PSRef, trc_call.call_info, trc_call.call_info_count, 16)) {
        ParamNotFoundErr("PSRef");
		error = true;
	} else if (!ParamFromStrArr("ParamCount", rank, trc_call.call_info, trc_call.call_info_count, 10)) {
		ParamNotFoundErr("ParamCount");
		error = true;
	} else if (rank > 0) { 
		if ((AxisArray=GetArrayParam("AxisArray", trc_call.call_info, trc_call.call_info_count)) == NULL) {
			ParamNotFoundErr("AxisArray");
			error = true;
		} else if ((DistrParamArray=GetArrayParam("DistrParamArray", trc_call.call_info, trc_call.call_info_count)) == NULL) {
			ParamNotFoundErr("DistrParamArray");
			error = true;
		}
	}

	if (error) {
		delete tmp;
		tmp = NULL;
		delete [] AxisArray;
		delete [] DistrParamArray;
	} else {
		tmp->AxisArray.resize(rank);
		tmp->DistrParamArray.resize(rank);
		for (int i=0; i < rank; i++) {
			tmp->AxisArray[i] = AxisArray[i];
			tmp->DistrParamArray[i] = DistrParamArray[i];
		}
		delete [] AxisArray;
		delete [] DistrParamArray;
	}

	return tmp;
}

//-------------------------------------------- Get_redis_Info ------------------------------------------

redis_Info* Get_redis_Info(TraceCall& trc_call)
{ 
	redis_Info* tmp = new redis_Info;
	long *AxisArray = NULL;
	long *DistrParamArray = NULL;
	long rank = 0;
	bool error = false;

	tmp->ID = 0;
	tmp->AID = 0;

    if ((!ParamFromStrArr("AMViewRef", tmp->ID, trc_call.call_info, trc_call.call_info_count, 16)) &&
		(!ParamFromStrArr("ArrayHeader", tmp->AID, trc_call.call_info, trc_call.call_info_count, 16))) {	
        ParamNotFoundErr("AMViewRef/ArrayHeader");
		error = true;
	} else if (!ParamFromStrArr("PSRef", tmp->PSRef, trc_call.call_info, trc_call.call_info_count, 16)) {
        ParamNotFoundErr("PSRef");
		error = true;
	} else if (!ParamFromStrArr("ParamCount", rank, trc_call.call_info, trc_call.call_info_count, 10)) {
		ParamNotFoundErr("ParamCount");
		error = true;
	} else if(!ParamFromStrArr("NewSign", tmp->NewSign, trc_call.call_info, trc_call.call_info_count, 10)) {
		ParamNotFoundErr("NewSign");
		error = true;
	} else if ((AxisArray=GetArrayParam("AxisArray", trc_call.call_info, trc_call.call_info_count)) == NULL) {
		ParamNotFoundErr("AxisArray");
		error = true;
    } else if ((DistrParamArray=GetArrayParam("DistrParamArray", trc_call.call_info, trc_call.call_info_count)) == NULL) {
		ParamNotFoundErr("DistrParamArray");
		error = true;
	}

	if (error) {
		delete tmp;
		tmp = NULL;
		delete [] AxisArray;
		delete [] DistrParamArray;
	} else {
		tmp->AxisArray.resize(rank);
		tmp->DistrParamArray.resize(rank);
		for (int i=0; i < rank; i++) {
			tmp->AxisArray[i] = AxisArray[i];
			tmp->DistrParamArray[i] = DistrParamArray[i];
		}
		delete [] AxisArray;
		delete [] DistrParamArray;
	}

	return tmp;
}

//-------------------------------------------- Get_crtda_Info ------------------------------------------

crtda_Info* Get_crtda_Info(TraceCall& trc_call)
{ 
	long	rank = 0;
	long *	SizeArray = NULL;
	long *	LowShdWidthArray = NULL;
	long *	HiShdWidthArray = NULL;
	bool	error = false;

	crtda_Info* tmp = new crtda_Info;
	assert(tmp != NULL);

    if (!ParamFromStrArr("ArrayHandlePtr", tmp->ArrayHandlePtr, trc_call.ret_info, trc_call.ret_info_count, 16)) {
        ParamNotFoundErr("ArrayHandlePtr");
		error = true;
	} else if (!ParamFromStrArr("ArrayHeader", tmp->ArrayHeader, trc_call.call_info, 
		trc_call.call_info_count, 16)) {
        ParamNotFoundErr("ArrayHeader");
		error = true; 
    } else if(!ParamFromStrArr("Rank", rank , trc_call.call_info, 
		trc_call.call_info_count, 10)) {
        ParamNotFoundErr("Rank");
		error = true;
    } else if (!ParamFromStrArr("TypeSize", tmp->TypeSize, trc_call.call_info, 
		trc_call.call_info_count, 10)) {
        ParamNotFoundErr("TypeSize");
		error = true;
    } else if (!ParamFromStrArr("StaticSign", tmp->StaticSign, trc_call.call_info, 
		trc_call.call_info_count, 10)) {
        ParamNotFoundErr("StaticSign");
		error = true;
    } else if (!ParamFromStrArr("ReDistrSign", tmp->ReDistrSign, trc_call.call_info, 
		trc_call.call_info_count, 10)) {
        ParamNotFoundErr("ReDistrSign");
		error = true;
    } else if ((SizeArray=GetArrayParam("SizeArray", trc_call.call_info, trc_call.call_info_count)) == NULL) {
        ParamNotFoundErr("SizeArray");
		error = true;
    } else if ((LowShdWidthArray=GetArrayParam("LowShdWidthArray", trc_call.call_info, trc_call.call_info_count)) == NULL) {
        ParamNotFoundErr("LowShdWidthArray");
		error = true;
    } else if ((HiShdWidthArray=GetArrayParam("HiShdWidthArray", trc_call.call_info, trc_call.call_info_count)) == NULL) {
        ParamNotFoundErr("HiShdWidthArray");
		error = true;
	}

	if (error) {
		delete tmp;
		tmp = NULL;
	} else {
		tmp->SizeArray.resize(rank);
		tmp->LowShdWidthArray.resize(rank);
		tmp->HiShdWidthArray.resize(rank);
		for (int i = 0; i < rank; i++) {
			tmp->SizeArray[i] = SizeArray[i];
			tmp->LowShdWidthArray[i] = LowShdWidthArray[i];
			tmp->HiShdWidthArray[i] = HiShdWidthArray[i];
		}
	}

	delete [] SizeArray;
	delete [] LowShdWidthArray;
	delete [] HiShdWidthArray;

    return tmp;
}

//-------------------------------------------- Get_align_Info ------------------------------------------

align_Info* Get_align_Info(TraceCall& trc_call)
{
	bool		error = false;
	long	*	AxisArray = NULL;
	long	*	CoeffArray = NULL;
	long	*	ConstArray = NULL;

	align_Info* tmp = new align_Info;
	assert(tmp != NULL);

    if (!ParamFromStrArr("ArrayHeader", tmp->ArrayHeader, trc_call.call_info, trc_call.call_info_count, 16)) {
        ParamNotFoundErr("ArrayHeader");
		error = true;
    } else if (!ParamFromStrArr("ArrayHandlePtr", tmp->ArrayHandlePtr, trc_call.call_info, trc_call.call_info_count, 16)) {
        ParamNotFoundErr("ArrayHandlePtr");
		error = true;
    } else if(!ParamFromStrArr("PatternRefPtr", tmp->PatternRefPtr, trc_call.call_info, trc_call.call_info_count, 16)) {
        ParamNotFoundErr("PatternRefPtr");
		error = true;
    } else if(!ParamFromStrArr("PatternRef", tmp->PatternRef, trc_call.call_info, trc_call.call_info_count, 16)) {
        ParamNotFoundErr("PatternRef");
		error = true;
	} else if ((tmp->PatternType = ModifierFromStrArr(trc_call.call_info, trc_call.call_info_count)) == 0) {
        ParamNotFoundErr("(AMView)/(DisArray)");
		error = true;
    } else if ((AxisArray=GetArrayParam("AxisArray", trc_call.call_info, trc_call.call_info_count)) == NULL) {
        ParamNotFoundErr("AxisArray");
		error = true;
	} else if ((CoeffArray=GetArrayParam("CoeffArray", trc_call.call_info, trc_call.call_info_count)) == NULL) {
        ParamNotFoundErr("CoeffArray");
		error = true;
	} else if ((ConstArray=GetArrayParam("ConstArray", trc_call.call_info, trc_call.call_info_count)) == NULL) {
        ParamNotFoundErr("CoeffArray");
		error = true;
	}

	if (error) {
		delete tmp;
		tmp = NULL;
		delete [] AxisArray;
		delete [] CoeffArray;
		delete [] ConstArray;
	} else {
		tmp->AxisArray.resize(arrayRank);
		tmp->CoeffArray.resize(arrayRank);
		tmp->ConstArray.resize(arrayRank);
		for (int i=0; i < arrayRank; i++) {
			tmp->AxisArray[i] = AxisArray[i];
			tmp->CoeffArray[i] = CoeffArray[i];
			tmp->ConstArray[i] = ConstArray[i];
		}
		delete [] AxisArray;
		delete [] CoeffArray;
		delete [] ConstArray;
	}
	
    return tmp;
}

//-------------------------------------------- Get_realn_Info ------------------------------------------

realn_Info* Get_realn_Info(TraceCall& trc_call)
{
	bool		error = false;
	long	*	AxisArray = NULL;
	long	*	CoeffArray = NULL;
	long	*	ConstArray = NULL;

	realn_Info* tmp = new realn_Info;
	assert(tmp != NULL);

    if (!ParamFromStrArr("ArrayHeader", tmp->ArrayHeader, trc_call.call_info, trc_call.call_info_count, 16)) {
        ParamNotFoundErr("ArrayHeader");
		error = true;
    } else if (!ParamFromStrArr("ArrayHandlePtr", tmp->ArrayHandlePtr, trc_call.call_info, trc_call.call_info_count, 16)) {
        ParamNotFoundErr("ArrayHandlePtr");
		error = true;
    } else if(!ParamFromStrArr("PatternRefPtr", tmp->PatternRefPtr, trc_call.call_info, trc_call.call_info_count, 16)) {
        ParamNotFoundErr("PatternRefPtr");
		error = true;
    } else if(!ParamFromStrArr("PatternRef", tmp->PatternRef, trc_call.call_info, trc_call.call_info_count, 16)) {
        ParamNotFoundErr("PatternRef");
		error = true;
	} else if ((tmp->PatternType = ModifierFromStrArr(trc_call.call_info, trc_call.call_info_count)) == 0) {
        ParamNotFoundErr("(AMView)/(DisArray)");
		error = true;
	} else if (!ParamFromStrArr("NewSign", tmp->NewSign, trc_call.call_info, trc_call.call_info_count, 10)) {
        ParamNotFoundErr("NewSign");
		error = true;
    } else if ((AxisArray=GetArrayParam("AxisArray", trc_call.call_info, trc_call.call_info_count)) == NULL) {
        ParamNotFoundErr("AxisArray");
		error = true;
	} else if ((CoeffArray=GetArrayParam("CoeffArray", trc_call.call_info, trc_call.call_info_count)) == NULL) {
        ParamNotFoundErr("CoeffArray");
		error = true;
	} else if ((ConstArray=GetArrayParam("ConstArray", trc_call.call_info, trc_call.call_info_count)) == NULL) {
        ParamNotFoundErr("CoeffArray");
		error = true;
	}

	if (error) {
		delete tmp;
		tmp = NULL;
	} else {
		tmp->AxisArray.resize(arrayRank);
		tmp->CoeffArray.resize(arrayRank);
		tmp->ConstArray.resize(arrayRank);
		for (int i=0; i < arrayRank; i++) {
			tmp->AxisArray[i] = AxisArray[i];
			tmp->CoeffArray[i] = CoeffArray[i];
			tmp->ConstArray[i] = ConstArray[i];
		}
	}

	delete [] AxisArray;
	delete [] CoeffArray;
	delete [] ConstArray;
	
    return tmp;
	
}

//-------------------------------------------- Get_arrcpy_Info ------------------------------------------
//
//	FromBufferPtr=4be40c;	| FromArrayHeader=4bf32c;	FromArrayHandlePtr=833750; 
//	ToBufferPtr=4be38c;		| ToArrayHeader=498490;		ToArrayHandlePtr=805550;  
//	CopyRegim=2;
// 
//	if parameter FromArrayHeader exist:
//	[ FromInitIndexArray[0]=-1;	FromLastIndexArray[0]=2;	FromStepArray[0]=9; ] 
// 
//	if parameter  ToArrayHeader exist:
//	[ ToInitIndexArray[0]=-1;	ToLastIndexArray[0]=0;		ToStepArray[0]=1; ]  
//-------------------------------------------- Get_arrcpy_Info ------------------------------------------
/* 
 FromArrayHeader=12ff74; 
 ToArrayHeader=4ae560; 
 CopyRegim=0;
 
 FromInitIndexArray[0]=0;
 FromInitIndexArray[1]=-1;  
 FromLastIndexArray[0]=0; 
 FromLastIndexArray[1]=-1;  
 FromStepArray[0]=1;      
 FromStepArray[1]=1;
 
 ToInitIndexArray[0]=-1;  
 ToLastIndexArray[0]=0;  
 ToStepArray[0]=1;  

  FromArrayHeader[],
  FromInitIndexArray[],
  FromLastIndexArray[],
  FromStepArray[],

  ToArrayHeader[],
  ToInitIndexArray[],
  ToLastIndexArray[],
  ToStepArray[],
  CopyRegimPtr             

ret_aarrcp_                TIME=0.00000100 LINE=31     FILE=gauss_c.cdv
Res=11;
*/ 


arrcpy_Info* Get_arrcpy_Info(TraceCall& trc_call)
{
	bool	error = false;

	bool	FromBuffer = false;
    long *	FromInitIndexArray = NULL;
    long *	FromLastIndexArray = NULL;
    long *	FromStepArray = NULL;
	long	FromRank = 0;

	bool	ToBuffer = false;
    long *	ToInitIndexArray = NULL;
    long *	ToLastIndexArray = NULL;
    long *	ToStepArray = NULL;
	long	ToRank = 0;

	arrcpy_Info* tmp = new arrcpy_Info;
	assert(tmp != NULL);

	tmp->FromBufferPtr = 0;
    if (ParamFromStrArr("FromBufferPtr", tmp->FromBufferPtr, trc_call.call_info, trc_call.call_info_count, 16)) {
		FromBuffer = true;
	} else if (ParamFromStrArr("FromArrayHeader", tmp->FromArrayHeader, trc_call.call_info, trc_call.call_info_count, 16)) {
		if (!ParamFromStrArr("FromArrayHandlePtr", tmp->FromArrayHandlePtr, trc_call.call_info, trc_call.call_info_count, 16)) {
			ParamNotFoundErr("FromArrayHandlePtr");
			error = true;
		} else if ((FromInitIndexArray=GetArrayParam("FromInitIndexArray", trc_call.call_info, trc_call.call_info_count)) == NULL) {
			ParamNotFoundErr("FromInitIndexArray");
			error = true;
		} else if ((FromLastIndexArray=GetArrayParam("FromLastIndexArray", trc_call.call_info, trc_call.call_info_count)) == NULL) {
			ParamNotFoundErr("FromLastIndexArray");
			error = true;
		} else if ((FromStepArray=GetArrayParam("FromStepArray", trc_call.call_info, trc_call.call_info_count)) == NULL) {
			ParamNotFoundErr("FromStepArray");
			error = true;
		} else
			FromRank = arrayRank;
	} else {
        ParamNotFoundErr("FromBufferPtr/FromArrayHeader");
        error = true;
		goto err;
	}

	tmp->ToBufferPtr = 0;
    if (ParamFromStrArr("ToBufferPtr", tmp->ToBufferPtr, trc_call.call_info, trc_call.call_info_count, 16)) {
		ToBuffer = true;
	} else if (ParamFromStrArr("ToArrayHeader", tmp->ToArrayHeader, trc_call.call_info, trc_call.call_info_count, 16)) {
		if (!ParamFromStrArr("ToArrayHandlePtr", tmp->ToArrayHandlePtr, trc_call.call_info, trc_call.call_info_count, 16)) {
			ParamNotFoundErr("ToArrayHandlePtr");
			error = true;
		} else if ((ToInitIndexArray=GetArrayParam("ToInitIndexArray", trc_call.call_info, trc_call.call_info_count)) == NULL) {
			ParamNotFoundErr("ToInitIndexArray");
			error = true;
		} else if ((ToLastIndexArray=GetArrayParam("ToLastIndexArray", trc_call.call_info, trc_call.call_info_count)) == NULL) {
			ParamNotFoundErr("ToLastIndexArray");
			error = true;
		} else if ((ToStepArray=GetArrayParam("ToStepArray", trc_call.call_info, trc_call.call_info_count)) == NULL) {
			ParamNotFoundErr("ToStepArray");
			error = true;
		} else 
			ToRank = arrayRank;
	} else {
        ParamNotFoundErr("ToBufferPtr/ToArrayHeader");
        error = true;
		goto err;
	}

	if (FromBuffer || ToBuffer) {
		if (!ParamFromStrArr("CopyRegim", tmp->CopyRegim, trc_call.call_info, trc_call.call_info_count, 10)) {
			ParamNotFoundErr("CopyRegim");
			error = true;
		}
		tmp->CopyFlagPtr = 0;
	}

err:if (error) {
		delete tmp;
		tmp = NULL;
	} else { 
		if (!FromBuffer) {
			tmp->FromInitIndexArray.resize(FromRank);
			tmp->FromLastIndexArray.resize(FromRank);
			tmp->FromStepArray.resize(FromRank);
			for (int i=0; i < FromRank; i++) {
				tmp->FromInitIndexArray[i] = FromInitIndexArray[i];
				tmp->FromLastIndexArray[i] = FromLastIndexArray[i];
				tmp->FromStepArray[i] = FromStepArray[i];
			}
		}
		if (!ToBuffer) {
			tmp->ToInitIndexArray.resize(ToRank);
			tmp->ToLastIndexArray.resize(ToRank);
			tmp->ToStepArray.resize(ToRank);
			for (int i=0; i < ToRank; i++) {
				tmp->ToInitIndexArray[i] = ToInitIndexArray[i];
				tmp->ToLastIndexArray[i] = ToLastIndexArray[i];
				tmp->ToStepArray[i] = ToStepArray[i];
			}
		}
	}

	delete [] FromInitIndexArray;
	delete [] FromLastIndexArray;
	delete [] FromStepArray;

	delete [] ToInitIndexArray;
	delete [] ToLastIndexArray;
	delete [] ToStepArray;
//	cout << "FromRank = " << FromRank << ", ToRAnk = " << ToRank << endl;
	
    return tmp;
}

arrcpy_Info* Get_aarrcp_Info(TraceCall& trc_call)
{
	bool	error = false;

	bool	FromBuffer = false;
    long *	FromInitIndexArray = NULL;
    long *	FromLastIndexArray = NULL;
    long *	FromStepArray = NULL;
	long	FromRank = 0;

	bool	ToBuffer = false;
    long *	ToInitIndexArray = NULL;
    long *	ToLastIndexArray = NULL;
    long *	ToStepArray = NULL;
	long	ToRank = 0;

	arrcpy_Info* tmp = new arrcpy_Info;
	assert(tmp != NULL);

	if (!ParamFromStrArr("CopyFlagPtr", tmp->CopyFlagPtr, trc_call.call_info, trc_call.call_info_count, 16)) {
		ParamNotFoundErr("CopyFlagPtr");
		error = true;
	}

	tmp->FromBufferPtr = 0;
    if (ParamFromStrArr("FromBufferPtr", tmp->FromBufferPtr, trc_call.call_info, trc_call.call_info_count, 16)) {
		FromBuffer = true;
	} else if (ParamFromStrArr("FromArrayHeader", tmp->FromArrayHeader, trc_call.call_info, trc_call.call_info_count, 16)) {
		if (!ParamFromStrArr("FromArrayHandlePtr", tmp->FromArrayHandlePtr, trc_call.call_info, trc_call.call_info_count, 16)) {
			ParamNotFoundErr("FromArrayHandlePtr");
			error = true;
		} else if ((FromInitIndexArray=GetArrayParam("FromInitIndexArray", trc_call.call_info, trc_call.call_info_count)) == NULL) {
			ParamNotFoundErr("FromInitIndexArray");
			error = true;
		} else if ((FromLastIndexArray=GetArrayParam("FromLastIndexArray", trc_call.call_info, trc_call.call_info_count)) == NULL) {
			ParamNotFoundErr("FromLastIndexArray");
			error = true;
		} else if ((FromStepArray=GetArrayParam("FromStepArray", trc_call.call_info, trc_call.call_info_count)) == NULL) {
			ParamNotFoundErr("FromStepArray");
			error = true;
		} else
			FromRank = arrayRank;
	} else {
        ParamNotFoundErr("FromBufferPtr/FromArrayHeader");
        error = true;
		goto err;
	}

	tmp->ToBufferPtr = 0;
    if (ParamFromStrArr("ToBufferPtr", tmp->ToBufferPtr, trc_call.call_info, trc_call.call_info_count, 16)) {
		ToBuffer = true;
	} else if (ParamFromStrArr("ToArrayHeader", tmp->ToArrayHeader, trc_call.call_info, trc_call.call_info_count, 16)) {
		if (!ParamFromStrArr("ToArrayHandlePtr", tmp->ToArrayHandlePtr, trc_call.call_info, trc_call.call_info_count, 16)) {
			ParamNotFoundErr("ToArrayHandlePtr");
			error = true;
		} else if ((ToInitIndexArray=GetArrayParam("ToInitIndexArray", trc_call.call_info, trc_call.call_info_count)) == NULL) {
			ParamNotFoundErr("ToInitIndexArray");
			error = true;
		} else if ((ToLastIndexArray=GetArrayParam("ToLastIndexArray", trc_call.call_info, trc_call.call_info_count)) == NULL) {
			ParamNotFoundErr("ToLastIndexArray");
			error = true;
		} else if ((ToStepArray=GetArrayParam("ToStepArray", trc_call.call_info, trc_call.call_info_count)) == NULL) {
			ParamNotFoundErr("ToStepArray");
			error = true;
		} else 
			ToRank = arrayRank;
	} else {
        ParamNotFoundErr("ToBufferPtr/ToArrayHeader");
        error = true;
		goto err;
	}

	if (FromBuffer || ToBuffer) {
		if (!ParamFromStrArr("CopyRegim", tmp->CopyRegim, trc_call.call_info, trc_call.call_info_count, 10)) {
			ParamNotFoundErr("CopyRegim");
			error = true;
		}
	}

err:if (error) {
		delete tmp;
		tmp = NULL;
	} else { 
		if (!FromBuffer) {
			tmp->FromInitIndexArray.resize(FromRank);
			tmp->FromLastIndexArray.resize(FromRank);
			tmp->FromStepArray.resize(FromRank);
			for (int i=0; i < FromRank; i++) {
				tmp->FromInitIndexArray[i] = FromInitIndexArray[i];
				tmp->FromLastIndexArray[i] = FromLastIndexArray[i];
				tmp->FromStepArray[i] = FromStepArray[i];
			}
		}
		if (!ToBuffer) {
			tmp->ToInitIndexArray.resize(ToRank);
			tmp->ToLastIndexArray.resize(ToRank);
			tmp->ToStepArray.resize(ToRank);
			for (int i=0; i < ToRank; i++) {
				tmp->ToInitIndexArray[i] = ToInitIndexArray[i];
				tmp->ToLastIndexArray[i] = ToLastIndexArray[i];
				tmp->ToStepArray[i] = ToStepArray[i];
			}
		}
	}

	delete [] FromInitIndexArray;
	delete [] FromLastIndexArray;
	delete [] FromStepArray;

	delete [] ToInitIndexArray;
	delete [] ToLastIndexArray;
	delete [] ToStepArray;
//	cout << "FromRank = " << FromRank << ", ToRAnk = " << ToRank << endl;
	
	return tmp;
}

waitcp_Info* Get_waitcp_Info(TraceCall& trc_call)
{
	bool	error = false;
	waitcp_Info* tmp = new waitcp_Info;
	assert(tmp != NULL);

	if (!ParamFromStrArr("CopyFlagPtr", tmp->CopyFlagPtr, trc_call.call_info, trc_call.call_info_count, 16)) {
		ParamNotFoundErr("CopyFlagPtr");
		error = true;
	}
	if (error) {
		delete tmp;
		tmp = NULL;
	}
	return tmp;
}


//-------------------------------------------- Get_crtpl_Info ------------------------------------------

crtpl_Info* Get_crtpl_Info(TraceCall& trc_call)
{
	bool error = false;

	crtpl_Info* tmp = new crtpl_Info;
	assert(tmp != NULL);

    if(!ParamFromStrArr("LoopRef", tmp->ID, trc_call.ret_info, trc_call.ret_info_count, 16)) {
        ParamNotFoundErr("LoopRef");
        error = true;
    } else if(!ParamFromStrArr("Rank", tmp->Rank, trc_call.call_info, trc_call.call_info_count, 10)) {
        ParamNotFoundErr("Rank");
        error = true;
    }

	if (error) {
		delete tmp;
		return NULL;
	} else
		return tmp;
}

//-------------------------------------------- Get_exfrst_Info ------------------------------------------

exfrst_Info* Get_exfrst_Info(TraceCall& trc_call)
{
	bool error = false;

	exfrst_Info* tmp = new exfrst_Info;
	assert(tmp != NULL);

    if(!ParamFromStrArr("LoopRef", tmp->ID, trc_call.call_info, trc_call.call_info_count, 16)) {
        ParamNotFoundErr("LoopRef");
        error = true;
    } else if(!ParamFromStrArr("ShadowGroupRef", tmp->ShadowGroupRef, trc_call.call_info, trc_call.call_info_count, 16)) {
        ParamNotFoundErr("ShadowGroupRef");
        error = true;
    }

	if (error) {
		delete tmp;
		return NULL;
	} else
		return tmp;
}

//-------------------------------------------- Get_across_Info ------------------------------------------

/*
call_across_               TIME=0.00000100 LINE=81     FILE=jacr_3dK.cdv
AcrossType=0; 
OldShadowGroupRef=8c5cc0; 
NewShadowGroupRef=8c5b50; 
PipeLinePar=0;
CondPipeLine=0  
ErrPipeLine=30
ret_across_                TIME=0.00000100 LINE=81     FILE=jacr_3dK.cdv
PipeLinePLAxis=0
*/

across_Info* Get_across_Info(TraceCall& trc_call)
{
	bool error = false;

	across_Info* tmp = new across_Info;
	assert(tmp != NULL);

    if(!ParamFromStrArr("AcrossType", tmp->AcrossType, trc_call.call_info, trc_call.call_info_count, 10)) {
        ParamNotFoundErr("AcrossType");
        error = true;
	} else if(!ParamFromStrArr("OldShadowGroupRef", tmp->OldShadowGroupRef, trc_call.call_info, trc_call.call_info_count, 16)) {
        ParamNotFoundErr("OldShadowGroupRef");
        error = true;
	} else if(!ParamFromStrArr("NewShadowGroupRef", tmp->NewShadowGroupRef, trc_call.call_info, trc_call.call_info_count, 16)) {
        ParamNotFoundErr("NewShadowGroupRef");
        error = true;
	} else if (!ParamFromStrArr("PipeLinePar", tmp->PipeLinePar, trc_call.call_info, trc_call.call_info_count/*, 10*/)) {
        ParamNotFoundErr("PipeLinePar");
        error = true;
	} else if (!ParamFromStrArr("CondPipeLine", tmp->CondPipeLine, trc_call.call_info, trc_call.call_info_count, 10)) {
        ParamNotFoundErr("CondPipeLine");
        error = true;
	} else if (!ParamFromStrArr("ErrPipeLine", tmp->ErrPipeLine, trc_call.call_info, trc_call.call_info_count, 10)) {
        ParamNotFoundErr("ErrPipeLine");
        error = true;
//	} else if (!ParamFromStrArr("PipeLinePLAxis", tmp->PipeLinePLAxis, trc_call.ret_info, trc_call.ret_info_count, 10)) {
//        ParamNotFoundErr("PipeLinePLAxis");
//        error = true;
	}

 	if (error) {
		delete tmp;
		return NULL;
	} else
		return tmp;
}

//-------------------------------------------- Get_mappl_Info ------------------------------------------

mappl_Info* Get_mappl_Info(TraceCall& trc_call)
{
	bool		error = false;
	long	*	AxisArray = NULL;
	long	*	CoeffArray = NULL;
	long	*	ConstArray = NULL;
	long	*	InInitIndexArray = NULL;
	long	*	InLastIndexArray = NULL;
	long	*	InStepArray = NULL;
	int			axisRank = 0;
	int			i;


	mappl_Info* tmp = new mappl_Info;
	assert(tmp != NULL);

    if (!ParamFromStrArr("LoopRef", tmp->LoopRef, trc_call.call_info, trc_call.call_info_count, 16)) {
        ParamNotFoundErr("LoopRef");
		error = true;
    } else if (!ParamFromStrArr("PatternRefPtr", tmp->PatternRefPtr, trc_call.call_info, trc_call.call_info_count, 16)) {
        ParamNotFoundErr("PatternRefPtr");
		error = true;
	} else if (!ParamFromStrArr("PatternRef", tmp->PatternRef, trc_call.call_info, trc_call.call_info_count, 16)) {
        ParamNotFoundErr("PatternRef");
		error = true;
	} else if ((tmp->PatternType = ModifierFromStrArr(trc_call.call_info, trc_call.call_info_count)) == 0) {
        ParamNotFoundErr("(AMView)/(DisArray)");
		error = true;
    } else { 
		if ((AxisArray=GetArrayParam("AxisArray", trc_call.call_info, trc_call.call_info_count)) != NULL) {
			axisRank = arrayRank;
		} else {
			ParamNotFoundErr("AxisArray");
			error = true;
			goto err;
		}
	}
	if ((CoeffArray=GetArrayParam("CoeffArray", trc_call.call_info, trc_call.call_info_count)) == NULL) {
        ParamNotFoundErr("CoeffArray");
		error = true;
	} else if ((ConstArray=GetArrayParam("ConstArray", trc_call.call_info, trc_call.call_info_count)) == NULL) {
        ParamNotFoundErr("CoeffArray");
		error = true;
    } else if ((InInitIndexArray=GetArrayParam("InInitIndexArray", trc_call.call_info, trc_call.call_info_count)) == NULL) {
        ParamNotFoundErr("InInitIndexArray");
		error = true;
	} else if ((InLastIndexArray=GetArrayParam("InLastIndexArray", trc_call.call_info, trc_call.call_info_count)) == NULL) {
        ParamNotFoundErr("InLastIndexArray");
		error = true;
	} else if ((InStepArray=GetArrayParam("InStepArray", trc_call.call_info, trc_call.call_info_count)) == NULL) {
        ParamNotFoundErr("InStepArray");
		error = true;
	}
err:
	if (error) {
		delete tmp;
		tmp = NULL;
	} else {
		tmp->AxisArray.resize(axisRank);
		tmp->CoeffArray.resize(axisRank);
		tmp->ConstArray.resize(axisRank);

		tmp->InInitIndexArray.resize(arrayRank);
		tmp->InLastIndexArray.resize(arrayRank);
		tmp->InStepArray.resize(arrayRank);
		for (i=0; i < axisRank; i++) {
			tmp->AxisArray[i] = AxisArray[i];
//		prot << trc_call.source_line << ": i=" << i << ", AxisArray[i]=" << AxisArray[i] << endl;
			tmp->CoeffArray[i] = CoeffArray[i];
			tmp->ConstArray[i] = ConstArray[i];
		}
		for (i=0; i < arrayRank; i++) {
			tmp->InInitIndexArray[i] = InInitIndexArray[i];
			tmp->InLastIndexArray[i] = InLastIndexArray[i];
			tmp->InStepArray[i] = InStepArray[i];
		}
	}
	
	delete [] AxisArray;
	delete [] CoeffArray;
	delete [] ConstArray;
	delete [] InInitIndexArray;
	delete [] InLastIndexArray;
	delete [] InStepArray;

    return tmp;
}
//---------------Get_dopl_Info----------------------
//grig

dopl_full_Info* Get_dopl_Info(TraceCall& trc_call)
{
   bool error = false;
   long * AStep=NULL;
   long * ALow=NULL;
   long * AHigh=NULL;
   long * ADim=NULL;
   bool thereisDim=false;
   long DimRank,HighRank,LowRank,stepRank;


	dopl_full_Info* tmp = new dopl_full_Info;
	assert(tmp != NULL);
	
//====
  if(!ParamFromStrArr("LoopRef", tmp->ID, trc_call.call_info, trc_call.call_info_count, 16)) {
    ParamNotFoundErr("LoopRef");
    error = true;
  } 
	if(!ParamFromStrArr("DoPL", tmp->ReturnVar, trc_call.ret_info, trc_call.ret_info_count, 10)) {
	    ParamNotFoundErr("DoPL");
      error = true;
  }
//=***

//was-grig	ParamFromStrArr("LoopRef", tmp->ID, trc_call.call_info, trc_call.call_info_count, 16);
	//tmp->ID=Get_IDOnlyInfo("LoopRef",trc_call.call_info,trc_call.call_info_count,16);	
	if(!(ADim=GetArrayParam("Dim",trc_call.call_info,trc_call.call_info_count))==NULL)
	{
		thereisDim=true;	
		DimRank=arrayRank;
	}
	if(thereisDim)
	{
	ALow=GetArrayParam("Lower",trc_call.call_info,trc_call.call_info_count);
	LowRank=arrayRank;
	AHigh=GetArrayParam("Upper",trc_call.call_info,trc_call.call_info_count);
	HighRank=arrayRank;
	AStep=GetArrayParam("Step",trc_call.call_info,trc_call.call_info_count);
	stepRank=arrayRank;
	}

	
	if(thereisDim)
	{
		if(DimRank!=LowRank)
		{
			printf("something wrong with dopl _function - DimRank!=Lowrank\n");		
		}

		tmp->Dim.resize(DimRank);
		tmp->Lower.resize(LowRank);
		tmp->Upper.resize(HighRank);
		tmp->Step.resize(stepRank);

		for(int i=0;i<DimRank;i++)
		{
			tmp->Dim[i]=ADim[i];
			tmp->Lower[i]=ALow[i];
			tmp->Upper[i]=AHigh[i];
			tmp->Step[i]=AStep[i];
		}	
	}
 return tmp;
}
//\grig



//-------------------------------------------- Get_crtred_Info ------------------------------------------

crtred_Info* Get_crtred_Info(TraceCall& trc_call)
{
	bool			error = false;
	crtred_Info*	tmp = new crtred_Info;
	assert(tmp != NULL);

    if (!ParamFromStrArr("RedRef", tmp->ID, trc_call.ret_info, trc_call.ret_info_count, 16)) {
        ParamNotFoundErr("RedRef");
        error = true;
    } else if (!ParamFromStrArr("RedArrayType", tmp->RedArrayType, trc_call.call_info, trc_call.call_info_count, 10)) {
        ParamNotFoundErr("RedArrayType");
        error = true;
    } else if (!ParamFromStrArr("RedArrayLength", tmp->RedArrayLength, trc_call.call_info, trc_call.call_info_count, 10)) {
        ParamNotFoundErr("RedArrayLength");
        error = true;
    } else if (!ParamFromStrArr("LocElmLength", tmp->LocElmLength, trc_call.call_info, trc_call.call_info_count, 10)) {
        ParamNotFoundErr("LocElmLength");
        error = true;
    }

	if (error) {
		delete tmp;
		return NULL;
	} else 
	    return tmp;
}

//-------------------------------------------- Get_insred_Info ------------------------------------------

insred_Info* Get_insred_Info(TraceCall& trc_call)
{
	bool			error = false;
	insred_Info*	tmp = new insred_Info;
	assert(tmp != NULL);

    if (!ParamFromStrArr("RedGroupRef", tmp->RG_ID, trc_call.call_info, trc_call.call_info_count, 16)) {
        ParamNotFoundErr("RedGroupRef");
        error = true;
    } else if (!ParamFromStrArr("RedRef", tmp->RV_ID, trc_call.call_info, trc_call.call_info_count, 16)) {
        ParamNotFoundErr("RedRef");
        error = true;
    }

	if (error) {
		delete tmp;
		return NULL;
	} else 
	    return tmp;
}

//------------------------------------------- Get_crtshg_Info ------------------------------------------

crtshg_Info* Get_crtshg_Info(TraceCall& trc_call)
{
	bool			error = false;
	crtshg_Info*	tmp = new crtshg_Info;
	assert(tmp != NULL);

    if (!ParamFromStrArr("StaticSign", tmp->StaticSign, trc_call.call_info, trc_call.call_info_count, 10)) {
        ParamNotFoundErr("StaticSign");
        error = true;
    } else if (!ParamFromStrArr("ShadowGroupRef", tmp->ShadowGroupRef, trc_call.ret_info, trc_call.ret_info_count, 16)) {
        ParamNotFoundErr("ShadowGroupRef");
        error = true;
    }

	if (error) {
		delete tmp;
		return NULL;
	} else 
	    return tmp;
}

//-------------------------------------------- Get_inssh_Info ------------------------------------------

inssh_Info* Get_inssh_Info(TraceCall& trc_call)
{
	bool		error = false;
	long*		LowShdWidthArray = NULL;
	long*		HiShdWidthArray = NULL;	

	inssh_Info* tmp = new inssh_Info;
	assert(tmp != NULL);

	if (!ParamFromStrArr("ShadowGroupRef", tmp->ShadowGroupRef, trc_call.call_info, trc_call.call_info_count, 16)) {
		ParamNotFoundErr("ShadowGroupRef");
		error = true;
	} else if (!ParamFromStrArr("ArrayHeader", tmp->ArrayHeader, trc_call.call_info, trc_call.call_info_count, 16)) {
        ParamNotFoundErr("ArrayHeader");
        error = true;
	} else if (!ParamFromStrArr("ArrayHandlePtr", tmp->ArrayHandlePtr, trc_call.call_info, trc_call.call_info_count, 16)) {
        ParamNotFoundErr("ArrayHandlePtr");
        error = true;
    } else if (!ParamFromStrArr("FullShdSign", tmp->FullShdSign, trc_call.call_info, trc_call.call_info_count, 10)) {
        ParamNotFoundErr("FullShdSign");
        error = true;
    } else if ((LowShdWidthArray=GetArrayParam("LowShdWidthArray", trc_call.call_info, trc_call.call_info_count)) == NULL) {
        ParamNotFoundErr("LowShdWidthArray");
		error = true;
	} else if ((HiShdWidthArray=GetArrayParam("HiShdWidthArray", trc_call.call_info, trc_call.call_info_count)) == NULL) {
        ParamNotFoundErr("HiShdWidthArray");
		error = true;
	}

   	if (error) {
		delete tmp;
		tmp = NULL;
	} else {
		tmp->LowShdWidthArray.resize(arrayRank);
		tmp->HiShdWidthArray.resize(arrayRank);
		for (int i=0; i < arrayRank; i++) {
//====
//			ShdWid[i]=abs(LowShdWidthArray[i]);//<HiShdWidthArray[i])?HiShdWidthArray[i]:LowShdWidthArray[i];
//=***
			tmp->LowShdWidthArray[i] = LowShdWidthArray[i];
			tmp->HiShdWidthArray[i] = HiShdWidthArray[i];
//====
//printf("tmp->LowShdWidthArray[i]=%d tmp->HiShdWidthArray[i]=%d\n",tmp->LowShdWidthArray[i],tmp->HiShdWidthArray[i]);		
//printf("ShdWid[%d]=%d\n",i,ShdWid[i]);		
//=***
		}
	}
	

	delete [] LowShdWidthArray;
	delete [] HiShdWidthArray;

	tmp->func = inssh_;

    return tmp;
}

//-------------------------------------------- Get_insshd_Info -----------------------------------------

inssh_Info* Get_insshd_Info(TraceCall& trc_call)
{
	bool		error = false;
	long*		LowShdWidthArray = NULL;
	long*		HiShdWidthArray = NULL;	
	long*		ShdSignArray = NULL;	

	inssh_Info* tmp = new inssh_Info;
	assert(tmp != NULL);

	if (!ParamFromStrArr("ShadowGroupRef", tmp->ShadowGroupRef, trc_call.call_info, trc_call.call_info_count, 16)) {
		ParamNotFoundErr("ShadowGroupRef");
		error = true;
	} else if (!ParamFromStrArr("ArrayHeader", tmp->ArrayHeader, trc_call.call_info, trc_call.call_info_count, 16)) {
        ParamNotFoundErr("ArrayHeader");
        error = true;
	} else if (!ParamFromStrArr("ArrayHandlePtr", tmp->ArrayHandlePtr, trc_call.call_info, trc_call.call_info_count, 16)) {
        ParamNotFoundErr("ArrayHandlePtr");
        error = true;
    } else if (!ParamFromStrArr("MaxShdCount", tmp->MaxShdCount, trc_call.call_info, trc_call.call_info_count, 10)) {
        ParamNotFoundErr("MaxShdCount");
        error = true;
    } else if ((LowShdWidthArray=GetArrayParam("LowShdWidthArray", trc_call.call_info, trc_call.call_info_count)) == NULL) {
        ParamNotFoundErr("LowShdWidthArray");
		error = true;
	} else if ((HiShdWidthArray=GetArrayParam("HiShdWidthArray", trc_call.call_info, trc_call.call_info_count)) == NULL) {
        ParamNotFoundErr("HiShdWidthArray");
		error = true;
	} else if ((ShdSignArray=GetArrayParam("ShdSignArray", trc_call.call_info, trc_call.call_info_count)) == NULL) {
        ParamNotFoundErr("ShdSignArray");
		error = true;
	}

   	if (error) {
		delete tmp;
		tmp = NULL;
	} else {
		tmp->LowShdWidthArray.resize(arrayRank);
		tmp->HiShdWidthArray.resize(arrayRank);
		tmp->ShdSignArray.resize(arrayRank);
		for (int i=0; i < arrayRank; i++) {
			tmp->LowShdWidthArray[i] = LowShdWidthArray[i];
			tmp->HiShdWidthArray[i] = HiShdWidthArray[i];
			tmp->ShdSignArray[i] = ShdSignArray[i];
		}
	}
	
	delete [] LowShdWidthArray;
	delete [] HiShdWidthArray;
	delete [] ShdSignArray;

	tmp->func = insshd_;

    return tmp;
}

//-------------------------------------------- Get_incsh_Info ------------------------------------------

inssh_Info* Get_incsh_Info(TraceCall& trc_call)
{
	bool		error = false;

	long*		InitDimIndex = NULL;
	long*		LastDimIndex = NULL;	

	long*		InitLowShdIndex = NULL;
	long*		LastLowShdIndex = NULL;	

	long*		InitHiShdIndex = NULL;
	long*		LastHiShdIndex = NULL;	

	inssh_Info* tmp = new inssh_Info;
	assert(tmp != NULL);

	if (!ParamFromStrArr("ShadowGroupRef", tmp->ShadowGroupRef, trc_call.call_info, trc_call.call_info_count, 16)) {
		ParamNotFoundErr("ShadowGroupRef");
		error = true;
	} else if (!ParamFromStrArr("ArrayHeader", tmp->ArrayHeader, trc_call.call_info, trc_call.call_info_count, 16)) {
        ParamNotFoundErr("ArrayHeader");
        error = true;
	} else if (!ParamFromStrArr("ArrayHandlePtr", tmp->ArrayHandlePtr, trc_call.call_info, trc_call.call_info_count, 16)) {
        ParamNotFoundErr("ArrayHandlePtr");
        error = true;
    } else if (!ParamFromStrArr("FullShdSign", tmp->FullShdSign, trc_call.call_info, trc_call.call_info_count, 10)) {
        ParamNotFoundErr("FullShdSign");
        error = true;

    } else if ((InitDimIndex=GetArrayParam("InitDimIndex", trc_call.call_info, trc_call.call_info_count)) == NULL) {
        ParamNotFoundErr("InitDimIndex");
		error = true;
	} else if ((LastDimIndex=GetArrayParam("LastDimIndex", trc_call.call_info, trc_call.call_info_count)) == NULL) {
        ParamNotFoundErr("LastDimIndex");
		error = true;

    } else if ((InitLowShdIndex=GetArrayParam("InitLowShdIndex", trc_call.call_info, trc_call.call_info_count)) == NULL) {
        ParamNotFoundErr("InitLowShdIndex");
		error = true;
	} else if ((LastLowShdIndex=GetArrayParam("LastLowShdIndex", trc_call.call_info, trc_call.call_info_count)) == NULL) {
        ParamNotFoundErr("LastLowShdIndex");
		error = true;

    } else if ((InitHiShdIndex=GetArrayParam("InitHiShdIndex", trc_call.call_info, trc_call.call_info_count)) == NULL) {
        ParamNotFoundErr("InitHiShdIndex");
		error = true;
	} else if ((LastHiShdIndex=GetArrayParam("LastHiShdIndex", trc_call.call_info, trc_call.call_info_count)) == NULL) {
        ParamNotFoundErr("LastHiShdIndex");
		error = true;

	}

   	if (error) {
		delete tmp;
		tmp = NULL;
	} else {
		tmp->InitDimIndex.resize(arrayRank);
		tmp->LastDimIndex.resize(arrayRank);

		tmp->InitLowShdIndex.resize(arrayRank);
		tmp->LastLowShdIndex.resize(arrayRank);

		tmp->InitHiShdIndex.resize(arrayRank);
		tmp->LastHiShdIndex.resize(arrayRank);

		for (int i=0; i < arrayRank; i++) {
			tmp->InitDimIndex[i] = InitDimIndex[i];
			tmp->LastDimIndex[i] = LastDimIndex[i];

			tmp->InitLowShdIndex[i] = InitLowShdIndex[i];
			tmp->LastLowShdIndex[i] = LastLowShdIndex[i];

			tmp->InitHiShdIndex[i] = InitHiShdIndex[i];
			tmp->LastHiShdIndex[i] = LastHiShdIndex[i];

		}
	}
	
	delete [] InitDimIndex;
	delete [] LastDimIndex;

	delete [] InitLowShdIndex;
	delete [] LastLowShdIndex;

	delete [] InitHiShdIndex;
	delete [] LastHiShdIndex;

	tmp->func = incsh_;

    return tmp;
}

//-------------------------------------------- Get_incshd_Info -----------------------------------------

inssh_Info* Get_incshd_Info(TraceCall& trc_call)
{
	bool		error = false;
	long*		ShdSignArray = NULL;	

	long*		InitDimIndex = NULL;
	long*		LastDimIndex = NULL;	

	long*		InitLowShdIndex = NULL;
	long*		LastLowShdIndex = NULL;	

	long*		InitHiShdIndex = NULL;
	long*		LastHiShdIndex = NULL;	


	inssh_Info* tmp = new inssh_Info;
	assert(tmp != NULL);

	if (!ParamFromStrArr("ShadowGroupRef", tmp->ShadowGroupRef, trc_call.call_info, trc_call.call_info_count, 16)) {
		ParamNotFoundErr("ShadowGroupRef");
		error = true;
	} else if (!ParamFromStrArr("ArrayHeader", tmp->ArrayHeader, trc_call.call_info, trc_call.call_info_count, 16)) {
        ParamNotFoundErr("ArrayHeader");
        error = true;
	} else if (!ParamFromStrArr("ArrayHandlePtr", tmp->ArrayHandlePtr, trc_call.call_info, trc_call.call_info_count, 16)) {
        ParamNotFoundErr("ArrayHandlePtr");
        error = true;
    } else if (!ParamFromStrArr("MaxShdCount", tmp->MaxShdCount, trc_call.call_info, trc_call.call_info_count, 10)) {
        ParamNotFoundErr("MaxShdCount");
        error = true;

    } else if ((InitDimIndex=GetArrayParam("InitDimIndex", trc_call.call_info, trc_call.call_info_count)) == NULL) {
        ParamNotFoundErr("InitDimIndex");
		error = true;
	} else if ((LastDimIndex=GetArrayParam("LastDimIndex", trc_call.call_info, trc_call.call_info_count)) == NULL) {
        ParamNotFoundErr("LastDimIndex");
		error = true;

    } else if ((InitLowShdIndex=GetArrayParam("InitLowShdIndex", trc_call.call_info, trc_call.call_info_count)) == NULL) {
        ParamNotFoundErr("InitLowShdIndex");
		error = true;
	} else if ((LastLowShdIndex=GetArrayParam("LastLowShdIndex", trc_call.call_info, trc_call.call_info_count)) == NULL) {
        ParamNotFoundErr("LastLowShdIndex");
		error = true;

    } else if ((InitHiShdIndex=GetArrayParam("InitHiShdIndex", trc_call.call_info, trc_call.call_info_count)) == NULL) {
        ParamNotFoundErr("InitHiShdIndex");
		error = true;
	} else if ((LastHiShdIndex=GetArrayParam("LastHiShdIndex", trc_call.call_info, trc_call.call_info_count)) == NULL) {
        ParamNotFoundErr("LastHiShdIndex");
		error = true;

	} else if ((ShdSignArray=GetArrayParam("ShdSignArray", trc_call.call_info, trc_call.call_info_count)) == NULL) {
        ParamNotFoundErr("ShdSignArray");
		error = true;
	}

   	if (error) {
		delete tmp;
		tmp = NULL;
	} else {
		tmp->InitDimIndex.resize(arrayRank);
		tmp->LastDimIndex.resize(arrayRank);

		tmp->InitLowShdIndex.resize(arrayRank);
		tmp->LastLowShdIndex.resize(arrayRank);

		tmp->InitHiShdIndex.resize(arrayRank);
		tmp->LastHiShdIndex.resize(arrayRank);

		tmp->ShdSignArray.resize(arrayRank);

		for (int i=0; i < arrayRank; i++) {
			tmp->InitDimIndex[i] = InitDimIndex[i];
			tmp->LastDimIndex[i] = LastDimIndex[i];

			tmp->InitLowShdIndex[i] = InitLowShdIndex[i];
			tmp->LastLowShdIndex[i] = LastLowShdIndex[i];

			tmp->InitHiShdIndex[i] = InitHiShdIndex[i];
			tmp->LastHiShdIndex[i] = LastHiShdIndex[i];

			tmp->ShdSignArray[i] = ShdSignArray[i];
		}
	}
	
	delete [] InitDimIndex;
	delete [] LastDimIndex;

	delete [] InitLowShdIndex;
	delete [] LastLowShdIndex;

	delete [] InitHiShdIndex;
	delete [] LastHiShdIndex;

	delete [] ShdSignArray;

	tmp->func = incshd_;

    return tmp;
}

//----------------------------------------- Get_crtrbp_Info ---------------------------------------

crtrbp_Info * Get_crtrbp_Info(TraceCall& trc_call)
{
	bool		error = false;
	long*		coordArray = NULL;

	crtrbp_Info* tmp = new crtrbp_Info;
	assert(tmp != NULL);

	if (!ParamFromStrArr("RemArrayHeader", tmp->RemArrayHeader, trc_call.call_info, trc_call.call_info_count, 16)) {
		ParamNotFoundErr("RemArrayHeader");
		error = true;
	} else if (!ParamFromStrArr("BufferHeader", tmp->ID, trc_call.call_info, trc_call.call_info_count, 16)) {
        ParamNotFoundErr("BufferHeader");
        error = true;
	} else if (!ParamFromStrArr("StaticSign", tmp->StaticSign, trc_call.call_info, trc_call.call_info_count, 10)) {
        ParamNotFoundErr("StaticSign");
        error = true;
	} else if (!ParamFromStrArr("PSRef", tmp->PSRef, trc_call.call_info, trc_call.call_info_count, 16)) {
        ParamNotFoundErr("PSRef");
        error = true;
    } else if ((coordArray=GetArrayParam("CoordArray", trc_call.call_info, trc_call.call_info_count)) == NULL) {
        ParamNotFoundErr("CoordArray");
		error = true;
    } else if (!ParamFromStrArr("IsLocal", tmp->IsLocal, trc_call.ret_info, trc_call.ret_info_count, 10)) {
        ParamNotFoundErr("IsLocal");
        error = true;
    }
   	if (error) {
		delete tmp;
		tmp = NULL;
	} else {
		tmp->CoordArray.resize(arrayRank);
		for (int i=0; i < arrayRank; i++) {
			tmp->CoordArray[i] = coordArray[i];
		}
	}
	
	delete [] coordArray;

    return tmp;
}
//----------------------------------------- Get_crtrbl_Info ---------------------------------------

crtrbl_Info * Get_crtrbl_Info(TraceCall& trc_call)
{
	bool		error = false;
	long*		AxisArray = NULL;
	long*		CoeffArray = NULL;
	long*		ConstArray = NULL;

	crtrbl_Info* tmp = new crtrbl_Info;
	assert(tmp != NULL);

	if (!ParamFromStrArr("RemArrayHeader", tmp->RemArrayHeader, trc_call.call_info, trc_call.call_info_count, 16)) {
		ParamNotFoundErr("RemArrayHeader");
		error = true;
	} else if (!ParamFromStrArr("BufferHeader", tmp->BufferHeader, trc_call.call_info, trc_call.call_info_count, 16)) {
        ParamNotFoundErr("BufferHeader");
        error = true;
	} else if (!ParamFromStrArr("StaticSign", tmp->StaticSign, trc_call.call_info, trc_call.call_info_count, 10)) {
        ParamNotFoundErr("StaticSign");
        error = true;
	} else if (!ParamFromStrArr("LoopRef", tmp->LoopRef, trc_call.call_info, trc_call.call_info_count, 16)) {
        ParamNotFoundErr("LoopRef");
        error = true;
    } else if ((AxisArray=GetArrayParam("AxisArray", trc_call.call_info, trc_call.call_info_count)) == NULL) {
        ParamNotFoundErr("AxisArray");
		error = true;
    } else if ((CoeffArray=GetArrayParam("CoeffArray", trc_call.call_info, trc_call.call_info_count)) == NULL) {
        ParamNotFoundErr("CoeffArray");
		error = true;
    } else if ((ConstArray=GetArrayParam("ConstArray", trc_call.call_info, trc_call.call_info_count)) == NULL) {
        ParamNotFoundErr("ConstArray");
		error = true;
    }
   	if (error) {
		delete tmp;
		tmp = NULL;
	} else {
		tmp->AxisArray.resize(arrayRank);
		tmp->CoeffArray.resize(arrayRank);
		tmp->ConstArray.resize(arrayRank);
		for (int i=0; i < arrayRank; i++) {
			tmp->AxisArray[i] = AxisArray[i];
			tmp->CoeffArray[i] = CoeffArray[i];
			tmp->ConstArray[i] = ConstArray[i];
		}
	}
	
	delete [] AxisArray;
	delete [] CoeffArray;
	delete [] ConstArray;

    return tmp;
}

//----------------------------------------- Get_loadrb_Info ---------------------------------------
loadrb_Info * Get_loadrb_Info(TraceCall& trc_call)
{
	bool		error = false;

	loadrb_Info* tmp = new loadrb_Info;
	assert(tmp != NULL);

	if (!ParamFromStrArr("BufferHeader", tmp->ID, trc_call.call_info, trc_call.call_info_count, 16)) {
		ParamNotFoundErr("BufferHeader");
		error = true;
	} else if (!ParamFromStrArr("RenewSign", tmp->RenewSign, trc_call.call_info, trc_call.call_info_count, 10)) {
        ParamNotFoundErr("RenewSign");
        error = true;
	}
   	if (error) {
		delete tmp;
		tmp = NULL;
	}
    return tmp;

}

srmem_Info * Get_srmem_Info(TraceCall& trc_call)
{
	bool		error = false;
	long*		lengthArray = NULL;

	srmem_Info* tmp = new srmem_Info;
	assert(tmp != NULL);
	if (!ParamFromStrArr("MemoryCount", tmp->MemoryCount, trc_call.call_info, trc_call.call_info_count, 10)) {
		ParamNotFoundErr("MemoryCount");
		error = true;
    } else if ((lengthArray=GetArrayParam("LengthArray", trc_call.call_info, trc_call.call_info_count)) == NULL) {
        ParamNotFoundErr("LengthArray");
		error = true;
    }
   	if (error) {
		delete tmp;
		tmp = NULL;
	} else {
		tmp->LengthArray.resize(arrayRank);
		for (int i=0; i < arrayRank; i++) {
			tmp->LengthArray[i] = lengthArray[i];
		}
	}
	
	delete [] lengthArray;

    return tmp;
}


//----------------------------------------- Service routines ---------------------------------------

root_Info* Get_Root_Info()
{
	bool				error = false;
	unsigned long*		VPSSize = NULL;
/*	traceLines */

	root_Info* tmp = new root_Info;
	assert(tmp != NULL);

	if (strcmp(traceLines->p_lines[0], RTS_VERSION) != 0) {
		cerr << " NEEDED " << RTS_VERSION << endl;
		prot << " NEEDED " << RTS_VERSION << endl;
		//grig!!!!
		//abort();
		//\grig
	}

	if (!ParamFromStrArr("ProcCount", TraceProcNum, traceLines->p_lines, traceLines->p_size, 10)) {
//		ParamNotFoundErr("ProcCount");
//		error = true;
	}

	if (!ParamFromStrArr("VProcCount", tmp->VProcCount, traceLines->p_lines, 
			traceLines->p_size, 10)) {
		ParamNotFoundErr("VProcCount");
		error = true;
	}

	if (tmp->VProcCount != 0) {
		if (!ParamFromStrArr("VPSRank", tmp->VPSRank, traceLines->p_lines, traceLines->p_size, 10)) {
			ParamNotFoundErr("VPSRank");
			error = true;
		} else if ((VPSSize=GetVectorParam("VPSSize", traceLines->p_lines, traceLines->p_size)) == NULL) {
			ParamNotFoundErr("VPSSize");
			error = true;
		}
	}

/*my grig
  	if (error) {
		delete tmp;
		tmp = NULL;
	} else {
		tmp->VPSSize.resize(arrayRank);
		for (int i=0; i < arrayRank; i++) {
			tmp->VPSSize[i] = VPSSize[i];
		}
	}
*/	
	delete [] VPSSize;

    return tmp;
}
//----------------------------------------- Service routines ---------------------------------------

long* GetArrayParam(char* param_name, char** str_list, int str_count)
{ 
	long* arr_param=NULL;
    long tmp;
    int i=0;

	arrayRank = 0;

/*	printf("in get array param serching '%s'\n",param_name);
	for(int ii=0;ii<str_count;ii++)
	{
		printf("str_list[%d] = %s \n",ii,str_list[ii]);
	
	}*/

    while (IndexParamFromStrArr(param_name, i++, tmp, str_list, str_count, 10)) {
        arr_param = (long*)realloc(arr_param,i*sizeof(long));
		arrayRank++;
		assert(arr_param != NULL);
        arr_param[i-1]=tmp;
    }
    return arr_param;
}

//grig
double* GetArrayParamD(char* param_name, char** str_list, int str_count)
{ 
	double* arr_param=NULL;
    double tmp;
    int i=0;

	arrayRank = 0;

	while (IndexParamFromStrArr(param_name, i++, tmp, str_list, str_count)) {
        arr_param = (double*)realloc(arr_param,i*sizeof(double));
		arrayRank++;
		assert(arr_param != NULL);
        arr_param[i-1]=tmp;
    }
    return arr_param;
}
//\grig







int ParamFromString1(char * par_name, char* par_val, char* str)
{
	char* pos;
    int len;
//	prot << "par_name=" << par_name << " par_val=" << par_val << endl;
    if (str==NULL) 
		return 0;
    pos = strstr(str,par_name);
    if(pos==NULL) 
		return 0;
    pos += strlen(par_name);
    len=strcspn(++pos,";");
    strncpy(par_val,pos,len);
    par_val[len]='\0';
    return 1;
}


int ParamFromStrArr1(char * par_name, char*& par_val, char** str_arr, int str_cnt)
  { int i;
    int res = 0;
    if(str_cnt<=0) return 0;
    for(i=0; i<str_cnt; i++)
      if ((res=ParamFromString1(par_name, par_val, str_arr[i])) != 0) break;
    return res;
  };


unsigned long* GetVectorParam(char* param_name, char** str_list, int str_count)
{ 
	unsigned long*	arr_param = NULL;
	char	*buffer = new char[256];
//	char	*str = buffer;

	arrayRank = 0;

	if (ParamFromStrArr1(param_name, buffer, str_list, str_count) == 0)
		return NULL;
	do {
		while (buffer[0] == ' ')
			buffer++;
		arrayRank++;
		arr_param = (unsigned long*)realloc(arr_param,arrayRank * sizeof(unsigned long));
		assert(arr_param != NULL);
        arr_param[arrayRank-1] = strtoul(buffer, &buffer, 10);;
	} while (strlen(buffer) != 0);

    return arr_param;
}

//-------------------------------------------- Main switch ------------------------------------------
extern Event global_func_id;
extern int global_source_line;
extern char* global_source_file;

bool GetCallParams(TraceCall& trc_call, void*& params)
{ 
    global_func_id = trc_call.func_id;
    global_source_line = trc_call.source_line;
    global_source_file = trc_call.source_file;

    switch(trc_call.func_id) {
        // Interval info
        case binter_ :
        case bsloop_ :
        case bploop_ :
          return (params = (void*)Get_binter_Info(trc_call)) != NULL;
        case einter_ :
        case enloop_ :
          return (params = (void*)Get_IDOnlyInfo("nline",trc_call.call_info,trc_call.call_info_count,10)) != NULL;

		// IO info
		case srmem_:
		  return (params = (void*)Get_srmem_Info(trc_call)) != NULL;

        // MPS/AM/AMV info 
		case crtps_ :
			return (params = (void*)Get_crtps_Info(trc_call)) != NULL;
		case psview_ :
			return (params = (void*)Get_psview_Info(trc_call)) != NULL;
		case getps_ :
			return (params = (void*)Get_getps_Info(trc_call)) != NULL;
        case delps_ :
			return (params = (void*)Get_IDOnlyInfo("PSRef",trc_call.call_info,trc_call.call_info_count,16)) != NULL;
		case setelw_ :
			return (params = (void*)Get_setelw_Info(trc_call)) != NULL;
        case getam_ :
			return (params = (void*)Get_IDOnlyInfo("AMRef",trc_call.ret_info,trc_call.ret_info_count,16)) != NULL;
		case getamr_ :
			return (params = (void*)Get_getamr_Info(trc_call)) != NULL;
        case runam_ :
          return (params = (void*)Get_IDOnlyInfo("AMRef",trc_call.call_info,trc_call.call_info_count,16)) != NULL;
        case crtamv_:
          return (params = (void*)Get_crtamv_Info(trc_call)) != NULL;
		case getamv_ :
          return (params = (void*)Get_getamv_Info(trc_call)) != NULL;
		case mapam_ :
          return (params = (void*)Get_mapam_Info(trc_call)) != NULL;
        case delamv_ :
          return (params = (void*)Get_IDOnlyInfo("AMViewRef",trc_call.call_info,trc_call.call_info_count,16)) != NULL;
//====
        case blkdiv_ :
          return (params = (void*)Get_blkdiv_Info(trc_call)) != NULL;
//=***
        case distr_ :
          return (params = (void*)Get_distr_Info(trc_call)) != NULL;
        case redis_ :
          return (params = (void*)Get_redis_Info(trc_call)) != NULL;

        // DisArray info
        case crtda_ :
          return (params = (void*)Get_crtda_Info(trc_call)) != NULL;
        case align_ :
          return (params = (void*)Get_align_Info(trc_call)) != NULL;
        case realn_ :
          return (params = (void*)Get_realn_Info(trc_call)) != NULL;
        case delda_ :
          return (params = (void*)Get_IDOnlyInfo("ArrayHeader",trc_call.call_info,trc_call.call_info_count,16)) != NULL;
        case arrcpy_ :
          return (params = (void*)Get_arrcpy_Info(trc_call)) != NULL;
        case aarrcp_ :
          return (params = (void*)Get_aarrcp_Info(trc_call)) != NULL;
        case waitcp_ :
          return (params = (void*)Get_waitcp_Info(trc_call)) != NULL;

		// ParLoop info 
        case crtpl_ :
          return (params = (void*)Get_crtpl_Info(trc_call)) != NULL;
        case mappl_ :
          return (params = (void*)Get_mappl_Info(trc_call)) != NULL;
        case dopl_ :
			return (params = (void*)Get_dopl_Info(trc_call)) != NULL;
        case endpl_ :
          return (params = (void*)Get_IDOnlyInfo("LoopRef",trc_call.call_info,trc_call.call_info_count,16)) != NULL;

        // Reduction info
        case crtrg_ :
          return (params = (void*)Get_IDOnlyInfo("RedGroupRef",trc_call.ret_info,trc_call.ret_info_count,16)) != NULL;
        case crtred_ :
          return (params = (void*)Get_crtred_Info(trc_call)) != NULL;
        case insred_ :
          return (params = (void*)Get_insred_Info(trc_call)) != NULL;
        case strtrd_ :
        case waitrd_ :
        case delrg_ :
          return (params = (void*)Get_IDOnlyInfo("RedGroupRef",trc_call.call_info,trc_call.call_info_count,16)) != NULL;
        case delred_ :
          return (params = (void*)Get_IDOnlyInfo("RedRef",trc_call.call_info,trc_call.call_info_count,16)) != NULL;

        // Shadow info
        case crtshg_ :
           return (params = (void*)Get_crtshg_Info(trc_call)) != NULL;
        case inssh_ :
          return (params = (void*)Get_inssh_Info(trc_call)) != NULL;
        case insshd_ :
          return (params = (void*)Get_insshd_Info(trc_call)) != NULL;
        case incsh_ :
          return (params = (void*)Get_incsh_Info(trc_call)) != NULL;
        case incshd_ :
          return (params = (void*)Get_incshd_Info(trc_call)) != NULL;
        case delshg_ :
        case strtsh_ :
        case waitsh_ :
		case sendsh_ :
		case recvsh_ :
          return (params = (void*)Get_IDOnlyInfo("ShadowGroupRef",trc_call.call_info,trc_call.call_info_count,16)) != NULL;
		case exfrst_ :
		case imlast_ :
			return (params = (void*)Get_exfrst_Info(trc_call)) != NULL;
		case across_:
			return (params = (void *) Get_across_Info(trc_call)) != NULL;
		// Remote access info
        case crtrbp_ :
           return (params = (void*)Get_crtrbp_Info(trc_call)) != NULL;
        case crtrbl_ :
           return (params = (void*)Get_crtrbl_Info(trc_call)) != NULL;
        case loadrb_ :
           return (params = (void*)Get_loadrb_Info(trc_call)) != NULL;
		case waitrb_ :
          return (params = (void*)Get_IDOnlyInfo("BufferHeader",trc_call.call_info,trc_call.call_info_count,16)) != NULL;

        default :
          return true;
	}
}

//-------------------------------------------- FreeCallParams ------------------------------------------
#ifdef nodef

void FreeCallParams(FuncCall& func_call)
{ 
    if (func_call.call_params == NULL) 
		return;
    switch(func_call.func_id) {

        // Interval info
        case binter_ :
        case bsloop_ :
        case bploop_ :
          free(((binter_Info*)func_call.call_params)->file);
          break;
        case crtamv_:
//          free(((crtamv_Info*)func_call.call_params)->SizeArray);
          break;
        case distr_ :
//          free(((distr_Info*)func_call.call_params)->AxisArray);
//          free(((distr_Info*)func_call.call_params)->DistrParamArray);
          break;
        case redis_ :
//          free(((redis_Info*)func_call.call_params)->AxisArray);
//          free(((redis_Info*)func_call.call_params)->DistrParamArray);
          break;

        // DisArray info
        case crtda_ :
//          free(((crtda_Info*)func_call.call_params)->SizeArray);
          break;
        case align_ :
//          free(((align_Info*)func_call.call_params)->AxisArray);
//          free(((align_Info*)func_call.call_params)->CoeffArray);
//          free(((align_Info*)func_call.call_params)->ConstArray);
          break;
        case realn_ :
//          free(((realn_Info*)func_call.call_params)->AxisArray);
//          free(((realn_Info*)func_call.call_params)->CoeffArray);
//          free(((realn_Info*)func_call.call_params)->ConstArray);
          break;
        case aarrcp_ :
//          free(((aarrcp_Info*)func_call.call_params)->FromInitIndexArray);
//          free(((aarrcp_Info*)func_call.call_params)->FromLastIndexArray);
//          free(((aarrcp_Info*)func_call.call_params)->FromStepArray);
//          free(((aarrcp_Info*)func_call.call_params)->ToInitIndexArray);
//          free(((aarrcp_Info*)func_call.call_params)->ToLastIndexArray);
//          free(((aarrcp_Info*)func_call.call_params)->ToStepArray);
          break;

        // ParLoop info
        case mappl_ :
//          free(((mappl_Info*)func_call.call_params)->AxisArray);
//          free(((mappl_Info*)func_call.call_params)->CoeffArray);
//          free(((mappl_Info*)func_call.call_params)->ConstArray);
//          free(((mappl_Info*)func_call.call_params)->InInitIndexArray);
//          free(((mappl_Info*)func_call.call_params)->InLastIndexArray);
//          free(((mappl_Info*)func_call.call_params)->InStepArray);
          break;

        // Shadow info
        case inssh_ :
//          free(((inssh_Info*)func_call.call_params)->LowShdWidthArray);
//          free(((inssh_Info*)func_call.call_params)->HiShdWidthArray);
          break;
        case exfrst_ :
        case imlast_ :
			break;
      }
//    free(func_call.call_params);
//    func_call.call_params=NULL;
}
#endif
