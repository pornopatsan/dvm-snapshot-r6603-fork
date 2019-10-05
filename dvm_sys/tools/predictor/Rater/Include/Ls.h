#ifndef LSH
#define LSH

//////////////////////////////////////////////////////////////////////
//
// LS.h: interface for Line Segment (LS) class.
//
//////////////////////////////////////////////////////////////////////

#include <algorithm>

// Line Segment
class LS { 
	
	long Lower;
	long Upper;
	long Stride;

public:	

	bool IsRight(long elem) const;
	bool IsLeft(long elem) const ;
	bool IsBoundIn(long ALeftBSize, long ARightBSize) const;

	LS(long ALower, long AUpper, long AStride = 1);
	LS();
	virtual ~LS();
	long GetLSSize() const; 
	bool IsEmpty() const;
	// изменяет LS AMView в LS DArray-я
	void transform(long A, long B, long daDimSize);

	// intersection operator (Lower > Upper if intersection empty)
	LS operator^ (const LS &x) const;

	friend bool operator==(const LS& x, const LS& y);
	friend bool operator<(const LS& x, const LS& y);

//====
	long GetLower();
	long GetUpper();
//=***

};


#endif 
