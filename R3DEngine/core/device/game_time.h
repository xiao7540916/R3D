//
// Created by Administrator on 2021/4/2.
//

#ifndef VULKANLAB_GAMETIME_H
#define VULKANLAB_GAMETIME_H
#include <iostream>
#include <chrono>

class GameTime {
public:
    GameTime();
    float TotalTime() const;
    float DeltaTime() const;
    void Reset();//重置
    void Start();//开始计时
    void Stop();//暂停计时器调用
    void Tick();//每帧调用
private:
    std::chrono::duration<double> mDeltaTime{};//间隔时间
    std::chrono::time_point<std::chrono::steady_clock> mBaseTime;//创建时时间
    std::chrono::duration<double> mPausedTime{};//暂停时间段，用于再次start
    std::chrono::time_point<std::chrono::steady_clock> mStopTime;//停止时间
    std::chrono::time_point<std::chrono::steady_clock> mPrevTime;//暂存上一个tick的时间
    std::chrono::time_point<std::chrono::steady_clock> mCurrTime;//现在时间
    bool mStoped;
};

#endif //VULKANLAB_GAMETIME_H
