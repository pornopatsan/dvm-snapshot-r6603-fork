// LoopLS.cpp: implementation of the LoopLS class.
//
//////////////////////////////////////////////////////////////////////

#include "LoopLS.h"

#include <math.h>
#include <stdlib.h>

#include <algorithm>

#if defined (__GNUG__) && (__GNUC__  >= 3)
template <class T>
T min(T a, T b)
{
    return a < b ? a : b;
}
template <class T>
T max(T a, T b)
{
    return a >= b ? a : b;
}
#endif

LoopLS::LoopLS() : 	
	Lower(-1)
{ 
}

LoopLS::LoopLS(long ALower, long AUpper, long AStep)
{	
	if((ALower <= AUpper) && (AStep > 0)) {
		Lower = ALower;
		Upper = AUpper;
		Step  = AStep;
	} else
		Lower = -1;
}

LoopLS::~LoopLS()
{
}

bool operator==(const LoopLS& x, const LoopLS& y)
{ 
	return x.Lower == y.Lower && x.Upper == y.Upper && x.Step == y.Step;
}

bool operator<(const LoopLS& x, const LoopLS& y)
{	
	return true;
}

long LoopLS::GetLoopLSSize()
{
	return empty() ? 0 : (Upper - Lower + Step) / Step ; //====/
}

bool LoopLS::empty()
{	return (Lower < 0) ? true : false;
}

void LoopLS::transform(long A, long B, long plDimSize)
{
	long daLower, daUpper;
	long displace, displace_0, temp;

	long daB1 = (Lower - B) / A;
	long daB2 = (Upper - B) / A;
	displace = (Lower - B) % A;
	displace_0 = (Upper - B) % A;
	if (A < 0)
	{
		temp = displace;
		displace = displace_0;
		displace_0 = -temp;
	}
#ifdef _MSC_VER
	daLower = __min(daB1, daB2);
	daUpper = __max(daB1, daB2);
#else
	daLower = min(daB1, daB2);
	daUpper = max(daB1, daB2);
#endif
	if ((daLower < 0 && daUpper < 0) || 
		(daLower >= plDimSize) ||
		(daUpper == 0 && displace_0 < 0))
	{
		Lower = -1;
		return;
	}

	if (displace != 0 && daUpper != 0)
		daLower++;
	if (daLower < 0)
		daLower = 0;
	if (daUpper >= plDimSize)
		daUpper = plDimSize - 1;
	if (daUpper < daLower)
	{		
		Lower = -1;
		return;
	}
/* //====//
	Lower = daLower;
	Upper = daUpper;
*/ //====//
	Lower = daLower + B;
	Upper = daUpper + B;
}

