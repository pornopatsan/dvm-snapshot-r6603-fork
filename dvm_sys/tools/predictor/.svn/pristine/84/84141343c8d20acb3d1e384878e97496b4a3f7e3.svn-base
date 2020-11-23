#ifndef ParLoopH
#define ParLoopH

//////////////////////////////////////////////////////////////////////
//
// LoopLS.h: interface for the LoopLS class.
//
//////////////////////////////////////////////////////////////////////


#include "Space.h"
#include "AMView.h"
#include "AlignAxis.h"

//====
#include "CommCost.h"
//=***
#include "LoopBlock.h"

#include <vector>
#include <fstream>

class AMView;
//====
class CommCost;
//=***
class ParLoop { 
    void PrepareAlign(long& TempRank, const std::vector<long>& AAxisArray, 
	const std::vector<long>& ACoeffArray, const std::vector<long>& AConstArray, 
	std::vector<AlignAxis>& IniRule);
    void SaveLoopParams(const std::vector<long>& AInInitIndex, 
		const std::vector<long>& AInLastIndex, const std::vector<long>& AInLoopStep);
public:
    long Rank;
    AMView *AM_Dis;						// AMView  for ParLoopmapping
	std::vector<AlignAxis> AlignRule;	// Rule for alignment of AM_Dis
    std::vector<long> LowerIndex; 
    std::vector<long> HigherIndex;
    std::vector<long> LoopStep;
    std::vector<long> Invers;
//====
    int AcrossFlag;
    CommCost* AcrossCost;
//=***

 	ParLoop(long ARank);
    ~ParLoop();
//====
 //   int isAcross();
    void Across(CommCost* BoundCost,int type_size);
//=***

    long GetSize(long plDim);
    long GetLoopSize();

    void MapPL(AMView *APattern, const std::vector<long>& AAxisArray, 
		const std::vector<long>& ACoeffArray, const std::vector<long>& AConstArray,
		const std::vector<long>& AInInitIndex, const std::vector<long>& AInLastIndex, 
		const std::vector<long>& AInLoopStep);

    void MapPL(DArray *APattern, const std::vector<long>& AAxisArray, 
		const std::vector<long>& ACoeffArray, const std::vector<long>& AConstArray,
		const std::vector<long>& AInInitIndex, const std::vector<long>& AInLastIndex, 
		const std::vector<long>& AInLoopStep);
};


#endif 
