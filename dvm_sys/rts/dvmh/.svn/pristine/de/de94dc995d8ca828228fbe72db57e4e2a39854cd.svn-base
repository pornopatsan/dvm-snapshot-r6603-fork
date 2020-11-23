#pragma once

#include <cassert>
#include <cstring>
#include <vector>

#include "dvmh_types.h"
#include "dvmh_log.h"
#include "util.h"

namespace libdvmh {

#define PIECES_LEVEL DONT_LOG

// Constant to mark piece as 'absolute'. That means biggest possible order.
// Order 0 means no piece.
#define ABS_ORDER DVMTYPE_MAX

enum SubtractMethod {LEAVE_GREATER, FROM_ABS};
// 1. Subtract only from pieces with lesser or equal order.
// 2. Convert stated in second argument pieces from absolute state to specified order.
enum IntersectMethod {SET_MIN, SET_NOT_LESS};
// 1. Intersect all pieces. Order of the result will be minimum of orders of arguments.
// 2. Intersect only pieces with lesser or equal order. Order of the result will be order of second argument if it is not less than order of first.

class DvmhPiece {
public:
    HybridVector<Interval, 10> rect; // Array of Intervals (length=rank)
    DvmType order;
    int rank;
public:
    explicit DvmhPiece(int aRank = 0) {
        rank = aRank;
        rect.resize(rank, Interval::createEmpty());
        order = ABS_ORDER;
    }
    explicit DvmhPiece(int aRank, const Interval inter[], DvmType aOrder) {
        set(aRank, inter, aOrder);
    }
public:
    void set(int aRank, const Interval inter[], DvmType aOrder) {
        rank = aRank;
        rect.resize(rank);
        (~rect)->blockAssign(rank, inter);
        order = aOrder;
    }
    DvmhPiece &operator=(const DvmhPiece &piece) {
        set(piece.rank, ~piece.rect, piece.order);
        return *this;
    }
    DvmhPiece &moveAssign(DvmhPiece &piece) {
        rank = piece.rank;
        order = piece.order;
        rect.moveAssign(piece.rect);
        return *this;
    }
};

class DvmhPieces {
public:
    int getRank() const {
        return rank;
    }
    int getCount() const {
        return pieces.size();
    }
    bool isEmpty() const {
        return pieces.empty();
    }
    const Interval *getPiece(int i, DvmType *pOrder = 0) const {
        assert(i >= 0 && i < (int)pieces.size());
        if (pOrder)
            *pOrder = pieces[i].order;
        return ~pieces[i].rect;
    }
public:
    explicit DvmhPieces(int aRank) {
        assert(aRank >= 0);
        rank = aRank;
        lastCompactCount = 0;
        dvmh_log(PIECES_LEVEL, "pieces %p (rank=%d) created", this, rank);
    }
    static DvmhPieces *createFromLinear(int rank, const Interval &interval, const Interval space[], DvmType order = ABS_ORDER);
    DvmhPieces(const DvmhPieces &p) {
        rank = p.rank;
        lastCompactCount = p.lastCompactCount;
        pieces = p.pieces;
        dvmh_log(PIECES_LEVEL, "pieces %p(count=%d) (rank=%d) dupped new=%p", &p, (int)p.pieces.size(), rank, this);
    }
public:
    void clear() {
        pieces.clear();
        lastCompactCount = 0;
    }
    void append(const DvmhPieces *p) {
        appendInternal(p);
        compactifyLarge();
    }
    void appendOne(const Interval inter[], DvmType order = ABS_ORDER) {
        if (order != 0 && !inter->blockEmpty(rank)) {
            appendOneInternal(inter, order);
            compactifyLarge();
        }
    }
    void subtractOne(const Interval inter[], DvmType order = ABS_ORDER, SubtractMethod method = LEAVE_GREATER) {
        if (order != 0 && !inter->blockEmpty(rank)) {
            subtractOneInternal(inter, order, method);
            compactifyLarge();
        }
    }
    void subtractInplace(const DvmhPieces *pieces2, SubtractMethod method = LEAVE_GREATER) {
        assert(rank == pieces2->rank);
        subtractInplaceInternal(pieces2, method);
        compactifyLarge();
    }
    DvmhPieces *subtract(const DvmhPieces *pieces2, SubtractMethod method = LEAVE_GREATER) const {
        assert(rank == pieces2->rank);
        DvmhPieces *res = subtractInternal(pieces2, method);
        res->compactifyLarge();
        return res;
    }
    void intersectInplace(const DvmhPieces *p2, IntersectMethod method = SET_MIN) {
        intersectInternal(p2, method, true);
        compactifyLarge();
    }
    DvmhPieces *intersect(const DvmhPieces *p2, IntersectMethod method = SET_MIN) const {
        DvmhPieces *res = ((DvmhPieces *)this)->intersectInternal(p2, method, false);
        res->compactifyLarge();
        return res;
    }
    void uniteOne(const DvmType pt[], DvmType order = ABS_ORDER) {
#ifdef NON_CONST_AUTOS
        Interval inter[rank];
#else
        Interval inter[MAX_PIECES_RANK];
#endif
        for (int i = 0; i < rank; i++)
            inter[i][1] = inter[i][0] = pt[i];
        uniteOne(inter, order);
    }
    void uniteOne(const Interval inter[], DvmType order = ABS_ORDER) {
        if (order != 0 && !inter->blockEmpty(rank)) {
            DvmhPieces *p = new DvmhPieces(rank);
            p->appendOneInternal(inter, order);
            uniteInternal(p, true);
            delete p;
            compactifyLarge();
        }
    }
    void unite(const DvmhPieces *pieces2) {
        if (!pieces2->isEmpty()) {
            uniteInternal((DvmhPieces *)pieces2, false);
            compactifyLarge();
        }
    }
    void compactify();
    DvmhPieces *cartesianProduct(const DvmhPieces *p2, DvmhPieces *res = 0) const;
    Interval toInterval() const {
        assert(rank == 1);
        assert(pieces.size() <= 1);
        return (isEmpty() ? Interval::createEmpty() : pieces[0].rect[0]);
    }
    const Interval *toRect() const { assert(getCount() == 1); return ~pieces[0].rect; }
    DvmhPiece getBoundRect() const {
        DvmhPiece res(rank);
        for (int i = 0; i < (int)pieces.size(); i++) {
            if (i == 0)
                res = pieces[i];
            else
                (~res.rect)->blockEncloseInplace(rank, ~pieces[i].rect);
        }
        return res;
    }
public:
    ~DvmhPieces() {
        dvmh_log(PIECES_LEVEL, "pieces %p deleted", this);
    }
protected:
    void appendOneInternal(const Interval inter[], DvmType order) {
        pieces.push_back(DvmhPiece(rank, inter, order));
    }
    DvmhPieces *dup() const {
        return new DvmhPieces(*this);
    }
    void appendInternal(const DvmhPieces *p2) {
        assert(rank == p2->rank);
        if (!p2->pieces.empty()) {
            pieces.reserve(pieces.size() + p2->pieces.size());
            for (int i = 0; i < (int)p2->pieces.size(); i++)
                pieces.push_back(p2->pieces[i]);
        }
    }
    void compactifyLarge() {
        if (lastCompactCount > (int)pieces.size())
            lastCompactCount = pieces.size();
        if (pieces.size() > 20 && (int)pieces.size() >= lastCompactCount * 2)
            compactify();
    }
    void subtractOneInternal(const Interval inter[], DvmType order, SubtractMethod method);
    static DvmhPieces *subtractOneOne(int rank, const Interval inter1[], const Interval inter2[], DvmType order);
    void subtractInplaceInternal(const DvmhPieces *pieces2, SubtractMethod method) {
        for (int i = 0; i < (int)pieces2->pieces.size(); i++)
            subtractOneInternal(~pieces2->pieces[i].rect, pieces2->pieces[i].order, method);
    }
    DvmhPieces *subtractInternal(const DvmhPieces *pieces2, SubtractMethod method) const {
        DvmhPieces *res = dup();
        res->subtractInplaceInternal(pieces2, method);
        return res;
    }
    DvmhPieces *intersectInternal(const DvmhPieces *p2, IntersectMethod method, bool inplace);
    void uniteInternal(DvmhPieces *p, bool canModify);
protected:
    int rank;
    std::vector<DvmhPiece> pieces; // Array of DvmhPiece's
    int lastCompactCount;
};

void piecesOut(LogLevel level, const char *fileName, int lineNumber, const DvmhPieces *p);

}
