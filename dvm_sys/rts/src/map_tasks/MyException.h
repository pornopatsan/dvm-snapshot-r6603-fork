#ifndef _MyException
#define _MyException

#include <string>

using std::string;

class MyException
{
protected:
	string text;
public:
	MyException(string txt):text(txt) {}
	string getMessage() {return text;}
};

#endif
