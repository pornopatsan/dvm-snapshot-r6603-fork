#include "utils.h"

#include <cassert>
#include <algorithm>
#include <cstring>
#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <vector>

#ifndef WIN32
#include <unistd.h>
#else
#include <windows.h>
#include <tchar.h>
#endif

#include "settings.h"
#include "cdvmh_log.h"
#include "messages.h"

namespace cdvmh {

std::string deleteTrailingComma(const std::string &str) {
    int commapos = 0;
    for ( int i = str.size() - 1; i > -1; --i ) {
        if ( str[ i ] == ',' ) {
            commapos = i;
            break;
	}
    }
    if ( commapos > 0 ) {
        return str.substr( 0, commapos );
    } else {
        return str;
    }
}
    
static std::string genIndentExact(int len, bool useTabs = false) {
    return (useTabs ? std::string(len / tabWidth, '\t') + std::string(len % tabWidth, ' ') : std::string(len, ' '));
}

std::string genIndent(int depth, bool useTabs) {
    return genIndentExact(depth * tabWidth, useTabs);
}

static int getIndentLen(const char *s) {
    int res = 0;
    for (; *s == ' ' || *s == '\t'; s++)
        if (*s == ' ')
            res++;
        else if (*s == '\t')
            res = (res / tabWidth + 1) * tabWidth;
    return res;
}

static int getIndentLen(const std::string &indent) {
    return getIndentLen(indent.c_str());
}

std::string subtractIndent(std::string indent, int depth) {
    int prevLen = getIndentLen(indent);
    assert(prevLen >= depth * tabWidth && "Wrong call subtractIndent");
    bool useTabs = !indent.empty() && indent[0] == '\t';
    return genIndentExact(prevLen - depth * tabWidth, useTabs);
}

std::string extractIndent(const char *s) {
    bool useTabs = s[0] == '\t';
    return genIndentExact(getIndentLen(s), useTabs);
}

void removeFirstIndent(std::string &s) {
    int count = 0;
    for (int i = 0; i < (int)s.size(); i++)
        if (s[i] == ' ' || s[i] == '\t')
            count++;
        else
            break;
    if (count > 0) {
        for (int i = 0; i < (int)s.size() - count; i++)
            s[i] = s[i + count];
        s.resize(s.size() - count);
    }
}

void removeLastSemicolon(std::string &s) {
    int count = 0;
    for (int i = (int)s.size() - 1; i >= 0; i--) {
        if (s[i] == ' ' || s[i] == '\t' || s[i] == '\n')
            count++;
        else if (s[i] == ';') {
            count++;
            break;
        } else
            break;
    }
    if (count > 0)
        s.resize(s.size() - count);
}

void trimList(std::string &s) {
    if (s.size() >= 2 && s[s.size() - 2] == ',' && s[s.size() - 1] == ' ')
        s.resize(s.size() - 2);
    if (s.size() >= 2 && s[0] == ',' && s[1] == ' ')
        s.erase(0, 2);
}

std::string toLower(std::string a) {
    std::string res;
    res.resize(a.size());
    std::transform(a.begin(), a.end(), res.begin(), tolower);
    return res;
}

std::string toUpper(std::string a) {
    std::string res;
    res.resize(a.size());
    std::transform(a.begin(), a.end(), res.begin(), toupper);
    return res;
}

std::string toCIdent(std::string a, bool allowFirstDigit) {
    for (int i = 0; i < (int)a.size(); i++) {
        char c = a[i];
        if (!((c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z') || c == '_' || ((i > 0 || allowFirstDigit) && c >= '0' && c <= '9')))
            a[i] = '_';
    }
    return a;
}

std::string escapeStr(std::string a) {
    std::string res;
    for (int i = 0; i < (int)a.size(); i++) {
        if (a[i] == '"' || a[i] == '\\') {
            res += '\\';
            res += a[i];
        } else if (a[i] == '\b') {
            res += "\\b";
        } else if (a[i] == '\f') {
            res += "\\f";
        } else if (a[i] == '\r') {
            res += "\\r";
        } else if (a[i] == '\n') {
            res += "\\n";
        } else if (a[i] == '\t') {
            res += "\\t";
        } else if (a[i] < 32) {
            res += "\\x";
            res += a[i] / 16 + '0';
            if (a[i] % 16 < 10)
                res += a[i] % 16 + '0';
            else
                res += a[i] % 16 - 10 + 'A';
        } else
            res += a[i];
    }
    return res;
}

long long toNumber(const std::string &s, bool *pSuccess) {
    long long res = 0;
    int scanned = sscanf(s.c_str(), "%lld", &res);
    if (pSuccess)
        *pSuccess = (scanned == 1);
    return res;
}

std::string mySubstr(const std::string &s, std::pair<int, int> range) {
    return s.substr(range.first, range.second - range.first + 1);
}

char *myStrDup(const char *orig, int n) {
    if (n < 0)
        n = strlen(orig);
    char *res = new char[n + 1];
    strncpy(res, orig, n);
    res[n] = 0;
    return res;
}

std::string readFile(const std::string &fn) {
    std::ifstream f(fn.c_str());
    std::ostringstream out;
    out << f.rdbuf();
    return out.str();
}

std::string getBaseName(std::string fileName) {
    size_t last_sep1 = fileName.rfind(PATH_SEP);
    size_t last_sep2 = fileName.rfind('/');
    size_t last_sep = last_sep1;
    if (last_sep == std::string::npos || (last_sep2 != std::string::npos && last_sep2 > last_sep))
        last_sep = last_sep2;
    if (last_sep != std::string::npos)
        fileName = fileName.substr(last_sep + 1);
    return fileName;
}

std::string getExtension(std::string fileName) {
    std::string baseName = getBaseName(fileName);
    size_t last_sep = baseName.rfind('.');
    if (last_sep != std::string::npos)
        return baseName.substr(last_sep + 1);
    else
        return "";
}

bool isRelative(std::string fileName, std::string *fixedFN) {
    bool res = true;
    if (!isWin) {
        if (!fileName.empty() && fileName[0] == '/')
            res = false;
    } else {
        if (fileName.length() >= 2 && (toupper(fileName[0]) >= 'A' && toupper(fileName[0]) <= 'Z') && fileName[1] == ':') {
            // Drive is specified
            if (fileName.length() == 2)
                res = false;
            else if (fileName[2] == '\\' || fileName[2] == '/')
                res = false;
            // For example, C:tmp.txt is relative to the current directory on drive C
        } else if (fileName.length() >= 4 && fileName[0] == '\\' && fileName[1] == '\\' && (fileName[2] == '?' || fileName[2] == '.') && fileName[3] == '\\') {
            // UNC path
            res = false;
            fileName = fileName.substr(4);
            // From now on, not an UNC path
        }
        // We consider paths starting with a slash relative (in contrast with the MSDN's glossary)
    }
    if (fixedFN)
        *fixedFN = fileName;
    return res;
}

std::string getDirectory(std::string fileName) {
    //cdvmh_log(TRACE, "getDirectory's input fn is '%s'", fileName.c_str());
    bool isRel = isRelative(fileName, &fileName);
    assert(!isRel);
    size_t last_sep1 = fileName.rfind(PATH_SEP);
    size_t last_sep2 = fileName.rfind('/');
    size_t last_sep = last_sep1;
    if (last_sep == std::string::npos || (last_sep2 != std::string::npos && last_sep2 > last_sep))
        last_sep = last_sep2;
    assert(last_sep != std::string::npos);
    fileName = fileName.substr(0, last_sep);
    if (fileName.empty())
        fileName = "/";
    //cdvmh_log(TRACE, "Directory is '%s'", fileName.c_str());
    return fileName;
}

std::string getAbsoluteFileName(std::string fileName, std::string forcedWorkingDir) {
    //cdvmh_log(TRACE, "getAbsoluteFileName's input fn is '%s' wd '%s'", fileName.c_str(), forcedWorkingDir.c_str());
    if (isRelative(fileName, &fileName)) {
        std::string wd;
        if (!forcedWorkingDir.empty()) {
            wd = forcedWorkingDir;
        } else {
#ifndef WIN32
            char cwd[FILENAME_MAX + 1] = {0};
            if (getcwd(cwd, FILENAME_MAX))
                wd = cwd;
#else
            TCHAR cwd[FILENAME_MAX + 1];
            GetCurrentDirectory(FILENAME_MAX, cwd);
            if (sizeof(TCHAR) == sizeof(char)) {
                wd = (char *)cwd;
            } else {
                char mbs[FILENAME_MAX + 1];
                wcstombs(mbs, (wchar_t *)cwd, sizeof(mbs));
                wd = mbs;
            }
            if (fileName.length() >= 2 && fileName[1] == ':') {
                assert(toupper(wd[0]) == toupper(fileName[0]));
                fileName.erase(0, 2);
            }
            if (!fileName.empty() && fileName[0] == '\\')
                fileName.erase(0, 1);
#endif
        }
        assert(!wd.empty());
        if (wd[wd.length() - 1] == '/' || wd[wd.length() - 1] == PATH_SEP)
            fileName = wd + fileName;
        else
            fileName = wd + PATH_SEP + fileName;
    }
    //cdvmh_log(TRACE, "Absolute fn is '%s'", fileName.c_str());
    return fileName;
}

std::string simplifyFileName(std::string fileName, bool keepLinks) {
    bool isRel = isRelative(fileName, &fileName);
    std::string drivePrefix;
    if (isWin) {
        if (fileName.length() >= 2 && (toupper(fileName[0]) >= 'A' && toupper(fileName[0]) <= 'Z') && fileName[1] == ':') {
            drivePrefix = fileName.substr(0, 2);
            fileName = fileName.substr(2);
        }
        if (fileName.length() >= 1 && (fileName[0] == '\\' || fileName[0] == '/'))
            isRel = false;
    }
    // Change all separators to slashes for convenience
    if (PATH_SEP != '/') {
        for (int i = 0; i < (int)fileName.length(); i++) {
            if (fileName[i] == PATH_SEP)
                fileName[i] = '/';
        }
    }
    std::vector<std::string> path;
    std::string pathStr;
    int afterPrevSep = 0;
    int minPathLen = 0;
    while (afterPrevSep < (int)fileName.length()) {
        std::string::size_type nextSepB = fileName.find('/', afterPrevSep);
        if (nextSepB == std::string::npos)
            nextSepB = fileName.length();
        int nextSep = nextSepB;
        if (nextSep > afterPrevSep) {
            path.push_back(fileName.substr(afterPrevSep, nextSep - afterPrevSep));
            pathStr += "/" + path.back();
            if (path.back() == "..") {
                if (!isRel && path.size() == 1) {
                    // Parent of root is root
                    path.pop_back();
                    pathStr.resize(pathStr.rfind('/'));
                } else if ((int)path.size() >= minPathLen + 2) {
                    // Go to parent
                    path.pop_back();
                    path.pop_back();
                    pathStr.resize(pathStr.rfind('/'));
                    pathStr.resize(pathStr.rfind('/'));
                } else {
                    // Can not go to parent, keep it as ".."
                    minPathLen = path.size();
                }
            } else if (path.back() == ".") {
                path.pop_back();
                pathStr.resize(pathStr.rfind('/'));
            } else {
#ifndef WIN32
                char realDirectory[FILENAME_MAX + 1] = {0};
                if (readlink(pathStr.c_str() + (isRel ? 1 : 0), realDirectory, FILENAME_MAX) > 0) {
                    if (!keepLinks) {
                        if (isRelative(realDirectory)) {
                            fileName = getDirectory(pathStr) + "/" + realDirectory + fileName.substr(nextSep);
                        } else {
                            fileName = realDirectory + fileName.substr(nextSep);
                            isRel = false;
                        }
                        path.clear();
                        pathStr.clear();
                        nextSep = -1;
                        minPathLen = 0;
                    } else {
                        minPathLen = path.size();
                    }
                }
#endif
            }
        }
        afterPrevSep = nextSep + 1;
    }
    fileName = (isRel ? pathStr.substr(1) : pathStr);
    if (fileName.empty())
        fileName = (isRel ? "." : "/");
    fileName = drivePrefix + fileName;
    // Change separators to the native ones
    if (PATH_SEP != '/') {
        for (int i = 0; i < (int)fileName.length(); i++) {
            if (fileName[i] == '/')
                fileName[i] = PATH_SEP;
        }
    }
    return fileName;
}

std::string getCanonicalFileName(std::string fileName, std::string forcedWorkingDir) {
    //cdvmh_log(TRACE, "getCanonicalFileName's input fn is '%s' wd '%s'", fileName.c_str(), forcedWorkingDir.c_str());
    fileName = getAbsoluteFileName(fileName, forcedWorkingDir);
    // Now it is absolute, simplify it + follow links
    fileName = simplifyFileName(fileName, false);
    // Windows is case-insensitive, convert to lower case
    if (isWin)
        fileName = toLower(fileName);
    //cdvmh_log(TRACE, "Canonical fn is '%s'", fileName.c_str());
    return fileName;
}

std::string getShortestPath(std::string fromDir, std::string toFile, char useSeparator) {
    fromDir = getCanonicalFileName(fromDir);
    toFile = getCanonicalFileName(toFile);
    if (PATH_SEP != '/') {
        for (int i = 0; i < (int)fromDir.length(); i++) {
            if (fromDir[i] == PATH_SEP)
                fromDir[i] = '/';
        }
        for (int i = 0; i < (int)toFile.length(); i++) {
            if (toFile[i] == PATH_SEP)
                toFile[i] = '/';
        }
    }
    if (fromDir.empty() || fromDir[fromDir.length() - 1] != '/')
        fromDir += '/';
    int commonParts = 0;
    int lastCommonSep = -1;
    for (int i = 0; i < (int)fromDir.length() && i < (int)toFile.length(); i++) {
        if (fromDir[i] != toFile[i])
            break;
        if (fromDir[i] == '/') {
            lastCommonSep = i;
            commonParts++;
        }
    }
    std::string res;
    if (lastCommonSep == (int)fromDir.length() - 1) {
        res = toFile.substr(lastCommonSep + 1);
    } else if (commonParts >= 2) {
        // We can make it relative and won't reach the root
        int fromDirParts = 0;
        for (int i = 0; i < (int)fromDir.length(); i++)
            fromDirParts += (fromDir[i] == '/');
        for (int i = 0; i < fromDirParts - commonParts; i++)
            res += "../";
        res += toFile.substr(lastCommonSep + 1);
    } else {
        // Just leave it absolute
        res = toFile;
    }
    if (useSeparator != '/') {
        for (int i = 0; i < (int)res.length(); i++) {
            if (res[i] == '/')
                res[i] = useSeparator;
        }
    }
    return res;
}

std::string getUniqueName(std::string desired, const std::set<std::string> *prohibited1, const std::set<std::string> *prohibited2) {
    static int curSuffix = 0;
    std::string res = desired;
    for (;;) {
        bool okFlag = true;
        if (prohibited1->find(res) != prohibited1->end())
            okFlag = false;
        if (okFlag && prohibited2 && (prohibited2->find(res) != prohibited2->end()))
            okFlag = false;
        if (okFlag)
            break;
        res = desired + "_" + toStr(curSuffix);
        curSuffix++;
    }
    return res;
}

bool fileExists(std::string fileName) {
    FILE *f = fopen(fileName.c_str(), "rb");
    if (f) {
        fclose(f);
        return true;
    } else {
        return false;
    }
}

std::string flagsToStr(unsigned long long flags, const char *strs[], int strCount) {
    std::string res = "0";
    for (int i = 0; i < strCount; i++)
        if ((flags >> i) & 1) {
            if (res == "0")
                res = strs[i];
            else
                res += std::string(" | ") + strs[i];
        }
    return res;
}

bool isRedType(std::string scalarType) {
    const char redTypes[] = "char  int  long  float  double  float _Complex  double _Complex";
    return strstr(redTypes, scalarType.c_str()) != 0;
}

std::string toRedType(std::string scalarType) {
    if (scalarType == "char")
        return "rt_CHAR";
    else if (scalarType == "int")
        return "rt_INT";
    else if (scalarType == "long")
        return "rt_LONG";
    else if (scalarType == "float")
        return "rt_FLOAT";
    else if (scalarType == "double")
        return "rt_DOUBLE";
    else if (scalarType == "float _Complex")
        return "rt_FLOAT_COMPLEX";
    else if (scalarType == "double _Complex")
        return "rt_DOUBLE_COMPLEX";
    intErrN(917, scalarType.c_str());
}

}
