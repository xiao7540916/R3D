//
// Created by Administrator on 2022/9/4.
//

#include "route.h"
#include <util/math_util.h>
namespace R3D{
    float Path::CompLen() const {
        if (pathType == PATH_STRAIGHT) {
            float pathlen = glm::distance(startPoint, endPoint);
            return pathlen;
        } else if (pathType == PATH_CURVED) {
            float pathlen = 0.5f *
                            (glm::distance(startPoint, controlPoint1) + glm::distance(controlPoint1, controlPoint2)
                             + glm::distance(controlPoint2, endPoint) + glm::distance(endPoint, startPoint));
            return pathlen;
        }
    }
    vec3 Path::GetLenPoint(float lenin) const {
        if (pathType == PATH_STRAIGHT) {
            return startPoint + lenin * glm::normalize(endPoint - startPoint);
        } else if (pathType == PATH_CURVED) {
            return SetBezier(startPoint, controlPoint1, controlPoint2, endPoint, lenin / length);
        }
        return glm::vec3(0);
    }
    vec3 Path::GetPercPoint(float percin) const {
        return GetLenPoint(percin * length);
    }
    void Route::AddPath(Path &pathin) {
        m_paths.push_back(pathin);
    }
    void Route::InitRoute() {
        if (m_paths.empty()) {
            m_routeLen = 0.0f;
        }
        int pathcount = int(m_paths.size());
        m_routeLen = 0.0f;
        for (int i = 0;i < pathcount;++i) {
            float pathlen = m_paths[i].CompLen();
            m_paths[i].length = pathlen;
            m_paths[i].start = m_routeLen;
            m_routeLen = m_routeLen + pathlen;
        }
    }
    vec3 Route::GetLenPoint(float lenin) const {
        int pathindex = 0;
        int pathcount = int(m_paths.size());
        for (int i = 0;i < pathcount;++i) {
            if (lenin <= m_paths[i].start) {
                break;
            }
            pathindex = i;
        }
        float lenonthispath = lenin - m_paths[pathindex].start;
        return m_paths[pathindex].GetLenPoint(lenonthispath);
    }
    vec3 Route::GetPercPoint(float percin) const {
        return GetLenPoint(percin * m_routeLen);
    }
    vec3 Route::GetLenNormal(float lenin) const {
        if (abs(lenin) < EPSILON_E6) {
            lenin = 0.0001f * m_routeLen;
        }
        if (abs(lenin - m_routeLen) < EPSILON_E6) {
            lenin = 0.9999f * m_routeLen;
        }
        vec3 pointper;
        vec3 pointnext;
        int pathperindex = 0;
        int pathnextindex = 0;
        float lenper = glm::max(lenin - m_routeLen * 0.001f, 0.0f);
        float lennext = glm::min(lenin + m_routeLen * 0.001f, m_routeLen);
        int pathcount = int(m_paths.size());
        {
            for (int i = 0;i < pathcount;++i) {
                if (lenper <= m_paths[i].start) {
                    break;
                }
                pathperindex = i;
            }
            float lenonthispath = lenper - m_paths[pathperindex].start;
            pointper = m_paths[pathperindex].GetLenPoint(lenonthispath);
        }
        {
            for (int i = 0;i < pathcount;++i) {
                if (lennext <= m_paths[i].start) {
                    break;
                }
                pathnextindex = i;
            }
            float lenonthispath = lennext - m_paths[pathnextindex].start;
            pointnext = m_paths[pathnextindex].GetLenPoint(lenonthispath);
        }
        if (pathperindex == pathnextindex) {
            return glm::normalize(pointnext - pointper);
        } else {
            int pathindex = 0;
            for (int i = 0;i < pathcount;++i) {
                if (lenin <= m_paths[i].start) {
                    break;
                }
                pathindex = i;
            }
            float lenonthispath = lenin - m_paths[pathindex].start;
            vec3 pointthis = m_paths[pathindex].GetLenPoint(lenonthispath);
            return (pathindex == pathperindex) ? glm::normalize(pointthis - pointper) : glm::normalize(
                    pointnext - pointthis);
        }
    }
    vec3 Route::GetPercNormal(float percin) const {
        return GetLenNormal(percin * m_routeLen);
    }
}

