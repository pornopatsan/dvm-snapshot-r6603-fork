#ifndef _ProcessorLoad
#define _ProcessorLoad

#include <map>
#include "TaskData.h"
#include "Task.h"
#include "TaskInfo.h"
#include <vector>
#include <set>

using std::vector;
using std::set;
using std::map;

struct ProcessorLoadItem
{
	double startTime;
	TaskData *td;
	double endTime;
	ProcessorLoadItem() {startTime = 0; td = NULL; endTime = 0;}
	ProcessorLoadItem(double st, TaskData *atd, double et):startTime(st), td(atd), endTime(et) {}
	bool operator< (const ProcessorLoadItem &pli) const {return startTime < pli.startTime;}
};

class ProcessorLoad
{
protected:
	vector<ProcessorLoadItem> tasks;
	bool sorted;
public:
	ProcessorLoad() {sorted = true;}
	void add(ProcessorLoadItem pli)
	{
		tasks.push_back(pli);
		sorted = false;
	}
	void add(Task *t)
	{
		double st = tasks.size() > 0 ? tasks[tasks.size() - 1].endTime : 0;
		add(ProcessorLoadItem(st, t, st + t->getTime()));
	}
	bool isConsistent();
	bool toMap(map<TaskData *, TaskInfo> &perTask, int myIndex) const;
	void getMinMaxTimes(double &minT, double &maxT) const;
	TaskData *getCurrentTask(double t);
	void makeReplacement(map<TaskData *, TaskData *> &replacement);
	void destroy(set<TaskData *> &already);
};

#endif
