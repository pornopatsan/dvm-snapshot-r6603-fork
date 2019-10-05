#ifndef _TYPES_H_
#define _TYPES_H_

#pragma warning(disable:4786)
#include <cctype>
#include <vector>
#include <string>
using namespace std;

enum ELevelTypes
{
    TASK_REGION,
    LOOP,
    ITERATION
};

struct CLevel
{
    ELevelTypes m_eType;
    vector<long> m_vIndexes;

    CLevel(ELevelTypes eType, const vector<long>& vIndexes) :
        m_eType(eType),
        m_vIndexes(vIndexes)
    {}

    CLevel(const CLevel& sOther)
    {
        *this = sOther;
    }

    const CLevel& operator =(const CLevel& sOther)
    {
        m_eType = sOther.m_eType;
        //m_vIndexes.assign(sOther.m_vIndexes.begin(), sOther.m_vIndexes.end());
    	m_vIndexes.erase(m_vIndexes.begin(), m_vIndexes.end());
	m_vIndexes.insert(m_vIndexes.begin(), sOther.m_vIndexes.begin(), sOther.m_vIndexes.end());
	return *this;
    }
};

inline bool operator ==(const CLevel& x, const CLevel& y)
{
    return (x.m_eType == y.m_eType) && (x.m_vIndexes == y.m_vIndexes);
}

inline bool operator !=(const CLevel& x, const CLevel& y)
{
    return !(operator ==(x, y));
}

typedef vector<CLevel> CLevelList;

// Trim spaces
inline string trim(const string& str)
{
    return str.substr(str.find_first_not_of(" \t\n"),
        str.find_last_not_of(" \t\n"));
}

CLevelList parse(const string& strContext) throw (string&);
vector<long> parseString(const string& strPattern, string& strContext) throw(string&);
string removeSpaces(const string& str);
void throwBadFormat() throw(string&);
int compareString(const string& str, const char * templ);
string removeError(const string& str);
string TrcErr(const string& str);
string FindErr(const string& str);
string GenerateCom(const string& str);

#endif // _TYPES_H_