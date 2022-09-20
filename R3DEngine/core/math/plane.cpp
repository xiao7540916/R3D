//
// Created by Administrator on 2022/9/20.
//

#include "plane.h"
namespace R3D {
    Plane::Plane() : m_normal(vec3(0.0f, 1.0f, 0.0f)), m_d(0.0f) {
    }
    void Plane::init(const Plane &rhs) {
        m_normal = rhs.m_normal;
        m_d = rhs.m_d;
    }
    void Plane::init(const vec3 &in_normal, float in_constant) {
        m_normal = in_normal;
        m_d = in_constant;
    }
    void Plane::init(const vec3 &in_normal, const vec3 &in_point) {
        m_normal = in_normal;
        m_d = -glm::dot(m_normal, in_point);
    }
    void Plane::init(const vec3 &in_point0, const vec3 &in_point1, const vec3 &in_point2) {
        vec3 edge1 = in_point1 - in_point0;
        vec3 edge2 = in_point2 - in_point0;
        m_normal = glm::normalize(glm::cross(edge1, edge2));
        m_d = -glm::dot(m_normal, in_point0);
    }
    Plane::Side Plane::GetSide(const vec3 &in_point) const {
        float distance = GetDistance(in_point);
        if (distance < 0.0f) {
            return Plane::NEGATIVE_SIDE;
        }
        if (distance > 0.0f) {
            return Plane::POSITIVE_SIDE;
        }
        return Plane::NO_SIDE;
    }
    float Plane::GetDistance(const vec3 &in_point) const {
        return glm::dot(m_normal, in_point) + m_d;
    }
}