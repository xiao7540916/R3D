//
// Created by Administrator on 2022/9/10.
//

#include "mesh.h"
#include <resource/material.h>
#include <resource/render_state_manage.h>
#include <iostream>
namespace R3D {
    void Mesh::Remder() {
        if (RenderStateManage::GetInstance()->NeedChangeState(m_material->m_shader.ID)) {
            glEnable(GL_DEPTH_TEST);
            glDepthFunc(GL_LESS);
        }
        m_material->m_shader.use();
        m_material->BindResource();
        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, m_indiceSize, GL_UNSIGNED_INT, nullptr);
    }
    void Mesh::SetMaterial(Material *in_material) {
        m_material = in_material;
    }
}