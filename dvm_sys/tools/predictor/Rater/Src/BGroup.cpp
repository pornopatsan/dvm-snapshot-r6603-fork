// BGroup.cpp: implementation of the BoundGroup class.
//
//////////////////////////////////////////////////////////////////////
#include <iostream>
#include "BGroup.h"

using namespace std;

extern ofstream prot; 


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

BoundGroup::BoundGroup()
{
	amPtr = 0;
}

BoundGroup::~BoundGroup()
{

}

void BoundGroup::AddBound(DArray *ADArray, const vector<long>& BLeftBSizeArray, 
	const vector<long>& BRightBSizeArray, long ACornerSign)
{
	long	i, 
			proc, 
			vmDim, 
			arrDim;
	int		dir, 
			count = 0; 
	bool	IsCorner = false;
	Block	b;

//	vector<DimBound> dimInfo;
	long daRank = ADArray->Rank();
	vector<long> ALeftBSizeArray(BLeftBSizeArray);
	vector<long> ARightBSizeArray(BRightBSizeArray);


	for (i = 0; i < daRank; i++) {
		if (ALeftBSizeArray[i] == -1)
			ALeftBSizeArray[i] = ADArray->LowShdWidthArray[i];
		if (ARightBSizeArray[i] == -1)
			ARightBSizeArray[i] = ADArray->HiShdWidthArray[i];
	}

	if (!amPtr)
		amPtr = ADArray->AM_Dis;
	else if (amPtr != ADArray->AM_Dis) {
		// arrays is align on different AMView
		prot << "Wrong call AddBound: arrays is align on different AMView" << endl;
		cerr << "Wrong call AddBound: arrays is align on different AMView" << endl;
		exit(1);
	}

#ifdef P_DEBUG
	for (i = 0; i < daRank; i++) {
		vmDim = ADArray->GetMapDim(i+1, dir);
			prot << "arDim=" << i+1 
				 << ", vmDim=" << vmDim
				 << ", Left=" << ALeftBSizeArray[i] 
				 << ", Right=" << ARightBSizeArray[i]
				 <<", dir =" << dir << endl;
	}
#endif



	if (boundCost.transfer.size() == 0)
		boundCost = CommCost(ADArray->AM_Dis->VM_Dis);

	if (ADArray->Repl)
		return;

	if (!ADArray->IsAlign()) {
		// Array is'n align on any AMView
		prot << "Wrong call AddBound: Array is'n align on any AMView" << endl;
		exit(1);
	}

	for (i = 0; i < daRank; i++) {
		if (ALeftBSizeArray[i] < 0 || ARightBSizeArray[i] < 0) {
			prot << "Wrong call AddBound" << endl;
			exit(1); 
		}
	}

	for (proc = 0; proc < amPtr->VM_Dis->GetLSize(); proc++) {
		//grig b = Block(ADArray, proc);
		b = Block(ADArray, proc,1);

//		prot << "proc=" << proc << ", empty=" << b.empty() << ", IsBoundIn=" << b.IsBoundIn(ALeftBSizeArray, ARightBSizeArray) << endl;
		if (!b.empty() && !b.IsBoundIn(ALeftBSizeArray, ARightBSizeArray)) {
			prot << "Fatal error: Local array size is less then shadow width." << endl;
			cerr << "Fatal error: Local array size is less then shadow width." << endl; 
			exit(1);
		}
	}

	for (arrDim = 1; arrDim <= daRank; arrDim++) {
		vmDim = ADArray->GetMapDim(arrDim, dir);
		if (vmDim >= 0 && //====// почему-то было строго ">" в результате попадали только ненулевые числа а дописывался мусором
			(ALeftBSizeArray[arrDim-1] >= 0 || ARightBSizeArray[arrDim-1] >= 0)) { //====// почему-то было строго ">" в результате вместо 0 попадал мусор
			dimInfo.push_back(
				DimBound(arrDim, vmDim, dir, ALeftBSizeArray[arrDim-1], ARightBSizeArray[arrDim-1])
			); 
			count++;
		}
	}

	if (ACornerSign == 1 && count > 1)
		IsCorner = true;

	boundCost.BoundUpdate(ADArray, dimInfo, IsCorner);
}

double BoundGroup::StartB()
{
	return boundCost.GetCost();
}

//====
CommCost* BoundGroup::GetBoundCost()
{ return &boundCost;
}
//=***
