//
// Created by Administrator on 2022/9/20.
//
//平面
#pragma once
#include <structdefine.h>
/** 3D空间中的平面.
	@remarks
		3D空间中的平面方程如下所示
		Ax + By + Cz + D = 0
	@par
		其中Vector3（A,B,C）是平面的法向，D是原点到平面的距离
*/
namespace R3D{
    class Plane {
    public:
        vec3 m_normal;//平面法线
        float m_d;//平面到原点的有向距离,原点在平面法向方向，m_d为正，否则为负
        Plane();
        void init(const Plane &rhs);//拷贝构造
        void init(const vec3 &in_normal, float in_constant);//数值构造
        void init(const vec3 &in_normal, const vec3 &in_point);//法线，平面上一点
        void init(const vec3 &in_point0, const vec3 &in_point1, const vec3 &in_point2);//三个平面上的点

        /** "positive side" 表示与平面法向同向的半个空间。
		"negative side" 表示与平面法向反向的半个空间
		"no side" 在平面上*/
        enum Side {
            NO_SIDE,
            POSITIVE_SIDE,
            NEGATIVE_SIDE
        };
        //判定rkPoint在平面的那一侧，对于平面的侧定义了正，负，无，分别表示与法向量同向，反向，在平面上
        Side GetSide(const vec3 &in_point) const;
        //伪距离，如果在面的正面则返回正值，反面则返回负值，0 说明在面上
        float GetDistance(const vec3 &in_point) const;
        //重载运算符==,判断两个平面是否相等
        bool operator==(const Plane &rhs) const {return (rhs.m_d == m_d && rhs.m_normal == m_normal);}
    };
}



