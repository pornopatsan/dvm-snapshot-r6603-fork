#ifndef LoopBlockH
#define LoopBlockH

//////////////////////////////////////////////////////////////////////
//
// LoopBlock.h: interface for the LoopBlock class.
//
//////////////////////////////////////////////////////////////////////

#include <vector>

#include "LoopLS.h"
#include "ParLoop.h"

class ParLoop;

class LoopBlock {
public:	
	std::vector<LoopLS> LSDim; // vector of LoopLS for every dimensions

	long GetRank();
	bool empty();
	long GetBlockSize();

	LoopBlock();
	LoopBlock(std::vector<LoopLS> arg) 
	{
		LSDim.resize(0); 
		for(int i=0;i<arg.size();i++) 
			LSDim.push_back(arg[i]); 
	}
	virtual ~LoopBlock();
	LoopBlock(ParLoop *pl, long ProcLI);
//grig
	LoopBlock(ParLoop *pl, long ProcLI,int a); // a - незначащий параметр
//grig

	friend bool operator == (LoopBlock& x, LoopBlock& y);
	friend int intersection(LoopBlock& x,LoopBlock&y);
};


#endif 

