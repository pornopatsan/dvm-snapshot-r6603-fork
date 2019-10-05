#include "Distribution.h"
#include "MyException.h"
#include <map>

using std::map;
using std::endl;

inline int min(int a, int b)
{
	return a<b?a:b;
}

inline int max(int a, int b)
{
	return a>b?a:b;
}

map<TaskData *, TaskInfo> Distribution::toMap(bool *pRes) {
	map<TaskData *, TaskInfo> perTask;
	bool res = pRes ? *pRes : true;
	for (unsigned i = 0; i < processors.size(); i++)
		res = res && processors[i].toMap(perTask, i);
	if (pRes)
		*pRes = res;
	return perTask;
}

bool Distribution::isConsistent()
{
	bool res=true;
	for (unsigned i=0;i<processors.size();i++)
		res = res && processors[i].isConsistent();
	map<TaskData *, TaskInfo> perTask = toMap(&res);
	map<TaskData *, TaskInfo>::iterator it = perTask.begin();
	while (it != perTask.end())
	{
		res = res && (*it).first->inBounds((*it).second.getK());
		it++;
	}
	return res;
}

void Distribution::toTextDiagram(ostream &out, int height, bool forceDiagram)
{
	double minTime,maxTime;
	minTime=1e6;
	maxTime=-1e6;
	for (unsigned i=0;i<processors.size();i++)
	{
		double amin,amax;
		processors[i].getMinMaxTimes(amin,amax);
		if (minTime>amin) minTime=amin;
		if (maxTime<amax) maxTime=amax;
	}
	if (height<=0) throw MyException("Height must be positive");
	double step=(maxTime-minTime)/height;
	if (step<=0) throw MyException("There are no load");
	map<TaskData *, char> notation;
	notation.insert(pair<TaskData *,char>(NULL,'-'));
	const char notationBase = 'A';
	const char notationMax = 'Z';
	out<<"Total time: "<<time<<"  Summar tasks time: "<<totalTime<<"  Compactness: "<<totalTime/time/numProc<<endl;
	if (numProc <= 26 || forceDiagram)
		for (int j = height - 1; j >= 0; j--)
		{
			double curTime=minTime + (j + 0.5) * step;
			int p1 = curTime;
			int p2 = (int)(curTime * 1000);
			out << p1 << "." << p2 / 100 << p2 / 10 % 10 << p2 % 10 << "\t";
			for (unsigned i=0; i < processors.size(); i++)
			{
				out << " ";
				TaskData *curTask = processors[i].getCurrentTask(curTime);
				map<TaskData *,char>::iterator it;
				if ((it = notation.find(curTask)) == notation.end())
					it=notation.insert(pair<TaskData *,char> (curTask, min(notationBase+max(0,curTask->getId()),notationMax))).first;
				out<<(*it).second;
				out<<" ";
			}
			out<<endl;
		}
	//out<<"Average time of task: "<<totalTime/(notation.size()-1)<<endl;
}

ostream& operator<< (ostream &out, Distribution d)
{
	d.toTextDiagram(out);
	return out;
}

Distribution::Distribution(const Distribution &ad):numProc(ad.numProc),time(ad.time),totalTime(ad.totalTime),freeFlag(ad.freeFlag),processors(ad.processors)
{
	if (freeFlag)
	{
		map<TaskData *, TaskData *> replacement;
		for (unsigned i=0;i<processors.size();i++)
		{
			processors[i].makeReplacement(replacement);
		}
	}
}

Distribution::~Distribution()
{
	if (freeFlag)
	{
		set<TaskData *> already;
		for (unsigned i=0;i<processors.size();i++)
		{
			processors[i].destroy(already);
		}
	}
}
