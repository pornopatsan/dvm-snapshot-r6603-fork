#ifndef CommCostH
#define CommCostH

//////////////////////////////////////////////////////////////////////
//
// CommCost.h: interface for the CommCost class.
//
//////////////////////////////////////////////////////////////////////

#include <cmath>
#include <vector>
#include <algorithm>

//#include "Vm.h"
//#include "Ps.h"

#include "DArray.h"
#include "DimBound.h"

//====
#include "LoopBlock.h"
#include "Interval.h"
//=***

typedef std::vector<long> lvector;
typedef std::vector<lvector> Dim2Array;

class VM;
class Block;
//====
class LoopBlock;
class ClustInfo;
extern ClustInfo *CurrentCluster;
//=***

// коммуникационные издержки
class CommCost {  

	std::vector<double> tmp_channels;

public:


	// массив содержащий информацию о количестве байтов 
	// пересылаемых между парой процессоров виртуальной машины vm 
	Dim2Array transfer;	
	VM *vm;  
	
	CommCost(VM *Avm);
	CommCost();
	virtual ~CommCost();

	// копирование распределенного массива на все процессоры
	void CopyUpdate(DArray *FromArray, Block & readBlock);
//====
	double BeginTime;
	double EndTime;
	double WaitStart;
	void CommSend(float send_proc_time, long from_id, long to_id, long bytes);
	void CommSend(long from_id, long to_id, long bytes) { CommSend(CurrProcTime(currentVM->map(from_id)), from_id, to_id, bytes); }

	void SaveSubChannels(ClustInfo *clust);
	void RestoreSubChannels(ClustInfo *clust);
	void SaveChannels() { tmp_channels.resize(0); SaveSubChannels(CurrentCluster); }
	void RestoreChannels() { RestoreSubChannels(CurrentCluster); }

	void CopyUpdateDistr(DArray * FromArray, Block &readBlock, long p1);
	long GetLSize();
	void Across(double call_time, long LoopSZ, LoopBlock** ProcBlock,int type_size);
//=***

	void BoundUpdate(DArray *daPtr, std::vector<DimBound> & dimInfo, bool IsCorner);

	// машиннозависима€ функци€ возвращающа€ стоимость 
	// накладнных расходов доступа к удаленным данным
	double GetCost();

	CommCost & operator =(const CommCost &);

	// измен€ет массив transfer в соответствии
	// с возникающими пересылками между процессорами VM в результате 
	// перехода от одного распределени€ массива к другому
	void Update(DArray *oldDA, DArray *newDA);

};

#endif 
