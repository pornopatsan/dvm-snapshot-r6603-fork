#pragma once

#include <cstdio>
#include <string>

#include "dvmh_async.h"

// #include "dvmh_stdio.cpp" // TODO: do not include this
namespace libdvmh {
enum RWKind { rwkRead, rwkWrite }; // TODO: get original from cdmh_stdio.cpp
enum DvmhCpMode {LOCAL, PARALLEL};

class ControlPoint {

  public:
    //    typedef std::vector Vector;
    typedef std::string String;
    typedef std::vector<size_t> VectorSize;
    typedef std::vector<String> VectorStr;
    typedef std::vector<DvmType *> VectorDesc;

  public:
    static const std::string DIRNAME;
    String directory;
    String name;

  public:
    VectorDesc varDescList;
    VectorSize varSizeList;
    VectorSize varNmembList;

    int axesNum;
    VectorSize axesSizeList;


    String fname;
    DvmhCpMode mode;

  public:
    int nfiles;
    int nextfile;

  public:
    ControlPoint() {}

    // ControlPoint(String name, VectorDesc varlist, String filename, int nfiles);
    ControlPoint(String name, VectorDesc varlist, String filename, int nfiles, DvmhCpMode mode=LOCAL);
    ControlPoint(String name, VectorDesc varlist);

  public:
    void save(const DvmType dvmDesc[] = NULL, FILE *astream = NULL);

  private:
    template <typename T>
    string NumberToString ( T Number )
    {
        std::ostringstream ss;
        ss << Number;
        return ss.str();
    }

    template <typename T>
    T StringToNumber ( const string &Text )
    {
        istringstream ss(Text);
        T result;
        return ss >> result ? result : 0;
    }

  public:
    int getNfiles() const { return nfiles; }
    int getNextfile() const { return nextfile; }
    void incFileQueue() { nextfile = (nextfile + 1) % nfiles; }
    void decFileQueue() { nextfile = (nextfile - 1) % nfiles; }

    String getNextFilename() const {
        if (this->mode == LOCAL) {
            return directory + "/" + fname + "_" + NumberToString(this->getNextfile()) + "_" + "%d" + ".txt";
        } else if (this->mode == PARALLEL){
            return directory + "/" + fname + "_" + NumberToString(this->getNextfile()) + ".txt";
        } else {
            // TODO: error
            return "";
        }
    }

    String getLastFilename() const {
        if (this->mode == LOCAL) {
            return directory + "/" + fname + "_" + NumberToString((this->getNextfile() - 1) % nfiles) + "_" + "%d" + ".txt";
        } else if (this->mode == PARALLEL){
            return directory + "/" + fname + "_" + NumberToString((this->getNextfile() - 1) % nfiles) + ".txt";
        } else {
            // TODO: error
            return "";
        }
    }
    String getHeaderFilename() const {
        return this->directory + "/Header" + ".txt";
    }
    String getOpenMode(const String &rw, bool async=false, bool binary=true) {
        String res = rw;
        if (binary) { res += "b"; }
        if (async) { res += "s"; }
        if (this->mode == LOCAL) {
            res += "l";
        } else if (this->mode == PARALLEL){
            res += "p";
        } else {
            // error
        }
        return res;
    }

}; // class ControlPoint

const std::string ControlPoint::DIRNAME = "./control_points";

}; // namespace libdvmh
