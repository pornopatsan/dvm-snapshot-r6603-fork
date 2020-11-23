#ifndef LoopLSH
#define LoopLSH
//////////////////////////////////////////////////////////////////////
//
// LoopLS.h: interface for the LoopLS class.
//
//////////////////////////////////////////////////////////////////////

class LoopLS { 

public:	

	long Lower;
	long Upper;
	long Step;
	bool Invers;//====//

	LoopLS();
	LoopLS(long ALower, long AUpper, long AStep);
	virtual ~LoopLS();

	void transform(long A, long B, long plDimSize);
	long GetLoopLSSize(); 
	bool empty();
	friend bool operator==(const LoopLS& x, const LoopLS& y);
	friend bool operator<(const LoopLS& x, const LoopLS& y);
};

#endif 
