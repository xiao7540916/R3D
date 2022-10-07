//
// Created by Administrator on 2022/9/9.
//
/*
 * imgui屏蔽系统事件说明，主要在glfw_event.cpp中处理输入冲突问题
 * ImGuiIO &io = ImGui::GetIO();
 *您可以读取 io.WantCaptureMouse、io.WantCaptureKeyboard 标志来判断亲爱的 imgui 是否想要使用您的输入。
 *当 io.WantCaptureMouse 为真时，不要将鼠标输入数据发送到您的主应用程序，或者清除/覆盖您的鼠标数据副本。
 *当 io.WantCaptureKeyboard 为真时，不要将键盘输入数据发送到您的主应用程序，或清除/覆盖您的键盘数据副本。
 * */
#include "gui.h"
#include <device/device.h>
namespace R3D {
    Gui::Gui() {
    }
    Gui *Gui::m_gui = nullptr;
    Gui *Gui::GetInstance() {
        if (m_gui == nullptr) {
            m_gui = NEW Gui();
        }
        return m_gui;
    }
    void Gui::Init(Device *in_device) {
        Gui *gui = GetInstance();
        gui->m_device = in_device;

        // Setup Dear ImGui context
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGuiIO &io = ImGui::GetIO();
        (void) io;
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;       // Enable Keyboard Controls
        io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;           // Enable Docking
        io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;         // Enable Multi-Viewport / Platform Windows
//        io.ConfigViewportsNoAutoMerge = true;//是否自动调整窗口融合的显示层
//        io.ConfigViewportsNoTaskBarIcon = true;

        //设置主题
        ImGui::StyleColorsDark();

        //用于多窗口融合的妥协设置
        ImGuiStyle &style = ImGui::GetStyle();
        if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
            style.WindowRounding = 0.0f;
            style.Colors[ImGuiCol_WindowBg].w = 1.0f;
        }

        // Setup Platform/Renderer backends
        ImGui_ImplGlfw_InitForOpenGL(in_device->GetWindow(), true);
        ImGui_ImplOpenGL3_Init("#version 130");
    }
    void Gui::Begin() {
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
    }
    void Gui::End() {
        ImGui::Render();
    }
    void Gui::Render() {
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        //用于多重口时恢复绘制上下文
        ImGuiIO &io = ImGui::GetIO();
        if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
            GLFWwindow *backup_current_context = glfwGetCurrentContext();
            ImGui::UpdatePlatformWindows();
            ImGui::RenderPlatformWindowsDefault();
            glfwMakeContextCurrent(backup_current_context);
        }
    }
}

