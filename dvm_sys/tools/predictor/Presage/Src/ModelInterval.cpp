#include "Interval.h"
#include "CallInfoStructs.h"

void FuncCall::IntervalTime()
{ 
	binter_Info* tmp=(binter_Info*) call_params; 

	RegularTime();	// add time beforå changing interval

    switch(func_id) {
        case binter_ :
			Interval::Enter(__IT_USER, tmp->line, tmp->file, tmp->index);
			break;
        case bsloop_ :
			Interval::Enter(__IT_SEQ, tmp->line, tmp->file, tmp->index);
			break;
        case bploop_ :
			Interval::Enter(__IT_PAR, tmp->line, tmp->file, tmp->index);
			break;
        case einter_ :
        case enloop_ :
			Interval::Leave();
			break;
      }

}

