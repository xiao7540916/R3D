//
// Created by Administrator on 2021/4/2.
//

#include "game_time.h"
//Stoped
//                     |<--paused time-->|
// ----*---------------*-----------------*------------*------------*------> time
//  mBaseTime       mStopTime        startTime     mStopTime    mCurrTime

//current
//                     |<--paused time-->|
// ----*---------------*-----------------*------------*------> time
//  mBaseTime       mStopTime        startTime     mCurrTime

//restart
//                     |<-------d------->|
// ----*---------------*-----------------*------------> time
//  mBaseTime       mStopTime        startTime

GameTime::GameTime() {
    mBaseTime = std::chrono::steady_clock::now();
    mCurrTime = mBaseTime;
    mStopTime = mBaseTime;
    mPrevTime = mBaseTime;
    mDeltaTime = mCurrTime - mCurrTime;
    mPausedTime = mCurrTime - mCurrTime;
    mStoped = false;
}
float GameTime::TotalTime() const {
    if (mStoped) {
        return float((std::chrono::duration<double>(mStopTime - mBaseTime)).count() - mPausedTime.count());
    } else {
        return float((std::chrono::duration<double>(mCurrTime - mBaseTime)).count() - mPausedTime.count());
    }
}
float GameTime::DeltaTime() const {
    return float(mDeltaTime.count());
}
void GameTime::Reset() {
    mBaseTime = std::chrono::steady_clock::now();
    mPrevTime = mBaseTime;
    mStopTime = mBaseTime;
    mPausedTime = mCurrTime - mCurrTime;
    mStoped = false;
}
void GameTime::Start() {
    std::chrono::time_point<std::chrono::steady_clock> startTime = std::chrono::steady_clock::now();
    if (mStoped) {
        mPausedTime += (startTime - mStopTime);
        mPrevTime = startTime;
        mStopTime = mBaseTime;
        mStoped = false;
    }
}
void GameTime::Stop() {
    if (!mStoped) {
        mCurrTime = std::chrono::steady_clock::now();
        mStopTime = mCurrTime;
        mStoped = true;
    }
}
void GameTime::Tick() {
    if (mStoped) {
        mDeltaTime = mCurrTime - mCurrTime;
        return;
    }
    mCurrTime = std::chrono::steady_clock::now();
    mDeltaTime = mCurrTime - mPrevTime;
    mPrevTime = mCurrTime;
    if (mDeltaTime.count() < 0.0) {
        mDeltaTime = mCurrTime - mCurrTime;
    }
}
