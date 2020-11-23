#ifndef RedGroupH 
#define RedGroupH

//////////////////////////////////////////////////////////////////////
//
// RedGroup.h: interface for the RedGroup class.
//
//////////////////////////////////////////////////////////////////////

#include <vector>

#include "RedVar.h"
#include "DArray.h"

class RedGroup {  
public:
	// ??? нужен ли long CentralProc; // линейнный индекс геометрического центра виртуальной машины
	VM *vmPtr;
	std::vector<RedVar *> redVars;  // массив редукционных переменных
	long TotalSize; // общий размер в байтах редукционных переменых включенных в группу и их дополнительной информации
	long CentralProc; // линейнный индекс геометрического центра виртальной машины

	RedGroup(VM *AvmPtr);
	virtual ~RedGroup();
	double StartR(DArray * APattern, long ALoopRank, const std::vector<long>& AAxisArray);  // машинозависимая // ??? определить какие еще нужны параметры
	double StartR(AMView * APattern, long ALoopRank, const std::vector<long>& AAxisArray);  // машинозависимая // ??? определить какие еще нужны параметры
	void AddRV(RedVar *ARedVar);
};

#endif 
