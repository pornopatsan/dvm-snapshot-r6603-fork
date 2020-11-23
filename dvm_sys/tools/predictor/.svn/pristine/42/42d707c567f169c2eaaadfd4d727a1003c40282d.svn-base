#ifndef RedVarH
#define RedVarH

//////////////////////////////////////////////////////////////////////
//
// RedVar.h: interface for the RedVar class.
//
//////////////////////////////////////////////////////////////////////

#include "Vm.h"

class RedVar {
public:
	long RedElmSize;	// размер одного элемента редукционной переменной-массива в байтах
	long RedArrLength; // число элементов в редукционной переменной-массиве
	long LocElmSize; // размер в байтах одного элемента массива с дополнительной информацией

	RedVar(long ARedElmSize, long ARedArrLength, long ALocElmSize);
	RedVar();
	virtual ~RedVar();
	long GetSize(); //  размер в байтах редукционной переменной-массива вместе с массивом дополнительной информации
};

#endif 
