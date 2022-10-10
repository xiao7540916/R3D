//
// Created by Administrator on 2022/10/7.
//

#include "cascaded_shadow_map.h"
#include <math/camera.h>
#include <device/scene.h>
#include <math/camera.h>
#include <resource/object.h>
#include <device/device.h>
extern OptionConfig optionConfig;
namespace R3D {
    void CascadedShadowMap::Init(CSMInfo &in_csmInfo) {
        //计算shadowmap分割比例
        switch (in_csmInfo.csmLayerCount) {
            case 1: {
                m_splitPoint = {0.0, 1.0};
                m_csmLayerCount = 1;
                break;
            }
            case 2: {
                m_splitPoint = {0.0, 0.3, 1.0};
                m_csmLayerCount = 2;
                break;
            }
            case 3: {
                m_splitPoint = {0.0, 0.15, 0.45, 1.0};
                m_csmLayerCount = 3;
                break;
            }
            case 4: {
                m_splitPoint = {0.0, 0.1, 0.3, 0.6, 1.0};
                m_csmLayerCount = 4;
                break;
            }
            case 5: {
                m_splitPoint = {0.0, 0.04, 0.12, 0.28, 0.4, 1.0};
                m_csmLayerCount = 5;
                break;
            }
            case 6: {
                m_splitPoint = {0.0, 0.02, 0.0625, 0.125, 0.25, 0.5, 1.0};
                m_csmLayerCount = 6;
                break;
            }
            default: {
                std::cout << "细分数目过多，限制细分数目为6" << std::endl;
                m_splitPoint = {0.0, 0.02, 0.0625, 0.125, 0.25, 0.5, 1.0};
                m_csmLayerCount = 6;
                break;
            }
        }
        m_shadowMapFBO.resize(m_csmLayerCount);
        m_CSMData.resize(m_csmLayerCount);
        m_lightViewProjs.resize(m_csmLayerCount * 2);
        m_viewconepointsW.resize(m_csmLayerCount);
        m_renderList.resize(m_csmLayerCount);
        m_shadowmapHandles.resize(m_csmLayerCount);
        //生成shadowmap
        for (int i = 0;i < m_csmLayerCount;++i) {
            m_shadowMapFBO[i].Init(in_csmInfo.shadowMapSize, in_csmInfo.shadowMapSize);
            glBindTexture(GL_TEXTURE_2D, m_shadowMapFBO[i].m_depthAttach);
            m_shadowmapHandles[i] = glGetTextureHandleARB(m_shadowMapFBO[i].m_depthAttach);
            glMakeTextureHandleResidentARB(m_shadowmapHandles[i]);
        }
        ShaderCache &shaderCache = Device::GetInstance()->m_shaderCache;
        m_shader = shaderCache.GetShader("shadowmap");
    }
    void CascadedShadowMap::Release() {
        for (int i = 0;i < m_csmLayerCount;++i) {
            m_shadowMapFBO[i].Release();
        }
    }
    void CascadedShadowMap::UpdataLightViewProj(Camera &in_camera, Scene &in_scene) {
        vec3 viewconepointsN[8] = {
                {-1, -1, -1},
                {1,  -1, -1},
                {-1, 1,  -1},
                {1,  1,  -1},
                {-1, -1, 1},
                {1,  -1, 1},
                {-1, 1,  1},
                {1,  1,  1}
        };
        vec3 lightdir = in_scene.m_dirLights[0].direction;
        //计算整个场景在光照空间下的包围盒，用以计算光照的near，far
        vec3 worldcanseelightpos;
        float worldcanseefar;
        {
            //场景包围盒的8个顶点
            vec3 worldPoints[8];
            AABB worldAABB;
            worldAABB = in_scene.m_aabb;
            worldPoints[0] = vec3(worldAABB.min.x, worldAABB.min.y, worldAABB.min.z);
            worldPoints[1] = vec3(worldAABB.min.x, worldAABB.min.y, worldAABB.max.z);
            worldPoints[2] = vec3(worldAABB.min.x, worldAABB.max.y, worldAABB.min.z);
            worldPoints[3] = vec3(worldAABB.min.x, worldAABB.max.y, worldAABB.max.z);
            worldPoints[4] = vec3(worldAABB.max.x, worldAABB.min.y, worldAABB.min.z);
            worldPoints[5] = vec3(worldAABB.max.x, worldAABB.min.y, worldAABB.max.z);
            worldPoints[6] = vec3(worldAABB.max.x, worldAABB.max.y, worldAABB.min.z);
            worldPoints[7] = vec3(worldAABB.max.x, worldAABB.max.y, worldAABB.max.z);
            //将8个顶点转换到灯光相机空间，计算包围盒，得到顶点，再转换到世界空间
            vec3 lightup = vec3(0, 1, 0);
            if (glm::dot(lightup, lightdir) > 0.999) {
                lightup = glm::normalize(vec3(0.000173645378, 1.0, 0.0003847586));
            }
            mat4 lightview = lookAt(-lightdir, vec3(0),
                                    lightup);
            mat4 lightviewinv = glm::inverse(lightview);
            vec3 lightspacepoints[8];//存储视锥体转换到灯光空间的坐标
            vec3 lightminpos = vec3(65536.0f);
            vec3 lightmaxpos = vec3(-65536.0f);
            for (int j = 0;j < 8;++j) {
                lightspacepoints[j] = lightview * vec4(worldPoints[j], 1.0f);
            }
            for (int j = 0;j < 8;++j) {
                lightminpos.x = glm::min(lightminpos.x, lightspacepoints[j].x);
                lightminpos.y = glm::min(lightminpos.y, lightspacepoints[j].y);
                lightminpos.z = glm::min(lightminpos.z, lightspacepoints[j].z);
                lightmaxpos.x = glm::max(lightmaxpos.x, lightspacepoints[j].x);
                lightmaxpos.y = glm::max(lightmaxpos.y, lightspacepoints[j].y);
                lightmaxpos.z = glm::max(lightmaxpos.z, lightspacepoints[j].z);
            }
            float lightplanewidth = lightmaxpos.x - lightminpos.x;
            float lightplaneheight = lightmaxpos.y - lightminpos.y;
            float lightplanedepth = lightmaxpos.z - lightminpos.z;
            vec3 lightnearmid = vec3(lightminpos.x + lightplanewidth * 0.5, lightminpos.y + lightplaneheight * 0.5,
                                     lightminpos.z);//求出相机空间近平面中心点坐标，转化到世界空间，就是灯光的位置
            vec3 worldnearmid = lightviewinv * vec4(lightnearmid, 1.0f);
            vec3 worldminpos = lightviewinv * vec4(lightminpos, 1.0f);
            vec3 worldmaxpos = lightviewinv * vec4(lightmaxpos, 1.0f);
            worldcanseefar = lightplanedepth;
            worldcanseelightpos = worldnearmid;
        }
        //计算平截头体顶点在世界空间下的坐标
        for (int i = 0;i < m_csmLayerCount;++i) {
            Camera camera;
            camera.SetLens(in_camera.GetFovY(), in_camera.GetAspect(),
                           in_camera.GetNearZ() + m_splitPoint[i] * (in_camera.GetFarZ() - in_camera.GetNearZ()),
                           in_camera.GetNearZ() + m_splitPoint[i + 1] * (in_camera.GetFarZ() - in_camera.GetNearZ()));
            vec3 campos = in_camera.GetPosition();
            vec3 camtarget = in_camera.GetForward() + in_camera.GetPosition();
            vec3 worldup(0, 1, 0);
            camera.SetPosition(campos);
            camera.LookAt(campos, camtarget, worldup);
            camera.UpdateViewMatrix();

            //计算摄像机近平面，远平面8各顶点位置
            mat4 view = camera.GetView();
            mat4 proj = camera.GetProjection();
            mat4 viewproj = proj * view;
            mat4 invviewproj = glm::inverse(viewproj);
            float vieww[8] = {
                    camera.GetNearZ(),
                    camera.GetNearZ(),
                    camera.GetNearZ(),
                    camera.GetNearZ(),
                    camera.GetFarZ(),
                    camera.GetFarZ(),
                    camera.GetFarZ(),
                    camera.GetFarZ()
            };
            for (int j = 0;j < 8;++j) {
                m_viewconepointsW[i][j] = invviewproj * vec4(viewconepointsN[j] * vieww[j], vieww[j]);
            }
        }
        //计算光源空间它们的包围盒
        for (int i = 0;i < m_csmLayerCount;++i) {
            //计算视锥平截头体的最大顶点距离作为灯光视锥长宽
            float neartofardstmax = glm::distance(m_viewconepointsW[i][0], m_viewconepointsW[i][7]);
            float fartofatdstmax = glm::distance(m_viewconepointsW[i][4], m_viewconepointsW[i][7]);
            float lightplanw = glm::max(neartofardstmax, fartofatdstmax);

            //将8个顶点转换到灯光相机空间，计算包围盒，得到顶点，再转换到世界空间
            vec3 lightup = vec3(0, 1, 0);
            if (glm::dot(lightup, lightdir) > 0.999) {
                lightup = glm::normalize(vec3(0.000173645378, 1.0, 0.0003847586));
            }
            mat4 lightview = lookAt(-lightdir, vec3(0),
                                    lightup);
            mat4 lightviewinv = glm::inverse(lightview);
            vec3 lightspacepoints[8];//存储视锥体转换到灯光空间的坐标
            vec3 lightminpos = vec3(65536.0f);
            vec3 lightmaxpos = vec3(-65536.0f);
            for (int j = 0;j < 8;++j) {
                lightspacepoints[j] = lightview * vec4(m_viewconepointsW[i][j], 1.0f);
            }
            for (int j = 0;j < 8;++j) {
                lightminpos.x = glm::min(lightminpos.x, lightspacepoints[j].x);
                lightminpos.y = glm::min(lightminpos.y, lightspacepoints[j].y);
                lightminpos.z = glm::min(lightminpos.z, lightspacepoints[j].z);
                lightmaxpos.x = glm::max(lightmaxpos.x, lightspacepoints[j].x);
                lightmaxpos.y = glm::max(lightmaxpos.y, lightspacepoints[j].y);
                lightmaxpos.z = glm::max(lightmaxpos.z, lightspacepoints[j].z);
            }
            float lightplanewidth = lightmaxpos.x - lightminpos.x;
            float lightplaneheight = lightmaxpos.y - lightminpos.y;
            float lightplanedepth = lightmaxpos.z - lightminpos.z;
            vec3 lightnearmid = vec3(lightminpos.x + lightplanewidth * 0.5, lightminpos.y + lightplaneheight * 0.5,
                                     lightminpos.z);//求出相机空间近平面中心点坐标，转化到世界空间，就是灯光的位置
            //限制灯光空间位置为每个像素对应世界空间长度的整数倍，减小阴影抖动
            float worldUnitsPerPixel = lightplanw / 1024.0f;
            vec3 lightposuint = lightnearmid / worldUnitsPerPixel;
            lightnearmid = vec3(float(int(lightposuint.x)) * worldUnitsPerPixel,
                                float(int(lightposuint.y)) * worldUnitsPerPixel,
                                float(int(lightposuint.z)) * worldUnitsPerPixel);
            vec3 worldnearmid = lightviewinv * vec4(lightnearmid, 1.0f);
            m_CSMData[i].orthoDate.left = -0.5f * lightplanw;
            m_CSMData[i].orthoDate.right = 0.5f * lightplanw;
            m_CSMData[i].orthoDate.bottom = -0.5f * lightplanw;
            m_CSMData[i].orthoDate.top = 0.5f * lightplanw;
            m_CSMData[i].orthoDate.zNear = 0.0f;
            vec3 lightpos = worldnearmid + optionConfig.lightPosOffset * lightdir;
            m_CSMData[i].orthoDate.zFar = lightplanedepth + optionConfig.lightPosOffset;
            m_CSMData[i].orthoDate.target = lightpos - lightdir;
            m_CSMData[i].orthoDate.up = lightup;
            m_CSMData[i].orthoDate.pos = lightpos;
            mat4 shadowprojection = Camera::GetOrthoProjectionOpengl(m_CSMData[i].orthoDate);
            mat4 shadowview = lookAt(m_CSMData[i].orthoDate.pos,
                                     m_CSMData[i].orthoDate.target,
                                     m_CSMData[i].orthoDate.up);
            m_CSMData[i].viewproj = shadowprojection * shadowview;
        }
        for (int i = 0;i < m_csmLayerCount;++i) {
            m_lightViewProjs[i] = m_CSMData[i].viewproj;
            m_lightViewProjs[i + m_csmLayerCount] = glm::inverse(m_CSMData[i].viewproj);
        }
        OrthoDate orthoDate;
        orthoDate.pos = lightdir * 12.0f;
        orthoDate.target = vec3(0);
        orthoDate.up = vec3(0, 1, 0);
        orthoDate.left = -16;
        orthoDate.right = 16;
        orthoDate.bottom = -16;
        orthoDate.top = 16;
        orthoDate.zNear = 0.0f;
        orthoDate.zFar = 40.0;
        mat4 proj = Camera::GetOrthoProjectionOpengl(orthoDate);
        mat4 view = lookAt(orthoDate.pos,
                           orthoDate.target,
                           orthoDate.up);
        m_lightViewProjs[0] = proj * view;
        m_lightViewProjs[3] = glm::inverse(m_lightViewProjs[0]);
    }
    void CascadedShadowMap::PrepareRenderData(Camera &in_camera, Scene &in_scene) {
        UpdataLightViewProj(in_camera, in_scene);
        MakeRenderList(in_scene);
    }
    void CascadedShadowMap::MakeRenderList(Scene &in_scene) {
        for (int i = 0;i < m_csmLayerCount;++i) {
            m_renderList[i].m_objectList.clear();
            OpaqueObjectCull(in_scene.GetRoot(), m_renderList[i]);
        }
    }
    void CascadedShadowMap::OpaqueObjectCull(Object *in_object, RenderList &in_renderList) {
        switch (in_object->GetMaterial()->m_mtrQueue) {
            case MTRQUEUE_BACKGROUND:
                break;
            case MTRQUEUE_GEOMETRY:
                in_renderList.m_objectList.push_back(in_object);
                break;
            case MTRQUEUE_GEOMETRYREF:
                in_renderList.m_objectList.push_back(in_object);
                break;
            case MTRQUEUE_ALPHATEST:
                break;
            case MTRQUEUE_TRANSPARENT:
                break;
            case MTRQUEUE_OVERLAY:
                break;
        }
        for (std::list<Object *>::iterator p = in_object->GetChildList().begin();
             p != in_object->GetChildList().end();++p) {
            OpaqueObjectCull(*p, in_renderList);
        }
    }
    void CascadedShadowMap::MakeShadowMap(Scene &in_scene) {
        for (int i = 0;i < m_csmLayerCount;++i) {
            glBindFramebuffer(GL_FRAMEBUFFER, m_shadowMapFBO[i].m_frameBuffer);
            glClear(GL_DEPTH_BUFFER_BIT);
            glBindBufferBase(GL_UNIFORM_BUFFER, 0, BufferManage::GetInstance()->m_uniCSMBaseBuffer);
            glNamedBufferSubData(BufferManage::GetInstance()->m_uniCSMBaseBuffer, 0, sizeof(mat4),
                                 &m_lightViewProjs[i]);
            glUnmapBuffer(GL_UNIFORM_BUFFER);
            m_renderList[i].RenderShadowMap();
        }
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }
}