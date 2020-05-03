#include "cdvmh_cp.h"
#include "dvmh_stdio.h"
#include "include/dvmh_runtime_api.h"
#include "include/dvmhlib_stdio.h"

#include <cassert>
#include <cctype>
#include <cerrno>
#include <cstdarg>
#ifdef HAVE_PTRDIFF_T
#include <cstddef>
#endif
#include <cstdio>
#include <cstdlib>
#include <cstring>
#ifdef HAVE_INTMAX_T
#include <stdint.h>
#endif

#include <algorithm>
#include <cstdio>
#include <iterator>
#include <map>
#include <string>
#include <vector>

#ifndef WIN32
#include <unistd.h>
#include <sys/stat.h>
#else
#pragma GCC visibility push(default)
#include <io.h>
#pragma GCC visibility pop
#endif

#include "include/dvmhlib_const.h"

#include "distrib.h"
#include "dvmh_buffer.h"
#include "dvmh_data.h"
#include "dvmh_log.h"
#include "dvmh_pieces.h"
#include "dvmh_rts.h"
#include "loop.h"
#include "mps.h"
#include "util.h"

#include "string.h"

namespace libdvmh {

std::map<std::string, ControlPoint *, std::less<std::string> > ActiveControlPoints;

ControlPoint::String BuildName(const ControlPoint::String name) {
    ControlPoint::String suffix = "";
    for(int i = 0; i < dvmh_get_num_proc_axes(); ++i) {
        suffix += "_" + ControlPoint::NumberToString(dvmh_get_num_procs(i + 1));
    }
    return name + suffix;
}

std::pair<size_t, size_t> getSizeAndNmemb(DvmType dvmDesc[]) {
    DvmhObject *obj = passOrGetOrCreateDvmh(dvmDesc[0], true);
    DvmhData *data = obj->as<DvmhData>();

    size_t size = data->getTypeSize();
    const Interval *spc = data->getSpace();
    size_t nmemb = 1;
    for (int i = 0; i < data->getRank(); ++i) {
        nmemb *= spc[i].size();
    }
    return std::make_pair(size, nmemb);
}

ControlPoint::ControlPoint(ControlPoint::String name, ControlPoint::VectorDesc varlist, int nfiles, DvmhCpMode mode) {
    this->varDescList = varlist;
    this->nfiles = nfiles;
    this->nextfile = 0;
    this->mode = mode;
    this->isSaved = false;
    this->isLoaded = false;
    this->saveLock = false;
    this->fileSaveStream = nullptr;

    this->varSizeList.clear();
    this->varNmembList.clear();
    for (int i = 0; i < varlist.size(); ++i) {
        std::pair<size_t, size_t> tmp = getSizeAndNmemb(varlist[i]);
        this->varSizeList.push_back(tmp.first);
        this->varNmembList.push_back(tmp.second);
    }

    this->axesNum = dvmh_get_num_proc_axes();
    this->axesSizeList.clear();
    for(int i = 0; i < axesNum; ++i) {
        this->axesSizeList.push_back(dvmh_get_num_procs(i + 1));
    }

    this->name = BuildName(name);
    this->directory = DIRNAME + "/" + this->name;
}

ControlPoint::ControlPoint(ControlPoint::String name, ControlPoint::VectorDesc varlist) {
    this->varDescList = varlist;
    this->nextfile = 0;
    this->isSaved = false;
    this->isLoaded = false;
    this->saveLock = false;
    this->fileSaveStream = nullptr;

    this->varSizeList.clear();
    this->varNmembList.clear();
    for (int i = 0; i < varlist.size(); ++i) {
        std::pair<size_t, size_t> tmp = getSizeAndNmemb(varlist[i]);
        this->varSizeList.push_back(tmp.first);
        this->varNmembList.push_back(tmp.second);
    }

    this->axesNum = dvmh_get_num_proc_axes();
    this->axesSizeList.clear();
    for(int i = 0; i < axesNum; ++i) {
        this->axesSizeList.push_back(dvmh_get_num_procs(i + 1));
    }

    this->name = BuildName(name);
    this->directory = DIRNAME + "/" + this->name;
}

}; // namespace libdvmh

using namespace libdvmh;

// Creates <DIRNAME> directory
// returns false if it already exists or true of is was just created
bool checkOrCreateCpDirectory(const std::string &DIRNAME=ControlPoint::DIRNAME)
{
    // TODO: what about windows
    struct stat sb;
    if (!(stat(DIRNAME.c_str(), &sb) == 0 && S_ISDIR(sb.st_mode))) {
        mkdir(DIRNAME.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
        return true;
    }
    return false;
}

bool checkFileExists(const std::string &filename)
{
    // TODO: what about windows
    struct stat sb;
    return (stat(filename.c_str(), &sb) == 0);
}

void saveControlPointHeader(const ControlPoint *cp)
{
    // TODO: decide either write header async or sync?
    FILE *header = dvmh_fopen(cp->getHeaderFilename().c_str(), cp->getSyncOpenMode("w").c_str());
    // TODO: Pack data to write only once
    // TODO: Save many other params
    dvmh_void_fwrite(&cp->mode, sizeof(DvmhCpMode), 1, header);
    dvmh_void_fwrite(&cp->nextfile, sizeof(int), 1, header);
    dvmh_void_fwrite(&cp->nfiles, sizeof(int), 1, header);
    dvmh_void_fwrite(&cp->isSaved, sizeof(bool), 1, header);
//    dvmh_void_fwrite(&cp->isLoaded, sizeof(bool), 1, header);

    size_t nVars = cp->varDescList.size();
    dvmh_void_fwrite(&nVars, sizeof(nVars), 1, header);
    for (size_t i = 0; i < cp->varSizeList.size(); ++i) {
        dvmh_void_fwrite(&cp->varSizeList[i], sizeof(size_t), 1, header);
    }
    for (size_t i = 0; i < cp->varNmembList.size(); ++i) {
        dvmh_void_fwrite(&cp->varNmembList[i], sizeof(size_t), 1, header);
    }

    dvmh_void_fwrite(&cp->axesNum, sizeof(cp->axesNum), 1, header);
    for (size_t i = 0; i < cp->axesSizeList.size(); ++i) {
        dvmh_void_fwrite(&cp->axesSizeList[i], sizeof(size_t), 1, header);
    }

    dvmh_fclose(header);
}

void loadControlPointFromHeader(ControlPoint *cp)
{
    FILE *header = dvmh_fopen(cp->getHeaderFilename().c_str(), cp->getOpenMode("r").c_str());
    // TODO: Pack data to read only once
    dvmh_void_fread(&cp->mode, sizeof(DvmhCpMode), 1, header);
    dvmh_void_fread(&cp->nextfile, sizeof(int), 1, header);
    dvmh_void_fread(&cp->nfiles, sizeof(int), 1, header);
    dvmh_void_fread(&cp->isSaved, sizeof(bool), 1, header);
//    dvmh_void_fread(&cp->isLoaded, sizeof(bool), 1, header);

    size_t nVars = cp->varDescList.size();
    dvmh_void_fread(&nVars, sizeof(nVars), 1, header);
    for (size_t i = 0; i < cp->varSizeList.size(); ++i) {
        dvmh_void_fread(&cp->varSizeList[i], sizeof(size_t), 1, header);
    }
    for (size_t i = 0; i < cp->varNmembList.size(); ++i) {
        dvmh_void_fread(&cp->varNmembList[i], sizeof(size_t), 1, header);
    }

    dvmh_void_fread(&cp->axesNum, sizeof(cp->axesNum), 1, header);
    for (size_t i = 0; i < cp->axesNum; ++i) {
        dvmh_void_fread(&cp->axesSizeList[i], sizeof(size_t), 1, header);
    }

    dvmh_fclose(header);
}

bool checkVarlistFitsHeader(ControlPoint::VectorDesc vlist, FILE *header)
{
    int buf;
    dvmh_void_fread(&buf, sizeof(DvmhCpMode), 1, header);
    dvmh_void_fread(&buf, sizeof(int), 1, header);
    dvmh_void_fread(&buf, sizeof(int), 1, header);

    size_t nvars;
    dvmh_void_fread(&nvars, sizeof(nvars), 1, header);
    if (nvars != vlist.size()) {
        return false;
    }

    size_t sizeList[nvars], nmembList[nvars];
    dvmh_void_fread(&sizeList, sizeof(sizeList), 1, header);
    dvmh_void_fread(&nmembList, sizeof(nmembList), 1, header);
    for (size_t i = 0; i < nvars; ++i) {
        std::pair<size_t, size_t> target = getSizeAndNmemb(vlist[i]);
        if (sizeList[i] != target.first || nmembList[i] != target.second) {
            return false;
        }
    }
    return true;
}

std::vector<DvmType *> buildVarlist(DvmType *dvmDesc[], const size_t var_size) {
    std::vector<DvmType *> varlist;
    for (size_t i = 0; i < var_size; ++i) {
        varlist.push_back(dvmDesc[i]);
    }
    return varlist;
}

void activateControlPoint(ControlPoint *cp) {
    ActiveControlPoints.erase(cp->name);
    ActiveControlPoints.insert(std::make_pair(cp->name, cp));
}

void waitControlPoint(ControlPoint *cp) {
    if (cp->isCpAsync()) {
        if (cp->isSaveLocked()) {
            cp->fileSaveStream->syncOperations();
            dvmh_fclose((FILE *) cp->fileSaveStream);
            cp->fileSaveStream = nullptr;
            cp->isSaved = true;
            saveControlPointHeader(cp);
            cp->unlockSave();
        } else {
            printf("%s\n", "Cannot wait Control Point, no operations are ongoing. Aborting");
            exit(1);
        }
    } else {
        printf("%s\n", "Cannot wait sync Control Point. Aborting");
        exit(1);
    }
}

// Deprecated
extern "C" void dvmh_create_control_point(const char *cpName, DvmType *dvmDesc[], const size_t var_size, const size_t nfiles, const int mode) {
    checkOrCreateCpDirectory();
    std::vector<DvmType *> varlist = buildVarlist(dvmDesc, var_size);
    ControlPoint *cp = new ControlPoint(cpName, varlist, nfiles, static_cast<DvmhCpMode>(mode));
    cp->isLoaded = true; // Just created CP does not need to be loaded first
    // TODO: what if cp already exists? Maybe just delete it, cause someone manually called create cp
    checkOrCreateCpDirectory(cp->directory);
    saveControlPointHeader(cp);
    activateControlPoint(cp);
}

// Deprecated
extern "C" void dvmh_bind_control_point(const char *cpName, DvmType *dvmDesc[], const size_t var_size) {
    std::vector<DvmType *> varlist = buildVarlist(dvmDesc, var_size);
    ControlPoint *cp = new ControlPoint(cpName, varlist);
    if (!checkFileExists(cp->directory) || !checkFileExists(cp->getHeaderFilename().c_str())) {
        printf("Control Point is corrupted or not found. Aborting\n");
        exit(1);
    }

    FILE *header = dvmh_fopen(cp->getHeaderFilename().c_str(), cp->getOpenMode("r").c_str());
    if (checkVarlistFitsHeader(varlist, header)) {
        activateControlPoint(cp);
        loadControlPointFromHeader(cp);
        cp->isLoaded = false;
    } else {
        printf("Variables don't fit this Control Point\n");
        exit(1);
    }
    dvmh_fclose(header);
}

extern "C" void dvmh_create_or_bind_control_point(const char *cpName, DvmType *dvmDesc[], const size_t var_size, const size_t nfiles, const int mode) {
    checkOrCreateCpDirectory();
    std::vector<DvmType *> varlist = buildVarlist(dvmDesc, var_size);
    ControlPoint *cp = new ControlPoint(cpName, varlist, nfiles, static_cast<DvmhCpMode>(mode));
    // We need to be sure that if any process created directory, all processes should have `true`
    bool &&isCreated = checkOrCreateCpDirectory(cp->directory);
    dvmh_barrier();
    currentMPS->allreduce(isCreated, rf_MAX);
    if (isCreated) {
        cp->isLoaded = true; // Just created CP does not need to be loaded first
        saveControlPointHeader(cp);
    } else {
        loadControlPointFromHeader(cp);
        cp->isLoaded = false;

    }
    activateControlPoint(cp);
}

extern "C" void dvmh_save_control_point(const char *cpName) {
    std::map<std::string, ControlPoint *, std::less<std::string> >::iterator iter = ActiveControlPoints.find(BuildName(cpName));
    if (iter != ActiveControlPoints.end()) {
        ControlPoint *cp = iter->second;
        if (cp->isLoaded) {
            if (cp->isSaveLocked()) { waitControlPoint(cp); }
            cp->lockSave();
            cp->incFileQueue();
            cp->fileSaveStream = (DvmhFile *) dvmh_fopen((cp->getLastFilename()).c_str(), cp->getOpenMode("w").c_str());
            for (size_t i = 0; i < cp->varDescList.size(); ++i) {
                dvmh_smart_void_write(cp->varDescList[i], (FILE *) cp->fileSaveStream);
            }
            if (!cp->isCpAsync()) {
                dvmh_fclose((FILE *) cp->fileSaveStream);
                cp->fileSaveStream = nullptr;
                cp->isSaved = true;
                saveControlPointHeader(cp);
                dvmh_barrier();
                cp->unlockSave();
            }
        } else {
            printf("%s\n", "Load Control Point first, else some data will be lost");
            exit(1);
        }
    } else {
        printf("%s\n", "ControlPoint not found. Try creating it first");
        exit(1);
    }
}

extern "C" void dvmh_load_control_point(const char *cpName) {
    std::map<std::string, ControlPoint *, std::less<std::string> >::iterator iter = ActiveControlPoints.find(BuildName(cpName));
    if (iter != ActiveControlPoints.end()) {
        ControlPoint *cp = iter->second;
        if (cp->isSaveLocked()) { waitControlPoint(cp); }
        if (cp->isSaved) {
            FILE *astream = dvmh_fopen((cp->getLastFilename()).c_str(), cp->getOpenMode("r").c_str());
            for (size_t i = 0; i < cp->varDescList.size(); ++i) {
                dvmh_smart_void_read(cp->varDescList[i], astream);
            }
            dvmh_fclose(astream);
        }
        cp->isLoaded = true;
        dvmh_barrier();
    } else {
        printf("%s\n", "ControlPoint is not Active, use <dvmh_bind_control_point()> first");
        exit(1);
    }
}

extern "C" void dvmh_wait_control_point(const char *cpName) {
    std::map<std::string, ControlPoint *, std::less<std::string> >::iterator iter = ActiveControlPoints.find(BuildName(cpName));
    if (iter != ActiveControlPoints.end()) {
        std::pair<std::string, ControlPoint *> it = *iter;
        waitControlPoint(it.second);
    } else {
        printf("%s\n", "ControlPoint is not Active, use <dvmh_bind_control_point()> first");
        exit(1);
    }
}

extern "C" void dvmh_deactivate_control_point(const char *cpName) {
    std::map<std::string, ControlPoint *, std::less<std::string> >::iterator iter = ActiveControlPoints.find(BuildName(cpName));
    if (iter != ActiveControlPoints.end()) {
        ActiveControlPoints.erase(iter);
    } else {
        printf("%s\n", "ControlPoint is not Active, use <dvmh_bind_control_point()> first");
        exit(1);
    }
}

extern "C" void dvmh_smart_void_write(DvmType dvmDesc[], FILE *stream) {
    const std::pair<size_t, size_t> sizeAndNmemb = getSizeAndNmemb(dvmDesc);
    const size_t size = sizeAndNmemb.first;
    const size_t nmemb = sizeAndNmemb.second;
    dvmh_void_fwrite_distrib(dvmDesc, size, nmemb, stream);
}

extern "C" void dvmh_smart_void_read(DvmType dvmDesc[], FILE *stream) {
    const std::pair<size_t, size_t> sizeAndNmemb = getSizeAndNmemb(dvmDesc);
    const size_t size = sizeAndNmemb.first;
    const size_t nmemb = sizeAndNmemb.second;
    dvmh_void_fread_distrib(dvmDesc, size, nmemb, stream);
}
