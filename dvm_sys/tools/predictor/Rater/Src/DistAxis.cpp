// DistAxis.cpp: implementation of the DistAxis class.
//
//////////////////////////////////////////////////////////////////////

#include "DistAxis.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

DistAxis::DistAxis()
{

}

DistAxis::~DistAxis()
{
	
}

DistAxis::DistAxis(map_Type AAttr, long AAxis, long APAxis) :
Attr(AAttr), Axis(AAxis), PAxis(APAxis)
{

}

DistAxis& DistAxis :: operator= (const DistAxis& DA)
{
	this->Attr = DA.Attr;
	this->Axis = DA.Axis;
	this->PAxis = DA.PAxis;
	return *this;
}

bool operator==(const DistAxis& x, const DistAxis& y)
{
	return x.Attr == y.Attr && x.Axis == y.Axis && x.PAxis == y.PAxis;
}

bool operator<(const DistAxis& x, const DistAxis& y)
{
	if (x.Attr == map_BLOCK || x.Attr == map_COLLAPSE)
		if (y.Attr == map_BLOCK || y.Attr == map_COLLAPSE)
			return x.Axis < y.Axis;
		else
			return true;
	else
		if (y.Attr == map_BLOCK || y.Attr == map_COLLAPSE)
			return false;
		else
			return x.PAxis < y.PAxis;
}
