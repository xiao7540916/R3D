//
// Created by Administrator on 2022/9/20.
//

#pragma once
#include <structdefine.h>
#include <string>
#include <math/sphere.h>
#include <functional>
namespace R3D {
    using std::string;
    class Mesh;
    class Material;
    class Object {
    public:
        Material *m_bndSphMaterial;
        std::function<void()> m_acctionfunc = nullptr;
    protected:
        string m_name;//节点名字
        bool m_visible;//是否可见
        bool m_dynamic = false;
        Mesh *m_mesh;
        Material *m_material;
        Object *m_father;//父节点指针
        std::list<Object *> m_chileList;//子节点链表
        vec2 m_uvoffset = vec2(0);
        vec2 m_uvscale = vec2(1);
        vec3 m_positionRelative;//相对父节点位置
        mat3 m_rotateRelative;//相对父节点的旋转矩阵
        //采用绝对缩放的原因是比如一辆汽车和车上的人都缩小了，如果采用相对缩放，车上的人下车了，相对缩放就没有了，人就会突然变大
        float m_scale;//模型绝对缩放比例，不影响子节点
        //不存储缩放同样是因为采用绝对缩放，在得到变换矩阵后右乘缩放矩阵
        mat4 m_transformation;//世界变换矩阵(只包括平移，旋转，无缩放)
        mat4 m_finalTransformMatrix;//最终变换矩阵(包括平移，旋转，缩放)

        Sphere m_selfBoundingSphere;//自身包围球，不含子节点
        Sphere m_boundingSphere;//节点的包围球，包含子节点

        bool m_dirty;//是否需要重新计算最终变换矩阵
        bool m_dirtyBB;//是否需要重新计算自身包围球
    public:
        Object(const string &in_name, Object *in_father, const vec3 &in_offsetFromParent, float in_radianX,
               float in_radianY, float in_radianZ, bool in_visible);
        //取得名字
        inline const string &GetName() const {return m_name;}
        inline bool GetDynamic() const {return m_dynamic;}
        inline void SetDynamic(bool in_dynamic) {m_dynamic = in_dynamic;}
        //获取当前位置(世界空间中)
        inline vec3 GetPosition() const {
            return vec3(m_transformation[3][0], m_transformation[3][1], m_transformation[3][2]);
        }
        //得到前向向量的z的归一化向量
        inline vec3 GetDir() const {
            return vec3(m_transformation[2][0], m_transformation[2][1], m_transformation[2][2]);
        }
        //得到右向向量的x的归一化向量
        inline vec3 GetRight() const {
            return vec3(m_transformation[0][0], m_transformation[0][1], m_transformation[0][2]);
        }
        //得到上向向量的y的归一化向量
        inline vec3 GetUp() const {
            return vec3(m_transformation[1][0], m_transformation[1][1], m_transformation[1][2]);
        }
        //得到物体的旋转矩阵
        const mat3 &GetRotationMatrix() const;
        //得到世界变换矩阵(只包含平移和旋转,不包含缩放)
        inline const mat4 &GetTransformMatrix() const {
            return m_transformation;
        }
        //得到最终世界变换矩阵
        inline mat4 GetFinalTransformMatrix() const {
            return m_finalTransformMatrix;
        }
        //得到相对位置
        inline const vec3 &GetPositionRelative() const {
            return m_positionRelative;
        }
        //得到相对前向向量的z的归一化向量
        inline vec3 GetDirRelative() const {
            return vec3(m_rotateRelative[2][0], m_rotateRelative[2][1], m_rotateRelative[2][2]);
        }
        //得到相对右向向量的x的归一化向量
        inline vec3 GetRightRelative() const {
            return vec3(m_rotateRelative[0][0], m_rotateRelative[0][1], m_rotateRelative[0][2]);
        }
        //得到相对上向向量的y的归一化向量
        inline vec3 GetUpRelative() const {
            return vec3(m_rotateRelative[1][0], m_rotateRelative[1][1], m_rotateRelative[1][2]);
        }
        //得到物体的相对旋转矩阵
        inline const mat3 &GetRotationMatrixRelative() const {
            return m_rotateRelative;
        }
        //得到物体的相对变换矩阵(只包含平移和旋转,不包含缩放)
        const mat4 &GetTransformMatrixRelative() const;
        //得到物体的缩放信息
        inline float GetScale() const {
            return m_scale;
        }
        //检测是否有这个子节点,无返回nullptr
        Object *CheckHasChild(const string &childName) const;
        //得到子节点列表
        inline std::list<Object *> &GetChildList() {
            return m_chileList;
        }
        //移动一段距离
        void Move(const vec3 &in_offset);
        //移动到
        void MoveTo(const vec3 &in_newPos);
        //围绕x轴旋转
        void RotationPitch(float in_radian);
        //围绕y轴旋转
        void RotationYaw(float in_radian);
        //围绕z轴旋转
        void RotationRool(float in_radian);
        //同时三轴
        void RotationPitchYawRool(float in_radianX, float in_radianY, float in_radianZ);
        //将节点定位到朝向某个方向，上方是指定方向
        void SetDirectiveRelative(const vec3 &in_dir, const vec3 &in_up = vec3(0, 1, 0));
        //缩放
        void Scale(float in_scale);
        //更新自身变换矩阵(不包括缩放)，返回最终变换矩阵，不更新父节点或子节点
        const mat4 &UpdataTransformation();
        //更新以自己为根的一颗子树的变换矩阵,compusive设置是否强制更新
        void UpdataSubSceneGraph(bool in_compusive);
        //获取包围球
        inline const Sphere &GetBoundingSphere() const {return m_boundingSphere;}
        //获取自身包围球
        inline const Sphere &GetSelfBoundingSphere() const {return m_selfBoundingSphere;}
        //重新计算此结点及其子结点的包围球
        void UpdateBoundingSphere();
        //更新某个节点所有子孙节点包围球
        void UpdataBoundSphere(Object *in_root);
        //挂载此节点到一个父节点
        bool Attach(Object *in_father);
        //添加一个子节点到此节点上
        bool AddAttach(Object *in_child);
        //删除一个子节点
        bool Detach(const string &in_name);
        //删除所有子节点
        bool DetachAll();
        bool SetMesh(Mesh *in_mesh);
        void Render();
        void RenderBndSphere();
        void RenderSlfBndSphere();
        void RenderDepth();
        void SetMaterial(Material *in_material);
        void SetUvConfig(const vec2 &in_uvoffset, const vec2 &in_uvscale);
        inline Material *GetMaterial() {return m_material;};
        void Updata(float in_deltaTime, EventInfo &in_eventInfo);
        void SetActionFunc(std::function<void()> &&in_function);
    };
}



