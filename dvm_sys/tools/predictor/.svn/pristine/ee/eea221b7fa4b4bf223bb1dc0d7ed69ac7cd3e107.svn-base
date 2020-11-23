//////////////////////////////////////////////////////////////////////
//
// LoopBlock.cpp: implementation of the Block class.
//
//////////////////////////////////////////////////////////////////////
#include <assert.h>

#include "LoopBlock.h"

#ifndef _MSC_VER

template <class T>
T _MIN(T a, T b)
{
	return a < b ? a : b;
}

template <class T> 
T _MAX(T a, T b) 
{ 
	return a >= b ? a : b;
}

#endif

using namespace std;

long LoopBlock::GetRank()
{ 
	return LSDim.size();
}

bool LoopBlock::empty()
{ 
	return LSDim.empty();
}

long LoopBlock::GetBlockSize()
{ 
	unsigned int i;
	long size = 1;

	if (LSDim.empty())
		return 0;

	for (i = 0; i < LSDim.size(); i++)
		{
			size *= LSDim[i].GetLoopLSSize();
		}
	return size;
}

LoopBlock::LoopBlock()
{ 
	LSDim = vector<LoopLS>(0);
}

LoopBlock::~LoopBlock()
{
}
/*
LoopBlock::LoopBlock(ParLoop *pl, long ProcLI)
{ 
	int i;
	long vmDimSize, dimProcI;
	long amDimSize, amAxis;
	long plAxis;
	long amLower, amUpper, BlockSize; // Param, Module;
	bool IsBlockEmpty = false;
	vector<long> ProcSI;
	DistAxis dist;
	AlignAxis align, plAlign;
	LoopLS ls;

	

	AMView* am = pl->AM_Dis;
	assert(am != NULL);
	VM* vm = am->VM_Dis;
	assert(vm != NULL);
	long amRank = am->Rank();
	long vmRank = vm->Rank();
	long plRank = pl->Rank;
	vm->GetSI(ProcLI, ProcSI);

	LSDim.reserve(plRank);

	// ѕредварительна€ инициализаци€ блока (равен циклу с шагом 1)
	for (i = 0; i < plRank; i++)
	  LSDim.push_back(LoopLS(pl->LowerIndex[i], pl->HigherIndex[i], 1));

  // ‘ормирование локальной части без учета шага
	for (i = 0; i < vmRank; i++)
	  {	dist = am->DistRule[amRank + i];
		  switch (dist.Attr)
		    {	case map_NORMVMAXIS :
			        amAxis = dist.Axis;
			        amDimSize = am->GetSize(amAxis);
			        dimProcI = ProcSI[i];
			        vmDimSize = vm->GetSize(i+1);
			        BlockSize = (amDimSize - 1) / vmDimSize + 1;
                     
					
			        amLower = dimProcI * BlockSize;		
			        amUpper = _MIN(amDimSize, amLower+BlockSize) - 1;
					
			        IsBlockEmpty = IsBlockEmpty || amLower > amUpper;
			        if(IsBlockEmpty) break;
			        align = pl->AlignRule[plRank+amAxis-1];

			        switch(align.Attr)
			          { case align_NORMTAXIS :
				              plAxis = align.Axis;
				              plAlign = pl->AlignRule[plAxis-1];
				              ls = LoopLS(amLower, amUpper, 1);
				              ls.transform(plAlign.A, plAlign.B, pl->GetSize(plAxis-1));
								      if (ls.empty())
				                IsBlockEmpty = true; 
				              else
                        { ls.Lower = _MAX(ls.Lower, (long)0);
                          ls.Upper = _MIN(ls.Upper, (long)(pl->GetSize(plAxis-1)-1));
					                LSDim[plAxis-1] = ls;  // LSDim с нул€
                        };
				              break;

			            case align_BOUNDREPL :
				              ls = LoopLS(amLower, amUpper, 1);
				              ls.transform(align.A, align.B, align.Bound);
				              if (ls.empty())
					              IsBlockEmpty = true;
				              break;
			            case align_REPLICATE :
				              break;
			            case align_CONSTANT :
				              if (align.B < amLower || align.B > amUpper)
					              IsBlockEmpty = true;
				              break;
			          } // end internal switch
			        break;
		      case map_REPLICATE :
			        break;
		    }  // end main switch
		if (IsBlockEmpty)
			break;
	} // end for

	if (IsBlockEmpty)
	  {	// Ћокальный блок пуст даже без учета шага
      LSDim = vector<LoopLS>(0);
	  }
  else
    {
      // ‘ормирование локальной части с учетом шага
      for(i=0; i<plRank; i++)
        { LSDim[i].Lower = pl->LoopStep[i] * (long)ceil((double)LSDim[i].Lower/(double)pl->LoopStep[i]);
          LSDim[i].Upper = pl->LoopStep[i] * (LSDim[i].Upper / pl->LoopStep[i]);
          LSDim[i].Step = pl->LoopStep[i];
          if(LSDim[i].Lower > LSDim[i].Upper)
            break;
        };
      if(i==plRank)
        { //  оррекци€ индексов с учетом начальных значений
          for(i=0; i<plRank; i++)
            { LSDim[i].Lower += pl->LowerIndex[i];
              LSDim[i].Upper += pl->LowerIndex[i];
            };
        }
      else
        { // Ћокальный блок с учетом шага пуст
          LSDim = vector<LoopLS>(0);
        };
    }
}
*/
LoopBlock::LoopBlock(ParLoop *pl, long ProcLI,int a)
{ 
	int i;
	long vmDimSize, dimProcI;
	long amDimSize, amAxis;
	long plAxis;
	long amLower, amUpper, BlockSize; // Param, Module;
	bool IsBlockEmpty = false;
	vector<long> ProcSI;
	DistAxis dist;
	AlignAxis align, plAlign;
	LoopLS ls;

	

	AMView* am = pl->AM_Dis;

//	printf("LOOPBLOCK AM=%lx   am->weightEl.ID=%lx\n",am, am->weightEl.ID);

	assert(am != NULL);
	VM* vm = am->VM_Dis;
	assert(vm != NULL);
	long amRank = am->Rank();
	long vmRank = vm->Rank();
	long plRank = pl->Rank;
	vm->GetSI(ProcLI, ProcSI);

	//grig
	std::vector<double> avWeights;
	int j;
	long local_sum=0; // индекс с которого начанаютс€ веса дл€ данного измерени€ VM
	long jmax;  //  размер текущего измерени€ Vm
	double vBlockSize,temp_w=0;    //
	double sum1=0;
	//grig

	LSDim.reserve(plRank);

	// ѕредварительна€ инициализаци€ блока (равен циклу с шагом 1)
	for (i = 0; i < plRank; i++)
	{
//		printf("1. INIT %d %d %d\n",pl->LowerIndex[i], pl->HigherIndex[i], 1);
		LSDim.push_back(LoopLS(pl->LowerIndex[i], pl->HigherIndex[i], 1));
	}


  // ‘ормирование локальной части без учета шага
	for (i = 0; i < vmRank; i++)
	  {	dist = am->DistRule[amRank + i];
		  switch (dist.Attr)
		    {	case map_NORMVMAXIS :
			        amAxis = dist.Axis;
			        amDimSize = am->GetSize(amAxis);
			        dimProcI = ProcSI[i];
			        vmDimSize = vm->GetSize(i+1);
			        BlockSize = (amDimSize - 1) / vmDimSize + 1;
                     
					//grig
					am->weightEl.GetWeights(avWeights);
			 local_sum=0; // индекс с которого начинаютс€ веса дл€ данного измерени€ VM
			 jmax=vm->GetSize(i+1);  //  размер текущего измерени€ Vm
			 vBlockSize,temp_w=0;    //
			 sum1=0;
			 long lBlockSize;
					for(j=0;j<i;j++)
					{
					  local_sum+=vm->GetSize(j+1);
					}
					

//					printf("size()=%d arr=%d %d %d %d %d %d\n",am->weightEl.body.size(),am->weightEl.body[0],am->weightEl.body[1],am->weightEl.body[2],am->weightEl.body[3],am->weightEl.body[4],am->weightEl.body[5]);

					if(am->weightEl.body.size() == 0) temp_w=1;
					else
					{ for(j=0;j<jmax;j++)
							temp_w+=am->weightEl.body[j+local_sum]; // находим сумму весов
					}

//					printf("temp_w=%f\n",temp_w);
					vBlockSize = amDimSize/temp_w; // размер блока
					lBlockSize=ceil((double)amDimSize/temp_w) > 0.5 ? amDimSize/temp_w+ 1 : amDimSize/temp_w; // размер блока

					//====

					if(am->BSize.size()>0)
					{	// если не задан мультиблок, то будет считатьс€ что блоки размером в 1 и не надо ничего пересчитывать
						// а вот если он задан, то пересчитаем, даже если там единицы
						if(amDimSize % am->BSize[i] !=0 ) { printf("Error: Dimension %d is not dividible by %d \n",amDimSize, am->BSize[i]); exit(0);}
						lBlockSize=(long)ceil(vBlockSize);
						if( ( lBlockSize % am->BSize[i]) > 0)   
							lBlockSize = ( lBlockSize / am->BSize[i] + 1) * am->BSize[i]; 
						vBlockSize=lBlockSize;
					}

//					printf("ok 66666\n");
					//=***
/*
					if(vBlockSize - ceil(vBlockSize)<0.5) // если VBlocksize - celoe
					{					
					lBlockSize=floor(vBlockSize);
					}
					else   // нет
						lBlockSize= ceil(vBlockSize);
*/
					
					for(j=0;j<dimProcI;j++)
					{
						sum1+= ((am->weightEl.body.size() != 0)? (vBlockSize*am->weightEl.body[j+local_sum]) : vBlockSize);
					}

					amLower=sum1;
					amUpper=(double)sum1 + ((am->weightEl.body.size() != 0)? (vBlockSize*am->weightEl.body[dimProcI+local_sum]-1) : vBlockSize - 1);

//					printf("ok 777\n");

//==== убрал 
// так как вызывала ошибку в случае когда на каждый проц по одному элементу
// и предпоследний проц захватывал 2 элемента, а последний брал еще раз свой элемент
//					if(amUpper+1==amDimSize-1)    
//						amUpper=amDimSize-1;
//=***
			    IsBlockEmpty = IsBlockEmpty || amLower > amUpper;
			    if(IsBlockEmpty) break;
			    align = pl->AlignRule[plRank+amAxis-1];

			        switch(align.Attr)
			          { case align_NORMTAXIS :
				              plAxis = align.Axis;
				              plAlign = pl->AlignRule[plAxis-1];
				              ls = LoopLS(amLower, amUpper, 1);

				              ls.transform(plAlign.A, plAlign.B, (pl->HigherIndex[plAxis-1] - pl->LowerIndex[plAxis-1]+1 ));

//			for(j=0; j<plRank; j++)
//      { 
//printf("LSDimCheck empty[%d] %d %d %d\n",ProcLI, LSDim[j].Lower,LSDim[j].Upper,LSDim[j].Step);
//printf("LSPLDimFirst[%d] %d %d %d\n",ProcLI, pl->LowerIndex[j],pl->HigherIndex[j],pl->LoopStep[j]);
//			}
//printf("LS[%d] %d %d %d\n",ProcLI, ls.Lower, ls.Upper, ls.Step);
							  if (ls.empty())
								{											
	//								printf("EMPTY %d\n",IsBlockEmpty);
									IsBlockEmpty = true; 
								}
							  else
							  { 
//====//									ls.Lower = _MAX(ls.Lower, (long)0);
//====//		              ls.Upper = _MIN(ls.Upper, (long)(pl->HigherIndex[plAxis-1] - pl->LowerIndex[plAxis-1]));
					        LSDim[plAxis-1] = ls;  // LSDim с нул€
								};
//==== this block was moved from down
//								for(i=0; i<plRank; i++)
//								{ LSDim[i].Lower += pl->LowerIndex[i];
//									LSDim[i].Upper += pl->LowerIndex[i];
//								}
//							  LSDim[plAxis-1].Lower += pl->LowerIndex[plAxis-1];
//				        LSDim[plAxis-1].Upper += pl->LowerIndex[plAxis-1];
//printf("PlAxis=%d\n",plAxis);

					   

 //=***
							  break;

			            case align_BOUNDREPL :
							ls = LoopLS(amLower, amUpper, 1);
				              ls.transform(align.A, align.B, align.Bound);
				              if (ls.empty())
					              IsBlockEmpty = true;
				              break;
			            case align_REPLICATE :
				              break;
			            case align_CONSTANT :
				              if (align.B < amLower || align.B > amUpper)
					              IsBlockEmpty = true;
				              break;
			          } // end internal switch
			        break;
		      case map_REPLICATE :
//====
/*				  printf("----%d-%d-%d-%d-",i,vm->GetSize(0),vm->GetSize(1),vm->GetSize(2));
				  LSDim[i-1].Upper -= pl->LowerIndex[i-1];;
				  LSDim[i-1].Lower -= pl->LowerIndex[i-1];;

	{ int ii;
	  for(ii=0; ii<plRank; ii++)
  	  printf("*****LoopBlock %d %d %d**",LSDim[ii].Lower,LSDim[ii].Upper,pl->LowerIndex[ii]);
	}
*/
//=***
			        break;
		    }  // end main switch
			
		if (IsBlockEmpty)
			break;
		
	} // end for



	if (IsBlockEmpty)
	  {	// Ћокальный блок пуст даже без учета шага
      LSDim = vector<LoopLS>(0);
	  }
  else
    {
      // ‘ормирование локальной части с учетом шага
      for(i=0; i<plRank; i++)
      { 
				LSDim[i].Lower = pl->LoopStep[i] * (long)ceil((double)LSDim[i].Lower/(double)pl->LoopStep[i]);
        LSDim[i].Upper = pl->LoopStep[i] * (LSDim[i].Upper / pl->LoopStep[i]);
        LSDim[i].Step = pl->LoopStep[i];
//printf("LSDim[%d] %d %d %d\n",ProcLI, LSDim[i].Lower,LSDim[i].Upper,LSDim[i].Step);
				LSDim[i].Invers = pl->Invers[i];
        if(LSDim[i].Lower > LSDim[i].Upper)
          break;
      };
//      if(i==plRank)
//        { //  оррекци€ индексов с учетом начальных значений
//		  printf("**%d*%d** ",maxAlign,plRank);
//        for(i=0; i<plRank; i++)
//====  this block was moved up
//            { LSDim[i].Lower += pl->LowerIndex[i];
//              LSDim[i].Upper += pl->LowerIndex[i];
//=***
// 		  printf("-LoopBlock %d %d %d           ",LSDim[i].Lower,LSDim[i].Upper,pl->LowerIndex[i]);
//            };
//        }
      if(i!=plRank)
        { // Ћокальный блок с учетом шага пуст
          LSDim = vector<LoopLS>(0);
        };
    }

//		printf("Proc number-%d\n",ProcLI);
//		for(i=0;i<this->GetRank();i++)
//			printf("%d %d %d\n",this->LSDim[i].Lower,this->LSDim[i].Upper,this->LSDim[i].Step);
 
}


bool operator ==(LoopBlock& x, LoopBlock& y)
{
	
	bool equal = (x.GetRank() == y.GetRank());
    int i;

    if(equal)
	    for (i = 0; i < x.GetRank(); i++)
  		  equal = equal && (x.LSDim[i] == y.LSDim[i]);

    return equal;
}

int intersection(LoopBlock& x,LoopBlock&y)
{
	int temp;
	std::vector<long> arr1,arr2;
	long i,j,k;
	int t=0;

	arr1.resize(0);
	arr2.resize(0);

//	printf("finding intersection of two arrays:\n");
//	printf("array 1 :\n");
//		for(i=0;i<x.GetRank();i++)
//			printf(" %d %d %d \n",x.LSDim[i].Lower, x.LSDim[i].Upper,x.LSDim[i].Step);
  //  printf("array 2 :\n");
	//	for(i=0;i<y.GetRank();i++)
	//		printf(" %d %d %d \n",y.LSDim[i].Lower, y.LSDim[i].Upper,y.LSDim[i].Step);
	
	//	printf("Block size1 = %d  Block  size2 = %d \n",x.GetBlockSize(),y.GetBlockSize());
	
	temp=-1;
	
	for(i=0;i<x.LSDim.size();i++)
	{
		t=0;
		for(j=x.LSDim[i].Lower;j<=x.LSDim[i].Upper;j+=x.LSDim[i].Step)
		{
			arr1.push_back(j);
		}

		for(j=y.LSDim[i].Lower;j<=y.LSDim[i].Upper;j+=y.LSDim[i].Step)
		{
			arr2.push_back(j);
		}

//		printf("size1 = %d , size 2 = %d \n",arr1.size(),arr2.size());

		for(j=0;j<arr1.size();j++)
			for(k=0;k<arr2.size();k++)
			{
				if(arr1[j]==arr2[k])
					t++;
			}
	//		printf("dim =%d t=%d\n",i,t);
			temp*=t;
			arr1.resize(0);
			arr2.resize(0);
	}
//	printf("tempt=%d\n",temp);

	if(temp<0)
		 return -temp;
	else 
		return 0;
}