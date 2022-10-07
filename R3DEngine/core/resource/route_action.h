//
// Created by Administrator on 2022/10/7.
//

#pragma once
#include <structdefine.h>
#include <math/route.h>
namespace R3D {
    class RouteAction {
    public:
        Route m_route;
        float m_oneActionTime = -1.0f;//运行整个周期所需的时间
        float m_speed = -1.0f;//运行速度
        float m_actionTime = 0.0f;//动画运行时间
        vec3 GetPosition();
        vec3 GetForward();
        void AddPath(Path &in_path);
        void Init();
    };
}



