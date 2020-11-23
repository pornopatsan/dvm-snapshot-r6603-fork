// RedVar.cpp: implementation of the RedVar class.
//
//////////////////////////////////////////////////////////////////////

#include "RedVar.h"

using namespace std;
 
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

RedVar::RedVar()
{
}

RedVar::~RedVar()
{
}

RedVar::RedVar(long ARedElmSize, long ARedArrLength, long ALocElmSize) : 
	RedElmSize(ARedElmSize), 
	RedArrLength(ARedArrLength), 
	LocElmSize(ALocElmSize)
{

}

long RedVar::GetSize()
{
	return (LocElmSize + RedElmSize) * RedArrLength;
}
