//---------------------------------------------------------------------------
#include "Space.h"

using namespace std;

extern ofstream prot; 
 
Space::Space(const vector<long>& ASizeArray) :
	SizeArray(ASizeArray)
{
	unsigned int rank = ASizeArray.size();
	vector<long> ini(rank);
//==== "long i" -> "int i"
	int i;
//=***
	
//	Rank = ARank;
//	SizeArray = ASizeArray;
//	SizeArray.reserve(rank); 
//	copy(ASizeArray.begin(), ASizeArray.end(), back_inserter(SizeArray));
	ini[rank-1] = 1;
	for (i = rank-2; i >= 0; i--)
		ini[i] = ini[i+1] * SizeArray[i+1];
	MultArray = ini;
}

#ifdef P_DEBUG

ostream& operator << (ostream& os, const Space& s)
{
	int i;
	unsigned int rank = s.SizeArray.size();

	os << "Space: rank = " << rank << " SizeArray =";
	for (i = 0; i < rank; i++ )
		os << ' ' << s.SizeArray[i];
	os << "; MultArray =";
	for (i = 0; i < rank; i++ )
		os << ' ' << s.MultArray[i];
	os << ';';
	return os;
}

#endif

Space::~Space()
{
}

/*long Space::GetRank()
{
	return Rank;
}
*/
long Space::GetSize(long AAxis)
{
//	printf("space::getsize :: rank()=%d\n",Rank());
	if (AAxis < 1 || AAxis > Rank())
		return -1;
	// измерения нумеруются с 1
	return SizeArray[AAxis-1];
}

long Space::GetMult(long AAxis)
{
	if (AAxis < 1 || AAxis > Rank())
		return -1;
	// измерения нумеруются с 1
	return MultArray[AAxis-1];
}


long Space::GetLSize()
{
	long i, lsize = 1;

	for(i = 0; i < Rank(); i++)
		lsize *= SizeArray[i];
	return lsize;
}

void Space::GetSI(long LI, vector<long> & SI)
{
  int i;
  
  SI = vector<long>(Rank());

  for (i=0; i < Rank(); i++)
  { 
	  SI[i] = LI / MultArray[i];
	  LI  -= SI[i] * MultArray[i];
  }
}

Space::Space(const Space &x) :
	SizeArray(x.SizeArray),
	MultArray(x.MultArray)
{
//	Rank = x.Rank();
//	SizeArray = x.SizeArray;
//	MultArray = x.MultArray;
}

Space & Space::operator= (const Space & x)
{
	if (this != &x)
	{
//		Rank = x.Rank();
		SizeArray = x.SizeArray;
		MultArray = x.MultArray;
	}
	return * this;
}


Space::Space(const vector<long>& ASizeArray, const vector<long> AMultArray) :
	SizeArray(ASizeArray), 
	MultArray(AMultArray)
{

}

	Space::Space() :
	SizeArray(vector<long>(0)), 
	MultArray(vector<long>(0))
{
//	Rank = 0;
//	SizeArray = vector<long>(0);
//	MultArray = vector<long>(0);
}

long Space::GetDistance(long LI1, long LI2)
{
	vector<long> SI1, SI2;
	int i;
	long distance = 0;

	GetSI(LI1, SI1);
	GetSI(LI2, SI2);

	for (i = 0; i < Rank(); i++)
		distance += abs(SI1[i] - SI2[i]);

	return distance;
}

/*inline long abs(long x)
{
	return (x < 0) ? (-x) : (x);
}
*/
long Space::GetLI(const vector<long> & SI)
{
	int     i;
	long   LI = 0;

//	printf("GET LI rank=%d size=%d\n",Rank(),SI.size());
	if (Rank() != SI.size())
	{
		prot << "Wrong call GetLI" << endl;
//		printf("Wrong call GetLI\n");
		exit(1);
	}

	for (i=0; i < Rank(); i++)
	{ 
		LI  += SI[i] * MultArray[i];
	}

	return LI;
}

long Space::GetSpecLI(long LI, long dim, int shift)
{
	vector<long> SI;

	GetSI(LI, SI);
	SI[dim-1] += shift;
	return GetLI(SI);
}

long Space::GetCenterLI()
{
	vector<long> CenterSI(Rank());
	int i;

	// геометрический центр (наиболее удален от 0 процессора)
	for (i = 0; i < Rank(); i++) 
		CenterSI[i] = SizeArray[i] / 2;

	return GetLI(CenterSI);
}

long Space::GetNumInDim(long LI, long dimNum)
{
	vector<long> SI;

	GetSI(LI, SI);
	return SI[dimNum - 1];
}
