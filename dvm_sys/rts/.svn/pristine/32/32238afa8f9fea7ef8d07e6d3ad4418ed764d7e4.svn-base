#include "TransposedDistribution.h"
#include "Task.h"
#include <vector>
#include "Distribution.h"
#include <map>
#include <set>
#include <cassert>
#include <cstdio>

using std::vector;
using std::map;
using std::set;

#ifndef WIN32
#define DVMH_API __attribute__((visibility("default")))
#else
#define DVMH_API
#endif

extern "C" DVMH_API void map_tasks_(long *taskCount, long *procCount, double *params, long *low_proc, long *high_proc, long *renum) {
	int num = *taskCount;
	int num_proc = *procCount;
	vector<TaskData> initTasks;
	for (int i = 0; i < num; i++) {
		double coef, complexity;
		complexity = params[i * 4 + 0];
		coef = params[i * 4 + 3];
		int minProcs = (int)(params[i * 4 + 1] + 0.5);
		int maxProcs = (int)(params[i * 4 + 2] + 0.5);
		if (maxProcs < 1) maxProcs = num_proc;
		TaskData td(i, complexity * (1.0 - coef), complexity * coef, minProcs, maxProcs);
		initTasks.push_back(td);
	}
	Distribution d;
	try {
	d = getDistributionTransposed(initTasks, num_proc);
	} catch (MyException &e) {
		fprintf(stderr, "Error occured: %s\n", e.getMessage().c_str());
		throw;
	}
	map<TaskData *, TaskInfo> perTask = d.toMap();
	map<double, set<int> > forRenum;
	for (map<TaskData *, TaskInfo>::iterator it = perTask.begin(); it != perTask.end(); it++) {
		//Assuming consequent processors
		int id = (*it).first->getId();
		low_proc[id] = (*it).second.getLowestProcessor() + 1;
		high_proc[id] = (*it).second.getHighestProcessor() + 1;
		assert(high_proc[id] - low_proc[id] + 1 == (*it).second.getK());
		forRenum[(*it).second.getStartTime()].insert(id);
	}
	int i = 0;
	for (map<double, set<int> >::iterator it = forRenum.begin(); it != forRenum.end(); it++) {
		for (set<int>::iterator it2 = (*it).second.begin(); it2 != (*it).second.end(); it2++) {
			assert(i < num);
			renum[i++] = (*it2) + 1;
		}
	}
}
