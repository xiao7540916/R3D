//
// Created by Administrator on 2022/9/9.
//

#pragma once
#include<imguidock/imgui.h>
#include<imguidock/imgui_impl_glfw.h>
#include<imguidock/imgui_impl_opengl3.h>
namespace R3D {
    class Device;
    class Gui {
    public:
        static Gui* GetInstance();
        void Init(Device *in_device);
        void Begin();
        void End();
        void Render();
    private:
        Gui();
        static Gui *m_gui;
        Device *m_device;
    };
}



