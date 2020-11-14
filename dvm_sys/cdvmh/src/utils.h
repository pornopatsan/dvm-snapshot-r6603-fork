#pragma once

#include <string>
#include <set>
#include <sstream>
#include <cstdlib>

namespace cdvmh {

std::string deleteTrailingComma(const std::string &str);
std::string genIndent(int depth, bool useTabs = false);
std::string subtractIndent(std::string indent, int depth = 1);
std::string extractIndent(const char *s);
void removeFirstIndent(std::string &s);
void removeLastSemicolon(std::string &s);
void trimList(std::string &s);
std::string toLower(std::string a);
std::string toUpper(std::string a);
std::string toCIdent(std::string a, bool allowFirstDigit = false);
std::string escapeStr(std::string a);
template<typename T>
inline std::string toStr(T v) {
    std::stringstream ss;
    ss << v;
    return ss.str();
}
long long toNumber(const std::string &s, bool *pSuccess = 0);
inline bool isNumber(std::string a) { return toStr(toNumber(a)) == a; }
std::string mySubstr(const std::string &s, std::pair<int, int> range);
char *myStrDup(const char *orig, int n = -1);
std::string readFile(const std::string &fn);
std::string getBaseName(std::string fileName);
std::string getExtension(std::string fileName);
bool isRelative(std::string fileName, std::string *fixedFN = 0);
std::string getDirectory(std::string fileName);
std::string getAbsoluteFileName(std::string fileName, std::string forcedWorkingDir = std::string());
std::string simplifyFileName(std::string fileName, bool keepLinks);
std::string getCanonicalFileName(std::string fileName, std::string forcedWorkingDir = std::string());
std::string getShortestPath(std::string fromDir, std::string toFile, char useSeparator);
std::string getUniqueName(std::string desired, const std::set<std::string> *prohibited1, const std::set<std::string> *prohibited2 = 0);
bool fileExists(std::string fileName);
std::string flagsToStr(unsigned long long flags, const char *strs[], int strCount);
bool isRedType(std::string scalarType);
std::string toRedType(std::string scalarType);

}
