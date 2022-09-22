//
// Created by Administrator on 2022/9/10.
//

#pragma once
#include <resource/mesh.h>
#include <unordered_map>
#include <string>
using std::unordered_map;
using std::string;
namespace R3D {
    class MeshManage {
    public:
        unordered_map<string, Mesh *> m_nameToMesh;
        void Init();//初始化用于演示的mesh，有需要可自行添加其他的
        void Release();
        void AddMesh(const string& in_name,Mesh* in_mesh);
        Mesh* GetMesh(const string& in_name);
        static MeshManage* GetInstance();
    private:
        MeshManage();
        static MeshManage* m_meshManage;
    };
}



