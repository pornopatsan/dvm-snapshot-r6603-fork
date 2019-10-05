#include <assert.h>

#include "Vm.h"

using namespace std;

extern ofstream prot; 

extern VM	*	rootVM = NULL;			// pointer to root VM
extern VM	*	currentVM = NULL;		// pointer to current VM

//grig

extern long_vect MinSizesOfAM; // для автоматического поиска
//\grig
 
//---------------------------------------------------------------------------------
//
// Constructor for root VM
//
//---------------------------------------------------------------------------------
VM::VM(const std::vector<long>& ASizeArray, mach_Type AMType, int AnumChanels, 
	   double Ascale, double ATStart, double ATByte, double AProcPower,std::vector<double> & AvProcPower) :
	Space(ASizeArray),
	parent(NULL),
	MType(AMType), 
	numChanels(AnumChanels),
	scale(Ascale),
	TStart(ATStart), 
	TByte(ATByte),
	ProcPower(AProcPower),
	ProcCount(procCount())
{
	int i,j,l;
	int rank = SizeArray.size();

///	for(i=0;i<ASizeArray.size();i++)
//	{
//		printf("%d ",ASizeArray[i]);
//	}
//	printf("\n");

//	printf("rank=%d\n",this->Rank());

	// weight
	int k = 0;
	for (i = 0; i < rank; i++)
		k += SizeArray[i];
	weight = vector<double>(k, 1.0);

//grig
	vWeights = vector<double>(k,1.0);  // init weights array
	vProcPower.resize(0);
	for(i=0;i<ProcCount;i++)
	{	
		vProcPower.push_back(AvProcPower[i]);	
	}
//\grig

	// initialize mapping;
	mapping = vector<int>(ProcCount); 

	//	prot << "ProcCount = " << ProcCount << " rank = " << rank << endl;
	// set mapping
	// root VM : mapping 1:1
	switch (rank) {
		case 1:
			for (i = 0; i < SizeArray[0]; i++)
				mapping[i] = i;
			break;
		case 2:
			for (j = 0; j < SizeArray[0]; j++)
				for (i = 0; i < SizeArray[1]; i++) {
//					cout << " i = " << i << " j = " << j << endl;
					mapping[i + MultArray[0] * j] = i + MultArray[0] * j;
				}
			break;
		case 3:
			for (k = 0; k < SizeArray[0]; k++)
				for (j = 0; j < SizeArray[1]; j++)
					for (i = 0; i < SizeArray[2]; i++)
						mapping[i + MultArray[1] * j + MultArray[0] * k] = 
							i + MultArray[1] * j + MultArray[0] * k;
			break;
		case 4:
			for (l = 0; l < SizeArray[0]; l++)
				for (k = 0; k < SizeArray[1]; k++)
					for (j = 0; j < SizeArray[2]; j++)
						for (i = 0; i < SizeArray[3]; i++)
							mapping[i + MultArray[2] * j + MultArray[1] * k + MultArray[0] * l] = 
								i + MultArray[2] * j + MultArray[1] * k + MultArray[0] * l;
			break;
		default:
			prot << "VM rank more then 4 (rank = " << rank << ')' << endl;
			exit(EXIT_FAILURE);
	}

//	for(i=0;i<ProcCount;i++)
//	{
//		printf("::TEST MAPPING : logical num=%d, it's mapped to %d processor of PS it's power is %f\n",i,map(i),getProcPower(i));
//	}

	//for(i=0;i<GetSize(i+1);i++)
	//	for(j=0;j<GetSize)
/*	std::vector<long> sss;
	sss.resize(2);
	sss[0]=1;
	sss[0]=2;
	long rrr=GetLI(sss);
*/	
}


VM::VM(const std::vector<long>& ASizeArray, mach_Type AMType, int AnumChanels, 
		double Ascale, double ATStart, double ATByte,  std::vector<double>& AvProcPower) :
	Space(ASizeArray),
	parent(NULL),
	MType(AMType), 
	numChanels(AnumChanels),
	scale(Ascale),
	TStart(ATStart), 
	TByte(ATByte),
	ProcPower(1.0),
	ProcCount(procCount())
{
	int i,j,l;
	int rank = SizeArray.size();

//	for(i=0;i<ASizeArray.size();i++)
//	{
//		printf("%d ",ASizeArray[i]);
//	}
//	printf("\n");

//	printf("rank=%d\n",this->Rank());

	// weight
	int k = 0;
	for (i = 0; i < rank; i++)
		k += SizeArray[i];
	weight = vector<double>(k, 1.0);

//grig
	vWeights = vector<double>(k,1.0);  // init weights array
	vProcPower.resize(0);
	for(i=0;i<ProcCount;i++)
	{	
		vProcPower.push_back(AvProcPower[i]);	
	}
//\grig

	// initialize mapping;
	mapping = vector<int>(ProcCount); 

	//	prot << "ProcCount = " << ProcCount << " rank = " << rank << endl;
	// set mapping
	// root VM : mapping 1:1
	switch (rank) {
		case 1:
			for (i = 0; i < SizeArray[0]; i++)
				mapping[i] = i;
			break;
		case 2:
			for (j = 0; j < SizeArray[0]; j++)
				for (i = 0; i < SizeArray[1]; i++) {
//					cout << " i = " << i << " j = " << j << endl;
					mapping[i + MultArray[0] * j] = i + MultArray[0] * j;
				}
			break;
		case 3:
			for (k = 0; k < SizeArray[0]; k++)
				for (j = 0; j < SizeArray[1]; j++)
					for (i = 0; i < SizeArray[2]; i++)
						mapping[i + MultArray[1] * j + MultArray[0] * k] = 
							i + MultArray[1] * j + MultArray[0] * k;
			break;
		case 4:
			for (l = 0; l < SizeArray[0]; l++)
				for (k = 0; k < SizeArray[1]; k++)
					for (j = 0; j < SizeArray[2]; j++)
						for (i = 0; i < SizeArray[3]; i++)
							mapping[i + MultArray[2] * j + MultArray[1] * k + MultArray[0] * l] = 
								i + MultArray[2] * j + MultArray[1] * k + MultArray[0] * l;
			break;
		default:
			prot << "VM rank more then 4 (rank = " << rank << ')' << endl;
			exit(EXIT_FAILURE);
	}

//	for(i=0;i<ProcCount;i++)
//	{
//		printf("::TEST MAPPING : logical num=%d, it's mapped to %d processor of PS it's power is %f\n",i,map(i),getProcPower(i));
//	}
//
	//for(i=0;i<GetSize(i+1);i++)
	//	for(j=0;j<GetSize)
/*	std::vector<long> sss;
	sss.resize(2);
	sss[0]=1;
	sss[0]=2;
	long rrr=GetLI(sss);
*/	
}





//---------------------------------------------------------------------------------
//
// Constructor for child VM (crtps_)
//
//---------------------------------------------------------------------------------

VM::VM(const std::vector<long>& lb, const std::vector<long>& ASizeArray, const VM* Aparent) :
	Space(ASizeArray),
	parent(Aparent),
	MType(parent->MType), 
	numChanels(parent->numChanels), 
	scale(parent->scale), 
	TStart(parent->TStart), 
	TByte(parent->TByte),
	ProcPower(parent->ProcPower),
	ProcCount(procCount())
{
	int i,j,l;
	int rank = SizeArray.size();

	// weight
	int k = 0;
	for (i = 0; i < rank; i++)
		k += SizeArray[i];
	weight = vector<double>(k, 1.0);
	
//grig
	vWeights=vector<double>(k,1.0);
//\grig

	// initialize mapping;
	mapping = vector<int>(ProcCount); 

	// set mapping
	// child VM
	switch (rank) {
		case 1:
			for (i = 0; i < SizeArray[0]; i++)
				mapping[i] = parent->mapping[i + lb[0]];
			break;
		case 2:
			for (j = 0; j < SizeArray[0]; j++)
				for (i = 0; i < SizeArray[1]; i++)
					mapping[i + MultArray[0] * j] = 
						parent->mapping[ i + lb[1] + 
										(j + lb[0]) * parent->MultArray[0]];
			break;
		case 3:
			for (k = 0; k < SizeArray[0]; k++)
				for (j = 0; j < SizeArray[1]; j++)
					for (i = 0; i < SizeArray[2]; i++)
						mapping[i + MultArray[1] * j + MultArray[0] * k] = 
							parent->mapping[ i + lb[2] + 
											(j + lb[1]) * parent->MultArray[1] +
											(k + lb[0]) * parent->MultArray[0]];
			break;
		case 4:
			for (l = 0; l < SizeArray[0]; l++)
				for (k = 0; k < SizeArray[1]; k++)
					for (j = 0; j < SizeArray[2]; j++)
						for (i = 0; i < SizeArray[3]; i++)
							mapping[i + MultArray[2] * j + MultArray[1] * k + MultArray[0] * l] = 
							parent->mapping[ i + lb[3] + 
											(j + lb[2]) * parent->MultArray[2] +
											(k + lb[1]) * parent->MultArray[1] +
											(l + lb[0]) * parent->MultArray[0]];

			break;
		default:
			prot << "VM rank more then 4 (rank = " << rank << ')' << endl;
			exit(EXIT_FAILURE);
	}

//grig
	vProcPower.resize(0);
	
	for(i=0;i<ProcCount;i++)
		this->vProcPower.push_back(1.0);

	for(i=0;i<ProcCount;i++)
		this->vProcPower[i]=Aparent->vProcPower[i];
//\grig



}

//---------------------------------------------------------------------------------
//
// Constructor for child VM (psview_)
//
//---------------------------------------------------------------------------------

VM::VM(const std::vector<long>& ASizeArray, const VM* Aparent) :
	Space(ASizeArray),
	parent(Aparent),
	MType(parent->MType), 
	numChanels(parent->numChanels), 
	scale(parent->scale), 
	TStart(parent->TStart), 
	TByte(parent->TByte),
	ProcPower(parent->ProcPower),
	ProcCount(procCount()),
	mapping(parent->mapping)
{
	int k,
		i;


//grig
	vProcPower.resize(0);
	for(i=0;i<Aparent->ProcCount;i++)
		this->vProcPower.push_back(1.0);
	for(i=0;i<ProcCount;i++)
		this->vProcPower[i]=Aparent->vProcPower[i];
//\grig

	// weight
	for (k = 0, i = 0; i < SizeArray.size(); i++)
		k += SizeArray[i];
	weight = vector<double>(k, 1.0);
	
	//grig
	vWeights=vector<double>(k,1.0);
	//\grig
}

//---------------------------------------------------------------------------------
//
// Set weights for VM
//
//---------------------------------------------------------------------------------
void VM::setWeight(const std::vector<double>& Aweight)
{
	assert(Aweight.size() == weight.size());
	weight = Aweight;

	//grig
	vWeights=Aweight;
	//\grig
}
//---------------------------------------------------------------------------------
//
// calculates number of processors in VM
//
//---------------------------------------------------------------------------------
int VM::procCount()
{
	ProcCount = 1;
	for (int i = 0; i < SizeArray.size(); i++) {
		ProcCount *= SizeArray[i];
	}
	return ProcCount;
}

VM::~VM()
{
	if(this->parent!=NULL) 
		parent=NULL;
	mapping.resize(0);
	weight.resize(0);

	vProcPower.resize(0);
	vWeights.resize(0);
}




#ifdef P_DEBUG

ostream& operator << (ostream& os, const VM& vm)
{
	int i;
	unsigned int rank = vm.mapping.size();

	os << (Space) vm;

	os << " Map =";
	for (i = 0; i < rank; i++ )
		os << ' ' << vm.mapping[i];
	os << ' ';
	return os;
}

#endif

