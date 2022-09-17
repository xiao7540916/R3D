//
// Created by Administrator on 2021/2/26.
//
#ifndef OPENGLLAB_CAMERA_CPP
#define OPENGLLAB_CAMERA_CPP
#include "camera.h"
Camera::Camera() = default;
Camera::~Camera() = default;
#endif
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
}
void Camera::LookAt(vec3 pos, vec3 target, vec3 worldUp) {
    mPosition = pos;
    mForward = normalize(target - pos);
    mRight = normalize(cross(worldUp, mForward));
    mUp = cross(mForward, mRight);
    yaw = atan2(mForward.z,mForward.x)*57.2957f;//初始化时计算俯仰角，偏航角
    pitch = asin(mForward.y)*57.2957f;
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
    std::cout<<mRight.x<<" "<<mRight.y<<" "<<mRight.z<<std::endl;
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
