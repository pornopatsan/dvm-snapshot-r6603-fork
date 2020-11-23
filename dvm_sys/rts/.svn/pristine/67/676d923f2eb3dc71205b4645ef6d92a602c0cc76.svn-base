#include "NaiveDistribution.h"
#include <algorithm>

bool lessThan(const TaskData &t1, const TaskData &t2)
{
	return t1.getTime(1)>t2.getTime(1);
}

Distribution getDistributionNaive(const vector<TaskData> &initData, int numProc)
{
	Distribution res(numProc, true);
	vector<TaskData> vec(initData);
	vector<double> maxs(numProc);
	sort(vec.begin(),vec.end(),lessThan);
	for (unsigned i=0;i<vec.size();i++)
	{
		int minInd=0;
		for (unsigned j=1;j<maxs.size();j++)
			if (maxs[j]<maxs[minInd]) minInd=j;
		res.add(new Task(vec[i],1),maxs[minInd],maxs[minInd]+vec[i].getTime(1),minInd);
		maxs[minInd]+=vec[i].getTime(1);
	}
	return res;
}
