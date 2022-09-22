//
// Created by Administrator on 2022/9/10.
//

#include "mesh_manage.h"
#include <resource/mesh_create.h>
namespace R3D {
    MeshManage::MeshManage() {
    }
    MeshManage *MeshManage::m_meshManage = nullptr;
    MeshManage *MeshManage::GetInstance() {
        if (m_meshManage == nullptr) {
            m_meshManage = new MeshManage();
        }
        return m_meshManage;
    }
    void MeshManage::AddMesh(const string &in_name, Mesh *in_mesh) {
        if (GetMesh(in_name)) {
            std::cout << in_name << " has add!" << std::endl;
        }
        m_nameToMesh.insert({in_name, in_mesh});
    }
    Mesh *MeshManage::GetMesh(const string &in_name) {
        if (m_nameToMesh.find(in_name) != m_nameToMesh.end()) {
            return m_nameToMesh[in_name];
        } else {
            std::cout << "can not find " << in_name << std::endl;
            return nullptr;
        }
    }
    void MeshManage::Init() {
        Mesh *boxmesh = new Mesh();
        MeshCreate::CreateBox(*boxmesh, 1, 1, 1, VERT_POS_NOR_TAN_UV);
        AddMesh("boxmesh", boxmesh);
        Mesh *geospheremesh = new Mesh();
        MeshCreate::CreateGeosphere(*geospheremesh, 1, 3, VERT_POS_NOR_TAN_UV);
        AddMesh("geospheremesh", geospheremesh);
        Mesh *planemesh = new Mesh();
        MeshCreate::CreatePlane(*planemesh, 1, 1, VERT_POS_NOR_TAN_UV);
        AddMesh("planemesh", planemesh);
    }
    void MeshManage::Release() {
        for (auto item = m_nameToMesh.begin();item != m_nameToMesh.end();item++) {
            glDeleteVertexArrays(1, &item->second->VAO);
            glDeleteBuffers(1, &item->second->VBO);
            glDeleteBuffers(1, &item->second->EBO);
        }
        m_nameToMesh.clear();
        //m_nameToMesh自身占用空间仍需要清理
    }
}