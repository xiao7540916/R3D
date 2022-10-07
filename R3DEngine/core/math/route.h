//
// Created by Administrator on 2022/9/4.
//

#ifndef OPENENGIN_ROUTE_H
#define OPENENGIN_ROUTE_H
#include <glm/glm.hpp>
#include <vector>
namespace R3D{
    using glm::vec3;
    using std::vector;
    enum PATH_TYPE {
        PATH_STRAIGHT,
        PATH_CURVED
    };
    struct Path {
        PATH_TYPE pathType;
        vec3 startPoint;//起始点
        vec3 endPoint;//终点
        vec3 controlPoint1;//控制点1
        vec3 controlPoint2;//控制点2
        float start;//开始的位置
        float length;//长度
        float CompLen() const;
        vec3 GetLenPoint(float lenin) const;//根据长度获取位置
        vec3 GetPercPoint(float percin) const;//根据百分比获取位置
    };
    class Route {
    public:
        void AddPath(Path &pathin);
        void InitRoute();
        vec3 GetLenPoint(float lenin) const;//根据长度获取位置
        vec3 GetPercPoint(float percin) const;//根据百分比获取位置
        vec3 GetLenNormal(float lenin) const;
        vec3 GetPercNormal(float percin) const;
        inline float GetRouteLen(){return m_routeLen;};
    private:
        vector<Path> m_paths;
        float m_routeLen;//长度
    };
}

#endif //OPENENGIN_ROUTE_H
