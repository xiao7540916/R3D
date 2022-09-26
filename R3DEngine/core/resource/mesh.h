//
// Created by Administrator on 2022/9/10.
//

#pragma once
#include <structdefine.h>
#include <vector>
#include <glad/glad.h>
#include <math/sphere.h>
namespace R3D {
    using std::vector;
    class Material;
    class Mesh {
    public:
        GLuint VAO;
        GLuint VBO;
        GLuint EBO;
        int m_indiceSize;
        int m_indeceStart = 0;
        VertexLayout m_vertexLayout;
        Sphere m_sphere;//模型空间的包围球
        void Render(Material *in_material);
    };
}



