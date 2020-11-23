#pragma once

#include <cstdio>
#include <string>
#include <sstream>

#include "dvmh_async.h"

namespace libdvmh {
    enum RWKind { rwkRead, rwkWrite }; // TODO: get original from cdmh_stdio.cpp
    enum DvmhCpMode {LOCAL, PARALLEL, LOCAL_ASYNC, PARALLEL_ASYNC};

    struct ControlPointHeader {
        static const size_t MaxVars = 64;
        static const size_t MaxProcAxes = 64;

        DvmhCpMode mode;
        size_t nfiles;
        size_t nextfile;
        bool isSaved;

        size_t nDescVars;
        int descVarSizeList[ControlPointHeader::MaxVars];
        int descVarNmembList[ControlPointHeader::MaxVars];
        size_t nScalVars;
        int scalarVarSizeList[ControlPointHeader::MaxVars];

    }; // class ControlPointHeader

    class ControlPoint {

    public:
        typedef std::vector<DvmType *> VectorDesc;
        typedef std::vector<std::pair<void *, size_t> > VectorScal;
        typedef std::pair<VectorDesc, VectorScal> cpDataPair;

    public:
        static const std::string DIRNAME;
        static std::string NumberToString(int number) {
            std::ostringstream stream;
            stream << number;
            return stream.str();
        }

    public:
        std::string directory;
        std::string name;
        cpDataPair dataPair;

        bool isLoaded;
        bool saveLock;
        DvmhFile *saveStreamFile;

        ControlPointHeader header;

    public:
        void initControlPoint(std::string name, cpDataPair dataPair);
        ControlPoint() {}
        ControlPoint(std::string name, const size_t nfiles, const DvmhCpMode mode, cpDataPair dataPair);
        ControlPoint(std::string name, cpDataPair dataPair);

    public:
        int getFilesNum() const { return header.nfiles; }
        int getNextFile() const { return header.nextfile; }
        int getLastFile() const { return (this->getNextFile() - 1) % this->getFilesNum(); }
        void incFileQueue() { header.nextfile = (this->getNextFile() + 1) % this->getFilesNum(); }
        void decFileQueue() { header.nextfile = (this->getNextFile() - 1) % this->getFilesNum(); }

        bool isCpLocal() const { return ((this->header.mode == LOCAL) || (this->header.mode == LOCAL_ASYNC)); }
        bool isCpParallel() const { return ((this->header.mode == PARALLEL) || (this->header.mode == PARALLEL_ASYNC)); }
        bool isCpAsync() const { return ((this->header.mode == LOCAL_ASYNC) || (this->header.mode == PARALLEL_ASYNC)); }

        bool isSaveLocked() { return this->saveLock; }
        void lockSave() { !this->isSaveLocked() ? ((void) (this->saveLock = true)) : exit(1); }
        void unlockSave() { this->isSaveLocked() ? ((void) (this->saveLock = false)) : exit(1); }

        std::string getNextFilename() const {
            if (this->isCpLocal()) {
                return directory + "/" + ControlPoint::NumberToString(this->getNextFile()) + "_" + "%d" + ".txt";
            } else if (this->isCpParallel()) {
                return directory + "/" + ControlPoint::NumberToString(this->getNextFile()) + ".txt";
            } else {
                exit(1);
            }
        }
        std::string getLastFilename() const {
            if (this->isCpLocal()) {
                return directory + "/" + ControlPoint::NumberToString(this->getLastFile()) + "_" + "%d" + ".txt";
            } else if (this->isCpParallel()) {
                return directory + "/" + ControlPoint::NumberToString(this->getLastFile()) + ".txt";
            } else {
                exit(1);
            }
        }
        std::string getHeaderFilename() const {
            if (this->isCpLocal()) {
                return this->directory + "/Header" + "_" + "%d" + ".txt";
            } else if (this->isCpParallel()) {
                return this->directory + "/Header" + ".txt";
            } else {
                exit(1);
            }
        }

        std::string getCpOpenMode(const std::string rw, bool local, bool async, bool binary) const {
            std::string res = rw;
            if (binary) { res += "b"; }
            if (async) { res += "s"; }
            res += (local ? "l" : "p");
            return res;
        }
        std::string getOpenMode(const std::string rw, bool binary=true) const {
            return this->getCpOpenMode(rw, this->isCpLocal(), this->isCpAsync(), binary);
        }
        std::string getSyncOpenMode(const std::string rw, bool binary=true) const {
            return this->getCpOpenMode(rw, this->isCpLocal(), false, binary);
        }

    }; // class ControlPoint

    const std::string ControlPoint::DIRNAME = "./control_points";

}; // namespace libdvmh
