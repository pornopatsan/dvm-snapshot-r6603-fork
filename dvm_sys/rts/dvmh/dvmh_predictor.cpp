#include "dvmh_predictor.h"

#include <iterator>
#include <algorithm>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

void DvmhPredictor::setBestStage(const int &st) {
    mBestStage = st;
    if ((int)mStages.size() < mMaxApproxPoints && std::find(mStages.begin(), mStages.end(), st) == mStages.end())
        mStages.push_back(st);

    if (!mStable && (int)mStages.size() == mMaxApproxPoints)
        mStable = true;
}

void DvmhPredictor::setBestTime(const int &stage, const double &time) {
    stages_t::iterator stIt = std::find(mStages.begin(), mStages.end(), stage);
    if (stIt == mStages.end())
        return;
    size_t ind = std::distance(mStages.begin(), stIt);
    if (ind > mTimes.size()) {
        printf("%d | Something went terribly wrong! There are more stages than times.\n", mAxis);
        return;
    }

    if (ind < mTimes.size()) {
        mTimes[ind] = time;
        if (mMult != 0)
            mTimes[ind] *= mMult;
    } else {
        mTimes.push_back(time);
    }
    mBestTime = time * (mMult == 0 ? 1 : mMult);
}

int DvmhPredictor::predictStage() {
    if (mDebug) {
        for (size_t i = 0; i < mStages.size(); i++)
            printf("rank %d | stage %d, time %f\n", mRank, mStages[i], mTimes[i]);
    }

    int ret = -1;
    if (mStable) {
        ret = mBestStage;
    } else {
        if ((int)mStages.size() < mMinApproxPoints) {
            if (mStages.empty()) {
                mStages.push_back(1);
                mLastStage = 1;
                return 1;
            }

            ret = mStages.back() + 1*mDevCnt + (mStages.size() / 3);
        } else if (mTimes.size() < mStages.size()) {
            ret = mStages[mTimes.size()];
        } else {
            double roots[3];
            int rootsNum = findCubicRoots(roots);
            if (rootsNum == 0) {
                mStable = true;
                setBestPerf();
                mLastStage = mBestStage;
                return mBestStage;
            }
            std::vector<double> root(roots, roots + rootsNum);
            std::vector<int> minInds;
            minInds = findMinimumPoints(root);

            if (minInds.empty()) {
                return mBestStage;
            }

            size_t old_size = mStages.size();
            if (minInds.size() > 0) {
                for (size_t i = 0; i < minInds.size(); i++) {
                    if (std::find(mStages.begin(), mStages.end(), ceil(root[minInds[i]])) == mStages.end())
                        mStages.push_back(ceil(root[minInds[i]]));
                }
            }
            if (old_size == mStages.size()) {
                mStable = true;
                setBestPerf();
                ret = mBestStage;
            } else {
                ret = mStages[old_size];
            }
            if (mDebug) {
                printf("proc %d| stages:\n", mRank);
                for (size_t i = 0; i < mStages.size(); i++) {
                    printf("%d\n", mStages[i]);
                }
                printf("proc %d| times size:%d\n", mRank, (int)mTimes.size());
                printf("3ret is now %d\n", ret);
            }
        }

    }
    mLastStage = ret;
    return ret;
}

void DvmhPredictor::addStage(const int &stage) {
    if (mStable == false) {
        if(std::find(mStages.begin(), mStages.end(), stage) != mStages.end())
            return;

        mStages.push_back(stage);
        if (mLastStage == 0)
            mLastStage = stage;

        if (mStages.size() == 1)
            mDevCnt = stage;
    }
}

void DvmhPredictor::addTime(const int &stage, const double &time) {
    if (mStable)
        return;
    stages_t::iterator stIt = std::find(mStages.begin(), mStages.end(), stage);
    if (stIt == mStages.end())
        return;
    size_t ind = std::distance(mStages.begin(), stIt);

    if (ind < mTimes.size()) {
        mTimes[ind] = time;
        if (stage == mBestStage)
            mBestTime = time;
    } else if (ind == mTimes.size()) {
        mTimes.push_back(time);
    } else {
        printf("axis %d | Something went terribly wrong during adding time! There are more stages than times.\n", mAxis);
        return;
    }

    if (mTimes.size() == 2) {
        double diff = fabs(mTimes[0] - mTimes[1]);
        if (diff > 0)
            mMult = 1;
        while (diff < 1 && diff != 0) {
            diff *= 10;
            mMult *= 10;
        }
        if (mMult != 0) {
            mTimes[0] *= mMult;
            mTimes[1] *= mMult;
            mBestTime *= mMult;
        }
    } else if (mTimes.size() > 2) {
        if (mMult == 0) {
            double diff = fabs(mTimes[mTimes.size() - 2] - mTimes[mTimes.size() - 1]);
            if (diff > 0)
                mMult = 1;
            while (diff < 1 && diff != 0) {
                diff *= 10;
                mMult *= 10;
            }
            if (mMult != 0) {
                for (times_t::iterator it = mTimes.begin(); it != mTimes.end(); it++) {
                    *it *= mMult;
                }
                mBestTime *= mMult;
            }
        } else {
            mTimes[mTimes.size() - 1] *= mMult;
        }
    }

    if ((int)mTimes.size() >= mMinApproxPoints && !mStable) {
        findApproxPolynom();
    }

    double adjustedTime = time * (mMult == 0 ? 1 : mMult);
    if (adjustedTime < mBestTime || mBestTime == 0) {
        mBestTime = adjustedTime;
        mBestStage = mStages[ind];
    }
    if ((int)mTimes.size() == mMaxApproxPoints) {
        mStable = true;
    }
}

void DvmhPredictor::findApproxPolynom() {
    const int n = mApproxPolynomDegree;
    const int N = mStages.size();
    int i, j, k;

    double X[2 * n + 1];
    double Y[n + 1];
    double B[(n + 1) * (n + 2)];
    const int B_N = n + 1;

    for (i = 0; i < 2 * n + 1; i++) {
        X[i] = 0;
        for (j = 0; j < N; j++)
            X[i] += pow(mStages[j], i);
    }

    for (i = 0; i <= n; i++) {
        for (j = 0; j <= n; j++)
            B[i * B_N + j] = X[i + j];
    }

    for (i = 0; i < n + 1; i++) {
        Y[i] = 0;
        for (j = 0; j < N; j++)
            Y[i] += pow(mStages[j], i) * mTimes[j];
    }

    for (i = 0; i <= n; i++)
        B[i * B_N + (n + 1)] = Y[i];
    const int n1 = n + 1;

    for (i = 0; i < n1; i++) {
        for (k = i + 1; k < n1; k++) {
            if (B[i * B_N + i] < B[k * B_N + i]) {
                for (j = 0; j <= n1; j++) {
                    const double temp = B[i * B_N + j];
                    B[i * B_N + j] = B[k * B_N + j];
                    B[k * B_N + j] = temp;
                }
            }
        }
    }

    for (i = 0; i < n; i++) {
        for (k = i + 1; k < n1; k++) {
            double t = B[k * B_N + i] / B[i * B_N + i];
            for (j = 0; j <= n1; j++)
                B[k * B_N + j] -= t*B[i * B_N + j];
        }
    }
    for (i = n; i >= 0; i--) {
        mApproxPolynom[i] = B[i * B_N + n1];
        for (j = 0; j < n1; j++) {
            if (j != i)
                mApproxPolynom[i] -= B[i * B_N + j] * mApproxPolynom[j];
        }
        mApproxPolynom[i] /= B[i * B_N + i];

    }
}

int DvmhPredictor::findCubicRoots(double roots[]) {
    const double a = 0.75 * mApproxPolynom[3] / mApproxPolynom[4];
    const double b = 0.5 * mApproxPolynom[2] / mApproxPolynom[4];
    const double c = 0.25 * mApproxPolynom[1] / mApproxPolynom[4];
    const double Q = (a * a - 3. * b) / 9.;
    const double R = (2. * pow(a, 3) - 9. * a * b + 27. * c) / 54.;
    const double S = pow(Q, 3) - pow(R, 2);
    if(S <= 0)
        return 0;

    const double phi = acos(R / sqrt(Q * Q * Q)) / 3.;
    roots[0] = ((-2) * sqrt(Q)) * cos(phi) - a / 3.0;
    roots[1] = ((-2) * sqrt(Q)) * cos(phi + 2 * M_PI / 3.0) - a / 3.0;
    roots[2] = ((-2) * sqrt(Q)) * cos(phi - 2 * M_PI / 3.0) - a / 3.0;
    return 3;
}

std::vector<int> DvmhPredictor::findMinimumPoints(std::vector<double> &roots) {
    std::vector<int> mins;

    std::sort(roots.begin(), roots.end());
    for (int i = 0; i < (int)roots.size(); i++) {
        double left_int = 0.;
        double right_int = 0.;

        if (roots[i] < 0 || roots[i] > 102)
            continue;

        if (i == 0)
            left_int = computeDerivative(std::max(roots[i] - 1,(double)mStages[0]));
        else
            left_int = computeDerivative((roots[i] + roots[i - 1]) / 2);

        if (i == (int)roots.size() - 1)
            right_int = computeDerivative(roots[i] + 1);
        else
            right_int = computeDerivative((roots[i] + roots[i + 1]) / 2);

        if(mDebug)
            printf("proc %d| root%d : %f %f\n", mRank, i, left_int, right_int);

        if (left_int < 0 && right_int > 0)
            mins.push_back(i);
    }

    for (size_t i = 0; i < mins.size(); i++) {
        if (mDebug)
            printf("proc %d| m %d\n", mRank, mins[i]);
    }
    return mins;
}

double DvmhPredictor::computeDerivative(const double &point) {
    return ((4 * mApproxPolynom[4] * point + 3 * mApproxPolynom[3]) * point + 2 * mApproxPolynom[2]) * point + mApproxPolynom[1];
}

void DvmhPredictor::setBestPerf() {
    double potentialBestTime = mTimes[0];
    int potentialBestStage = mStages[0];

    if (mBestStage - mStages[0] < 10) {
        if (mDebug)
            printf("proc %d | bestTime: %f; bestStage: %d\n", mRank, mBestTime, mBestStage);
        return;
    }

    for (size_t time_ind = 0; time_ind < mTimes.size(); ++time_ind) {
        if (mDebug) {
            printf("proc %d | time: %f, bestTime: %f \n", mRank, mTimes[time_ind], mBestTime);
            printf("proc %d | diff: %f\n", mRank, std::abs(mTimes[time_ind] - mBestTime));
        }

        if (std::abs(mStages[0] - mStages[time_ind]) < 10 && mTimes[time_ind] < potentialBestTime) {
            potentialBestTime = mTimes[time_ind];
            potentialBestStage = mStages[time_ind];
        }
    }
    mBestTime = potentialBestTime;
    mBestStage = potentialBestStage;
}
