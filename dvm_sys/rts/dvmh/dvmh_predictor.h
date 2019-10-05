#pragma once

#include <cstdlib>
#include <cstdio>
#include <cmath>
#include <vector>
#include <utility>

typedef std::vector<int> stages_t;
typedef std::vector<double> times_t;

class DvmhPredictor
{
public:
    void setGlobalStable(bool st) { mGlobalStable = st; }
    int getBestStage() { return mBestStage; }
    double getBestTime() { return mBestTime; }
    int getLastStage() { return mLastStage; }
    void setDebug(int rank) { mDebug = true; mRank = rank; }
    void setAxis(const int &axis) { mAxis = axis; }
    bool isStable() { return mStable; }
    bool isGlobalStable() { return mGlobalStable; }
public:
    DvmhPredictor() : mMult(0), mRank(-1),  mStable(false), mGlobalStable(false),
                      mBestStage(0), mBestTime(0.), mLastStage(0.), mDebug(false), mApproxPolynom(mApproxPolynomDegree + 1) {}
public:
    void setBestStage(const int &st);
    void setBestTime(const int &stage, const double &time);
    int predictStage();
    void addStage(const int &stage);
    void addTime(const int &stage, const double &time);
private:
    void findApproxPolynom();
    int findCubicRoots(double roots[]);
    std::vector<int> findMinimumPoints(std::vector<double> &roots);
    double computeDerivative(const double &point);
    void setBestPerf();
private:
    stages_t mStages;
    times_t mTimes;
    int mMult;
    int mRank;
    bool mStable;
    bool mGlobalStable;
    static const int mApproxPolynomDegree = 4;
    static const int mMinApproxPoints = 6;
    static const int mMaxApproxPoints = 10;
    int mBestStage;
    double mBestTime;
    int mLastStage;
    bool mDebug;
    int mAxis;
    int mDevCnt;
    std::vector<double> mApproxPolynom;
};
