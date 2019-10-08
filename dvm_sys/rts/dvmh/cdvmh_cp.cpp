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

ControlPoint::ControlPoint(ControlPoint::String name, ControlPoint::VectorDesc varlist,
                           ControlPoint::String filename, int nfiles, ControlPoint::DvmhCpMode mode) {
    this->name = name;
    this->varDescList = varlist;
    this->fname = filename;
    this->nfiles = nfiles;
    this->mode = mode;

    this->directory = DIRNAME + "/" + name;
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
}

ControlPoint::ControlPoint(ControlPoint::String name, ControlPoint::VectorDesc varlist) {
    this->name = name;
    this->varDescList = varlist;

    this->directory = DIRNAME + "/" + name;
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
}

}; // namespace libdvmh

using namespace libdvmh;

void checkOrCreateCpDirectory(const std::string &DIRNAME=ControlPoint::DIRNAME)
{
    // TODO: what about windows
    struct stat sb;
    if (!(stat(DIRNAME.c_str(), &sb) == 0 && S_ISDIR(sb.st_mode))) {
        mkdir(DIRNAME.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
    }
}

void saveControlPointHeader(const ControlPoint *cp)
{
    FILE *header = dvmh_fopen((cp->directory + "/Header" + ".txt").c_str(), "wbp");
    // TODO: Pack data to write only once
    // TODO: Save many other params
    // TODO: (?) MainHeader
    dvmh_void_fwrite(&cp->mode, sizeof(ControlPoint::DvmhCpMode), 1, header);
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

    size_t fnameSize = cp->fname.size() + 1;
    dvmh_void_fwrite(&fnameSize, sizeof(fnameSize), 1, header);
    dvmh_void_fwrite(cp->fname.data(), sizeof(char), fnameSize, header);

    dvmh_fclose(header);
}

void loadControlPointFromHeader(ControlPoint *cp)
{
    FILE *header = dvmh_fopen((cp->directory + "/Header" + ".txt").c_str(), "rbp");
    // TODO: Pack data to write only once
    dvmh_void_fread(&cp->mode, sizeof(ControlPoint::DvmhCpMode), 1, header);
    dvmh_void_fread(&cp->nextfile, sizeof(int), 1, header);
    dvmh_void_fread(&cp->nfiles, sizeof(int), 1, header);
    // cp->decFileQueue();

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

    size_t fnameSize = cp->fname.size();
    dvmh_void_fread(&fnameSize, sizeof(fnameSize), 1, header);
    char fname[fnameSize];
    dvmh_void_fread(fname, sizeof(char), fnameSize, header);
    cp->fname = fname;

    dvmh_fclose(header);
}

bool checkVarlistFitsHeader(ControlPoint::VectorDesc vlist, FILE *header)
{
    int buf;
    dvmh_void_fread(&buf, sizeof(ControlPoint::DvmhCpMode), 1, header);
    dvmh_void_fread(&buf, sizeof(buf), 1, header);
    dvmh_void_fread(&buf, sizeof(buf), 1, header);

    printf("TRUE1!\n");

    size_t nvars;
    dvmh_void_fread(&nvars, sizeof(nvars), 1, header);
    printf("%ld, %ld\n", nvars, vlist.size());
    if (nvars != vlist.size()) {
        return false;
    }

    printf("TRUE2!\n");

    size_t sizeList[nvars], nmembList[nvars];
    dvmh_void_fread(&sizeList, sizeof(sizeList), 1, header);
    dvmh_void_fread(&nmembList, sizeof(nmembList), 1, header);
    for (int i = 0; i < nvars; ++i) {
        std::pair<size_t, size_t> target = getSizeAndNmemb(vlist[i]);
        if (sizeList[i] != target.first || nmembList[i] != target.second) {
            return false;
        }
    }

    printf("TRUE3!\n");

    int axNum = 0;
    dvmh_void_fread(&axNum, sizeof(axNum), 1, header);
    printf("%08x\n", axNum);
    if (axNum != dvmh_get_num_proc_axes()) {
        return false;
    }

    printf("TRUE4!\n");

    size_t axSizeList[axNum];
    dvmh_void_fread(&axSizeList, sizeof(axSizeList), 1, header);
    for (int i = 0; i < axNum; ++i) {
        if (axSizeList[i] != dvmh_get_num_procs(i + 1)) {
            return false;
        }
    }

    printf("TRUE5!\n");

    size_t fnameSize = 0;
    dvmh_void_fread(&fnameSize, sizeof(fnameSize), 1, header);
    char fname[fnameSize];
    dvmh_void_fread(&fname, sizeof(fname), 1, header);

    return true;
}

extern "C" void dvmh_cp_save(const char *cp_name) {
    // TODO: rewrite this with dvmh_save_conrol_point()
    printf("Saving CP\n");
    std::pair<std::string, ControlPoint *> it = *ActiveControlPoints.find(std::string(cp_name));
    FILE *astream = dvmh_fopen((it.second->getNextFilename()).c_str(), "wb");
    it.second->incFileQueue();
    for (int i = 0; i < it.second->varDescList.size(); ++i) {
        dvmh_smart_void_write(it.second->varDescList[i], astream);
    }
    saveControlPointHeader(it.second);
    dvmh_fclose(astream);
}

// TODO: this works with active cp's only
extern "C" void dvmh_cp_load(const char *cp_name) {
    // TODO: rewrite this with dvmh_load_conrol_point()
    printf("Loading CP\n");
    std::map<std::string, ControlPoint *, std::less<std::string> >::iterator iter = ActiveControlPoints.find(std::string(cp_name));
    if (iter != ActiveControlPoints.end()) {
        std::pair<std::string, ControlPoint *> it = *iter;
        FILE *astream = dvmh_fopen((it.second->getLastFilename()).c_str(), it.second->getOpenMode("r", false).c_str());
        printf("%s\n", it.second->getLastFilename().c_str());
        for (int i = 0; i < it.second->varDescList.size(); ++i) {
            dvmh_smart_void_read(it.second->varDescList[i], astream);
        }
        dvmh_fclose(astream);
    } else {
        printf("%s\n", "ControlPoint is not Active, use <dvmh_load_conrol_point()>");
    }
}

extern "C" void dvmh_create_control_point(const char *cpName, DvmType *dvmDesc[], const size_t var_size,
                                          const char *filename, const size_t nfiles, const int mode) {
    printf("Creating CP\n");
    checkOrCreateCpDirectory();

    std::vector<DvmType *> varlist;
    for (int i = 0; i < var_size; ++i) {
        varlist.push_back(dvmDesc[i]);
    }

    ControlPoint *cp = new ControlPoint(cpName, varlist, filename, nfiles, static_cast<ControlPoint::DvmhCpMode>(mode));
    ActiveControlPoints.insert(std::make_pair(cpName, cp));
    printf("%d\n", cp->axesNum);

    // // TODO: what if cp already exists?
    checkOrCreateCpDirectory(cp->directory);
    saveControlPointHeader(cp);

    // printf("%s\n", (cp->directory + "/Header_" + "%d" + ".txt").c_str());
    FILE *hdr = dvmh_fopen(cp->getHeaderFilename().c_str(), "rbp");
    printf("%s\n", checkVarlistFitsHeader(cp->varDescList, hdr) ? "True" : "False");
    dvmh_fclose(hdr);

    // for (int i = 0; i < 64; ++i) {
    //     if (i % 8 == 0) { printf("\n"); }
    //     printf("%0.16lx  ", dvmDesc[0][i]);
    // }
    // printf("\n");
}

extern "C" void dvmh_save_conrol_point(const char *cpName) {
    printf("Saving Control Point\n");
    std::map<std::string, ControlPoint *, std::less<std::string> >::iterator iter = ActiveControlPoints.find(std::string(cpName));
    if (iter != ActiveControlPoints.end()) {
        ControlPoint *cp = iter->second;
        // 'wbl', 'wbp', 'wb', 'wbs*'
        FILE *astream = dvmh_fopen((cp->getNextFilename()).c_str(), cp->getOpenMode("w", false).c_str());
        cp->incFileQueue();
        for (int i = 0; i < cp->varDescList.size(); ++i) {
            dvmh_smart_void_write(cp->varDescList[i], astream);
        }
        saveControlPointHeader(cp);
        dvmh_fclose(astream);
    } else {
        printf("%s\n", "ControlPoint not found. create or load it first");
    }
}

extern "C" void dvmh_load_conrol_point(const char *cpName, DvmType *dvmDesc[], const size_t var_size) {
    // TODO: make this the only load function
    printf("Loading Control Point\n");

    std::vector<DvmType *> varlist;
    for (int i = 0; i < var_size; ++i) {
        varlist.push_back(dvmDesc[i]);
    }

    FILE *header = dvmh_fopen((ControlPoint::DIRNAME + "/" + cpName + "/Header" + ".txt").c_str(), "wbp");//cp->getOpenMode("w", false).c_str());
    if (!checkVarlistFitsHeader(varlist, header)) {
        printf("Variables don't fit this Control Point\n");
    } else {
        printf("Variables do fit!\nLOADING\n");
        ControlPoint *cp = new ControlPoint(cpName, varlist);
        loadControlPointFromHeader(cp);
        ActiveControlPoints.erase(cpName);
        ActiveControlPoints.insert(std::make_pair(cpName, cp));
        dvmh_cp_load(cpName);
    }
    dvmh_fclose(header);
}

extern "C" void dvmh_smart_void_write(DvmType dvmDesc[], FILE *astream) {
    DvmhFile *stream = (DvmhFile *)astream;
    DvmhObject *obj = passOrGetOrCreateDvmh(dvmDesc[0], true);
    DvmhData *data = obj->as<DvmhData>();

    size_t size = data->getTypeSize();
    const Interval *spc = data->getSpace();
    size_t nmemb = 1;
    for (int i = 0; i < data->getRank(); ++i) {
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
    for (int i = 0; i < data->getRank(); ++i) {
        nmemb *= spc[i].size();
    }
    dvmh_void_fread_distrib(dvmDesc, size, nmemb, astream);
}
