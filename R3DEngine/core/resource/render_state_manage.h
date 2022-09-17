//
// Created by Administrator on 2022/9/17.
//

#pragma once
#include <glad/glad.h>
namespace R3D {
    class RenderStateManage {
    public:
        static RenderStateManage *GetInstance();
        GLuint m_actionShaderID = 0;//通过对比shader是否一致执行是否需要管线切换
        inline bool NeedChangeState(GLuint in_shaderID){
            if (m_actionShaderID == in_shaderID) {
                return false;
            } else {
                m_actionShaderID = in_shaderID;
                return true;
            }
        };
    private:
        static RenderStateManage *m_renderStateManage;
        RenderStateManage();
    };
}



