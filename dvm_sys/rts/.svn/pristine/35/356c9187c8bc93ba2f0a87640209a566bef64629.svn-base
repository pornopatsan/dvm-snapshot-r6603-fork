#include "TransposedDistribution.h"
#include <list>
#include <algorithm>
#include <iostream>
#include "MyException.h"
#include <ctime>

using std::list;
using std::map;
using std::make_pair;
using std::endl;
using std::min;
using std::max;

static bool TDReverseLess(const TaskData &t1, const TaskData &t2)
{
	return t1.getMinTotalTime() > t2.getMinTotalTime();
}

class ApproxDouble
{
protected:
	double st;
	double eps;
	static double defEps;
public:
	ApproxDouble(double ast = 0.0):st(ast){eps = defEps;}
	ApproxDouble(double ast, double aeps):st(ast), eps(aeps){}
	static void setDefEps(double aeps){defEps = aeps;}
	double getValue() const {return st;}
	operator double() const {return getValue();}
	bool operator< (const ApproxDouble &st2) const {return st + eps / 2 < st2.st - st2.eps / 2;}
	bool operator== (const ApproxDouble &st2) const {return !((*this) < st2 || st2 < (*this));}
	bool operator<= (const ApproxDouble &st2) const {return !(st2 < (*this));}
	bool operator> (const ApproxDouble &st2) const {return st2 < (*this);}
	bool operator>= (const ApproxDouble &st2) const {return !((*this) < st2);}
	ApproxDouble &operator+= (const ApproxDouble &st2) {st += st2.st; return *this;}
	ApproxDouble &operator-= (const ApproxDouble &st2) {st -= st2.st; return *this;}
	ApproxDouble operator+ (const ApproxDouble &st2) const {return ApproxDouble(st + st2.st, (eps + st2.eps) / 2);}
};

double ApproxDouble::defEps = 1e-9;

typedef ApproxDouble StartTime;

class LineDuration: public ApproxDouble
{
public:
	explicit LineDuration(const ApproxDouble &ad):ApproxDouble(ad) {}
	LineDuration(double ast = 0.0):ApproxDouble(ast) {}
	LineDuration(double ast, double aeps):ApproxDouble(ast, aeps) {}
	LineDuration operator* (unsigned k) const {return LineDuration(st * k, eps);}
	LineDuration operator+ (const ApproxDouble &st2) const {return LineDuration((ApproxDouble)(*this) + st2);}
};

typedef unsigned int Processor;

/*
StartTimes
|	first: StartTime
|	second: State
|	|	first: LineDuration
|	|	second: set<Processor>
|	|	|	Processor
*/

typedef map<LineDuration, set<Processor> > State;
typedef map<StartTime, State> StartTimes;

static unsigned getModifiedLineHeight(const set<Processor> &procs)
{
	unsigned res = 0;
	unsigned tRes = 0;
	Processor prev;
	for (set<Processor>::const_iterator it = procs.begin(); it != procs.end(); it++) {
		if (!tRes || prev + 1 == (*it)) {
			tRes++;
		} else {
			if (tRes > res)
				res = tRes;
			tRes = 1;
		}
		prev = (*it);
	}
	if (tRes > res)
		res = tRes;
	return res;
}

static set<Processor> getProcInterval(const set<Processor> &procs, unsigned count, unsigned numProc) {
	map<unsigned, Processor> equals;
	map<unsigned, Processor> biggers;
	unsigned tRes = 0;
	Processor prev;
	for (set<Processor>::const_iterator it = procs.begin(); it != procs.end(); it++) {
		if (!tRes || prev + 1 == (*it)) {
			tRes++;
		} else {
			if (tRes == count)
				equals.insert(make_pair(min(numProc - prev - 1, prev + 1 - tRes), prev + 1 - tRes));
			else if (tRes > count)
				biggers.insert(make_pair(min(numProc - prev - 1, prev + 1 - tRes), prev + 1 - tRes));
			tRes = 1;
		}
		prev = (*it);
	}
	if (tRes == count)
		equals.insert(make_pair(min(numProc - prev - 1, prev + 1 - tRes), prev + 1 - tRes));
	else if (tRes > count)
		biggers.insert(make_pair(min(numProc - prev - 1, prev + 1 - tRes), prev + 1 - tRes));
	Processor startProc;
	if (!equals.empty())
		startProc = equals.begin()->second;
	else
		startProc = biggers.begin()->second;
	set<Processor> res;
	for (unsigned i = 0; i < count; i++)
		res.insert(startProc + i);
	return res;
}

static set<Processor> reservePlace(StartTimes &startTimes, StartTimes::iterator &minst, State::iterator &minstateIt, unsigned mink, LineDuration curTime, unsigned numProc)
{
	set<Processor> res;
	StartTime startTime(minst->first);
	State::iterator stateIt = minstateIt;//minst->second.lower_bound(curTime);
	//while (stateIt->second.size()<mink) stateIt++;
	for (set<Processor>::iterator procIt=stateIt->second.begin();res.size()<mink;procIt++)
		res.insert(*procIt);

	// XXX Remove after DVM RunTime upgrade
	res = getProcInterval(stateIt->second, mink, numProc);

	/*while (stateIt!=minst->second.end()&&stateIt->second.len>=curTime) {minstateIt=stateIt; stateIt++;}
	stateIt=minstateIt;
	while (stateIt!=minst->second.begin()&&res.size()<mink)
	{
		for (set<Processor>::iterator procIt=stateIt->second.procs.begin();procIt!=stateIt->second.procs.end()&&res.size()<mink;procIt++)
			res.insert(*procIt);
		stateIt--;
	}
	for (set<Processor>::iterator procIt=minst->second.begin()->second.procs.begin();procIt!=minst->second.begin()->second.procs.end()&&res.size()<mink;procIt++)
		res.insert(*procIt);*/
	StartTimes::iterator st = minst;
	set<Processor> remainder = res;
	//st++;
	while (st!=startTimes.begin()&&remainder.size()>0)
	{
		st--;
		set<Processor> nRem;
		LineDuration nlen = startTime-st->first;
		State::iterator nlenIt;
		nlenIt=st->second.find(nlen);
		if (nlenIt==st->second.end())
		{
			set<Processor> tmpProcs; 
			nlenIt=st->second.insert(make_pair(nlen, tmpProcs)).first;
		}
		for (State::reverse_iterator stateIt=st->second.rbegin();stateIt!=st->second.rend()&&stateIt->first>nlen;stateIt++)
		{
//			double len=stateIt->first;
			for (set<Processor>::iterator procIt = remainder.begin();procIt != remainder.end(); procIt++)
			{
				set<Processor>::iterator procIt2 = stateIt->second.find(*procIt);
				if (procIt2!=stateIt->second.end())
				{
					nRem.insert(*procIt);
					nlenIt->second.insert(*procIt);
					stateIt->second.erase(procIt2);
				}
			}
		}
		if (nlenIt->second.size()==0) st->second.erase(nlenIt);
		remainder.swap(nRem);
	}
	StartTimes::iterator right=startTimes.find(startTime+curTime);
	if (right==startTimes.end())
	{
		State tmpState;
		right=startTimes.insert(make_pair(startTime+curTime, tmpState)).first;
		StartTimes::iterator prevSt = right;
		prevSt--;
		for (State::reverse_iterator stateIt=prevSt->second.rbegin();stateIt!=prevSt->second.rend()&&stateIt->first > ApproxDouble(right->first-prevSt->first);stateIt++)
		{
			right->second.insert(make_pair(stateIt->first-(right->first-prevSt->first), stateIt->second));
		}
	}
	right++;
	for (st=minst;st!=right;)
	{
		bool toErase = true;
		for (stateIt=st->second.begin();stateIt!=st->second.end();)
		{
			for (set<Processor>::iterator procIt=stateIt->second.begin();procIt!=stateIt->second.end();)
			{
				if (res.find(*procIt)!=res.end()&&(st->first < ApproxDouble(startTime+curTime)))
					stateIt->second.erase(procIt++);
				else
				{
					if (toErase)
					{
						if (st==startTimes.begin()) toErase = false;
						else
						{
							StartTimes::iterator tmpIt = st;
							tmpIt--;
							State::iterator tmpStateIt = tmpIt->second.find(stateIt->first+st->first);
							if (tmpStateIt == tmpIt->second.end()) toErase = false;
							else
								if (tmpStateIt->second.find(*procIt)==tmpStateIt->second.end()) toErase = false;
						}
					}
					procIt++;
				}
			}
			if (stateIt->second.size()==0)
				st->second.erase(stateIt++);
			else
				stateIt++;
		}
		if (toErase)
			startTimes.erase(st++);
		else
			st++;
	}
	return res;
}

Distribution getDistributionTransposed(const vector<TaskData> &initData, unsigned numProc, ostream *logger)
{
	Distribution res(numProc, true);
	if (initData.size() < 1) return res;
	if (numProc < 1) throw MyException("Number of processors must be positive");
	list<TaskData> notDistributed(initData.begin(), initData.end());
	StartTimes startTimes;
	set<Processor> universum;
	for (Processor i = 0; i < numProc; i++)
		universum.insert(i);
	StartTime tMax=0;
	LineDuration tOccupied=0;
	LineDuration tRestMin=0;
	LineDuration tSumTimes=0;
//	double tMinTotalTime=0;
	for (unsigned i = 0; i < initData.size(); i++)
	{
		LineDuration curMinTotalTime = initData[i].getMinTotalTime();
		tRestMin += curMinTotalTime;
		tSumTimes += initData[i].getMaxTime();
	}
	ApproxDouble::setDefEps(1e-9 * tRestMin / initData.size() / numProc);
	State tmpState;
	tmpState.insert(make_pair(tSumTimes, universum));
	startTimes.insert(make_pair(0, tmpState));
	notDistributed.sort(TDReverseLess);
	clock_t cycleStart = clock();
	clock_t resTime = 0;
	//QueryPerformanceFrequency((LARGE_INTEGER*)&timeFreq);
	int vitkov = 0;
	for (list<TaskData>::iterator it = notDistributed.begin(); it != notDistributed.end(); it++)
	{
		LineDuration curMinTotalTime = it->getMinTotalTime();
		LineDuration curMinTime = it->getMinTime();
		int id = it->getId();
		unsigned kMin = it->getKmin();
		unsigned kMax = it->getKmax();
		if (kMin<=0 || kMax<kMin) throw MyException("SHIT! kMin<=0 || kMax<kMin is TRUE!");
		tRestMin -= curMinTotalTime;
		bool first = true;
		LineDuration mint;
		unsigned mink = numProc + 1;
		StartTimes::iterator minst;
		State::iterator minstateIt;
		set<unsigned> notExamined;
		vector<LineDuration> taskTimes(numProc + 1);
		for (unsigned aK = kMin; aK <= kMax && aK <= numProc; aK++) {
			vitkov++;
			notExamined.insert(aK);
			taskTimes[aK] = it->getTime(aK);
		}
		for (StartTimes::iterator st = startTimes.begin(); st != startTimes.end() && notExamined.size() > 0; st++) {
			StartTime curStartTime = st->first;
			set<unsigned>::iterator kIt = notExamined.begin();
			bool tMaxReached = false;
			State::iterator finishState = st->second.lower_bound(curMinTime);
			for (State::iterator stateIt = st->second.end(); !tMaxReached && kIt != notExamined.end() && stateIt != finishState; ) {
				stateIt--;
				unsigned curLineHeight = (unsigned)stateIt->second.size();

				// XXX Remove after DVM RunTime upgrade
				curLineHeight = getModifiedLineHeight(stateIt->second);

				LineDuration curLineDuration = stateIt->first;
				while (kIt != notExamined.end() && taskTimes[*kIt] > curLineDuration) kIt++;
				set<unsigned>::iterator firstKIt = kIt;
				for (; !tMaxReached && kIt != notExamined.end() && (*kIt) <= curLineHeight; kIt++) {
					unsigned k = *kIt;
					LineDuration curTime = taskTimes[k];
					LineDuration tSuggested = max((double)tMax, (double)(curStartTime + curTime));
					tMaxReached = tSuggested == tMax;
					tSuggested = max(tSuggested * numProc, tOccupied + curTime * k + tRestMin) / numProc;
					//TODO: increase in some way tSuggested
					if (first || tSuggested < mint) {
						first = false;
						mint = tSuggested;
						mink = k;
						minst = st;
						minstateIt = stateIt;
					}
				}
				notExamined.erase(firstKIt, kIt);
			}
			if (tMaxReached) notExamined.erase(kIt, notExamined.end());
		}
		if (first) throw MyException("Not enough processors to execute task");
		if (mink <= 0) throw MyException("mink<=0");
		StartTime tmpStartTime = minst->first;
		LineDuration tmpCurTime = taskTimes[mink];
		tMax = max(tMax, tmpStartTime + tmpCurTime);
		tOccupied += mink * tmpCurTime;
		clock_t resStart = clock();
		set<Processor> dist = reservePlace(startTimes, minst, minstateIt, mink, tmpCurTime, numProc);
		clock_t resEnd = clock();
		resTime += resEnd - resStart;
		if (logger) {
			(*logger) << "Id=" << id << "\tComplexity=" << it->getMinTotalTime() << "\tProcessors=" << mink << " StartTime=" << tmpStartTime << "\tDuration=" << tmpCurTime << endl;
			(*logger) << "Result goal=" << mint << endl;
		}
		Task *nt = new Task(*it, mink);
		for (set<Processor>::iterator distIt = dist.begin(); distIt != dist.end(); distIt++)
			res.add(nt, tmpStartTime, tmpStartTime+tmpCurTime, *distIt);
		//bool consistent = res.isConsistent();
	}
	if (logger)
		(*logger) << "TimeToCycle=" << (double)(clock() - cycleStart) / CLOCKS_PER_SEC << "sec. TimeToReserve=" << (double)resTime / CLOCKS_PER_SEC << "sec" << " vitkov=" << vitkov << endl;
	return res;
}
