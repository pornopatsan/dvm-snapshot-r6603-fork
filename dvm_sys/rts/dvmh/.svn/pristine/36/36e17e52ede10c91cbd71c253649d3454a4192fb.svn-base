#include "dvmh_pieces.h"

#include <algorithm>

#include "dvmh_log.h"

namespace libdvmh {

struct CompactComp {
    int lastAxis;
    CompactComp(int aLastAxis): lastAxis(aLastAxis) {}
    bool operator()(const DvmhPiece &p1, const DvmhPiece &p2) const {
        for (int i = 0; i < lastAxis; i++)
            for (int j = 0; j < 2; j++) {
                if (p1.rect[i][j] < p2.rect[i][j])
                    return true;
                if (p1.rect[i][j] > p2.rect[i][j])
                    return false;
            }
        for (int i = lastAxis + 1; i < p1.rank; i++)
            for (int j = 0; j < 2; j++) {
                if (p1.rect[i][j] < p2.rect[i][j])
                    return true;
                if (p1.rect[i][j] > p2.rect[i][j])
                    return false;
            }
        for (int j = 0; j < 2; j++) {
            if (p1.rect[lastAxis][j] < p2.rect[lastAxis][j])
                return true;
            if (p1.rect[lastAxis][j] > p2.rect[lastAxis][j])
                return false;
        }
        return false;
    }
};

DvmhPieces *DvmhPieces::createFromLinear(int rank, const Interval &interval, const Interval space[], DvmType order) {
    DvmhPieces *res = new DvmhPieces(rank);
    if (interval.empty())
        return res;
    UDvmType spaceSize = space->blockSize(rank);
    assert(interval[0] >= 0 && interval[1] < (DvmType)spaceSize);
    if (rank == 0) {
        res->appendOneInternal(space, order);
    } else {
#ifdef NON_CONST_AUTOS
        Interval piece[rank];
#else
        Interval piece[MAX_ARRAY_RANK];
#endif
        UDvmType collector = 1;
        for (int i = rank - 1; i >= 0; i--) {
            UDvmType curSize = space[i].size();
            UDvmType beginIdx = interval[0] / collector % curSize;
            piece[i][0] = space[i][0] + beginIdx;
            piece[i][1] = piece[i][0];
            collector *= curSize;
        }
        Interval rest = interval;
        collector = 1;
        for (int i = rank - 1; i >= 0; i--) {
            UDvmType curSize = space[i].size();
            UDvmType beginAccumIdx = rest[0] / collector;
            UDvmType endAccumIdx = (rest[1] + 1) / collector;
            if (beginAccumIdx % curSize != 0 && endAccumIdx / curSize > beginAccumIdx / curSize) {
                piece[i][0] = space[i][0] + beginAccumIdx % curSize;
                piece[i][1] = space[i][1];
                res->appendOneInternal(piece, order);
                rest[0] = (beginAccumIdx / curSize + 1) * curSize * collector;
            }
            piece[i] = space[i];
            collector *= curSize;
        }
        collector = spaceSize;
        for (int i = 0; i < rank; i++) {
            UDvmType curSize = space[i].size();
            collector /= curSize;
            UDvmType beginIdx = rest[0] / collector;
            UDvmType axisLen = (rest[1] + 1) / collector - beginIdx;
            piece[i][0] = space[i][0] + beginIdx;
            piece[i][1] = piece[i][0] + (DvmType)axisLen - 1;
            if (axisLen > 0)
                res->appendOneInternal(piece, order);
            piece[i][0] = piece[i][1] + 1;
            piece[i][1] = piece[i][0];
            rest[0] += axisLen * collector;
        }
    }
    return res;
}

void DvmhPieces::compactify() {
    dvmh_log(PIECES_LEVEL, "Trying to compactify. Size = %d", (int)pieces.size());
    custom_log(PIECES_LEVEL, piecesOut, this);
    if (pieces.size() > 1 && rank >= 1) {
        int prevSuccess = -1;
        bool stop = false;
        for (int ax = rank - 1; !stop; ax = (ax + rank - 1) % rank) {
            if (prevSuccess != ax) {
                CompactComp myComp(ax);
                std::sort(pieces.begin(), pieces.end(), myComp);
                bool curSuccess = false;
                int unitedCount = 0;
                for (int i = 1; i < (int)pieces.size(); i++) {
                    int prev = i - 1 - unitedCount;
                    bool uniteFlag = pieces[i].order == pieces[prev].order;
                    for (int j = 0; uniteFlag && j < ax; j++)
                        uniteFlag = uniteFlag && pieces[i].rect[j] == pieces[prev].rect[j];
                    for (int j = ax + 1; uniteFlag && j < rank; j++)
                        uniteFlag = uniteFlag && pieces[i].rect[j] == pieces[prev].rect[j];
                    uniteFlag = uniteFlag && pieces[i].rect[ax][0] == pieces[prev].rect[ax][1] + 1;
                    if (uniteFlag) {
                        pieces[prev].rect[ax][1] = pieces[i].rect[ax][1];
                        unitedCount++;
                    } else {
                        if (unitedCount > 0)
                            pieces[i - unitedCount].moveAssign(pieces[i]);
                    }
                }
                if (unitedCount > 0) {
                    pieces.resize(pieces.size() - unitedCount);
                    curSuccess = true;
                    if (pieces.size() <= 1)
                        stop = true;
                }
                if (curSuccess)
                    prevSuccess = ax;
                else if (prevSuccess == -1 && ax == 0)
                    stop = true;
            } else {
                stop = true;
            }
        }
    }
    lastCompactCount = pieces.size();
    dvmh_log(PIECES_LEVEL, "After compactification. Size = %d", (int)pieces.size());
}

DvmhPieces *DvmhPieces::cartesianProduct(const DvmhPieces *p2, DvmhPieces *res) const {
    int r1 = rank;
    int r2 = p2->rank;
    int r3 = r1 + r2;
    if (!res)
        res = new DvmhPieces(r3);
    else
        res->clear();
    assert(res->rank == r3);
#ifdef NON_CONST_AUTOS
    Interval resInter[r3];
#else
    Interval resInter[MAX_PIECES_RANK];
#endif
    for (int i = 0; i < (int)pieces.size(); i++) {
        resInter->blockAssign(r1, ~pieces[i].rect);
        for (int j = 0; j < (int)p2->pieces.size(); j++) {
            assert(pieces[i].order == p2->pieces[j].order);
            (resInter + r1)->blockAssign(r2, ~p2->pieces[j].rect);
            res->appendOneInternal(resInter, pieces[i].order);
        }
    }
    return res;
}

static bool lessOrEqual(const DvmType &order1, const DvmType &order2) {
    return order1 <= order2;
}

static const DvmType &minOrder(const DvmType &order1, const DvmType &order2) {
    return std::min(order1, order2);
}

void DvmhPieces::subtractOneInternal(const Interval inter[], DvmType order, SubtractMethod method) {
    dvmh_log(PIECES_LEVEL, "piecesSubtractOne %p(count=%d,rank=%d,method=%d)", this, (int)pieces.size(), rank, method);
    custom_log(PIECES_LEVEL, piecesOut, this);
    dvmh_log(PIECES_LEVEL, "-");
    custom_log(PIECES_LEVEL, blockOut, rank, inter, (order == ABS_ORDER ? 0 : order));
    int pc = pieces.size();
    for (int i = 0; i < pc; i++) {
        bool toProcess = false;
        switch (method) {
            case LEAVE_GREATER:
                toProcess = lessOrEqual(pieces[i].order, order);
                break;
            case FROM_ABS:
                toProcess = pieces[i].order == ABS_ORDER;
                break;
        }
        if (toProcess && (~pieces[i].rect)->blockIntersects(rank, inter)) {
            DvmhPieces *toInsert = subtractOneOne(rank, ~pieces[i].rect, inter, pieces[i].order);
            assert(toInsert);
            if (method == FROM_ABS && order != ABS_ORDER) {
                if (rank > 0) {
#ifdef NON_CONST_AUTOS
                    Interval tmpInter[rank];
#else
                    Interval tmpInter[MAX_PIECES_RANK];
#endif
                    bool intersects = (~pieces[i].rect)->blockIntersect(rank, inter, tmpInter);
                    assert(intersects);
                    toInsert->appendOne(tmpInter, order);
                } else {
                    toInsert->appendOne(0, order);
                }
            }
            if (toInsert->isEmpty()) {
                if (i < (int)pieces.size() - 1)
                    pieces[i].moveAssign(pieces.back());
                pieces.pop_back();
                if (pc > (int)pieces.size()) {
                    pc = pieces.size();
                    i--;
                }
            } else {
                pieces[i].moveAssign(toInsert->pieces.back());
                toInsert->pieces.pop_back();
                appendInternal(toInsert);
            }
            delete toInsert;
        }
    }
    dvmh_log(PIECES_LEVEL, "=");
    custom_log(PIECES_LEVEL, piecesOut, this);
}

DvmhPieces *DvmhPieces::subtractOneOne(int rank, const Interval inter1[], const Interval inter2[], DvmType order) {
    // here order - order of the result
    dvmh_log(PIECES_LEVEL, "piecesSubtractOneOne");
    custom_log(PIECES_LEVEL, blockOut, rank, inter1, (order == ABS_ORDER ? 0 : order));
    dvmh_log(PIECES_LEVEL, "and");
    custom_log(PIECES_LEVEL, blockOut, rank, inter2);
    DvmhPieces *res = new DvmhPieces(rank);
    if (!inter1->blockIntersects(rank, inter2)) {
        res->appendOneInternal(inter1, order);
    } else if (rank > 0) {
#ifdef NON_CONST_AUTOS
        Interval rest[rank];
#else
        Interval rest[MAX_PIECES_RANK];
#endif
        rest->blockAssign(rank, inter1);
        for (int r = 0; r < rank; r++) {
            if (rest[r][0] < inter2[r][0]) {
                DvmType hi = rest[r][1];
                if (hi >= inter2[r][0])
                    hi = inter2[r][0] - 1;
                if (rest[r][0] <= hi) {
#ifdef NON_CONST_AUTOS
                    Interval toInsert[rank];
#else
                    Interval toInsert[MAX_PIECES_RANK];
#endif
                    toInsert->blockAssign(rank, rest);
                    toInsert[r][1] = hi;
                    res->appendOneInternal(toInsert, order);
                }
                rest[r][0] = hi + 1;
            }
            if (rest[r][1] > inter2[r][1]) {
                DvmType lo = rest[r][0];
                if (lo <= inter2[r][1])
                    lo = inter2[r][1] + 1;
                if (rest[r][1] >= lo) {
#ifdef NON_CONST_AUTOS
                    Interval toInsert[rank];
#else
                    Interval toInsert[MAX_PIECES_RANK];
#endif
                    toInsert->blockAssign(rank, rest);
                    toInsert[r][0] = lo;
                    res->appendOneInternal(toInsert, order);
                }
                rest[r][1] = lo - 1;
            }
            if (rest[r].empty())
                break;
        }
    }
    dvmh_log(PIECES_LEVEL, "=");
    custom_log(PIECES_LEVEL, piecesOut, res);
    return res;
}

DvmhPieces *DvmhPieces::intersectInternal(const DvmhPieces *p2, IntersectMethod method, bool inplace) {
    assert(rank == p2->rank);
    dvmh_log(PIECES_LEVEL, "piecesIntersect %p(count=%d) %p(count=%d) rank=%d", this, (int)pieces.size(), p2, (int)p2->pieces.size(), rank);
    if (pieces.empty() || p2->pieces.empty()) {
        if (!inplace)
            return new DvmhPieces(rank);
        else {
            pieces.clear();
            return this;
        }
    }
    if (rank == 0) {
        if (!inplace) {
            DvmhPieces *res = new DvmhPieces(rank);
            switch (method) {
                case SET_MIN:
                    res->appendOneInternal(0, minOrder(pieces[0].order, p2->pieces[0].order));
                    break;
                case SET_NOT_LESS:
                    if (lessOrEqual(pieces[0].order, p2->pieces[0].order))
                        res->appendOneInternal(0, p2->pieces[0].order);
                    break;
            }
            return res;
        } else {
            switch (method) {
                case SET_MIN:
                    pieces[0].order = minOrder(pieces[0].order, p2->pieces[0].order);
                    break;
                case SET_NOT_LESS:
                    if (lessOrEqual(pieces[0].order, p2->pieces[0].order))
                        pieces[0].order = p2->pieces[0].order;
                    else
                        pieces.clear();
                    break;
            }
            return this;
        }
    }
    DvmhPieces *res = 0;
    if (inplace)
        res = this;
    else
        res = dup();
    assert(res);
    int pc = res->pieces.size();
    for (int i = 0; i < pc; i++) {
#ifdef NON_CONST_AUTOS
        Interval srcInter[rank];
#else
        Interval srcInter[MAX_PIECES_RANK];
#endif
        DvmType srcOrder = res->pieces[i].order;
        srcInter->blockAssign(rank, ~res->pieces[i].rect);
        bool writtenFlag = false;
        for (int j = 0; j < (int)p2->pieces.size(); j++) {
#ifdef NON_CONST_AUTOS
            Interval resInter[rank];
#else
            Interval resInter[MAX_PIECES_RANK];
#endif
            bool okFlag = true;
            if (method == SET_NOT_LESS)
                okFlag = lessOrEqual(srcOrder, p2->pieces[j].order);
            if (okFlag && srcInter->blockIntersect(rank, ~p2->pieces[j].rect, resInter)) {
                DvmType resOrder = srcOrder;
                switch (method) {
                    case SET_MIN:
                        resOrder = minOrder(srcOrder, p2->pieces[j].order);
                        break;
                    case SET_NOT_LESS:
                        assert(lessOrEqual(srcOrder, p2->pieces[j].order));
                        resOrder = p2->pieces[j].order;
                        break;
                }
                if (!writtenFlag) {
                    (~res->pieces[i].rect)->blockAssign(rank, resInter);
                    res->pieces[i].order = resOrder;
                    writtenFlag = true;
                } else {
                    res->appendOneInternal(resInter, resOrder);
                }
            }
        }
        if (!writtenFlag) {
            if (i < (int)res->pieces.size() - 1)
                res->pieces[i].moveAssign(res->pieces.back());
            res->pieces.pop_back();
            if (pc > (int)res->pieces.size()) {
                pc = res->pieces.size();
                i--;
            }
        }
    }
    dvmh_log(PIECES_LEVEL, "intersect result:");
    custom_log(PIECES_LEVEL, piecesOut, res);
    return res;
}

void DvmhPieces::uniteInternal(DvmhPieces *p, bool canModify) {
    bool copyDone = false;
    int pc = pieces.size();
    for (int i = 0; i < pc; i++) {
        int ppc = p->pieces.size();
        for (int j = 0; j < ppc; j++) {
            DvmhPieces *toInsert1 = 0;
            DvmhPieces *toInsert2 = 0;
            if (lessOrEqual(pieces[i].order, p->pieces[j].order))
                toInsert1 = subtractOneOne(rank, ~pieces[i].rect, ~p->pieces[j].rect, pieces[i].order);
            if (lessOrEqual(p->pieces[j].order, pieces[i].order))
                toInsert2 = subtractOneOne(rank, ~p->pieces[j].rect, ~pieces[i].rect, p->pieces[j].order);
            bool fastOut = false;
            if (toInsert1 && (!toInsert2 || toInsert2->getCount() > toInsert1->getCount())) {
                if (toInsert1->isEmpty()) {
                    if (i < (int)pieces.size() - 1)
                        pieces[i].moveAssign(pieces.back());
                    pieces.pop_back();
                    pc = pieces.size();
                    i--;
                    fastOut = true;
                } else {
                    pieces[i].moveAssign(toInsert1->pieces.back());
                    toInsert1->pieces.pop_back();
                    appendInternal(toInsert1);
                    pc = pieces.size();
                }
            } else {
                if (!canModify) {
                    p = p->dup();
                    canModify = true;
                    copyDone = true;
                }
                if (toInsert2->isEmpty()) {
                    if (j < (int)p->pieces.size() - 1)
                        p->pieces[j].moveAssign(p->pieces.back());
                    p->pieces.pop_back();
                    if (ppc > (int)p->pieces.size()) {
                        ppc = p->pieces.size();
                        j--;
                    }
                } else {
                    p->pieces[j].moveAssign(toInsert2->pieces.back());
                    toInsert2->pieces.pop_back();
                    p->appendInternal(toInsert2);
                }
            }
            delete toInsert1;
            delete toInsert2;
            if (fastOut)
                break;
        }
    }
    appendInternal(p);
    if (copyDone)
        delete p;
}

void piecesOut(LogLevel level, const char *fileName, int lineNumber, const DvmhPieces *p) {
    if (!p)
        return;
    dvmhLogger.startBlock(level);
    for (int i = 0; i < p->getCount(); i++) {
        const Interval *inter;
        DvmType order;
        inter = p->getPiece(i, &order);
        blockOut(level, fileName, lineNumber, p->getRank(), inter, (order == ABS_ORDER ? 0 : order));
    }
    dvmhLogger.endBlock(level, fileName, lineNumber);
}

}
