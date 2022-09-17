//
// Created by Administrator on 2022/9/10.
//

#pragma once
#include <structdefine.h>
#include <vector>
#include <glad/glad.h>
namespace R3D {
    using std::vector;
    class Material;
    class Mesh {
    public:
        GLuint VAO;
        GLuint VBO;
        GLuint EBO;
        int m_indiceSize;
        VertexLayout m_vertexLayout;
        void Remder();
        void SetMaterial(Material *in_material);

    private:
        Material *m_material;
    };
}



