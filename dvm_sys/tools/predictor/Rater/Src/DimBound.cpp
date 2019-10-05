// DimBound.cpp: implementation of the DimBound class.
//
//////////////////////////////////////////////////////////////////////

#include "DimBound.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

DimBound::DimBound()
{

}

DimBound::~DimBound()
{

}

DimBound::DimBound(long AarrDim, long AvmDim, int Adir, long ALeftBSize, long ARightBSize) :
	arrDim(AarrDim), 
	vmDim(AvmDim), 
	dir(Adir),
	LeftBSize(ALeftBSize), 
	RightBSize(ARightBSize)
{
}

bool operator < (const DimBound& x, const DimBound& y)
{
	return true;
}

bool operator == (const DimBound& x, const DimBound& y)
{
	return 
		(x.arrDim == y.arrDim && 
		x.dir == y.dir && 
		x.LeftBSize == y.LeftBSize && 
		x.RightBSize == y.RightBSize && 
		x.vmDim == y.vmDim) ?
		true : false;
}