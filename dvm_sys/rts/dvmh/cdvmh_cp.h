#pragma once

#include <cstdio>
#include <string>
#include <sstream>

#include "dvmh_async.h"

// #include "dvmh_stdio.cpp" // TODO: do not include this
namespace libdvmh {
enum RWKind { rwkRead, rwkWrite }; // TODO: get original from cdmh_stdio.cpp
enum DvmhCpMode {LOCAL, PARALLEL, LOCAL_ASYNC, PARALLEL_ASYNC};

class ControlPoint {

  public:
    //    typedef std::vector Vector;
    typedef std::string String;
    typedef std::vector<size_t> VectorSize;
    typedef std::vector<String> VectorStr;
    typedef std::vector<DvmType *> VectorDesc;

  public:
    static const std::string DIRNAME;
    static std::string NumberToString(int number) {
        std::ostringstream stream;
        stream << number;
        return stream.str();
    }

  public:
    String directory;
    String name;
    VectorDesc varDescList;
    VectorSize varSizeList;
    VectorSize varNmembList;

    int axesNum;
    VectorSize axesSizeList;

    DvmhCpMode mode;
    int nfiles;
    int nextfile;

    bool isLoaded;
    bool isSaved;
    bool saveLock;
    DvmhFile *fileSaveStream;

  public:
    ControlPoint() {}
    ControlPoint(String name, VectorDesc varlist, int nfiles, DvmhCpMode mode);
    ControlPoint(String name, VectorDesc varlist);

  public:
    int getFilesNum() const { return nfiles; }
    int getNextFile() const { return nextfile; }
    int getLastFile() const { return (this->getNextFile() - 1) % this->getFilesNum(); }
    void incFileQueue() { nextfile = (this->getNextFile() + 1) % this->getFilesNum(); }
    void decFileQueue() { nextfile = (this->getNextFile() - 1) % this->getFilesNum(); }

    bool isCpLocal() const { return ((this->mode == LOCAL) || (this->mode == LOCAL_ASYNC)); }
    bool isCpParallel() const { return ((this->mode == PARALLEL) || (this->mode == PARALLEL_ASYNC)); }
    bool isCpAsync() const { return ((this->mode == LOCAL_ASYNC) || (this->mode == PARALLEL_ASYNC)); }

    bool isSaveLocked() { return this->saveLock; }
    void lockSave() { !this->isSaveLocked() ? ((void) (this->saveLock = true)) : exit(1); }
    void unlockSave() { this->isSaveLocked() ? ((void) (this->saveLock = false)) : exit(1); }

    String getNextFilename() const {
        if (this->isCpLocal()) {
            return directory + "/" + ControlPoint::NumberToString(this->getNextFile()) + "_" + "%d" + ".txt";
        } else if (this->isCpParallel()) {
            return directory + "/" + ControlPoint::NumberToString(this->getNextFile()) + ".txt";
        } else {
            exit(1);
        }
    }
    String getLastFilename() const {
        if (this->isCpLocal()) {
            return directory + "/" + ControlPoint::NumberToString(this->getLastFile()) + "_" + "%d" + ".txt";
        } else if (this->isCpParallel()) {
            return directory + "/" + ControlPoint::NumberToString(this->getLastFile()) + ".txt";
        } else {
            exit(1);
        }
    }
    String getHeaderFilename() const {
        if (this->isCpLocal()) {
            return this->directory + "/Header" + "_" + "%d" + ".txt";
        } else if (this->isCpParallel()) {
            return this->directory + "/Header" + ".txt";
        } else {
            exit(1);
        }
    }

    String getCpOpenMode(const String rw, bool local, bool async, bool binary) const {
        String res = rw;
        if (binary) { res += "b"; }
        if (async) { res += "s"; }
        res += (local ? "l" : "p");
        return res;
    }

    String getOpenMode(const String rw, bool binary=true) const {
        return this->getCpOpenMode(rw, this->isCpLocal(), this->isCpAsync(), binary);
    }

    String getSyncOpenMode(const String rw, bool binary=true) const {
        return this->getCpOpenMode(rw, this->isCpLocal(), false, binary);
    }

}; // class ControlPoint

const std::string ControlPoint::DIRNAME = "./control_points";

}; // namespace libdvmh
