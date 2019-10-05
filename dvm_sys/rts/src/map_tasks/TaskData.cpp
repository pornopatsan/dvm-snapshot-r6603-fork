#include "TaskData.h"
#include <string>
#include <sstream>

using std::endl;

double TaskData::getTime (int k) const
{
	if (k<kMin) throw MyException("k is too small");
	if (k>kMax) throw MyException("k is too large");
	return tSeq+tTotalPar/k;
}

ostream& operator<< (ostream &out, const TaskData &td)
{
	out<<td.id<<" ";
	out<<td.tSeq<<" ";
	out<<td.tTotalPar<<" ";
	out<<td.kMin<<" ";
	out<<td.kMax<<" "<<endl;
	return out;
}

istream& operator>> (istream &in, TaskData &td)
{
	std::string s;
	in >> s;
	while (s == "#") {
		char buf[1000];
		in.getline(buf, 1000);
		in >> s;
	}
	std::stringstream ss(s);
	ss >> td.id;
	in >> td.tSeq;
	in >> td.tTotalPar;
	in >> td.kMin;
	in >> td.kMax;
	return in;
}
