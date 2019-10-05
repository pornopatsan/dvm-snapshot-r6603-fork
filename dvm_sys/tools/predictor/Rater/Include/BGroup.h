#ifndef BGroupH
#define BGroupH

//////////////////////////////////////////////////////////////////////
//
// BGroup.h interface for the Bgroup class.
//
//////////////////////////////////////////////////////////////////////
#include <vector>


#include "DArray.h"
#include "Block.h"
#include "DimBound.h"
#include "CommCost.h"

class BoundGroup {
	CommCost				boundCost;
//	std::vector<DimBound>	dimInfo;

	// for pipeline
//	long	vmDimension;
//	char	dimBound;		// L-left, R-right
public:

	std::vector<DimBound>	dimInfo; //====// было private
	AMView		*amPtr;
	BoundGroup();
	virtual ~BoundGroup();
	void AddBound(DArray *ADArray, const std::vector<long>& ALeftBSizeArray, 
		const std::vector<long>& ARightBSizeArray, long ACornerSign);
//====
    CommCost* GetBoundCost();
//=***

	double StartB();
//	char getDimBound() const { return dimBound; }
//	long getVmDimension() const { return vmDimension; }
};

#endif 

