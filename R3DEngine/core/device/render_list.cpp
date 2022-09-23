//
// Created by Administrator on 2022/9/22.
//

#include "render_list.h"
#include <resource/object.h>
#include <resource/material.h>
#include <algorithm>
namespace R3D {
    bool cmp(Object *a, Object *b) {
        if (a->GetMaterial()->m_mtrQueue == b->GetMaterial()->m_mtrQueue) {
            return (a->GetMaterial()->m_shader.ID) < (b->GetMaterial()->m_shader.ID);
        } else {
            return (a->GetMaterial()->m_mtrQueue) < (b->GetMaterial()->m_mtrQueue);
        }
    }
    void RenderList::Render() {
        uint32_t objectCount = m_objectList.size();
        for (int i = 0;i < objectCount;++i) {
            m_objectList[i]->Render();
        }
    }
    void RenderList::RenderBndSphere() {
        uint32_t objectCount = m_objectList.size();
        for (int i = 0;i < objectCount;++i) {
            m_objectList[i]->RenderBndSphere();
        }
    }
    void RenderList::Sort() {
        std::sort(m_objectList.begin(), m_objectList.end(), cmp);
    }
    void RenderList::RenderDepth() {
        glClear(GL_DEPTH_BUFFER_BIT);
        glEnable(GL_DEPTH_TEST);
        uint32_t objectCount = m_objectList.size();
        for (int i = 0;i < objectCount;++i) {
            m_objectList[i]->RenderDepth();
        }
    }
}
