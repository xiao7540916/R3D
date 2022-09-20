//
// Created by Administrator on 2022/9/16.
//

#include "mesh_create.h"
#include <resource/mesh.h>
#include <vector>
namespace R3D {
    using std::vector;
    void
    MeshCreate::CreateBox(Mesh &in_mesh, float in_width, float in_height, float in_depth,
                          VertexLayout in_vertexLayout) {
        switch (in_vertexLayout) {
            case VERT_POS: {
                break;
            }
            case VERT_POS_COL: {
                break;
            }
            case VERT_POS_NOR: {
                break;
            }
            case VERT_POS_NOR_UV: {
                break;
            }
            case VERT_POS_NOR_TAN_UV: {
                vector<VertexPosNorTanUv> vertices = {
                        // Positions             // Normals          //u切向             // texture coords
                        {{-0.5f, -0.5f, -0.5f}, {0.0f,  0.0f,  -1.0f}, {1.0f, 0.0f, 0.0f}, {0.0f, 0.0f}},
                        {{0.5f,  -0.5f, -0.5f}, {0.0f,  0.0f,  -1.0f}, {1.0f, 0.0f, 0.0f}, {1.0f, 0.0f}},
                        {{0.5f,  0.5f,  -0.5f}, {0.0f,  0.0f,  -1.0f}, {1.0f, 0.0f, 0.0f}, {1.0f, 1.0f}},
                        {{0.5f,  0.5f,  -0.5f}, {0.0f,  0.0f,  -1.0f}, {1.0f, 0.0f, 0.0f}, {1.0f, 1.0f}},
                        {{-0.5f, 0.5f,  -0.5f}, {0.0f,  0.0f,  -1.0f}, {1.0f, 0.0f, 0.0f}, {0.0f, 1.0f}},
                        {{-0.5f, -0.5f, -0.5f}, {0.0f,  0.0f,  -1.0f}, {1.0f, 0.0f, 0.0f}, {0.0f, 0.0f}},
                        {{-0.5f, -0.5f, 0.5f},  {0.0f,  0.0f,  1.0f},  {1.0f, 0.0f, 0.0f}, {0.0f, 0.0f}},
                        {{0.5f,  -0.5f, 0.5f},  {0.0f,  0.0f,  1.0f},  {1.0f, 0.0f, 0.0f}, {1.0f, 0.0f}},
                        {{0.5f,  0.5f,  0.5f},  {0.0f,  0.0f,  1.0f},  {1.0f, 0.0f, 0.0f}, {1.0f, 1.0f}},
                        {{0.5f,  0.5f,  0.5f},  {0.0f,  0.0f,  1.0f},  {1.0f, 0.0f, 0.0f}, {1.0f, 1.0f}},
                        {{-0.5f, 0.5f,  0.5f},  {0.0f,  0.0f,  1.0f},  {1.0f, 0.0f, 0.0f}, {0.0f, 1.0f}},
                        {{-0.5f, -0.5f, 0.5f},  {0.0f,  0.0f,  1.0f},  {1.0f, 0.0f, 0.0f}, {0.0f, 0.0f}},
                        {{-0.5f, 0.5f,  0.5f},  {-1.0f, 0.0f,  0.0f},  {0.0f, 1.0f, 0.0f}, {1.0f, 0.0f}},
                        {{-0.5f, 0.5f,  -0.5f}, {-1.0f, 0.0f,  0.0f},  {0.0f, 1.0f, 0.0f}, {1.0f, 1.0f}},
                        {{-0.5f, -0.5f, -0.5f}, {-1.0f, 0.0f,  0.0f},  {0.0f, 1.0f, 0.0f}, {0.0f, 1.0f}},
                        {{-0.5f, -0.5f, -0.5f}, {-1.0f, 0.0f,  0.0f},  {0.0f, 1.0f, 0.0f}, {0.0f, 1.0f}},
                        {{-0.5f, -0.5f, 0.5f},  {-1.0f, 0.0f,  0.0f},  {0.0f, 1.0f, 0.0f}, {0.0f, 0.0f}},
                        {{-0.5f, 0.5f,  0.5f},  {-1.0f, 0.0f,  0.0f},  {0.0f, 1.0f, 0.0f}, {1.0f, 0.0f}},
                        {{0.5f,  0.5f,  0.5f},  {1.0f,  0.0f,  0.0f},  {0.0f, 1.0f, 0.0f}, {1.0f, 0.0f}},
                        {{0.5f,  0.5f,  -0.5f}, {1.0f,  0.0f,  0.0f},  {0.0f, 1.0f, 0.0f}, {1.0f, 1.0f}},
                        {{0.5f,  -0.5f, -0.5f}, {1.0f,  0.0f,  0.0f},  {0.0f, 1.0f, 0.0f}, {0.0f, 1.0f}},
                        {{0.5f,  -0.5f, -0.5f}, {1.0f,  0.0f,  0.0f},  {0.0f, 1.0f, 0.0f}, {0.0f, 1.0f}},
                        {{0.5f,  -0.5f, 0.5f},  {1.0f,  0.0f,  0.0f},  {0.0f, 1.0f, 0.0f}, {0.0f, 0.0f}},
                        {{0.5f,  0.5f,  0.5f},  {1.0f,  0.0f,  0.0f},  {0.0f, 1.0f, 0.0f}, {1.0f, 0.0f}},
                        {{-0.5f, -0.5f, -0.5f}, {0.0f,  -1.0f, 0.0f},  {1.0f, 0.0f, 0.0f}, {0.0f, 1.0f}},
                        {{0.5f,  -0.5f, -0.5f}, {0.0f,  -1.0f, 0.0f},  {1.0f, 0.0f, 0.0f}, {1.0f, 1.0f}},
                        {{0.5f,  -0.5f, 0.5f},  {0.0f,  -1.0f, 0.0f},  {1.0f, 0.0f, 0.0f}, {1.0f, 0.0f}},
                        {{0.5f,  -0.5f, 0.5f},  {0.0f,  -1.0f, 0.0f},  {1.0f, 0.0f, 0.0f}, {1.0f, 0.0f}},
                        {{-0.5f, -0.5f, 0.5f},  {0.0f,  -1.0f, 0.0f},  {1.0f, 0.0f, 0.0f}, {0.0f, 0.0f}},
                        {{-0.5f, -0.5f, -0.5f}, {0.0f,  -1.0f, 0.0f},  {1.0f, 0.0f, 0.0f}, {0.0f, 1.0f}},
                        {{-0.5f, 0.5f,  -0.5f}, {0.0f,  1.0f,  0.0f},  {1.0f, 0.0f, 0.0f}, {0.0f, 1.0f}},
                        {{0.5f,  0.5f,  -0.5f}, {0.0f,  1.0f,  0.0f},  {1.0f, 0.0f, 0.0f}, {1.0f, 1.0f}},
                        {{0.5f,  0.5f,  0.5f},  {0.0f,  1.0f,  0.0f},  {1.0f, 0.0f, 0.0f}, {1.0f, 0.0f}},
                        {{0.5f,  0.5f,  0.5f},  {0.0f,  1.0f,  0.0f},  {1.0f, 0.0f, 0.0f}, {1.0f, 0.0f}},
                        {{-0.5f, 0.5f,  0.5f},  {0.0f,  1.0f,  0.0f},  {1.0f, 0.0f, 0.0f}, {0.0f, 0.0f}},
                        {{-0.5f, 0.5f,  -0.5f}, {0.0f,  1.0f,  0.0f},  {1.0f, 0.0f, 0.0f}, {0.0f, 1.0f}}
                };
                for (int i = 0;i < vertices.size();++i) {
                    vertices[i].position.x *= (in_width);
                    vertices[i].position.y *= (in_height);
                    vertices[i].position.z *= (in_depth);
                }
                vector<uint32_t> indices = {
                        2, 1, 0, 5, 4, 3,
                        6, 7, 8, 9, 10, 11,
                        12, 13, 14, 15, 16, 17,
                        20, 19, 18, 23, 22, 21,
                        24, 25, 26, 27, 28, 29,
                        32, 31, 30, 35, 34, 33
                };
                for (int i = 0; i < indices.size(); i = i + 3) {
                    GetTangent(vertices[indices[i]], vertices[indices[i+1]], vertices[indices[i+2]]);
                    GetTangent(vertices[indices[i+1]], vertices[indices[i+2]], vertices[indices[i]]);
                    GetTangent(vertices[indices[i+2]], vertices[indices[i]], vertices[indices[i+1]]);
                }

                in_mesh.m_indiceSize = indices.size();
                glCreateVertexArrays(1, &in_mesh.VAO);
                //定义顶点缓冲对象，将顶点数据复制到缓冲中
                glCreateBuffers(1, &in_mesh.VBO);
                glCreateBuffers(1, &in_mesh.EBO);
                glNamedBufferStorage(in_mesh.VBO, (long long) (vertices.size() * sizeof(VertexPosNorTanUv)),
                                     vertices.data(), 0);//创建缓冲并向其中写入数据
                glNamedBufferStorage(in_mesh.EBO, (long long) (indices.size() * sizeof(uint32_t)), indices.data(), 0);
                glBindVertexArray(in_mesh.VAO);//开始记录顶点信息
                glBindBuffer(GL_ARRAY_BUFFER, in_mesh.VBO);
                glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, in_mesh.EBO);
                //设置顶点缓冲的属性解释
                glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void *) 0);
                glEnableVertexAttribArray(0);
                glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void *) (3 * sizeof(float)));
                glEnableVertexAttribArray(1);
                glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void *) (6 * sizeof(float)));
                glEnableVertexAttribArray(2);
                glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void *) (9 * sizeof(float)));
                glEnableVertexAttribArray(3);
                glBindVertexArray(0);
                break;
            }
            default:
                break;
        }
    }
    void MeshCreate::GetTangent(VertexPosNorTanUv &v0, VertexPosNorTanUv &v1, VertexPosNorTanUv &v2) {
        vec3 POS1 = v1.position - v0.position;
        vec3 POS2 = v2.position - v0.position;
        vec2 TEX1 = v1.uv - v0.uv;
        vec2 TEX2 = v2.uv - v0.uv;
        //翻转y方向
        float a = 1.0f / (TEX1.x * TEX2.y - TEX2.x * TEX1.y);
        vec3 tanu;
        tanu.x = a * (TEX2.y * POS1.x - TEX1.y * POS2.x);
        tanu.y = a * (TEX2.y * POS1.y - TEX1.y * POS2.y);
        tanu.z = a * (TEX2.y * POS1.z - TEX1.y * POS2.z);
        v0.tangent = normalize(tanu);
    }
}