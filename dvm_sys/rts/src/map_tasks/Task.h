#ifndef _Task
#define _Task

#include "TaskData.h"

class Task: public TaskData
{
protected:
	int k;
public:
	Task(const TaskData &td, int aK = 1):TaskData(td), k(aK) {}
	double getTime() const {return TaskData::getTime(k);}
	void setK(int aK);
	int getK() const {return k;}
	virtual bool inBounds(int aK) const {return k == aK;}
};

#endif
