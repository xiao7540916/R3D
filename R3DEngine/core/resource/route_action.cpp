//
// Created by Administrator on 2022/10/7.
//

#include "route_action.h"
namespace R3D {
    vec3 RouteAction::GetPosition() {
        float roundcounts = m_actionTime/m_oneActionTime;
        float percent = roundcounts-floorf(roundcounts);
        return m_route.GetPercPoint(percent);
    }
    vec3 RouteAction::GetForward() {
        float roundcounts = m_actionTime/m_oneActionTime;
        float percent = roundcounts-floorf(roundcounts);
        return m_route.GetPercNormal(percent);
    }
    void RouteAction::AddPath(Path &in_path) {
        m_route.AddPath(in_path);
    }
    void RouteAction::Init() {
        m_route.InitRoute();
        //时长优先度高于速度优先度
        if (m_oneActionTime > 0.0f) {
            m_speed = m_route.GetRouteLen() / m_oneActionTime;
        } else {
            if (m_speed > 0.0f) {
                m_oneActionTime = m_route.GetRouteLen() / m_speed;
            } else {
                m_speed = 0.000001;
                m_oneActionTime = 100000.0f;
            }
        }
    }
}