//
// Created by Administrator on 2022/9/22.
//

#pragma once
#include <vector>
namespace R3D{
    class Object;
    using std::vector;
    class RenderList {
    public:
        vector<Object*> m_objectList;
        void Render();
        void RenderBndSphere();
        void RenderDepth();
        void Sort();
    };
}



