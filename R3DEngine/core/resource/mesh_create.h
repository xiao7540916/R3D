//
// Created by Administrator on 2022/9/16.
//

#pragma once
#include <structdefine.h>
namespace R3D {
    class Mesh;
    class MeshCreate{
    public:
        static void CreateBox(Mesh &in_mesh,float in_width, float in_height, float in_depth,VertexLayout in_vertexLayout);
    };

}



