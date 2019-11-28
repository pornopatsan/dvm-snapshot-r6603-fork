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
    this->mode = mode;
    this->nextfile = 0;

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

    this->name = this->buildName(name);
    this->directory = DIRNAME + "/" + this->name;
}

ControlPoint::ControlPoint(ControlPoint::String name, ControlPoint::VectorDesc varlist) {
    this->varDescList = varlist;
    this->nextfile = 0;

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

    this->name = this->buildName(name);
    this->directory = DIRNAME + "/" + this->name;
}

}; // namespace libdvmh

using namespace libdvmh;

// Creates DIRNAME
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

void saveControlPointHeader(const ControlPoint *cp)
{
    FILE *header = dvmh_fopen(cp->getHeaderFilename().c_str(), cp->getOpenMode("w").c_str());
    // TODO: Pack data to write only once
    // TODO: Save many other params
    dvmh_void_fwrite(&cp->mode, sizeof(DvmhCpMode), 1, header);
    dvmh_void_fwrite(&cp->nextfile, sizeof(int), 1, header);
    dvmh_void_fwrite(&cp->nfiles, sizeof(int), 1, header);

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
    FILE *header = dvmh_fopen(cp->getHeaderFilename().c_str(), cp->getOpenMode("w").c_str());
    // TODO: Pack data to read only once
    dvmh_void_fread(&cp->mode, sizeof(DvmhCpMode), 1, header);
    dvmh_void_fread(&cp->nextfile, sizeof(int), 1, header);
    dvmh_void_fread(&cp->nfiles, sizeof(int), 1, header);

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

    //  TODO: delete this, as it actually doesnt matter with new naming
    int axNum = 0;
    dvmh_void_fread(&axNum, sizeof(axNum), 1, header);
    if (axNum != dvmh_get_num_proc_axes()) {
        return false;
    }
    size_t axSizeList[axNum];
    dvmh_void_fread(&axSizeList, sizeof(axSizeList), 1, header);
    for (int i = 0; i < axNum; ++i) {
        if (axSizeList[i] != dvmh_get_num_procs(i + 1)) {
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

extern "C" void dvmh_create_control_point(const char *cpName, DvmType *dvmDesc[], const size_t var_size, const size_t nfiles, const int mode) {
    printf("Creating CP\n");
    checkOrCreateCpDirectory();

    std::vector<DvmType *> varlist = buildVarlist(dvmDesc, var_size);
    ControlPoint *cp = new ControlPoint(cpName, varlist, nfiles, static_cast<DvmhCpMode>(mode));

    // TODO: what if cp already exists? Maybe just delete it, cause someone manually called create cp
    checkOrCreateCpDirectory(cp->directory);
    saveControlPointHeader(cp);
    activateControlPoint(cp);

    printf("%s\n", cp->getHeaderFilename().c_str());
    FILE *hdr = dvmh_fopen(cp->getHeaderFilename().c_str(), cp->getOpenMode("r").c_str());
    printf("%s\n", checkVarlistFitsHeader(cp->varDescList, hdr) ? "True" : "False");
    dvmh_fclose(hdr);
}

extern "C" void dvmh_bind_control_point(const char *cpName, DvmType *dvmDesc[], const size_t var_size) {
    // TODO: make this the only load function
    printf("Binding Control Point\n");
    std::vector<DvmType *> varlist = buildVarlist(dvmDesc, var_size);
    ControlPoint *cp = new ControlPoint(cpName, varlist);

    FILE *header = dvmh_fopen(cp->getHeaderFilename().c_str(), cp->getOpenMode("r").c_str());
    if (checkVarlistFitsHeader(varlist, header)) {
        printf("Variables do fit!\n");
        activateControlPoint(cp);
    } else {
        printf("Variables don't fit this Control Point\n");
    }
    dvmh_fclose(header);
}

extern "C" void dvmh_create_or_bind_control_point(const char *cpName, DvmType *dvmDesc[], const size_t var_size, const size_t nfiles, const int mode) {
    printf("Creating or Binding Control Point\n");
    checkOrCreateCpDirectory();

    std::vector<DvmType *> varlist = buildVarlist(dvmDesc, var_size);
    ControlPoint *cp = new ControlPoint(cpName, varlist, nfiles, static_cast<DvmhCpMode>(mode));
    if (checkOrCreateCpDirectory(cp->directory)) {
        saveControlPointHeader(cp);
    }
    activateControlPoint(cp);
}

extern "C" void dvmh_save_control_point(const char *cpName) {
    printf("Saving Control Point\n");
    std::map<std::string, ControlPoint *, std::less<std::string> >::iterator iter = ActiveControlPoints.find(std::string(cpName));
    if (iter != ActiveControlPoints.end()) {
        ControlPoint *cp = iter->second;
        FILE *astream = dvmh_fopen((cp->getNextFilename()).c_str(), cp->getOpenMode("w", false).c_str());
        cp->incFileQueue();
        for (size_t i = 0; i < cp->varDescList.size(); ++i) {
            dvmh_smart_void_write(cp->varDescList[i], astream);
        }
        saveControlPointHeader(cp);
        dvmh_fclose(astream);
    } else {
        printf("%s\n", "ControlPoint not found. Try creating it first");
    }
}

extern "C" void dvmh_load_control_point(const char *cp_name) {
    printf("Loading Control Point\n");
    std::map<std::string, ControlPoint *, std::less<std::string> >::iterator iter = ActiveControlPoints.find(std::string(cp_name));
    if (iter != ActiveControlPoints.end()) {
        std::pair<std::string, ControlPoint *> it = *iter;
        FILE *astream = dvmh_fopen((it.second->getLastFilename()).c_str(), it.second->getOpenMode("r", false).c_str());
        for (size_t i = 0; i < it.second->varDescList.size(); ++i) {
            dvmh_smart_void_read(it.second->varDescList[i], astream);
        }
        dvmh_fclose(astream);
    } else {
        printf("%s\n", "ControlPoint is not Active, use <dvmh_bind_control_point() first>");
    }
}

extern "C" void dvmh_smart_void_write(DvmType dvmDesc[], FILE *astream) {
    DvmhFile *stream = (DvmhFile *)astream;
    DvmhObject *obj = passOrGetOrCreateDvmh(dvmDesc[0], true);
    DvmhData *data = obj->as<DvmhData>();

    size_t size = data->getTypeSize();
    const Interval *spc = data->getSpace();
    size_t nmemb = 1;
    for (size_t i = 0; i < data->getRank(); ++i) {
        nmemb *= spc[i].size();
    }
    dvmh_void_fwrite_distrib(dvmDesc, size, nmemb, astream);
}

extern "C" void dvmh_smart_void_read(DvmType dvmDesc[], FILE *astream) {
    DvmhFile *stream = (DvmhFile *)astream;
    DvmhObject *obj = passOrGetOrCreateDvmh(dvmDesc[0], true);
    DvmhData *data = obj->as<DvmhData>();

    size_t size = data->getTypeSize();
    const Interval *spc = data->getSpace();
    size_t nmemb = 1;
    for (size_t i = 0; i < data->getRank(); ++i) {
        nmemb *= spc[i].size();
    }
    dvmh_void_fread_distrib(dvmDesc, size, nmemb, astream);
}
