#ifndef AlignAxisH
#define AlignAxisH

//////////////////////////////////////////////////////////////////////
//
// AlignAxis.h: interface for the AlignAxis class.
//
//////////////////////////////////////////////////////////////////////

#include <fstream>

enum align_Type {
	align_NORMAL	= 0,	// 0
	align_REPLICATE,		// 1
	align_COLLAPSE,			// 2
	align_CONSTANT,			// 3
	align_BOUNDREPL,		// 4   - Additional align styles
	align_NORMTAXIS			// 5   - Normal template's axis
};


// ќписание выравнивани€ конкретного измерени€
class AlignAxis { 
public:

	align_Type Attr;
	long Axis;
	long TAxis;
	long A;
	long B;
	long Bound;

	AlignAxis(align_Type AAttr, long AAxis, long ATAxis, 
		long AA = 0, long AB = 0, long ABound = 0);
	AlignAxis();
	virtual ~AlignAxis();
	AlignAxis& operator= (const AlignAxis&); 

	friend bool operator==(const AlignAxis& x, const AlignAxis& y);
	friend bool operator<(const AlignAxis& x, const AlignAxis& y);

#ifdef P_DEBUG
	friend std::ostream& operator << (std::ostream& os, const AlignAxis& s);
#endif

};

#endif 
