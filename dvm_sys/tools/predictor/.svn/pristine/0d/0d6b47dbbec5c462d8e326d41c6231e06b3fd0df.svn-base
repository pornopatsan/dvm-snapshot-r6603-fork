#include <iostream>
using namespace std;
#include "types.h"
#include "trace.h"
const int BUFFER_SIZE = 1024;
void usage();

int main(int argc, char* argv[])
{
    if (argc < 3)
    {
        usage();
        return 1;
    }
	
    try
    {
    CLevelList vDefLevel;
    int it=0;
	string strCurrent;
    char szBuff[BUFFER_SIZE + 1];
    ifstream trcerr(argv[2]);
	if (trcerr.bad() || !trcerr.is_open())
        {
            cout << "Cannot open file with error \"" << argv[2] << "\".\n";
            return 1;
        }
    for (long lLine = 1; (lLine <2) &&!trcerr.eof(); ++lLine)
    {
        trcerr.getline(szBuff, BUFFER_SIZE);
        strCurrent = removeSpaces(szBuff);
        strCurrent=removeError(strCurrent);
		if (strCurrent.empty())
            continue;
        if (0 == strCurrent.find("TraceRecord"))
            strCurrent=TrcErr(strCurrent);
	strCurrent=GenerateCom(FindErr(strCurrent));
    }
        try
        {
            vDefLevel = parse(strCurrent);
        }
        catch (const string& err)
        {
            cout << err.data() << "\n";
            return 1;
        }

        ifstream in(argv[1]);
        if (in.bad() || !in.is_open())
        {
            cout << "Cannot open trace file \"" << argv[1] << "\".\n";
            return 1;
        }

        cout << "Lines mathed the context string \"" << strCurrent << "\":\n";
        findMatches(vDefLevel, in);
        cout << "Search finished.\n";
    }
    catch (...)
    {
        cout << "Unexpected error.\n";
        return 1;
    }

    return 0;
}

string TrcErr(const string& str)
{
    string strRes;
    int ok=1;
    for (string::const_iterator i = str.begin(); i != str.end(); ++i)
    {
	if (ok)
		{
		if (*i ==',') ok=0;
		continue;
		}
	else
		strRes.append(1, *i);
    }
    return strRes;
}

string FindErr(const string& str)
{
    string strRes;
    for (string::const_iterator i = str.begin(); i != str.end(); ++i)
    {
    if ('.' == *i )
	   break;
    strRes.append(1, *i);
    }
    return strRes;
}

string GenerateCom(const string& str)
{
    string strRes="";
    for (string::const_iterator i = str.begin(); i != str.end(); ++i)
    {
    if ('.' == *i )
	   break;
    strRes.append(1, *i);
    }
    return strRes;
}

string removeError(const string& str)
{
    string strRes;
    int ok=1;
	for (string::const_iterator i = str.begin(); i != str.end(); ++i)
    {
	if (ok)
		{
		if (*i ==':') ok=0;
		continue;
		}
	else
		strRes.append(1, *i);
	}
    return strRes;
}


void usage()
{
    cout << "Usage: trcutil.exe <trace file> <file with error>\n";
}
