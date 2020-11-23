#ifndef _TaskData
#define _TaskData

#include "MyException.h"
#include <iostream>

using std::pair;
using std::ostream;
using std::istream;

class TaskData
{
protected:
	int id;
	double tSeq;
	double tTotalPar;
	int kMin;
	int kMax;
public:
	TaskData() {id = 0; tSeq = 0; tTotalPar = 1; kMin = 1; kMax = 10000;}
	TaskData(int aid, double atSeq, double atTotalPar, int akMin, int akMax):
		id(aid), tSeq(atSeq), tTotalPar(atTotalPar), kMin(akMin), kMax(akMax) {}
	double getTime (int k) const;
	double getMinTotalTime() const {return kMin * getTime(kMin);}
	double getMaxTotalTime() const {return kMax * getTime(kMax);}
	double getMinTime() const {return getTime(kMax);}
	double getMaxTime() const {return getTime(kMin);}
	int getId() const {return id;}
	pair<int, int> getBounds() const {return pair<int, int> (kMin, kMax);}
	virtual bool inBounds(int k) const {return k >= kMin && k <= kMax;}
	int getKmax() const {return kMax;}
	int getKmin() const {return kMin;}
	bool operator== (const TaskData &td) const {return id == td.id;}
	friend ostream& operator<< (ostream &out, const TaskData &td);
	friend istream& operator>> (istream &in, TaskData &td);
};

#endif
