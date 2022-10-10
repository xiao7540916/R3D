//
// Created by Administrator on 2021/2/26.
//

#include "camera.h"
namespace R3D {
    Camera::Camera() = default;
    Camera::~Camera() = default;
    vec3 Camera::GetPosition() const {
        return mPosition;
    }
    void Camera::SetPosition(vec3 &pos) {
        mPosition = pos;
        mViewdirty = true;
    }
    vec3 Camera::GetUp() const {
        return mUp;
    }
    vec3 Camera::GetRight() const {
        return mRight;
    }
    vec3 Camera::GetForward() const {
        return mForward;
    }
    float Camera::GetNearZ() const {
        return mNearZ;
    }
    float Camera::GetFarZ() const {
        return mFarZ;
    }
    float Camera::GetAspect() const {
        return mAspect;
    }
    float Camera::GetFovY() const {
        return mFovY;
    }
    float Camera::GetFovX() const {
        float halfWidth = 0.5f * GetNearWindowWidth();
        return 2.0f * atan(halfWidth / mNearZ);
    }
    float Camera::GetNearWindowWidth() const {
        return mAspect * mNearWindowHeight;
    }
    float Camera::GetNearWindowHeight() const {
        return mNearWindowHeight;
    }
    float Camera::GetFarWindowWidth() const {
        return mAspect * mFarWindowHeight;
    }
    float Camera::GetFarWindowHeight() const {
        return mFarWindowHeight;
    }
    void Camera::SetLens(float FovY, float aspect, float zn, float zf) {
        mFovY = FovY;
        mAspect = aspect;
        mNearZ = zn;
        mFarZ = zf;
        mNearWindowHeight = 2.0f * mNearZ * tanf(0.5f * mFovY);
        mFarWindowHeight = 2.0f * mFarZ * tanf(0.5f * mFovY);
        mProjection = glm::perspective(mFovY, mAspect, mNearZ, mFarZ);

        //设置裁剪平面，坐标基于摄像机坐标系
        vec3 normalT = glm::normalize(vec3(0, -1.0f, -tanf(0.5f * mFovY)));
        vec3 normalB = glm::normalize(vec3(0, 1.0f, -tanf(0.5f * mFovY)));
        vec3 normalL = glm::normalize(vec3(1.0f, 0, -tanf(0.5f * mFovY) * mAspect));
        vec3 normalR = glm::normalize(vec3(-1.0f, 0, -tanf(0.5f * mFovY) * mAspect));
        m_frustum.m_planes[0].init(normalT, 0.0f);
        m_frustum.m_planes[1].init(normalB, 0.0f);
        m_frustum.m_planes[2].init(normalL, 0.0f);
        m_frustum.m_planes[3].init(normalR, 0.0f);
        m_frustum.m_planes[4].init(vec3(0, 0, -1.0f), -zn);
        m_frustum.m_planes[5].init(vec3(0, 0, 1.0f), zf);
    }
    void Camera::LookAt(vec3 pos, vec3 target, vec3 worldUp) {
        mPosition = pos;
        mForward = normalize(target - pos);
        mRight = normalize(cross(worldUp, mForward));
        mUp = cross(mForward, mRight);
        yaw = atan2(mForward.z, mForward.x) * 57.2957f;//初始化时计算俯仰角，偏航角
        pitch = asin(mForward.y) * 57.2957f;
        mViewdirty = true;
    }
    void Camera::LookAt(const vec3 *pos, const vec3 *target, const vec3 *worldUp) {
        LookAt(*pos, *target, *worldUp);
    }
    mat4 Camera::GetView() const {
        return mView;
    }
    mat4 Camera::GetProjection() const {
        return mProjection;
    }
    void Camera::Strafe(float d) {
        mPosition += d * mRight;
        mViewdirty = true;
    }
    void Camera::Walk(float d) {
        mPosition += d * mForward;
        mViewdirty = true;
    }
    vec3 Camera::rotatebymat4(vec3 &toberotate, mat4 &rotatemat) {
        vec4 ntoberotate(toberotate.x, toberotate.y, toberotate.z, 1.0);
        ntoberotate = ntoberotate * rotatemat;
        return vec3(ntoberotate.x, ntoberotate.y, ntoberotate.z);
    }
    void Camera::Pitch(float angle) {
        float mpitch = pitch + angle;
        if (mpitch > 89.0f) {
            pitch = 89.0f;
            mat4 I(1.0);
            mat4 rotatemat = rotate(I, radians(pitch + angle - mpitch), mRight);
            mForward = rotatebymat4(mForward, rotatemat);
            mUp = rotatebymat4(mUp, rotatemat);
        } else if (mpitch < -89.0f) {
            pitch = -89.0f;
            mat4 I(1.0);
            mat4 rotatemat = rotate(I, radians(pitch + angle - mpitch), mRight);
            mForward = rotatebymat4(mForward, rotatemat);
            mUp = rotatebymat4(mUp, rotatemat);
        } else {
            pitch = pitch + angle;
            mat4 I(1.0);
            mat4 rotatemat = rotate(I, radians(angle), mRight);
            mForward = rotatebymat4(mForward, rotatemat);
            mUp = rotatebymat4(mUp, rotatemat);
        }
        mViewdirty = true;
    }
    void Camera::RotateY(float angle) {
        mat4 I(1.0);
        mat4 rotatemat = rotate(I, radians(angle), mWorldUp);
        mRight = rotatebymat4(mRight, rotatemat);
        std::cout << mRight.x << " " << mRight.y << " " << mRight.z << std::endl;
        mForward = rotatebymat4(mForward, rotatemat);
        mUp = rotatebymat4(mUp, rotatemat);
        mViewdirty = true;
    }
    void Camera::UpdateViewMatrix() {
        if (mViewdirty) {
            mView = lookAt(mPosition, mPosition + mForward, mWorldUp);
        }
    }
    void Camera::SetFront(vec3 &front) {
        mForward = front;
        mRight = normalize(cross(mWorldUp, mForward));
        mUp = cross(mForward, mRight);
        mViewdirty = true;
    }
    void Camera::GoUp(float d) {
        mPosition += d * mWorldUp;
        mViewdirty = true;
    }
    glm::mat4 Camera::GetOrthoProjectionOpengl(OrthoDate &orthoDate) {
//        return glm::ortho(orthoDate.left, orthoDate.right, orthoDate.bottom, orthoDate.top, orthoDate.zNear,
//                          orthoDate.zFar);
        glm::mat4 orthoProjection(1.0f);
        float r = orthoDate.right;
        float l = orthoDate.left;
        float t = orthoDate.top;
        float b = orthoDate.bottom;
        float n = orthoDate.zNear;
        float f = orthoDate.zFar;
        orthoProjection[0][0] = 2.0f / (r - l);
        orthoProjection[1][1] = 2.0f / (t - b);
        orthoProjection[2][2] = -2.0f / (f - n);
        orthoProjection[3][0] = -(r + l) / (r - l);
        orthoProjection[3][1] = -(t + b) / (t - b);
        orthoProjection[3][2] = -(f + n) / (f - n);
        return orthoProjection;
    }
}
