#ifndef _Distribution
#define _Distribution

#include <vector>
#include <iostream>
#include "TaskData.h"
#include "Task.h"
#include "TaskInfo.h"
#include "ProcessorLoad.h"

using std::vector;

class Distribution
{
protected:
	int numProc;
	double time;
	double totalTime;
	bool freeFlag;
	vector<ProcessorLoad> processors;
public:
	Distribution() {numProc = 0; totalTime = time = 0; freeFlag = false;}
	explicit Distribution(int aNumProc, bool ff=false):numProc(aNumProc), freeFlag(ff) {
		totalTime = time = 0;
		processors.resize(numProc);
	}
	Distribution(const Distribution &ad);
	~Distribution();
	void add(TaskData *td, double st, double et, int np)
	{
		processors[np].add(ProcessorLoadItem(st, td, et));
		if (time < et) time = et;
		totalTime += et - st;
	}
	void add(Task *t, double st, int np)
	{
		add(t, st, st + t->getTime(), np);
	}
	map<TaskData *, TaskInfo> toMap(bool *pRes = NULL);
	bool isConsistent();
	void toTextDiagram(ostream& out, int height = 20, bool forceDiagram = false);
	double getTotalTime() const
	{
		return totalTime;
	}
};
ostream& operator<< (ostream &out, Distribution d);

#endif
