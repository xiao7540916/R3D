//
// Created by Administrator on 2022/9/20.
//

#include "sphere.h"
#include <math/plane.h>
#include <math/hexahedron.h>
namespace R3D {
    Sphere::Sphere() : m_radius(1.0f), m_center(vec3(0.0f)) {
    }
    Sphere::Sphere(const float in_radius, const vec3 &in_center) :
            m_radius(in_radius), m_center(in_center) {
    }
    bool Sphere::Intersects(const Plane &in_plane) const {
        //圆心到平面的距离小于等于圆半径
        return (fabsf(in_plane.GetDistance(m_center)) <= m_radius);
    }
    bool Sphere::Intersects(const Hexahedron &in_hexahedron) const {
        //只要球心在某个面的背面且有向距离小于-radius，则判断球在六面体外
        for (int i = 0;i < 6;++i) {
            if (in_hexahedron.GetPlane(i).GetDistance(m_center) < -m_radius) {
                return false;
            }
        }
        return true;
    }
}