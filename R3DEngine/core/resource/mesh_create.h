//
// Created by Administrator on 2022/9/16.
//

#pragma once
#include <structdefine.h>
#include <string>
#include <vector>
namespace R3D {
    using std::vector;
    using std::string;
    class Mesh;
    class MeshCreate {
    public:
        static VertexPosNorTanUv MidPoint(const VertexPosNorTanUv &v0, const VertexPosNorTanUv &v1);
        static void GetTangent(VertexPosNorTanUv &v0, VertexPosNorTanUv &v1, VertexPosNorTanUv &v2);
        static void Subdivide(vector<VertexPosNorTanUv>& in_vertices,vector<uint32_t>& in_indices);
        static void
        CreateBox(Mesh &in_mesh, float in_width, float in_height, float in_depth, VertexLayout in_vertexLayout);
        static void CreateGeosphere(Mesh &in_mesh, float in_radius, uint32_t in_numSubdivisions, VertexLayout in_vertexLayout);
        static void CreatePlane(Mesh& in_mesh,float in_x,float in_z,VertexLayout in_vertexLayout);
        static void CreateScreenBack(Mesh& in_mesh,VertexLayout in_vertexLayout);
        //加载单个网格obj文件，文件需支持uv
        static void LoadObjToMesh(Mesh& in_mesh,const string& in_url,VertexLayout in_vertexLayout);
        //加载多个网格组成的模型
        static void LoadObjToMeshes(vector<Mesh*>& in_meshes,const string& in_url,VertexLayout in_vertexLayout);
    };
}



