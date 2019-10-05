#ifndef _TaskInfo
#define _TaskInfo

#include <cmath>
#include <set>

using std::set;

class TaskInfo
{
	double startTime;
	double endTime;
	set<int> processors;
public:
	TaskInfo(double st, double et, int p) {startTime = st; endTime = et; processors.insert(p);}
	void setTimes(double st, double et) {startTime = st; endTime = et;}
	int getK() {return processors.size();}
	double getStartTime() {return startTime;}
	bool add(double st, double et, int p) {
		const double eps = 1e-9;
		if (fabs(st - startTime) > eps || fabs(et - endTime) > eps)
			return false;
		return processors.insert(p).second;
	}
	int getLowestProcessor() {return *(processors.begin());}
	int getHighestProcessor() {return *(processors.rbegin());}
};

#endif
