//
// Created by Administrator on 2022/8/16.
//

#include "RLight.h"
namespace R3D {
    RLight::RLight(RInt lightId, LIGHT_TYPE lightType) : _iId(lightId),
                                                         _eLightType(lightType), _bLightOn(true),
                                                         _vAmbient(1.0f, 1.0f, 1.0f),
                                                         _vDifuse(1.0f, 1.0f, 1.0f), _vSpecular(1.0f, 1.0f, 1.0f),
                                                         _vPostion(0.0f, 0.0f, 0.0f),
                                                         _vDirection(0.0f, 0.0f, 1.0f), _fKc(1.0f), _fKl(1.0f), _fKq(1.0f),
                                                         _fSpotInner(45.0f), _fSpotOuter(60.0f),
                                                         _fPower(1.0f), _fShadowFactor(0.0f) {}
    std::vector<RLight *> *GLights = nullptr;
    RInt createLight(LIGHT_TYPE lightType) {
        if (GLights == nullptr) {
            GLights = new std::vector<RLight *>;
        }
        if (GLights->size() > MAX_LIGHTS) {
            return -1;
        }
        RInt id = RInt(GLights->size());
        GLights->push_back(new RLight(id, lightType));
        return id;
    }
    RLight *getLight(RInt id) {
        if (GLights == nullptr ||
            GLights->size() > MAX_LIGHTS ||
            id < 0 ||
            id > GLights->size() - 1) {
            return nullptr;
        } else {
            return GLights->at(id);
        }
    }
    RInt getLightCount() {
        return RInt(GLights->size());
    }
    void destroyAllLight() {
        for (auto lightptr:*GLights) {
            SafeDelete(lightptr);
        }
        GLights->clear();
        SafeDelete(GLights);
    }
}