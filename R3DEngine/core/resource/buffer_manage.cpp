//
// Created by Administrator on 2022/9/17.
//

#include "buffer_manage.h"
#include <device/device.h>
#include <device/scene.h>
#include <effect/stochastic_ssr.h>
extern OptionConfig g_optionConfig;
namespace R3D {
    using std::cout;
    using std::endl;
    BufferManage::BufferManage() {
    }
    BufferManage::~BufferManage() {
    }
    BufferManage *BufferManage::m_bufferManage = nullptr;
    BufferManage *BufferManage::GetInstance() {
        if (m_bufferManage == nullptr) {
            m_bufferManage = NEW BufferManage();
        }
        return m_bufferManage;
    }
    void BufferManage::Init(Device *in_device) {
        m_device = in_device;
        glCreateBuffers(1, &m_uniBlockBaseBuffer);
        glNamedBufferData(m_uniBlockBaseBuffer, sizeof(UniformBlockBase),
                          nullptr, GL_DYNAMIC_DRAW);//DRAW代表会被用于GPU
        //模型数据
        glCreateBuffers(1, &m_uniBlockMeshBuffer);
        glNamedBufferData(m_uniBlockMeshBuffer, sizeof(UniformBlockMesh),
                          nullptr, GL_DYNAMIC_DRAW);
        //点光源数据
        glCreateBuffers(1, &m_pointLightBuffer);
        glNamedBufferData(m_pointLightBuffer, sizeof(PointLight) * POINT_LIGHT_COUNT,
                          nullptr, GL_DYNAMIC_DRAW);
        CreateTileLightBuffer(in_device->m_windowWidth, in_device->m_windowHeight, TILE_SIZE);
        //AO设置
        glCreateBuffers(1, &m_uniBlockAoCfgBuffer);
        glNamedBufferData(m_uniBlockAoCfgBuffer, sizeof(AOConfig),
                          nullptr, GL_DYNAMIC_DRAW);
        //CSM数据
        glCreateBuffers(1, &m_uniCSMBaseBuffer);
        glNamedBufferData(m_uniCSMBaseBuffer, sizeof(mat4),
                          nullptr, GL_DYNAMIC_DRAW);//DRAW代表会被用于GPU
        glCreateBuffers(1, &m_uniCSMHandleBuffer);
        glNamedBufferData(m_uniCSMHandleBuffer, sizeof(uint64_t) * m_device->m_cascadedShadowMap.m_csmLayerCount,
                          nullptr, GL_DYNAMIC_DRAW);//DRAW代表会被用于GPU
        glBindBuffer(GL_UNIFORM_BUFFER, m_uniCSMHandleBuffer);
        glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(GLuint64) * m_device->m_cascadedShadowMap.m_csmLayerCount,
                        m_device->m_cascadedShadowMap.m_shadowmapHandles.data());
        glUnmapBuffer(GL_UNIFORM_BUFFER);
        //模型数据
        glCreateBuffers(1, &m_uniCSMMeshBuffer);
        glNamedBufferData(m_uniCSMMeshBuffer, sizeof(mat4),
                          nullptr, GL_DYNAMIC_DRAW);
        //深度图bindless buffer
        {
            vector<GLuint64> depthMipHandles;
            depthMipHandles.resize(DEPTH_DOWN_LEVEL + 1);
            glBindTexture(GL_TEXTURE_2D, m_device->m_backHDRFBO.m_depthAttach);
            depthMipHandles[0] = glGetTextureHandleARB(m_device->m_backHDRFBO.m_depthAttach);
            glMakeTextureHandleResidentARB(depthMipHandles[0]);
            for (int i = 0;i < DEPTH_DOWN_LEVEL;++i) {
                glBindTexture(GL_TEXTURE_2D, StochasticSSR::GetInstance()->m_downDepthTex[i]);
                depthMipHandles[i + 1] = glGetTextureHandleARB(StochasticSSR::GetInstance()->m_downDepthTex[i]);
                glMakeTextureHandleResidentARB(depthMipHandles[i + 1]);
            }
            glCreateBuffers(1, &m_uniDepthMinMipBuffer);
            glNamedBufferData(m_uniDepthMinMipBuffer, sizeof(uint64_t) * depthMipHandles.size(),
                              nullptr, GL_DYNAMIC_DRAW);//DRAW代表会被用于GPU
            glBindBuffer(GL_UNIFORM_BUFFER, m_uniDepthMinMipBuffer);
            glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(GLuint64) * depthMipHandles.size(),
                            depthMipHandles.data());
            glUnmapBuffer(GL_UNIFORM_BUFFER);
        }
    }
    void BufferManage::Release() {
    }
    void BufferManage::UpdataUniBaseBuf(Scene &in_scene) {
        UniformBlockBase uniformBlockBase;
        Camera &camera = *m_device->m_camera;
        uniformBlockBase.view = camera.GetView();
        uniformBlockBase.proj = camera.GetProjection();
        uniformBlockBase.invproj = glm::inverse(camera.GetProjection());
        uniformBlockBase.viewproj = camera.GetProjection() * camera.GetView();
        uniformBlockBase.invviewproj = glm::inverse(camera.GetProjection() * camera.GetView());
        uniformBlockBase.camerapos = camera.GetPosition();
        uniformBlockBase.dirLights[0] = in_scene.m_dirLights[0];
        uniformBlockBase.dirLights[1] = in_scene.m_dirLights[1];
        uniformBlockBase.dirLights[2] = in_scene.m_dirLights[2];
        uniformBlockBase.dirLights[3] = in_scene.m_dirLights[3];
        uniformBlockBase.dirlightactivenum = in_scene.m_dirLightActiveNum;
        uniformBlockBase.pointlightactivenum = in_scene.m_pointLightActiveNum;
        uniformBlockBase.tilepointlightmax = in_scene.m_tilePointLightMax;
        uniformBlockBase.windowwidth = m_device->m_windowWidth;
        uniformBlockBase.windowheight = m_device->m_windowHeight;
        uniformBlockBase.workgroup_x =
                (m_device->m_windowWidth % TILE_SIZE) == 0 ? (m_device->m_windowWidth / TILE_SIZE) : (
                        m_device->m_windowWidth / TILE_SIZE + 1);
        uniformBlockBase.znear = camera.GetNearZ();
        uniformBlockBase.zfar = camera.GetFarZ();
        uniformBlockBase.depthbias = g_optionConfig.depthbias;
        uniformBlockBase.normalbias = g_optionConfig.normalbias;
        uniformBlockBase.hdrexp = g_optionConfig.hdrExp;
        uniformBlockBase.ambient = vec3(0.01);
        for (int i = 0;i < 6;++i) {
            uniformBlockBase.lightviewprojdata[i] = m_device->m_cascadedShadowMap.m_lightViewProjs[i];
        }
        uniformBlockBase.depthimagesize[0] = ivec4(m_device->m_windowWidth,m_device->m_windowHeight,0,0);
        ivec2 *downDepthTexSize = StochasticSSR::GetInstance()->m_downDepthTexSize.data();
        for (int i = 0;i < DEPTH_DOWN_LEVEL;++i) {
            uniformBlockBase.depthimagesize[i+1] = ivec4(downDepthTexSize[i].x,downDepthTexSize[i].y,0,0);
        }
        bool oncetime = true;
        static mat4 lastViewProj;
        if(oncetime){
            lastViewProj = camera.GetProjection() * camera.GetView();
            oncetime = false;
        }
        uniformBlockBase.lastviewproj = lastViewProj;
        glBindBufferBase(GL_UNIFORM_BUFFER, 0, m_uniBlockBaseBuffer);
        glNamedBufferSubData(m_uniBlockBaseBuffer, 0, sizeof(UniformBlockBase), &uniformBlockBase);
        glUnmapBuffer(GL_UNIFORM_BUFFER);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, m_pointLightBuffer);
        glNamedBufferSubData(m_pointLightBuffer, 0, sizeof(PointLight) * POINT_LIGHT_COUNT,
                             in_scene.m_pointLights.data());
        glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);
        lastViewProj = camera.GetProjection() * camera.GetView();
        //AO----------------------------------------------------------------------
        AOConfig aoConfig{};
        aoConfig.angleBias = g_optionConfig.angleBias;
        aoConfig.attenuation = g_optionConfig.attenuation;
        aoConfig.bfRang = g_optionConfig.bfRang;
        aoConfig.bfSpace = g_optionConfig.bfSpace;
        aoConfig.blurPass = g_optionConfig.blurPass;
        aoConfig.dirCount = g_optionConfig.dirCount;
        aoConfig.radiusScale = g_optionConfig.radiusScale;
        aoConfig.scaleAO = g_optionConfig.scaleAO;
        aoConfig.stepCount = g_optionConfig.stepCount;
        //新增
        //hbao uniforms
        aoConfig.projInfo = vec4(2.0f / camera.GetProjection()[0][0], 2.0f / camera.GetProjection()[1][1],
                                 -1.0f / camera.GetProjection()[0][0], -1.0f / camera.GetProjection()[1][1]);
        aoConfig.InvFullResolution = vec2(1.0f / float(m_device->m_windowWidth),
                                          1.0f / float(m_device->m_windowHeight));
        aoConfig.InvQuarterResolution = vec2(1.0f / float((m_device->m_windowWidth + 3) / 4),
                                             1.0f / float((m_device->m_windowHeight + 3) / 4));
        aoConfig.R = g_optionConfig.radiusScale;
        aoConfig.NegInvR2 = -1.0f / (g_optionConfig.radiusScale * g_optionConfig.radiusScale);
        aoConfig.RadiusToScreen =
                g_optionConfig.radiusScale * 0.5f * m_device->m_windowHeight / (tanf(camera.GetFovY() * 0.5f) * 2.0f);
        aoConfig.PowExponent = g_optionConfig.powExponent;
        aoConfig.NDotVBias = g_optionConfig.nDotVBias;
        aoConfig.AOMultiplier = 1.0f / (1.0f - aoConfig.NDotVBias);
        glBindBufferBase(GL_UNIFORM_BUFFER, 1, m_uniBlockAoCfgBuffer);
        glNamedBufferSubData(m_uniBlockAoCfgBuffer, 0, sizeof(AOConfig), &aoConfig);
        glUnmapBuffer(GL_UNIFORM_BUFFER);
    }
    void BufferManage::CreateTileLightBuffer(int in_windowwidth, int in_windowheight, int in_tilesize) {
        int workgroup_x = (in_windowwidth % TILE_SIZE) == 0 ? (in_windowwidth / TILE_SIZE) : (
                in_windowwidth / TILE_SIZE + 1);
        int workgroup_y = (in_windowheight % TILE_SIZE) == 0 ? (in_windowheight / TILE_SIZE) : (
                in_windowheight / TILE_SIZE + 1);
        //点光源数据
        glCreateBuffers(1, &m_tileLightsIdxBuffer);
        glNamedBufferData(m_tileLightsIdxBuffer, sizeof(uint32_t) * TILE_LIGHT_MAX * workgroup_x * workgroup_y,
                          nullptr, GL_STATIC_DRAW);
        glCreateBuffers(1, &m_tileLightsNumBuffer);
        glNamedBufferData(m_tileLightsNumBuffer, sizeof(uint32_t) * workgroup_x * workgroup_y,
                          nullptr, GL_DYNAMIC_DRAW);
    }
    void
    BufferManage::CreateTileClipBuffer(Camera &in_camera, int in_windowwidth, int in_windowheight, int in_tilesize) {
        int workgroup_x = (in_windowwidth % TILE_SIZE) == 0 ? (in_windowwidth / TILE_SIZE) : (
                in_windowwidth / TILE_SIZE + 1);
        int workgroup_y = (in_windowheight % TILE_SIZE) == 0 ? (in_windowheight / TILE_SIZE) : (
                in_windowheight / TILE_SIZE + 1);
        //tile截面
        glCreateBuffers(1, &m_tileClipPlaneBuffer);
        glNamedBufferData(m_tileClipPlaneBuffer, sizeof(mat4) * workgroup_x * workgroup_y,
                          nullptr, GL_STATIC_DRAW);
        vector<vec4> clipPlaneData;
        clipPlaneData.resize(4 * workgroup_x * workgroup_y);
        mat4 invproj = glm::inverse(in_camera.GetProjection());
        float zfar = in_camera.GetFarZ();
        for (int i = 0;i < workgroup_y;++i) {
            for (int j = 0;j < workgroup_x;++j) {
                vec2 tilepixelmin = vec2(in_tilesize * j, in_tilesize * i);
//                NDC空间vec3(x,y,z)中z为1时对应为zfar，ndc空间xyz*zfar得到裁剪空间。组成四维向量，经过投影矩阵逆变换，得到相机空间坐标
                vec3 clipmin = vec3((tilepixelmin / vec2(in_windowwidth, in_windowheight)) * 2.0f - vec2(1.0f), 1.0f) *
                               zfar;
                vec4 viewmin = invproj * (vec4(clipmin, clipmin.z));
                vec2 tilepixelmax = vec2(in_tilesize * (j + 1), in_tilesize * (i + 1));
                vec3 clipmax = vec3((tilepixelmax / vec2(in_windowwidth, in_windowheight)) * 2.0f - vec2(1.0f), 1.0f) *
                               zfar;
                vec4 viewmax = invproj * (vec4(clipmax, clipmax.z));
                clipPlaneData[4 * (j + i * workgroup_x) + 0] = vec4(
                        normalize(vec3(0.0, -zfar, -viewmax.y)), 0.0f);//上
                clipPlaneData[4 * (j + i * workgroup_x) + 1] = vec4(
                        normalize(vec3(0.0, zfar, viewmin.y)), 0.0f);;//下
                clipPlaneData[4 * (j + i * workgroup_x) + 2] = vec4(
                        normalize(vec3(zfar, 0.0, viewmin.x)), 0.0f);//左
                clipPlaneData[4 * (j + i * workgroup_x) + 3] = vec4(
                        normalize(vec3(-zfar, 0.0, -viewmax.x)), 0.0f);//右
            }
        }
        glNamedBufferSubData(m_tileClipPlaneBuffer, 0, sizeof(mat4) * workgroup_x * workgroup_y, clipPlaneData.data());
        glUnmapNamedBuffer(m_tileClipPlaneBuffer);
    }
}