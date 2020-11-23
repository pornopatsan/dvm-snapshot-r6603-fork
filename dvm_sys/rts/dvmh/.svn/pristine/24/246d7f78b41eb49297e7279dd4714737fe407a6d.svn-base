#include "dvmh_types.h"

#include <algorithm>

#include "dvmh_async.h"
#include "util.h"

namespace libdvmh {

// Interval

Interval Interval::intersect(const Interval &other) const {
    return create(std::max(values[0], other.values[0]), std::min(values[1], other.values[1]));
}

Interval &Interval::intersectInplace(const Interval &other) {
    if (values[0] < other.values[0])
        values[0] = other.values[0];
    if (values[1] > other.values[1])
        values[1] = other.values[1];
    return *this;
}

Interval &Interval::encloseInplace(const Interval &other) {
    if (values[0] > other.values[0])
        values[0] = other.values[0];
    if (values[1] < other.values[1])
        values[1] = other.values[1];
    return *this;
}

UDvmType Interval::blockSize(int rank) const {
    UDvmType res = 1;
    for (int i = 0; i < rank; i++)
        res *= this[i].size();
    return res;
}

bool Interval::blockEmpty(int rank) const {
    bool res = false;
    for (int i = 0; i < rank; i++)
        res = res || this[i].empty();
    return res;
}

bool Interval::blockEquals(int rank, const Interval other[]) const {
    bool res = true;
    for (int i = 0; i < rank; i++)
        res = res && this[i] == other[i];
    return res;
}

bool Interval::blockContains(int rank, const DvmType indexes[]) const {
    bool res = true;
    for (int i = 0; i < rank; i++)
        res = res && this[i].contains(indexes[i]);
    return res;
}

bool Interval::blockContains(int rank, const Interval other[]) const {
    bool res = true;
    for (int i = 0; i < rank; i++)
        res = res && this[i].contains(other[i]);
    return res;
}

bool Interval::blockIntersect(int rank, const Interval other[], Interval res[]) const {
    bool notEmpty = true;
    for (int i = 0; i < rank; i++) {
        res[i] = this[i].intersect(other[i]);
        notEmpty = notEmpty && !res[i].empty();
    }
    return notEmpty;
}

bool Interval::blockIntersectInplace(int rank, const Interval other[]) {
    bool notEmpty = true;
    for (int i = 0; i < rank; i++) {
        this[i].intersectInplace(other[i]);
        notEmpty = notEmpty && !this[i].empty();
    }
    return notEmpty;
}

bool Interval::blockIntersects(int rank, const Interval other[]) const {
    bool notEmpty = true;
    for (int i = 0; i < rank; i++)
        notEmpty = notEmpty && this[i][0] <= other[i][1] && other[i][0] <= this[i][1];
    return notEmpty;
}

Interval *Interval::blockEncloseInplace(int rank, const Interval other[]) {
    for (int i = 0; i < rank; i++)
        this[i].encloseInplace(other[i]);
    return this;
}

// LoopBounds

LoopBounds LoopBounds::create(DvmType begin, DvmType end, DvmType step) {
    LoopBounds res;
    res.begin() = begin;
    res.end() = end;
    res.step() = step;
    return res;
}

LoopBounds LoopBounds::create(const Interval &interval, DvmType step) {
    return (step > 0 ? create(interval[0], interval[1], step) : create(interval[1], interval[0], step));
}

void LoopBounds::toBlock(int rank, Interval res[]) const {
    for (int i = 0; i < rank; i++)
        res[i] = this[i].toInterval();
}

UDvmType LoopBounds::iterCount() const {
    return (UDvmType)std::max((DvmType)0, divDownS(values[1] - values[0], values[2]) + 1);
}

UDvmType LoopBounds::iterCount(int rank) const {
    UDvmType res = 1;
    for (int i = 0; i < rank; i++)
        res *= this[i].iterCount();
    return res;
}

// DvmhObject

static const char etalonSignature[10] = {'D', 'V', 'M', 'H', 'O', 'B', 'J', 'E', 'C', 'T'};

DvmhObject::DvmhObject() {
    typedMemcpy(signature, etalonSignature, sizeof(etalonSignature));
    deleteHook = 0;
}

bool DvmhObject::checkSignature() const {
    for (int i = 0; i < (int)sizeof(etalonSignature); i++) {
        if (signature[i] != etalonSignature[i])
            return false;
    }
    return true;
}

void DvmhObject::addOnDeleteHook(Executable *task) {
    if (!deleteHook)
        deleteHook = new BulkTask;
    deleteHook->prependTask(task);
}

DvmhObject::~DvmhObject() {
    if (deleteHook) {
        deleteHook->execute(this);
        delete deleteHook;
    }
}

}
