// ParLoop.cpp: implementation of the LoopLS class.
//
//////////////////////////////////////////////////////////////////////

#include "ParLoop.h"

using namespace std;

extern ofstream prot; 
 
ParLoop::ParLoop(long ARank)
{
  Rank = ARank;
	AlignRule = vector<AlignAxis>(0);
  LowerIndex = vector<long>(Rank);
  HigherIndex = vector<long>(Rank);
  LoopStep = vector<long>(Rank);
  Invers = vector<long>(Rank);
  AM_Dis = 0;
  //====
  AcrossFlag=0;
  AcrossCost=0;
	//=***

}

ParLoop::~ParLoop()
{
}

//====
void ParLoop::Across(CommCost *BoundCost,int type_size)
{ 
	AcrossFlag=type_size;
	AcrossCost=BoundCost;  
}
//=***


long ParLoop::GetLoopSize()
  { int i;
    long size=1;
    for(i=0;i<Rank;i++)
      size=size*GetSize(i);
    return size;
  };

long ParLoop::GetSize(long plDim)
  { if(plDim<Rank)
      return (HigherIndex[plDim] - LowerIndex[plDim] + LoopStep[plDim])/LoopStep[plDim];
    return 0;
  };

void ParLoop::SaveLoopParams(const vector<long>& AInInitIndex, 
		const vector<long>& AInLastIndex, const vector<long>& AInLoopStep)
  {
    int i; long lv;
    // запоминание параметров цикла  
    LowerIndex.clear();
    HigherIndex.clear();
    LoopStep.clear();
    Invers.clear();
  	for (i = 0; i < Rank; i++)
		if(AInLoopStep[i]>=0) {
			LowerIndex.push_back(AInInitIndex[i]);
			HigherIndex.push_back(AInLastIndex[i]);
			LoopStep.push_back(AInLoopStep[i]);
			Invers.push_back(0);
        } else {
			lv=(AInInitIndex[i]-AInLastIndex[i]) % AInLoopStep[i];
			if(lv)
				LowerIndex.push_back((AInLastIndex[i]+AInLoopStep[i]-lv));
			else
				LowerIndex.push_back(AInLastIndex[i]);
			HigherIndex.push_back(AInInitIndex[i]);
			LoopStep.push_back(-AInLoopStep[i]);
			Invers.push_back(1);
        }
}

void ParLoop::PrepareAlign(long& TempRank, const vector<long>& AAxisArray, 
	const vector<long>& ACoeffArray, const vector<long>& AConstArray, 
	vector<AlignAxis>& IniRule)
{
	int i;
	long IRSize = Rank + TempRank;

	IniRule.reserve(IRSize);

	// Предварительная инициализация
	for (i = 0; i < Rank; i++)
		IniRule.push_back(AlignAxis(align_COLLAPSE, i+1, 0));
	for (i = Rank; i < IRSize; i++)
		IniRule.push_back(AlignAxis(align_NORMTAXIS, 0, i-Rank+1));

	// Заполнение DistRule в соответствии с параметрами
	for (i = 0; i < TempRank; i++) {
//		prot << "i=" << i << ", AAxisArray[i]=" << AAxisArray[i] << endl;
		if (AAxisArray[i] == -1)
			  IniRule[i+Rank] = AlignAxis(align_REPLICATE, 0, i+1);
		else if (ACoeffArray[i] == 0)
			  IniRule[i+Rank] = AlignAxis(align_CONSTANT, 0, i+1, 0, AConstArray[i]);
		else {
			  IniRule[i+Rank] = AlignAxis(align_NORMTAXIS, AAxisArray[i], i+1, ACoeffArray[i], 
                                      AConstArray[i]+ACoeffArray[i]*LowerIndex[AAxisArray[i]-1]);
			  IniRule[AAxisArray[i]-1] = AlignAxis(align_NORMAL, AAxisArray[i], i+1, ACoeffArray[i], 
                                      AConstArray[i]+ACoeffArray[i]*LowerIndex[AAxisArray[i]-1]);
		}
	}
}

void ParLoop::MapPL(AMView *APattern, const vector<long>& AAxisArray, 
		const vector<long>& ACoeffArray, const vector<long>& AConstArray,
		const vector<long>& AInInitIndex, const vector<long>& AInLastIndex, 
		const vector<long>& AInLoopStep)
{
	if (!APattern->IsDistribute()) {
		prot << "Wrong call MapPL" << endl;
		abort();
	}

	long TempRank = APattern->Rank();
	vector<AlignAxis> IniRule;

  // запоминание а/м, на к-рую в итоге выравниваем
	AM_Dis = APattern;

  // запоминание параметров цикла  
  SaveLoopParams(AInInitIndex, AInLastIndex, AInLoopStep);

	// инициализация AlignRule
  PrepareAlign(TempRank, AAxisArray, ACoeffArray, AConstArray, IniRule);
	AlignRule = IniRule;
}

void ParLoop::MapPL(DArray *APattern, const vector<long>& AAxisArray, 
		const vector<long>& ACoeffArray, const vector<long>& AConstArray,
		const vector<long>& AInInitIndex, const vector<long>& AInLastIndex, 
		const vector<long>& AInLoopStep)
{
	if (!APattern->IsAlign()) {
		prot << "Wrong call MapPL" << endl;
		abort();
	}

	long TempRank = APattern->Rank();
  long ALSize;
	int i;
	vector<AlignAxis>	TAlign, 
						IniRule;
	AlignAxis aAl, tAl;

  // запоминание а/м, на к-рую в итоге выравниваем
	AM_Dis = APattern->AM_Dis;

  // запоминание параметров цикла  
  SaveLoopParams(AInInitIndex,AInLastIndex,AInLoopStep);

  // начальная инициализация AlignRule
	PrepareAlign(TempRank, AAxisArray, ACoeffArray, AConstArray, IniRule); 
	
	// Формирование суперпозиции отображений
	ALSize = Rank + AM_Dis->Rank(); 
	TAlign = APattern->AlignRule;
	AlignRule = vector<AlignAxis>(ALSize);

	// начальная иниц. 2-ой части правила 
	// выравнивания (если шаблон размножен, то и массив тем более)
  for (i = 0; i < AM_Dis->Rank(); i++) 
    AlignRule[i+Rank]=TAlign[i+TempRank];

  for (i = 0; i < Rank; i++)
    { aAl = IniRule[i];
		  if (aAl.Attr  == align_NORMAL)
		    { tAl = TAlign[aAl.TAxis - 1];
			    switch (tAl.Attr)
			      { case align_NORMAL :   aAl.TAxis = tAl.TAxis;
                                    aAl.A *= tAl.A;
                                    aAl.B = aAl.B * tAl.A + tAl.B;
                                    AlignRule[i] = aAl;
                                    AlignRule[Rank+aAl.TAxis-1].Axis = i+1;
                                    AlignRule[Rank+aAl.TAxis-1].A = aAl.A;
                                    AlignRule[Rank+aAl.TAxis-1].B = aAl.B;
                                    break;
              case align_COLLAPSE : aAl.TAxis = 0;
                                    aAl.Attr  = align_COLLAPSE;
                                    AlignRule[i] = aAl;
                                    break;
            };
        };
    };

	for (i = 0; i < TempRank; i++)
	  { aAl = IniRule[i+Rank];
  		switch (aAl.Attr)
        { case align_CONSTANT :   tAl = TAlign[aAl.TAxis-1];
                                  if (tAl.Attr == align_NORMAL)
                                    { aAl.TAxis = tAl.TAxis;
									                    aAl.B = tAl.A * aAl.B + tAl.B;
									                    AlignRule[Rank+tAl.TAxis-1] = aAl;
								                    };
								                  break;
		      case align_REPLICATE :  tAl = TAlign[aAl.TAxis-1];
			                            if (tAl.Attr == align_NORMAL)
								                    { aAl.Attr = align_BOUNDREPL;
									                    aAl.TAxis = tAl.TAxis;
									                    aAl.A = tAl.A;
									                    aAl.B = tAl.B;
									                    aAl.Bound = APattern->GetSize(tAl.TAxis);
									                    AlignRule[Rank+tAl.TAxis-1] = aAl;
								                    };
								                  break;
        };
    };
};

