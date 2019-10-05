// AlignAxis.cpp: implementation of the AlignAxis class.
//
//////////////////////////////////////////////////////////////////////
#include <assert.h>

#include "AlignAxis.h"

using namespace std;
 

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

AlignAxis::AlignAxis()
{

}

AlignAxis::~AlignAxis()
{

}

AlignAxis::AlignAxis(align_Type AAttr, long AAxis, long ATAxis, 
					 long AA, long AB, long ABound) : 
Attr(AAttr), Axis(AAxis), TAxis(ATAxis), A(AA), B(AB), Bound(ABound)
{
}

AlignAxis& AlignAxis :: operator= (const AlignAxis& AA)
{
	Attr = AA.Attr;
	Axis = AA.Axis;
	TAxis = AA.TAxis;
	A = AA.A;
	B = AA.B;
	Bound = AA.Bound;
	return *this;
} 

bool operator == (const AlignAxis& x, const AlignAxis& y)
{
	return x.Attr == y.Attr && x.Axis == y.Axis && x.TAxis == y.TAxis \
		&& x.A == y.A && x.B == y.B && x.Bound == y.Bound;
}

bool operator < (const AlignAxis& x, const AlignAxis& y)
{
	if (x.Attr == align_NORMAL || x.Attr == align_COLLAPSE)
		if (y.Attr == align_NORMAL || y.Attr == align_COLLAPSE)
			return x.Axis < y.Axis;
		else
			return true;
	else
		if (y.Attr == align_NORMAL || y.Attr == align_COLLAPSE)
			return false;
		else
			return x.TAxis < y.TAxis; // В этом случае что возвращать? (при формировании суперпозиции здесь смешаются TAxis)
						// а вроде все в порядке, так как здесь не важно
}

#ifdef P_DEBUG
ostream& operator << (ostream& os, const AlignAxis& aa)
{
	os  << "AlignAxis: Attr = " << aa.Attr << ", Axis = " << aa.Axis 
		<< ", TAxis = " << aa.TAxis << ", A = " << aa.A << ", B = " << aa.B
		<< ", Bound = " << aa.Bound;
	return os;
}
#endif
