//
// Created by Administrator on 2022/9/20.
//
//六面体
#pragma once
#include <math/plane.h>
namespace R3D {
    //六面体，相对平面平行
    class Hexahedron {
    public:
        //一个平截锥体由六个面组成 顺序：上，下，左，右，近，远
        Plane m_planes[6];
        const Plane &GetPlane(int in_index) const {
            return m_planes[in_index];
        }
    };
}



