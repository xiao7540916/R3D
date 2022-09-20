//
// Created by Administrator on 2022/9/20.
//

#pragma once
#include <structdefine.h>
#include <util/math_util.h>
namespace R3D {
    class Plane;
    class Hexahedron;
    class Frustum;
    class Sphere {
    public:
        Sphere();
        Sphere(float in_radius, const vec3 &in_center);
        inline float GetRadius() const {return m_radius;}
        inline void SetRadius(float in_radius) {m_radius = in_radius;}
        inline const vec3 &GetCenter() const {return m_center;}
        inline void SetCenter(const vec3 &in_center) {m_center = in_center;}
        //判断当前球与传入的球是否相交
        inline bool Intersects(const Sphere &rhs) const {
            return SquaredVec3(rhs.m_center - m_center) <=
                   ((rhs.m_radius + m_radius) * (rhs.m_radius + m_radius));
        }
        //判断当前球是否在传入球的内部
        inline bool InSphere(const Sphere &rhs) const {
            float d = rhs.m_radius - m_radius;
            if (d < 0.0f) {
                return false;
            }
            return (SquaredVec3(m_center - rhs.m_center) < d * d);
        }
        //判断传入球是否在当前球内部
        inline bool ContainSphere(const Sphere &rhs) const {
            float d = m_radius - rhs.m_radius;
            if (d < 0.0f) {
                return false;
            }
            return (SquaredVec3(m_center - rhs.m_center) < d * d);
        }
        //判断当前球与传入的平面是否相交
        bool Intersects(const Plane &in_plane) const;
        //判断传入的点是否在当前球内
        inline bool Intersects(const vec3 &in_point) const {
            return SquaredVec3(in_point - m_center) < m_radius * m_radius;
        }
        //判断球与六面体是否有交集
        bool Intersects(const Hexahedron &in_hexahedron) const;
        //判断两个球是否相同(半径和圆心相同)
        bool operator==(const Sphere &rhs) const {
            if (!EqualFloat(m_radius, rhs.m_radius)) {
                return false;
            }
            if ((!EqualFloat(m_center.x, rhs.m_center.x)) ||
                (!EqualFloat(m_center.y, rhs.m_center.y)) ||
                (!EqualFloat(m_center.z, rhs.m_center.z))) {
                return false;
            }
            return true;
        }
    protected:
        vec3 m_center;
        float m_radius;
    };
}



