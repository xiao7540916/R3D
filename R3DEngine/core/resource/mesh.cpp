//
// Created by Administrator on 2022/9/10.
//

#include "mesh.h"
#include <resource/material.h>
#include <resource/render_state_manage.h>
#include <iostream>
namespace R3D {
    void Mesh::Render(Material *in_material) {
        in_material->RenderPrepare();
        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, m_indiceSize, GL_UNSIGNED_INT, nullptr);
    }
}