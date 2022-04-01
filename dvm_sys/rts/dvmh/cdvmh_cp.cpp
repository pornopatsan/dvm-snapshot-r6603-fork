#include "cdvmh_cp.h"
#include "dvmh_stdio.h"
#include "dvmh2.h"
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

    std::string BuildName(const std::string name) {
        std::string suffix = "";
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

    void ControlPoint::initControlPoint(std::string name, ControlPoint::cpDataPair dataPair) {
        this->dataPair = dataPair;
        this->header.nextfile = 0;
        this->header.isSaved = false;
        this->isLoaded = false;
        this->saveLock = false;
        this->saveStreamFile = nullptr;
        this->name = BuildName(name);
        this->directory = DIRNAME + "/" + this->name;

        this->header.nDescVars = dataPair.first.size();
        this->header.nScalVars = dataPair.second.size();
        for (size_t i = 0; i < this->header.nDescVars; ++i) {
            std::pair<size_t, size_t> tmp = getSizeAndNmemb(dataPair.first[i]);
            this->header.descVarSizeList[i] = tmp.first;
            this->header.descVarNmembList[i] = tmp.second;
        }
        for (size_t i = 0; i < this->header.nScalVars; ++i) {
            this->header.scalarVarSizeList[i] = dataPair.second[i].second;
        }
    }

    ControlPoint::ControlPoint(std::string name, const size_t nfiles, DvmhCpMode mode, ControlPoint::cpDataPair dataPair) {
        initControlPoint(name, dataPair);
        this->header.nfiles = nfiles;
        this->header.mode = mode;
    }

    ControlPoint::ControlPoint(std::string name, ControlPoint::cpDataPair dataPair) {
        initControlPoint(name, dataPair);
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
    dvmh_void_fwrite(&cp->header, sizeof(cp->header), 1, header);
    dvmh_fclose(header);
}

bool checkControlPointFitsHeader(ControlPoint *cp, ControlPointHeader *header) {
    if (cp->header.nDescVars != header->nDescVars) { return false; }
    for (size_t i = 0; i < header->nDescVars; ++i) {
        if (cp->header.descVarSizeList[i] != header->descVarSizeList[i] || cp->header.descVarNmembList[i] != header->descVarNmembList[i]) {
            return false;
        }
    }
    if (cp->header.nScalVars != header->nScalVars) { return false; }
    for (size_t i = 0; i < header->nScalVars; ++i) {
        if (cp->header.scalarVarSizeList[i] != header->scalarVarSizeList[i]) {
            return false;
        }
    }
    return true;
}

ControlPoint::VectorDesc buildVarlist(DvmType *dvmDesc[], const size_t var_size) {
    ControlPoint::VectorDesc varlist;
    for (size_t i = 0; i < var_size; ++i) {
        varlist.push_back(dvmDesc[i]);
    }
    return varlist;
}

ControlPoint::VectorScal buildScalarVarlist(void **addresses, const size_t *sizes, const size_t var_size) {
    ControlPoint::VectorScal varlist;
    for (size_t i = 0; i < var_size; ++i) {
        varlist.push_back(std::make_pair(addresses[i], sizes[i]));
    }
    return varlist;
}

ControlPoint::cpDataPair buildDataPair(DvmType *dvmDesc[], const size_t n_distrib_vars,
                                       void **scalar_addresses, const size_t *scalar_sizes, const size_t n_scalar_vars) {
    ControlPoint::VectorDesc distrib_vars = buildVarlist(dvmDesc, n_distrib_vars);
    ControlPoint::VectorScal scalar_vars = buildScalarVarlist(scalar_addresses, scalar_sizes, n_scalar_vars);
    return std::make_pair(distrib_vars, scalar_vars);
}

void activateControlPoint(ControlPoint *cp) {
    ActiveControlPoints.erase(cp->name);
    ActiveControlPoints.insert(std::make_pair(cp->name, cp));
}

void bindControlPoint(ControlPoint *cp) {
    ControlPointHeader header;
    FILE *headerFile = dvmh_fopen(cp->getHeaderFilename().c_str(), cp->getOpenMode("r").c_str());
    dvmh_void_fread(&header, sizeof(header), 1, headerFile);
    dvmh_fclose(headerFile);
    if (checkControlPointFitsHeader(cp, &header)) {
        cp->header = header;
        cp->isLoaded = false;
    } else {
        printf("Variables don't fit this Control Point\n");
        exit(1);
    }
}

void finalizeControlPointSave(ControlPoint *cp) {
    dvmh_fclose((FILE *) cp->saveStreamFile);
    cp->saveStreamFile = nullptr;
    cp->header.isSaved = true;
    saveControlPointHeader(cp);
    dvmh_barrier();
    cp->unlockSave();
}

void waitControlPoint(ControlPoint *cp) {
    if (cp->isCpAsync()) {
        if (cp->isSaveLocked()) {
            cp->saveStreamFile->syncOperations();
            finalizeControlPointSave(cp);
        } else {
            printf("%s\n", "Cannot wait Control Point, no operations are ongoing. Aborting");
            exit(1);
        }
    } else {
        printf("%s\n", "Cannot wait sync Control Point. Aborting");
        exit(1);
    }
}

void saveControlPoint(ControlPoint *cp) {
    if (cp->isLoaded) {
        if (cp->isSaveLocked()) { waitControlPoint(cp); }
        cp->lockSave();
        cp->incFileQueue();
        cp->saveStreamFile = (DvmhFile *) dvmh_fopen((cp->getLastFilename()).c_str(), cp->getOpenMode("w").c_str());
        for (size_t i = 0; i < cp->dataPair.first.size(); ++i) {
            dvmh_smart_void_write(cp->dataPair.first[i], (FILE *) cp->saveStreamFile);
        }
        for (size_t i = 0; i < cp->dataPair.second.size(); ++i) {
            dvmh_void_fwrite(cp->dataPair.second[i].first, 1, cp->dataPair.second[i].second, (FILE *) cp->saveStreamFile);
        }
        if (!cp->isCpAsync()) {
            finalizeControlPointSave(cp);
        }
    } else {
        printf("%s\n", "Load Control Point first, else some data may be lost");
        exit(1);
    }
}

void loadControlPoint(ControlPoint *cp) {
    if (cp->isSaveLocked()) { waitControlPoint(cp); }
    if (cp->header.isSaved) {
        FILE *astream = dvmh_fopen((cp->getLastFilename()).c_str(), cp->getOpenMode("r").c_str());
        for (size_t i = 0; i < cp->dataPair.first.size(); ++i) {
            dvmh_smart_void_read(cp->dataPair.first[i], astream);
            dvmh_shadow_renew_C(cp->dataPair.first[i], 0, 0);
        }
        for (size_t i = 0; i < cp->dataPair.second.size(); ++i) {
            dvmh_void_fread(cp->dataPair.second[i].first, 1, cp->dataPair.second[i].second, astream);
        }
        dvmh_fclose(astream);
    }
    cp->isLoaded = true;
    dvmh_barrier();
}

// Deprecated
extern "C" void dvmh_create_control_point(const char *cpName, const size_t nfiles, const int mode,
                                          DvmType *dvmDesc[], const size_t n_distrib_vars,
                                          void **scalar_addresses, size_t *scalar_sizes, const size_t n_scalar_vars) {
    checkOrCreateCpDirectory();
    ControlPoint::cpDataPair dataPair = buildDataPair(dvmDesc, n_distrib_vars, scalar_addresses, scalar_sizes, n_scalar_vars);
    ControlPoint *cp = new ControlPoint(cpName, nfiles, static_cast<DvmhCpMode>(mode), dataPair);
    cp->isLoaded = true; // Just created CP does not need to be loaded firsts
    // TODO: what if cp already exists? Maybe just delete it, cause someone manually called create cp
    checkOrCreateCpDirectory(cp->directory);
    saveControlPointHeader(cp);
    activateControlPoint(cp);
}

// Deprecated
extern "C" void dvmh_bind_control_point(const char *cpName, DvmType *dvmDesc[], const size_t n_distrib_vars,
                                        void **scalar_addresses, const size_t *scalar_sizes, const size_t n_scalar_vars) {
    ControlPoint::cpDataPair dataPair = buildDataPair(dvmDesc, n_distrib_vars, scalar_addresses, scalar_sizes, n_scalar_vars);
    ControlPoint *cp = new ControlPoint(cpName, dataPair);
    if (!checkFileExists(cp->directory) || !checkFileExists(cp->getHeaderFilename().c_str())) {
        printf("Control Point is corrupted or not found. Aborting\n");
        exit(1);
    }
    bindControlPoint(cp);
    activateControlPoint(cp);
}

extern "C" void dvmh_create_or_bind_control_point(const char *cpName, const size_t nfiles, const int mode,
                                                  DvmType *dvmDesc[], const size_t n_distrib_vars,
                                                  void **scalar_addresses, const size_t *scalar_sizes, const size_t n_scalar_vars) {
    checkOrCreateCpDirectory();
    ControlPoint::cpDataPair dataPair = buildDataPair(dvmDesc, n_distrib_vars, scalar_addresses, scalar_sizes, n_scalar_vars);
    ControlPoint *cp = new ControlPoint(cpName, nfiles, static_cast<DvmhCpMode>(mode), dataPair);
    // We need to be sure that if any process created directory, all processes should have `true`
    bool &&isCreated = checkOrCreateCpDirectory(cp->directory);
    dvmh_barrier();
    currentMPS->allreduce(isCreated, rf_MAX);
    if (isCreated) {
        cp->isLoaded = true; // Just created CP does not need to be loaded first
        saveControlPointHeader(cp);
    } else {
        bindControlPoint(cp);
    }
    activateControlPoint(cp);
}

extern "C" void dvmh_save_control_point(const char *cpName) {
    std::map<std::string, ControlPoint *, std::less<std::string> >::iterator iter = ActiveControlPoints.find(BuildName(cpName));
    if (iter != ActiveControlPoints.end()) {
        saveControlPoint(iter->second);
    } else {
        printf("%s\n", "ControlPoint not found. Try creating it first");
        exit(1);
    }
}

extern "C" void dvmh_load_control_point(const char *cpName) {
    std::map<std::string, ControlPoint *, std::less<std::string> >::iterator iter = ActiveControlPoints.find(BuildName(cpName));
    if (iter != ActiveControlPoints.end()) {
        loadControlPoint(iter->second);
    } else {
        printf("%s\n", "ControlPoint is not Active, use <dvmh_bind_control_point()> first");
        exit(1);
    }
}

extern "C" void dvmh_wait_control_point(const char *cpName) {
    std::map<std::string, ControlPoint *, std::less<std::string> >::iterator iter = ActiveControlPoints.find(BuildName(cpName));
    if (iter != ActiveControlPoints.end()) {
        waitControlPoint(iter->second);
    } else {
        printf("%s\n", "ControlPoint is not Active, use <dvmh_bind_control_point()> first");
        exit(1);
    }
}

extern "C" void dvmh_deactivate_control_point(const char *cpName) {
    std::map<std::string, ControlPoint *, std::less<std::string> >::iterator iter = ActiveControlPoints.find(BuildName(cpName));
    if (iter != ActiveControlPoints.end()) {
        if (iter->second->isCpAsync() && iter->second->isSaveLocked()) {
            waitControlPoint(iter->second);
        }
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
