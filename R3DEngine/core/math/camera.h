//
// Created by Administrator on 2021/2/26.
//

#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <structdefine.h>
#include <math/hexahedron.h>
namespace R3D {
    using glm::vec2;
    using glm::vec3;
    using glm::vec4;
    using glm::mat3;
    using glm::mat4;
    using glm::radians;
    struct orthodate {
        float left;
        float right;
        float bottom;
        float top;
        float zNear;
        float zFar;
        vec3 pos;
        vec3 target;
        vec3 up;
    };
    class Camera {
    public:
        float speed = 4.0f;
        float rotatespeed = 30.f;
        float pitch = 0.0f;//俯仰角
        float yaw = 0.0f;//偏航角
        Hexahedron m_frustum;//视景体六面体
        Camera();
        ~Camera();
        static vec3 rotatebymat4(vec3 &toberotate, mat4 &rotatemat);
        //获取及设置世界空间位置
        vec3 GetPosition() const;
        void SetPosition(vec3 &pos);
        void SetFront(vec3 &front);
        //获取相机的基向量
        vec3 GetUp() const;
        vec3 GetRight() const;
        vec3 GetForward() const;
        //获取视锥体的属性
        float GetNearZ() const;
        float GetFarZ() const;
        float GetAspect() const;
        float GetFovY() const;
        float GetFovX() const;
        //获取观察空间坐标表示的近，远平面大小
        float GetNearWindowWidth() const;
        float GetNearWindowHeight() const;
        float GetFarWindowWidth() const;
        float GetFarWindowHeight() const;
        //设置视锥体
        void SetLens(float FovY, float aspect, float zn, float zf);
        //通过LookAt方法定义摄像机空间
        void LookAt(vec3 pos, vec3 target, vec3 worldUp);
        void LookAt(const vec3 *pos, const vec3 *target, const vec3 *worldUp);
        //获取观察矩阵，投影矩阵
        mat4 GetView() const;
        mat4 GetProjection() const;
        //相机前进或者左右移动
        //向右移动d
        void Strafe(float d);
        //前进d
        void Walk(float d);
        //向上移动
        void GoUp(float d);
        //相机旋转
        //围绕右轴旋转
        void Pitch(float angle);
        //围绕世界轴Y轴旋转
        void RotateY(float angle);
        //重建观察矩阵
        void UpdateViewMatrix();
    private:
        //相机相对于世界空间的位置
        vec3 mPosition = {0.0f, 0.0f, 0.0f};
        vec3 mUp = {0.0f, 1.0f, 0.0f};
        vec3 mForward = {0.0f, 0.0f, 1.0f};
        vec3 mRight = {1.0f, 0.0f, 0.0f};
        vec3 mWorldUp = {0.0f, 1.0f, 0.0f};
        //缓存视锥体属性
        float mNearZ = 0.0f;
        float mFarZ = 0.0f;
        float mAspect = 0.0f;//宽高比
        float mFovY = 0.0f;//纵向可视角度
        float mNearWindowHeight = 0.0f;
        float mFarWindowHeight = 0.0f;
        bool mViewdirty = true;//观察矩阵是否需要更新

        //缓存观察矩阵和投影矩阵
        mat4 mView = mat4(1.0f);
        mat4 mProjection = mat4(1.0f);
    };
}
