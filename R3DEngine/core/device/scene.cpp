//
// Created by Administrator on 2022/9/25.
//

#include "scene.h"
#include <device/device.h>
#include <resource/object.h>
extern string CURRENT_SOURCE_DIR;
namespace R3D {
    void Scene::SetRoot(Object *in_root) {
        m_root = in_root;
    }
    void Scene::UpdataAnimate(float in_deltaTime, EventInfo &in_eventInfo) {
        for (auto item = m_dynamicObjects.begin();item != m_dynamicObjects.end();item++) {
            item->second->Updata(in_deltaTime, in_eventInfo);
        }
    }
    void Scene::ClearDynamic() {
        m_dynamicObjects.clear();
    }
    void Scene::GatherDynamic(Object *in_object) {
        if (in_object->GetDynamic()) {
            if (m_dynamicObjects.find(in_object->GetName()) == m_dynamicObjects.end()) {
                m_dynamicObjects.insert({in_object->GetName(), in_object});
            }
        }
        for (std::list<Object *>::iterator p = in_object->GetChildList().begin();
             p != in_object->GetChildList().end();++p) {
            GatherDynamic(*p);
        }
    }
    void Scene::DeleteDymaic(Object *in_object) {
        if (m_dynamicObjects.find(in_object->GetName()) != m_dynamicObjects.end()) {
            m_dynamicObjects.erase(in_object->GetName());
            for (std::list<Object *>::iterator p = in_object->GetChildList().begin();
                 p != in_object->GetChildList().end();++p) {
                DeleteDymaic(*p);
            }
        }
    }
    void Scene::Init(Device *in_device) {
        m_device = in_device;
        for (auto &m_dirLight : m_dirLights) {
            m_dirLight.direction = vec3(0, 1, 0);
            m_dirLight.strength = vec3(0);
        }
        m_pointLights.resize(POINT_LIGHT_COUNT);
        for (auto &m_pointLight : m_pointLights) {
            m_pointLight.position = vec3(0);
            m_pointLight.strength = vec3(0);
            m_pointLight.constant = 1;
            m_pointLight.linear = 1;
            m_pointLight.quadratic = 1;
        }
    }
    void Scene::UpdataTransBound() {
        m_root->UpdataSubSceneGraph(true);
        m_root->UpdataBoundSphere(m_root);
    }
    void Scene::MakeRenderList() {
        m_opaqueList.m_objectList.clear();
        ObjectCull(m_root);
    }
    void Scene::SortRenderList() {
        m_opaqueList.Sort();
    }
    void Scene::ObjectCull(Object *in_object) {
        if (CameraSphereTest(*m_device->m_camera, in_object->GetBoundingSphere())) {
            if (CameraSphereTest(*m_device->m_camera, in_object->GetSelfBoundingSphere())) {
                switch (in_object->GetMaterial()->m_mtrQueue) {
                    case MTRQUEUE_BACKGROUND:
                        break;
                    case MTRQUEUE_GEOMETRY:
                        m_opaqueList.m_objectList.push_back(in_object);
                        break;
                    case MTRQUEUE_GEOMETRYREF:
                        m_opaqueList.m_objectList.push_back(in_object);
                        break;
                    case MTRQUEUE_ALPHATEST:
                        break;
                    case MTRQUEUE_TRANSPARENT:
                        break;
                    case MTRQUEUE_OVERLAY:
                        break;
                }
            }
            for (std::list<Object *>::iterator p = in_object->GetChildList().begin();
                 p != in_object->GetChildList().end();++p) {
                ObjectCull(*p);
            }
        }
    }
    bool Scene::HexahedSphereTest(Hexahedron &in_hexahedron, Sphere &in_sphere) {
        return in_sphere.Intersects(in_hexahedron);
    }
    bool Scene::CameraSphereTest(const Camera &in_camera, const Sphere &in_sphere) {
        Sphere sphere_view;//相机空间下的包围球
        vec3 spherecenter_view = in_camera.GetView() * vec4(in_sphere.GetCenter(), 1.0f);
        sphere_view.SetCenter(spherecenter_view);
        sphere_view.SetRadius(in_sphere.GetRadius());
        return sphere_view.Intersects(in_camera.m_frustum);
    }
    void Scene::RenderLightShow() {
        static Mesh *lightmesh = MeshManage::GetInstance()->GetMesh("lightmesh");
        static Material *lightshowmaterial = MaterialManage::GetInstance()->GetMaterial("lightshow");
        lightshowmaterial->RenderPrepare();
        glBindVertexArray(lightmesh->VAO);
        glDrawElementsInstanced(GL_TRIANGLES, lightmesh->m_indiceSize, GL_UNSIGNED_INT,
                                (void *) (lightmesh->m_indeceStart * 4), m_pointLightActiveNum);
    }
    void Scene::RenderLightRadius() {
        static Mesh *lightradiusmesh = MeshManage::GetInstance()->GetMesh("lightradiusmesh");
        static Material *lightradiusmaterial = MaterialManage::GetInstance()->GetMaterial("lightradius");
        lightradiusmaterial->RenderPrepare();
        glBindVertexArray(lightradiusmesh->VAO);
        glDrawElementsInstanced(GL_TRIANGLES, lightradiusmesh->m_indiceSize, GL_UNSIGNED_INT,
                                (void *) (lightradiusmesh->m_indeceStart * 4), m_pointLightActiveNum);
    }
    void Scene::SetLightCount(int in_dir, int in_point, int in_tilepoint) {
        m_dirLightActiveNum = in_dir;
        m_pointLightActiveNum = in_point;
        m_tilePointLightMax = in_tilepoint;
    }
    void Scene::CullLight() {
        ShaderCache &shaderCache = Device::GetInstance()->m_shaderCache;
        shaderCache.GetShader("lightcull").use();
        glBindBufferBase(GL_UNIFORM_BUFFER, 0, BufferManage::GetInstance()->m_uniBlockBaseBuffer);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, BufferManage::GetInstance()->m_pointLightBuffer);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, BufferManage::GetInstance()->m_tileLightsIdxBuffer);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, BufferManage::GetInstance()->m_tileLightsNumBuffer);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 3, BufferManage::GetInstance()->m_tileClipPlaneBuffer);
        glBindTextureUnit(0, m_device->m_preDepthFBO.m_depthAttach);
        int workgroup_x = (m_device->m_windowWidth % TILE_SIZE) == 0 ? (m_device->m_windowWidth / TILE_SIZE) : (
                m_device->m_windowWidth / TILE_SIZE + 1);
        int workgroup_y = (m_device->m_windowHeight % TILE_SIZE) == 0 ? (m_device->m_windowHeight / TILE_SIZE) : (
                m_device->m_windowHeight / TILE_SIZE + 1);
        glDispatchCompute(workgroup_x, workgroup_y, 1);
        glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
    }
    void Scene::MakeAO() {
        static Mesh *screenbackmesh = MeshManage::GetInstance()->GetMesh("screenbackmesh");
        ShaderCache &shaderCache = Device::GetInstance()->m_shaderCache;
        shaderCache.GetShader("gtao").use();
        glBindBufferBase(GL_UNIFORM_BUFFER, 0, BufferManage::GetInstance()->m_uniBlockBaseBuffer);
        glBindBufferBase(GL_UNIFORM_BUFFER, 1, BufferManage::GetInstance()->m_uniBlockAoCfgBuffer);
        glBindTextureUnit(0, m_device->m_preDepthFBO.m_depthAttach);
        glBindTextureUnit(1, m_device->m_preDepthFBO.m_normalAttach);
        glBindTextureUnit(2, TextureManage::GetInstance()->GetTextureByUrl(
                "noiseTexture"));
        glBindVertexArray(screenbackmesh->VAO);
        glDrawElements(GL_TRIANGLES, screenbackmesh->m_indiceSize, GL_UNSIGNED_INT, nullptr);
    }
}