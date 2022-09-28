//
// Created by Administrator on 2022/9/20.
//

#include "object.h"
#include <glm/gtx/transform.hpp>
#include <resource/mesh.h>
#include <resource/material.h>
#include <resource/render_state_manage.h>
#include <resource/buffer_manage.h>
#include <shader/glsldefine.h>
#include <resource/mesh_manage.h>
namespace R3D {
    Object::Object(const string &in_name, Object *in_father, const vec3 &in_offsetFromParent, float in_radianX,
                   float in_radianY, float in_radianZ, bool in_visible) :
            m_dirtyBB(true), m_dirty(true), m_name(in_name), m_visible(in_visible),
            m_scale(1.0f), m_mesh(nullptr), m_father(nullptr) {
        //默认包围球材质
        m_bndSphMaterial = MaterialManage::GetInstance()->GetMaterial("green");
        if (!in_father) {
            m_transformation = mat4(1);
            m_positionRelative = vec3(0.0f);
            m_rotateRelative = mat3(1.0f);
            return;
        }
        mat4 I(1.0f);
        I = glm::rotate(I, in_radianY, vec3(0, 1, 0));
        I = glm::rotate(I, in_radianX, vec3(1, 0, 0));
        I = glm::rotate(I, in_radianZ, vec3(0, 0, 1));
        m_positionRelative = in_offsetFromParent;
        m_rotateRelative = mat3(I);
        in_father->UpdataTransformation();
        //待总体更新时进行更新m_finalTransformMatrix
        m_finalTransformMatrix = m_transformation = in_father->m_transformation * GetTransformMatrixRelative();
        Attach(in_father);
        m_selfBoundingSphere.SetRadius(0.0f);
    }
    const mat3 &Object::GetRotationMatrix() const {
        static mat3 rot;
        rot = mat3(m_transformation);
        return rot;
    }
    const mat4 &Object::GetTransformMatrixRelative() const {
        static mat4 m;
        //旋转
        m[0][0] = m_rotateRelative[0][0];
        m[0][1] = m_rotateRelative[0][1];
        m[0][2] = m_rotateRelative[0][2];
        m[1][0] = m_rotateRelative[1][0];
        m[1][1] = m_rotateRelative[1][1];
        m[1][2] = m_rotateRelative[1][2];
        m[2][0] = m_rotateRelative[2][0];
        m[2][1] = m_rotateRelative[2][1];
        m[2][2] = m_rotateRelative[2][2];
        //平移
        m[3][0] = m_positionRelative.x;
        m[3][1] = m_positionRelative.y;
        m[3][2] = m_positionRelative.z;
        //整体缩放
        m[3][3] = 1.0f;
        //设定4*4矩阵的投影变换的值
        m[0][3] = 0;
        m[1][3] = 0;
        m[2][3] = 0;
        return m;
    }
    Object *Object::CheckHasChild(const string &childName) const {
        std::list<Object *>::const_iterator p;
        for (p = m_chileList.begin();p != m_chileList.end();++p) {
            if ((*p)->GetName() == childName) {
                return (*p);
            }
        }
        Object *r = nullptr;
        //递归寻找子节点的子节点是否满足需求
        for (p = m_chileList.begin();p != m_chileList.end();++p) {
            r = (*p)->CheckHasChild(childName);
            if (r != nullptr) {
                return r;
            }
        }
        return nullptr;
    }
    void Object::Move(const vec3 &in_offset) {
        if (ZERO(in_offset.x) && ZERO(in_offset.y) && ZERO(in_offset.z)) {
            return;
        }
/*        static mat3 mrv;
        mrv = mat3(1);
        if (m_father) {
            mrv = mat3(m_father->m_transformation);
        }
        mrv = glm::transpose(mrv);
        m_positionRelative +=  in_offset*mrv;*/
        m_positionRelative += in_offset;
        m_dirty = true;
        m_dirtyBB = true;
    }
    void Object::MoveTo(const vec3 &in_newPos) {
//        PrintVec3(in_newPos - GetPosition());
        Move(in_newPos - GetPositionRelative());
    }
    void Object::RotationPitch(float in_radian) {
        if (EqualFloat(in_radian, 0.0f)) {
            return;
        }
        mat4 I(1.0f);
        I = glm::rotate(I, in_radian, vec3(1, 0, 0));
        m_rotateRelative = mat3(I);
        m_dirty = true;
        m_dirtyBB = true;
    }
    void Object::RotationYaw(float in_radian) {
        if (EqualFloat(in_radian, 0.0f)) {
            return;
        }
        mat4 I(1.0f);
        I = glm::rotate(I, in_radian, vec3(0, 1, 0));
        m_rotateRelative = mat3(I);
        m_dirty = true;
        m_dirtyBB = true;
    }
    void Object::RotationRool(float in_radian) {
        if (EqualFloat(in_radian, 0.0f)) {
            return;
        }
        mat4 I(1.0f);
        I = glm::rotate(I, in_radian, vec3(0, 0, 1));
        m_rotateRelative = mat3(I);
        m_dirty = true;
        m_dirtyBB = true;
    }
    void Object::RotationPitchYawRool(float in_radianX, float in_radianY, float in_radianZ) {
        static mat4 I;
        I = mat4(1.0f);
        I = glm::rotate(I, in_radianY, vec3(0, 1, 0));
        I = glm::rotate(I, in_radianX, vec3(1, 0, 0));
        I = glm::rotate(I, in_radianZ, vec3(0, 0, 1));
        PrintMat4(I);
        m_rotateRelative = mat3(I);
        m_dirty = true;
        m_dirtyBB = true;
    }
    void Object::SetDirectiveRelative(const vec3 &in_dir, const vec3 &in_up) {
        m_rotateRelative = mat3(glm::lookAt(vec3(0), in_dir, in_up));
        m_dirty = true;
        m_dirtyBB = true;
    }
    void Object::Scale(float in_scale) {
        m_scale *= in_scale;
        m_dirty = true;
        m_dirtyBB = true;
        for (std::list<Object *>::iterator ptr = m_chileList.begin();
             ptr != m_chileList.end();++ptr) {
            //相对距离缩放
            (*ptr)->m_positionRelative *= in_scale;
            //递归调用
            (*ptr)->Scale(in_scale);
        }
    }
    const mat4 &Object::UpdataTransformation() {
        m_transformation = GetTransformMatrixRelative();//获取相对变换矩阵
        Object *pNode = m_father;
        if (pNode) {
//            std::cout << pNode->m_name << std::endl;
        }

//    递归乘以父节点变换矩阵
        while (pNode) {
            m_transformation = m_transformation * pNode->GetTransformMatrixRelative();
            pNode = pNode->m_father;
        }
        m_finalTransformMatrix = glm::scale(m_transformation, vec3(m_scale));
        m_dirty = false;
        return m_transformation;
    }
    void Object::UpdataSubSceneGraph(bool in_compusive) {
        bool dirty = m_dirty || in_compusive;
        if (dirty) {
            m_transformation = GetTransformMatrixRelative();
            if (m_father) {
                m_transformation = m_father->m_transformation * m_transformation;
            }
            m_finalTransformMatrix = glm::scale(m_transformation, glm::vec3(m_scale));
            m_dirty = false;
            m_dirtyBB = true;//更新了变换矩阵，同时需要将包围球标记为需要更新
        }
        //更新儿子(本节点不脏，子节点可能脏)
        for (std::list<Object *>::iterator ptr = m_chileList.begin();ptr != m_chileList.end();++ptr) {
            (*ptr)->UpdataSubSceneGraph(dirty);
        }
    }
    void Object::UpdateBoundingSphere() {
        if (!m_dirtyBB) {
            return;
        }
        bool ifSelfBBChanged = false;
        //自身包围球更新
        if (m_mesh) {
            vec3 sphereCenter = GetFinalTransformMatrix() * vec4(m_mesh->m_sphere.GetCenter(), 1.0f);
            m_selfBoundingSphere.SetCenter(sphereCenter);
            m_selfBoundingSphere.SetRadius(m_mesh->m_sphere.GetRadius() * m_scale);
        } else {
            m_selfBoundingSphere.SetCenter(GetPosition());
            m_selfBoundingSphere.SetRadius(0.0f);
        }
        Sphere boundingSphere = m_selfBoundingSphere;
        //如果子节点包围球改变，或该节点在外部被改动，则该节点包围球需要更新
        for (std::list<Object *>::iterator p = m_chileList.begin();p != m_chileList.end();++p) {
            //父包围球包含子包围球，则不需要更新包围球
            if (boundingSphere.ContainSphere((*p)->m_boundingSphere)) {
                continue;
            }
            //子包围球包含父包围球，则将父包围球更新为子包围球
            if (boundingSphere.InSphere((*p)->m_boundingSphere)) {
                boundingSphere.SetCenter((*p)->m_boundingSphere.GetCenter());
                boundingSphere.SetRadius((*p)->m_boundingSphere.GetRadius());
            } else {
                //父包围球与子包围球相交或相离，不存在一方包含另一方
                vec3 thistochild = (*p)->m_boundingSphere.GetCenter() - boundingSphere.GetCenter();
                vec3 thistochilddir = glm::normalize(thistochild);
                float d = glm::length(thistochild);
                //注意，对于本体此处使用的是缓存self包围球的boundingSphere，而非m_boundingSphere,
                //因为此时m_boundingSphere并未更新，可能获取不到最新的数据
                boundingSphere.SetCenter(boundingSphere.GetCenter() +
                                         thistochilddir *
                                         (d - boundingSphere.GetRadius() + (*p)->m_boundingSphere.GetRadius()) * 0.5f);
                boundingSphere.SetRadius((d + boundingSphere.GetRadius() + (*p)->m_boundingSphere.GetRadius()) * 0.5f);
            }
        }
        //如果得到的包围球和原包围球不同
        if (!(boundingSphere == m_boundingSphere)) {
            ifSelfBBChanged = true;
            m_boundingSphere = boundingSphere;
            if (m_father) {
                m_father->m_dirtyBB = true;
            }
        }
        m_dirtyBB = false;
    }
    void Object::UpdataBoundSphere(Object *in_root) {
        //后序遍历保证子节点在父节点更新前得到更新
        for (std::list<Object *>::iterator p = in_root->GetChildList().begin();
             p != in_root->GetChildList().end();++p) {
            UpdataBoundSphere(*p);
        }
        in_root->UpdateBoundingSphere();
    }
    bool Object::Attach(Object *in_father) {
        if (in_father == m_father) {
            return false;
        }
        //如果存在父节点，先将其从父节点移除
        if (m_father) {
            m_father->Detach(m_name);
        }
        m_father = in_father;//设置新的父节点
        m_father->UpdataTransformation();//更新父节点变换矩阵
        //更新子节点相对变换矩阵(平移，旋转)
        mat4 mChildRelative = m_transformation * glm::inverse(m_father->m_transformation);
        m_rotateRelative = mat3(mChildRelative);
        m_positionRelative.x = mChildRelative[3][0];
        m_positionRelative.y = mChildRelative[3][1];
        m_positionRelative.z = mChildRelative[3][2];
        //将其加入父节点的子节点列表
        m_father->m_chileList.push_back(this);
        if (!m_boundingSphere.InSphere(m_father->m_boundingSphere)) {
            m_father->m_dirtyBB = true;
        }
        return true;
    }
    bool Object::AddAttach(Object *in_child) {
        if (in_child == nullptr) {
            return false;
        }
        return in_child->Attach(this);
    }
    bool Object::Detach(const string &in_name) {
        std::list<Object *>::iterator p;
        for (p = m_chileList.begin();p != m_chileList.end();++p) {
            if ((*p)->GetName() == in_name) {
                (*p)->UpdataTransformation();//更新儿子世界变换矩阵
                (*p)->m_rotateRelative = (*p)->GetRotationMatrix();//将世界空间旋转，世界空间位置应用于子节点以便它完成卸载
                (*p)->m_positionRelative = (*p)->GetPosition();
                (*p)->m_father = nullptr;
                m_chileList.erase(p);
                m_dirtyBB = true;
                return true;
            }
        }
        return false;
    }
    bool Object::DetachAll() {
        m_dirtyBB = true;
        std::list<Object *>::iterator p;
        for (p = m_chileList.begin();p != m_chileList.end();++p) {
            delete (*p);
            *p = nullptr;
        }
        m_chileList.clear();
        return true;
    }
    bool Object::SetMesh(Mesh *in_mesh) {
        m_mesh = in_mesh;
        m_selfBoundingSphere = in_mesh->m_sphere;
        m_boundingSphere = in_mesh->m_sphere;
        return true;
    }
    void Object::SetMaterial(Material *in_material) {
        m_material = in_material;
    }
    void Object::Render() {
        //更新uniform缓冲
        static UniformBlockMesh uniformBlockMesh;
        uniformBlockMesh.model = GetFinalTransformMatrix();
        uniformBlockMesh.invmodelt = glm::transpose(glm::inverse(GetFinalTransformMatrix()));
        uniformBlockMesh.uvoffset = m_uvoffset;
        uniformBlockMesh.uvscale = m_uvscale;
        glBindBufferBase(GL_UNIFORM_BUFFER, 1, BufferManage::GetInstance()->m_uniBlockMeshBuffer);
        glNamedBufferSubData(BufferManage::GetInstance()->m_uniBlockMeshBuffer, 0, sizeof(UniformBlockMesh),
                             &uniformBlockMesh);
        glUnmapBuffer(GL_UNIFORM_BUFFER);
        m_mesh->Render(m_material);
    }
    void Object::RenderBndSphere() {
        static UniformBlockMesh uniformBlockMesh;
        if (m_bndSphMaterial) {
            mat4 I(1.0f);
            I = glm::translate(I, m_boundingSphere.GetCenter());
            I = glm::scale(I, vec3(m_boundingSphere.GetRadius()));
            uniformBlockMesh.model = I;
            glBindBufferBase(GL_UNIFORM_BUFFER, 1, BufferManage::GetInstance()->m_uniBlockMeshBuffer);
            glNamedBufferSubData(BufferManage::GetInstance()->m_uniBlockMeshBuffer, 0, sizeof(UniformBlockMesh),
                                 &uniformBlockMesh);
            glUnmapBuffer(GL_UNIFORM_BUFFER);
            MeshManage::GetInstance()->m_nameToMesh["geospheremesh"]->Render(m_bndSphMaterial);
        } else {
            std::cout << "no boundingsphere material" << std::endl;
        }
    }
    void Object::RenderSlfBndSphere() {
        static UniformBlockMesh uniformBlockMesh;
        if (m_bndSphMaterial) {
            mat4 I(1.0f);
            I = glm::translate(I, m_selfBoundingSphere.GetCenter());
            I = glm::scale(I, vec3(m_selfBoundingSphere.GetRadius()));
            uniformBlockMesh.model = I;
            glBindBufferBase(GL_UNIFORM_BUFFER, 1, BufferManage::GetInstance()->m_uniBlockMeshBuffer);
            glNamedBufferSubData(BufferManage::GetInstance()->m_uniBlockMeshBuffer, 0, sizeof(UniformBlockMesh),
                                 &uniformBlockMesh);
            glUnmapBuffer(GL_UNIFORM_BUFFER);
            MeshManage::GetInstance()->m_nameToMesh["geospheremesh"]->Render(m_bndSphMaterial);
        } else {
            std::cout << "no boundingsphere material" << std::endl;
        }
    }
    void Object::SetUvConfig(const vec2 &in_uvoffset, const vec2 &in_uvscale) {
        m_uvoffset = in_uvoffset;
        m_uvscale = in_uvscale;
    }
    void Object::RenderDepth() {
        static Material *depthMaterial = MaterialManage::GetInstance()->GetMaterial("depth");
        //更新uniform缓冲
        static UniformBlockMesh uniformBlockMesh;
        uniformBlockMesh.model = GetFinalTransformMatrix();
        uniformBlockMesh.invmodelt = glm::transpose(glm::inverse(GetFinalTransformMatrix()));
        uniformBlockMesh.uvoffset = m_uvoffset;
        uniformBlockMesh.uvscale = m_uvscale;
        glBindBufferBase(GL_UNIFORM_BUFFER, 1, BufferManage::GetInstance()->m_uniBlockMeshBuffer);
        glNamedBufferSubData(BufferManage::GetInstance()->m_uniBlockMeshBuffer, 0, sizeof(UniformBlockMesh),
                             &uniformBlockMesh);
        glUnmapBuffer(GL_UNIFORM_BUFFER);
        m_mesh->Render(depthMaterial);
    }
    void Object::Updata(float in_deltaTime, EventInfo &in_eventInfo) {
        if (in_eventInfo.type == EVENT_NONE) {
            if (m_acctionfunc) {
                m_acctionfunc();
            };
        }
    }
    void Object::SetActionFunc(const std::function<void()> &in_function) {
        SetDynamic(true);
        m_acctionfunc = in_function;
    }
}