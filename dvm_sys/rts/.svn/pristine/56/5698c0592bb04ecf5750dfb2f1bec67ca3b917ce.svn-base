#include <set>
#include <algorithm>
#include "ProcessorLoad.h"

bool ProcessorLoad::isConsistent()
{
	bool res=true;
	const double eps=1e-9;
	set<TaskData *> tds;
	if (!sorted) {sort(tasks.begin(),tasks.end());sorted=true;}
	if (tasks.size()>0)
	for (unsigned i = 0;i<tasks.size()-1;i++)
	{
		res=res&&tasks[i].endTime<tasks[i+1].startTime+eps&&tds.find(tasks[i].td)==tds.end();
		tds.insert(tasks[i].td);
	}
	return res;
}

bool ProcessorLoad::toMap(map<TaskData *, TaskInfo> &perTask, int myIndex) const
{
	bool res=true;
	for (unsigned i=0;i<tasks.size();i++)
	{
		map<TaskData *, TaskInfo>::iterator it;
		if ((it = perTask.find(tasks[i].td)) == perTask.end())
			perTask.insert(pair<TaskData *, TaskInfo>(tasks[i].td, TaskInfo(tasks[i].startTime, tasks[i].endTime, myIndex)));
		else
			res = res && (*it).second.add(tasks[i].startTime, tasks[i].endTime, myIndex);
	}
	return res;
}

void ProcessorLoad::getMinMaxTimes(double &minT, double &maxT) const
{
	if (!sorted||tasks.size()==0)
		for (unsigned i=0;i<tasks.size();i++)
		{
			if (minT>tasks[i].startTime) minT=tasks[i].startTime;
			if (maxT<tasks[i].endTime) maxT=tasks[i].endTime;
		}
	else
	{
		minT=tasks[0].startTime;
		maxT=tasks[tasks.size()-1].endTime;
	}
}

TaskData *ProcessorLoad::getCurrentTask(double t)
{
	if (!sorted) {sort(tasks.begin(),tasks.end());sorted=true;}
	int i1=0;
	int i2=(int)tasks.size()-1;
	if (tasks[i2].endTime<=t) return NULL;
	if (tasks[i1].startTime>t) return NULL;
	while (i2>i1+1)
	{
		int i3=(i1+i2)/2;
		if (tasks[i3].startTime>t) i2=i3;
		else i1=i3;
	}
	if (tasks[i1].endTime>t) return tasks[i1].td;
	if (tasks[i2-1].startTime<=t&&tasks[i2-1].endTime>t) return tasks[i2-1].td;
	if (tasks[i2].startTime<=t&&tasks[i2].endTime>t) return tasks[i2].td;
	return NULL;
}

void ProcessorLoad::makeReplacement(map<TaskData *, TaskData *> &replacement)
{
	for (unsigned i=0;i<tasks.size();i++)
	{
		map<TaskData *, TaskData *>::iterator it;
		it=replacement.find(tasks[i].td);
		if (it==replacement.end())
		{
			TaskData *ntd = new TaskData(*(tasks[i].td));
			replacement.insert(pair<TaskData *, TaskData *> (tasks[i].td, ntd));
			delete tasks[i].td;
			tasks[i].td=ntd;
		}
		else
			tasks[i].td=(*it).second;
	}
}

void ProcessorLoad::destroy(set<TaskData *> &already)
{
	for (unsigned i=0;i<tasks.size();i++)
	{
		if (already.find(tasks[i].td)!=already.end())
		{
			already.insert(tasks[i].td);
			delete tasks[i].td;
		}
	}
}
