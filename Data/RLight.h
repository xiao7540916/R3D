//
// Created by Administrator on 2022/8/16.
//

#ifndef RABBIT3D_RLIGHT_H
#define RABBIT3D_RLIGHT_H
#include "RCommon.h"
namespace R3D {
    enum LIGHT_TYPE {
        LIGHT_AMBIENT = 0,
        LIGHT_DIRECTION = 1,
        LIGHT_POINT = 2,
        LIGHT_SPOT = 3
    };
    class RLight {
    public:
        RInt _iId;//灯光ID
        RBool _bLightOn;//灯光是否被启用

        LIGHT_TYPE _eLightType;//灯光类型

        RFloat _fPower;//灯光强度
        RFloat _fShadowFactor;//灯光背面强度
        RColor _vAmbient;//环境光颜色
        RColor _vDifuse;//漫反射颜色
        RColor _vSpecular;//镜面反射颜色

        RVector3 _vPostion;//灯光位置
        RVector3 _vDirection;//灯光方向

        RVector3 _vTransPosition;//灯光在摄像机坐标系中的位置
        RVector3 _vTransDirection;//摄像机坐标系光源朝向

        RFloat _fKc, _fKl, _fKq;//衰减因子

        RFloat _fSpotInner;//聚光灯内锥角
        RFloat _fSpotOuter;//聚光灯外锥角
        RFloat _fPf;//聚光灯指数因子

        RLight(RInt lightId, LIGHT_TYPE lightType);
    };
#define MAX_LIGHTS 64
    extern std::vector<RLight *> *GLights;
    extern RInt createLight(LIGHT_TYPE lightType);//创建灯光，成功返回灯光ID，失败返回-1
    extern RLight *getLight(RInt id);//获取灯光
    extern RInt getLightCount();//获取灯光数量
    extern void destroyAllLight();//清除所有灯光
}
#endif //RABBIT3D_RLIGHT_H








