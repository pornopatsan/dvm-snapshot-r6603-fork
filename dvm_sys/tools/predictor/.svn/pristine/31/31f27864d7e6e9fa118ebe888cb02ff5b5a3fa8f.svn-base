#ifndef _FuncCall_H
#define _FuncCall_H

#include "TraceLine.h"
#include <vector>
using namespace std;

// Structure for the final stage of file parsing -- call graph with parameters

class FuncCall {
public:
	Event		func_id;		// function identifier
    double		call_time;		// call time
    double		ret_time;		// return time
    void	*	call_params;	// pointer to structure with function params
    int			source_line;
    char	*	source_file;

//grig
	vector<double> vcall_time;
	vector<double> vret_time;
//\grig


public:

	FuncCall();
	FuncCall(VectorTraceLine *traceLines);
	~FuncCall();

	void RegularTime();
	void UnknownTime();
	void IntervalTime();
	void DArrayTime();
		void crtda();
		void align();
		void delda();
		void realn(); 
		void arrcpy();
		void aarrcp();
		void waitcp();
	void MPS_AMTime();
		void crtps();
		void psview();
		void getps();
		void setelw();
		void delps();
		void getam();
		void getamr();
		void crtamv();
		void delamv();
		void mapam();
		void runam();
		void stopam();
		void blkdiv(); //====//
		void distr();
		void RedisTime();

	void ParLoopTime();
		void crtpl();
		void endpl();
		void mappl();
		void dopl();
	void ReductTime();
		void crtrg();
		void crtred();
		void insred();
		void delred();
		void delrg();
		void strtrd();
		void waitrd();
		void across();
	void ShadowTime();
		void crtshg();
		void inssh();
		void insshd();
		void incsh();
		void incshd();
		void delshg();
		void strtsh();
		void waitsh();
		void exfrst();
		void imlast();
		void sendsh();
		void recvsh();
	void IOTime();
		void ciotime();
		void biof();
		void tstio();
		void srmem();
		void eiof();
	void RemAccessTime();
//		void crtbl();
		void crtrbl();
		void crtrbp();
		void loadrb();
		void waitrb();
};

#endif 
