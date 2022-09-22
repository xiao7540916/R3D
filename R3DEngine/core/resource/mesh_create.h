//
// Created by Administrator on 2022/9/16.
//

#pragma once
#include <structdefine.h>
#include <vector>
namespace R3D {
    using std::vector;
    class Mesh;
    class MeshCreate {
    public:
        static VertexPosNorTanUv MidPoint(const VertexPosNorTanUv &v0, const VertexPosNorTanUv &v1);
        static void GetTangent(VertexPosNorTanUv &v0, VertexPosNorTanUv &v1, VertexPosNorTanUv &v2);
        static void Subdivide(vector<VertexPosNorTanUv>& in_vertices,vector<uint32_t>& in_indices);
        static void
        CreateBox(Mesh &in_mesh, float in_width, float in_height, float in_depth, VertexLayout in_vertexLayout);
        static void CreateGeosphere(Mesh &in_mesh, float in_radius, uint32_t in_numSubdivisions, VertexLayout in_vertexLayout);
    };
}



