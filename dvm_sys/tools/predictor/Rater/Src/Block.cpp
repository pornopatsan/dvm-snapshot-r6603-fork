// Block.cpp: implementation of the Block class.
//
//////////////////////////////////////////////////////////////////////
#include <assert.h>
#include "Block.h"

using namespace std;
 
extern ofstream prot; 

#if (defined (_MSC_VER) && (_MSC_VER < 1300)) || (defined (__GNUG__) && (__GNUC__  < 3))
template <class T>
T min(T a, T b)
{
	return a < b ? a : b;
}
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

Block::Block(vector<LS> &v)
{
	LSDim = v;
}

Block::Block()
{

}

Block::~Block()
{
//	printf("Block::~Block()=%0X\n", this);
}

Block operator^ (Block &x, Block &y)
{
	Block temp;
	vector<LS> empty_Block(0);
	long i;

	if (x.empty() || y.empty())
		return empty_Block;

	if (x.GetRank() != y.GetRank())
	{
		prot << "Wrong call operator^" << endl;
		exit(1);
	}

	temp.LSDim.reserve(x.GetRank());

	for (i = 0; i < x.GetRank(); i++)
		if ((x.LSDim[i] ^ y.LSDim[i]).IsEmpty() != true)
			temp.LSDim.push_back(x.LSDim[i] ^ y.LSDim[i]);
		else 
		{
			temp.LSDim = empty_Block;
			break;
		};
	return temp;
}

long Block::GetBlockSize()
{
	int i;
	long size = 1;

	if (LSDim.empty())
		return 0;

	for (i = 0; i < LSDim.size(); i++)
			size *= LSDim[i].GetLSSize();
//====	
//printf("GETSIZE[%d] %d\n",i,size);
//=***
	
	return size;
}

//grig
Block::Block(DArray * da, long ProcLI , int a)
{
	int i;
	long vmRank, vmDimSize, dimProcI;
	long amRank, amDimSize, amAxis;
	long daRank, daAxis;
	long amLower, amUpper, BlockSize; // Param, Module;
	bool IsBlockEmpty = false;
	vector<long> ProcSI;
	VM *vm;
	AMView *am;
	DistAxis dist;
	AlignAxis align, alignParam;
	LS ls;

	am = da->AM_Dis;
	amRank = am->Rank();
	vm = am->VM_Dis;
	vmRank = vm->Rank();
	vm->GetSI(ProcLI, ProcSI);

	daRank = da->Rank();
	LSDim.reserve(daRank);


	//grig
	std::vector<double> avWeights;
	int j;
	long local_sum=0; // индекс с которого начанаются веса для данного измерения VM
	long jmax;  //  размер текущего измерения Vm
	double vBlockSize,temp_w=0;    //
	double sum1=0;
	//grig


	// Предварительная инициализация блока (равен массиву)
	for (i = 0; i < daRank; i++)
		LSDim.push_back(LS(0, da->GetSize(i+1)-1));

	for (i = 0; i < vmRank; i++)
	{
		dist = am->DistRule[amRank + i];
		switch (dist.Attr)
		{
		case map_NORMVMAXIS :
			amAxis = dist.Axis;
			vmDimSize = vm->GetSize(i+1);
			amDimSize = am->GetSize(amAxis);
			dimProcI = ProcSI[i];
			
			BlockSize = (amDimSize - 1) / vmDimSize + 1;

			amLower = dimProcI * BlockSize;
			
			amUpper = min(amDimSize, amLower+BlockSize) - 1;
			

					am->weightEl.GetWeights(avWeights);
			 local_sum=0; // индекс с которого начанаются веса для данного измерения VM
			 jmax=vm->GetSize(i+1);  //  размер текущего измерения Vm
			 vBlockSize,temp_w=0;    //
			 sum1=0;
			 long lBlockSize;
					for(j=0;j<i;j++)
					{
					  local_sum+=vm->GetSize(j+1);
					}
					for(j=0;j<jmax;j++)
					{ if(j+local_sum>=am->weightEl.GetSize()) break;
						temp_w+=am->weightEl.body[j+local_sum]; // находим сумму весов
					}
					if(temp_w==0) temp_w=1; //====//
					vBlockSize = amDimSize/temp_w; // размер блока

//					lBlockSize=ceil((double)amDimSize/temp_w) > 0.5 ? amDimSize/temp_w+ 1 : amDimSize/temp_w; // размер блока
					//====
					if(am->BSize.size() > 0)
					{
						if(amDimSize % am->BSize[i] !=0 ) { printf("Error: Dimension %d is not dividible by %d \n",amDimSize, am->BSize[i]); exit(0);}
						lBlockSize=(long)ceil(vBlockSize);
						if( ( lBlockSize % am->BSize[i]) > 0)   
							lBlockSize = ( lBlockSize / am->BSize[i] + 1) * am->BSize[i]; 
						vBlockSize=lBlockSize;
					}
					//=***
/*
					if(vBlockSize - ceil(vBlockSize)<0.5) // если VBlocksize - celoe
					{					
					lBlockSize=floor(vBlockSize);
					}
					else   // нет
						lBlockSize= ceil(vBlockSize);
*/	
//					printf("Blocksize   v=%f l=%d\n",vBlockSize, lBlockSize);

					for(j=0;j<dimProcI;j++)
					{ if(j+local_sum>=am->weightEl.GetSize()) break; //====//
						sum1+=(vBlockSize*am->weightEl.body[j+local_sum]);
					}

					amLower=sum1;
					amUpper=(double)sum1;
					if(dimProcI+local_sum<am->weightEl.GetSize()) amUpper	+= vBlockSize*am->weightEl.body[dimProcI+local_sum]-1; //====//
					if(amUpper+1>=amDimSize-1)
						amUpper=amDimSize-1;


			IsBlockEmpty = IsBlockEmpty || amLower > amUpper;
			if (IsBlockEmpty)
				break;

//			printf("BLOCK[%d] %d %d\n",ProcLI, amLower,amUpper);

			align = da->AlignRule[daRank+amAxis-1];
			

			switch (align.Attr) {
			case align_NORMTAXIS :
				daAxis = align.Axis;
				assert(daAxis != 0);
				alignParam = da->AlignRule[daAxis-1];

				ls = LS(amLower, amUpper);
				ls.transform(alignParam.A, alignParam.B, da->GetSize(daAxis));
				
				if (ls.IsEmpty()) {
					IsBlockEmpty = true;
				}
				else
					LSDim[daAxis-1] = ls;  // LSDim с нуля
				break;

			case align_BOUNDREPL :
				ls = LS(amLower, amUpper);
				ls.transform(align.A, align.B, align.Bound);

				if (ls.IsEmpty())
					IsBlockEmpty = true;
				break;

			case align_REPLICATE :
				break;

			case align_CONSTANT :
				if (align.B < amLower || align.B > amUpper)
					IsBlockEmpty = true;
				break;
			} // end switch
			break;

		case map_REPLICATE :
			break;
		}  // end switch
		if (IsBlockEmpty)
			break;
	} // end for

	if (IsBlockEmpty)
	{
		LSDim = vector<LS>(0);
		#ifdef _TIME_TRACE_
		prot << LSDim.empty() << endl; // потом убрать
		#endif
	}


}
//\grig



Block::Block(DArray * da, long ProcLI)
{
	int i;
	long vmRank, vmDimSize, dimProcI;
	long amRank, amDimSize, amAxis;
	long daRank, daAxis;
	long amLower, amUpper, BlockSize; // Param, Module;
	bool IsBlockEmpty = false;
	vector<long> ProcSI;
	VM *vm;
	AMView *am;
	DistAxis dist;
	AlignAxis align, alignParam;
	LS ls;

	am = da->AM_Dis;
	amRank = am->Rank();
	vm = am->VM_Dis;
	vmRank = vm->Rank();
	vm->GetSI(ProcLI, ProcSI);

	daRank = da->Rank();
	LSDim.reserve(daRank);

	// Предварительная инициализация блока (равен массиву)
	for (i = 0; i < daRank; i++)
		LSDim.push_back(LS(0, da->GetSize(i+1)-1));

	for (i = 0; i < vmRank; i++)
	{
		dist = am->DistRule[amRank + i];
		switch (dist.Attr)
		{
		case map_NORMVMAXIS :
			amAxis = dist.Axis;
			vmDimSize = vm->GetSize(i+1);
			amDimSize = am->GetSize(amAxis);
			dimProcI = ProcSI[i];
			// Param = amDimSize / vmDimSize;
			// Module = amDimSize % vmDimSize;
			// amLower = dimProcI * Param;
			BlockSize = (amDimSize - 1) / vmDimSize + 1;
			amLower = dimProcI * BlockSize;
			
			//if ((Module != 0) && (dimProcI < Module))
			//{
			//	amLower += dimProcI;
			//	Param++;
			//}
			//else
			//	amLower += Module;

			amUpper = min(amDimSize, amLower+BlockSize) - 1;
			IsBlockEmpty = IsBlockEmpty || amLower > amUpper;
			if (IsBlockEmpty)
				break;
			align = da->AlignRule[daRank+amAxis-1];

			switch (align.Attr) {
			case align_NORMTAXIS :
				daAxis = align.Axis;
				assert(daAxis != 0);
				alignParam = da->AlignRule[daAxis-1];

				ls = LS(amLower, amUpper);
				ls.transform(alignParam.A, alignParam.B, da->GetSize(daAxis));
				
				if (ls.IsEmpty()) {
					IsBlockEmpty = true;
				}
				else
					LSDim[daAxis-1] = ls;  // LSDim с нуля
				break;

			case align_BOUNDREPL :
				ls = LS(amLower, amUpper);
				ls.transform(align.A, align.B, align.Bound);

				if (ls.IsEmpty())
					IsBlockEmpty = true;
				break;

			case align_REPLICATE :
				break;

			case align_CONSTANT :
				if (align.B < amLower || align.B > amUpper)
					IsBlockEmpty = true;
				break;
			} // end switch
			break;

		case map_REPLICATE :
			break;
		}  // end switch
		if (IsBlockEmpty)
			break;
	} // end for

	if (IsBlockEmpty)
	{
		LSDim = vector<LS>(0);
		#ifdef _TIME_TRACE_
		prot << LSDim.empty() << endl; // потом убрать
		#endif
	}
}

bool Block::empty()
{
	return LSDim.empty();
}

long Block::GetRank()
{
	return LSDim.size();
}

Block & Block::operator =(const Block & x)
{
	this->LSDim = x.LSDim;
	return *this;
}

bool Block::IsBoundIn(const vector<long>& ALeftBSizeArray, 
	const vector<long>& ARightBSizeArray)
{
	long i;
	for (i = 0; i < LSDim.size(); i++)
	{
		if (!LSDim[i].IsBoundIn(ALeftBSizeArray[i], ARightBSizeArray[i]))
			return false;
	}
	return true;
}

bool Block::IsLeft(long arrDim, long elem)
{
	if (empty())
		return false;
	return LSDim[arrDim-1].IsLeft(elem);
}

bool Block::IsRight(long arrDim, long elem)
{
	if (empty())
		return false;
	return LSDim[arrDim-1].IsRight(elem);
}

long Block::GetBlockSizeMult(long dim)
{
	int i;
	long size = 1;

	if (LSDim.empty())
		return 0;

	for (i = 0; i < LSDim.size(); i++)
	{
		if (i == dim-1)
			continue;
		size *= LSDim[i].GetLSSize();
	}
	return size;
}

long Block::GetBlockSizeMult2(long dim1, long dim2)
{
	int i;
	long size = 1;

	if (LSDim.empty())
		return 0;

	for (i = 0; i < LSDim.size(); i++)
	{
		if (i == dim1-1 || i == dim2-1)
			continue;
		size *= LSDim[i].GetLSSize();
	}
	return size;
}

//====
long Block::GetUpper(long i)
{ return LSDim[i].GetUpper();
}

long Block::GetLower(long i)
{ return LSDim[i].GetLower();
}
//=***


