//
// Created by Administrator on 2022/9/17.
//

#include "render_state_manage.h"
namespace R3D {
    RenderStateManage::RenderStateManage() {
    }
    RenderStateManage *RenderStateManage::m_renderStateManage = nullptr;
    RenderStateManage *RenderStateManage::GetInstance() {
        if (m_renderStateManage == nullptr) {
            m_renderStateManage = new RenderStateManage();
        }
        return m_renderStateManage;
    }
}