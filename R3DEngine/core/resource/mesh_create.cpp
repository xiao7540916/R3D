//
// Created by Administrator on 2022/9/16.
//

#include "mesh_create.h"
#include <resource/mesh.h>
#include <vector>
#define TINYOBJLOADER_IMPLEMENTATION
#include <tiny_obj_loader.h>
#include <util/r_log.h>
namespace R3D {
    using std::cout;
    using std::endl;
    struct SubMeshMtrIdInfo {
        int startindex;
        int count;
    };
    using std::vector;
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
    VertexPosNorTanUv MeshCreate::MidPoint(const VertexPosNorTanUv &v0, const VertexPosNorTanUv &v1) {
        vec3 p0 = vec3(v0.position.x, v0.position.y, v0.position.z);
        vec3 p1 = vec3(v1.position.x, v1.position.y, v1.position.z);
        vec3 n0 = vec3(v0.normal.x, v0.normal.y, v0.normal.z);
        vec3 n1 = vec3(v1.normal.x, v1.normal.y, v1.normal.z);
        vec3 tan0 = vec3(v0.tangent.x, v0.tangent.y, v0.tangent.z);
        vec3 tan1 = vec3(v1.tangent.x, v1.tangent.y, v1.tangent.z);
        vec2 tex0 = vec2(v0.uv.x, v0.uv.y);
        vec2 tex1 = vec2(v1.uv.x, v1.uv.y);
        VertexPosNorTanUv v;
        v.position = 0.5f * (p0 + p1);
        v.normal = normalize(0.5f * (n0 + n1));
        v.tangent = normalize(0.5f * (tan0 + tan1));
        v.uv = 0.5f * (tex0 + tex1);
        return v;
    }
    void MeshCreate::Subdivide(vector<VertexPosNorTanUv> &in_vertices, vector<uint32_t> &in_indices) {
        //拷贝出原始数据
        vector<VertexPosNorTanUv> vertices = in_vertices;
        vector<uint32_t> indices = in_indices;
        in_vertices.resize(0);
        in_indices.resize(0);

        //       v1
        //       *
        //      / \
	    //     /   \
	    //  m0*-----*m1
        //   / \   / \
	    //  /   \ /   \
	    // *-----*-----*
        // v0    m2     v2

        uint32_t trianglenum = (uint32_t) indices.size() / 3;
        for (uint32_t i = 0;i < trianglenum;++i) {
            VertexPosNorTanUv v0 = vertices[indices[i * 3 + 0]];
            VertexPosNorTanUv v1 = vertices[indices[i * 3 + 1]];
            VertexPosNorTanUv v2 = vertices[indices[i * 3 + 2]];
            VertexPosNorTanUv m0 = MidPoint(v0, v1);
            VertexPosNorTanUv m1 = MidPoint(v1, v2);
            VertexPosNorTanUv m2 = MidPoint(v0, v2);
            in_vertices.push_back(v0);//0
            in_vertices.push_back(v1);//1
            in_vertices.push_back(v2);//2
            in_vertices.push_back(m0);//3
            in_vertices.push_back(m1);//4
            in_vertices.push_back(m2);//5

            in_indices.push_back(i * 6 + 0);
            in_indices.push_back(i * 6 + 3);
            in_indices.push_back(i * 6 + 5);
            in_indices.push_back(i * 6 + 3);
            in_indices.push_back(i * 6 + 4);
            in_indices.push_back(i * 6 + 5);
            in_indices.push_back(i * 6 + 5);
            in_indices.push_back(i * 6 + 4);
            in_indices.push_back(i * 6 + 2);
            in_indices.push_back(i * 6 + 3);
            in_indices.push_back(i * 6 + 1);
            in_indices.push_back(i * 6 + 4);
        }
    }
    void
    MeshCreate::CreateBox(Mesh &in_mesh, float in_width, float in_height, float in_depth,
                          VertexLayout in_vertexLayout) {
        in_mesh.m_vertexLayout = in_vertexLayout;
        in_mesh.m_sphere.SetRadius(0.5f * sqrtf(SquaredVec3(vec3(in_width, in_height, in_depth))));
        in_mesh.m_sphere.SetCenter(vec3(0, 0, 0));
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
                        //UV需要注意观察方向
                        // Positions             // Normals          //u切向             // texture coords
                        {{-0.5f, -0.5f, -0.5f}, {0.0f,  0.0f,  -1.0f}, {1.0f, 0.0f, 0.0f}, {1.0f, 0.0f}},
                        {{0.5f,  -0.5f, -0.5f}, {0.0f,  0.0f,  -1.0f}, {1.0f, 0.0f, 0.0f}, {0.0f, 0.0f}},
                        {{0.5f,  0.5f,  -0.5f}, {0.0f,  0.0f,  -1.0f}, {1.0f, 0.0f, 0.0f}, {0.0f, 1.0f}},

                        {{0.5f,  0.5f,  -0.5f}, {0.0f,  0.0f,  -1.0f}, {1.0f, 0.0f, 0.0f}, {0.0f, 1.0f}},
                        {{-0.5f, 0.5f,  -0.5f}, {0.0f,  0.0f,  -1.0f}, {1.0f, 0.0f, 0.0f}, {1.0f, 1.0f}},
                        {{-0.5f, -0.5f, -0.5f}, {0.0f,  0.0f,  -1.0f}, {1.0f, 0.0f, 0.0f}, {1.0f, 0.0f}},

                        {{-0.5f, -0.5f, 0.5f},  {0.0f,  0.0f,  1.0f},  {1.0f, 0.0f, 0.0f}, {0.0f, 0.0f}},
                        {{0.5f,  -0.5f, 0.5f},  {0.0f,  0.0f,  1.0f},  {1.0f, 0.0f, 0.0f}, {1.0f, 0.0f}},
                        {{0.5f,  0.5f,  0.5f},  {0.0f,  0.0f,  1.0f},  {1.0f, 0.0f, 0.0f}, {1.0f, 1.0f}},

                        {{0.5f,  0.5f,  0.5f},  {0.0f,  0.0f,  1.0f},  {1.0f, 0.0f, 0.0f}, {1.0f, 1.0f}},
                        {{-0.5f, 0.5f,  0.5f},  {0.0f,  0.0f,  1.0f},  {1.0f, 0.0f, 0.0f}, {0.0f, 1.0f}},
                        {{-0.5f, -0.5f, 0.5f},  {0.0f,  0.0f,  1.0f},  {1.0f, 0.0f, 0.0f}, {0.0f, 0.0f}},

                        {{-0.5f, 0.5f,  0.5f},  {-1.0f, 0.0f,  0.0f},  {0.0f, 1.0f, 0.0f}, {1.0f, 1.0f}},
                        {{-0.5f, 0.5f,  -0.5f}, {-1.0f, 0.0f,  0.0f},  {0.0f, 1.0f, 0.0f}, {0.0f, 1.0f}},
                        {{-0.5f, -0.5f, -0.5f}, {-1.0f, 0.0f,  0.0f},  {0.0f, 1.0f, 0.0f}, {0.0f, 0.0f}},

                        {{-0.5f, -0.5f, -0.5f}, {-1.0f, 0.0f,  0.0f},  {0.0f, 1.0f, 0.0f}, {0.0f, 0.0f}},
                        {{-0.5f, -0.5f, 0.5f},  {-1.0f, 0.0f,  0.0f},  {0.0f, 1.0f, 0.0f}, {1.0f, 0.0f}},
                        {{-0.5f, 0.5f,  0.5f},  {-1.0f, 0.0f,  0.0f},  {0.0f, 1.0f, 0.0f}, {1.0f, 1.0f}},

                        {{0.5f,  0.5f,  0.5f},  {1.0f,  0.0f,  0.0f},  {0.0f, 1.0f, 0.0f}, {0.0f, 1.0f}},
                        {{0.5f,  0.5f,  -0.5f}, {1.0f,  0.0f,  0.0f},  {0.0f, 1.0f, 0.0f}, {1.0f, 1.0f}},
                        {{0.5f,  -0.5f, -0.5f}, {1.0f,  0.0f,  0.0f},  {0.0f, 1.0f, 0.0f}, {1.0f, 0.0f}},

                        {{0.5f,  -0.5f, -0.5f}, {1.0f,  0.0f,  0.0f},  {0.0f, 1.0f, 0.0f}, {1.0f, 0.0f}},
                        {{0.5f,  -0.5f, 0.5f},  {1.0f,  0.0f,  0.0f},  {0.0f, 1.0f, 0.0f}, {0.0f, 0.0f}},
                        {{0.5f,  0.5f,  0.5f},  {1.0f,  0.0f,  0.0f},  {0.0f, 1.0f, 0.0f}, {0.0f, 1.0f}},

                        {{-0.5f, -0.5f, -0.5f}, {0.0f,  -1.0f, 0.0f},  {1.0f, 0.0f, 0.0f}, {0.0f, 0.0f}},
                        {{0.5f,  -0.5f, -0.5f}, {0.0f,  -1.0f, 0.0f},  {1.0f, 0.0f, 0.0f}, {1.0f, 0.0f}},
                        {{0.5f,  -0.5f, 0.5f},  {0.0f,  -1.0f, 0.0f},  {1.0f, 0.0f, 0.0f}, {1.0f, 1.0f}},

                        {{0.5f,  -0.5f, 0.5f},  {0.0f,  -1.0f, 0.0f},  {1.0f, 0.0f, 0.0f}, {1.0f, 1.0f}},
                        {{-0.5f, -0.5f, 0.5f},  {0.0f,  -1.0f, 0.0f},  {1.0f, 0.0f, 0.0f}, {0.0f, 1.0f}},
                        {{-0.5f, -0.5f, -0.5f}, {0.0f,  -1.0f, 0.0f},  {1.0f, 0.0f, 0.0f}, {0.0f, 0.0f}},

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
                for (int i = 0;i < indices.size();i = i + 3) {
                    GetTangent(vertices[indices[i]], vertices[indices[i + 1]], vertices[indices[i + 2]]);
                    GetTangent(vertices[indices[i + 1]], vertices[indices[i + 2]], vertices[indices[i]]);
                    GetTangent(vertices[indices[i + 2]], vertices[indices[i]], vertices[indices[i + 1]]);
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
    void MeshCreate::CreateGeosphere(Mesh &in_mesh, float in_radius, uint32_t in_numSubdivisions,
                                     VertexLayout in_vertexLayout) {
        in_mesh.m_vertexLayout = in_vertexLayout;
        in_mesh.m_sphere.SetRadius(in_radius);
        in_mesh.m_sphere.SetCenter(vec3(0, 0, 0));
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
                uint32_t numSubdivisions = in_numSubdivisions <= 6 ? in_numSubdivisions : 6;//细分次数小于等于6
                //构建正二十面体
                const float X = 0.525731f;
                const float Z = 0.850651f;
                vec3 pos[12] = {
                        vec3(-X, 0.0f, Z), vec3(X, 0.0f, Z),
                        vec3(-X, 0.0f, -Z), vec3(X, 0.0f, -Z),
                        vec3(0.0f, Z, X), vec3(0.0f, Z, -X),
                        vec3(0.0f, -Z, X), vec3(0.0f, -Z, -X),
                        vec3(Z, X, 0.0f), vec3(-Z, X, 0.0f),
                        vec3(Z, -X, 0.0f), vec3(-Z, -X, 0.0f)
                };
                uint32_t k[60] = {
                        1, 4, 0, 4, 9, 0, 4, 5, 9, 8, 5, 4, 1, 8, 4,
                        1, 10, 8, 10, 3, 8, 8, 3, 5, 3, 2, 5, 3, 7, 2,
                        3, 10, 7, 10, 6, 7, 6, 11, 7, 6, 0, 11, 6, 1, 0,
                        10, 1, 6, 11, 0, 9, 2, 11, 9, 5, 2, 9, 11, 2, 7
                };
                vector<VertexPosNorTanUv> vertices;
                vector<uint32_t> indices;
                vertices.resize(12);
                indices.assign(&k[0], &k[60]);
                for (uint32_t i = 0;i < 12;++i) {
                    vertices[i].position = pos[i];
                    vertices[i].normal = vec3(1, 0, 0);
                    vertices[i].tangent = vec3(0, 1, 0);
                    vertices[i].uv = vec2(0, 0);
                };
                for (uint32_t i = 0;i < numSubdivisions;++i) {
                    Subdivide(vertices, indices);
                };
                //将每个顶点投影到球面，推导其纹理坐标
                for (int j = 0;j < vertices.size();++j) {
                    vec3 normalizedpos = normalize(vertices[j].position);
                    vertices[j].position = in_radius * normalizedpos;
                    vertices[j].normal = normalizedpos;
                    //由球面坐标推导纹理
                    float theta = atan2f(vertices[j].position.z, vertices[j].position.x);
                    if (theta < 0.0f) {
                        theta = theta + PI2;
                    };
                    float phi = acosf(vertices[j].position.y / in_radius);
                    vertices[j].uv.x = theta / (PI2);
                    vertices[j].uv.y = phi / PI;
                    //求出P关于theta的偏导数
                    vec3 TangentnotNormal;
                    TangentnotNormal.x = -sinf(phi) * sinf(theta);
                    TangentnotNormal.y = 0.0f;
                    TangentnotNormal.z = sinf(phi) * cosf(theta);
                    vertices[j].tangent = normalize(TangentnotNormal);
                }
                for (int i = 0;i < indices.size();i = i + 3) {
                    GetTangent(vertices[indices[i]], vertices[indices[i + 1]], vertices[indices[i + 2]]);
                    GetTangent(vertices[indices[i + 1]], vertices[indices[i + 2]], vertices[indices[i]]);
                    GetTangent(vertices[indices[i + 2]], vertices[indices[i]], vertices[indices[i + 1]]);
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
    void MeshCreate::CreatePlane(Mesh &in_mesh, float in_x, float in_z, VertexLayout in_vertexLayout) {
        in_mesh.m_vertexLayout = in_vertexLayout;
        in_mesh.m_sphere.SetRadius(0.5f * sqrtf(in_x * in_x + in_z * in_z));
        in_mesh.m_sphere.SetCenter(vec3(0, 0, 0));
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
                        // Positions            // Normals          //u切向             // texture coords
                        {{-0.5f, 0.0f, 0.5f},  {0.0f, 1.0f, 0.0f}, {1.0f, 0.0f, 0.0f}, {0.0f, 0.0f}},
                        {{0.5f,  0.0f, 0.5f},  {0.0f, 1.0f, 0.0f}, {1.0f, 0.0f, 0.0f}, {1.0f, 0.0f}},
                        {{0.5f,  0.0f, -0.5f}, {0.0f, 1.0f, 0.0f}, {1.0f, 0.0f, 0.0f}, {1.0f, 1.0f}},
                        {{-0.5f, 0.0f, -0.5f}, {0.0f, 1.0f, 0.0f}, {1.0f, 0.0f, 0.0f}, {0.0f, 1.0f}}
                };
                for (int i = 0;i < vertices.size();++i) {
                    vertices[i].position.x *= (in_x);
                    vertices[i].position.y *= 0.0f;
                    vertices[i].position.z *= (in_z);
                }
                vector<uint32_t> indices = {
                        0, 1, 3, 1, 2, 3
                };
                for (int i = 0;i < indices.size();i = i + 3) {
                    GetTangent(vertices[indices[i]], vertices[indices[i + 1]], vertices[indices[i + 2]]);
                    GetTangent(vertices[indices[i + 1]], vertices[indices[i + 2]], vertices[indices[i]]);
                    GetTangent(vertices[indices[i + 2]], vertices[indices[i]], vertices[indices[i + 1]]);
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
    //加载没有子网格的模型
    void MeshCreate::LoadObjToMesh(Mesh &in_mesh, const string &in_url, VertexLayout in_vertexLayout) {
        in_mesh.m_vertexLayout = in_vertexLayout;
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
                using namespace tinyobj;
                string basurl = GetPathFromUrl(in_url);
                spdlog::info("文件夹：{}", basurl);
                attrib_t attrib;
                std::vector<shape_t> shapes;
                std::vector<material_t> materials;
                std::string warn, err;
                bool b_read = LoadObj(&attrib, &shapes, &materials, &warn, &err, in_url.c_str(), basurl.c_str());
                if (!b_read) {
                    std::cout << "读取obj文件失败" << in_url << std::endl;
                }
                spdlog::info("顶点数：{}", attrib.vertices.size() / 3);
                spdlog::info("法线数：{}", attrib.normals.size() / 3);
                spdlog::info("UV数：{}", attrib.texcoords.size() / 2);
                spdlog::info("子模型数：{}", shapes.size());
                spdlog::info("材质数：{}", materials.size());
                std::unordered_map<VertexPosNorTanUv, uint32_t> uniqueVertices{};
                vector<VertexPosNorTanUv> vertices;
                vector<uint32_t> indices;
                vec3 minpoint(99999);
                vec3 maxpoint(-99999);
                for (const auto &shape : shapes) {
                    for (const auto &index : shape.mesh.indices) {
                        VertexPosNorTanUv vertex;
                        vertex.position.x = attrib.vertices[index.vertex_index * 3 + 0];
                        vertex.position.y = attrib.vertices[index.vertex_index * 3 + 1];
                        vertex.position.z = attrib.vertices[index.vertex_index * 3 + 2];
                        vertex.normal.x = attrib.normals[index.normal_index * 3 + 0];
                        vertex.normal.y = attrib.normals[index.normal_index * 3 + 1];
                        vertex.normal.z = attrib.normals[index.normal_index * 3 + 2];
                        vertex.tangent = vec3(1, 0, 0);
                        vertex.uv.x = attrib.texcoords[index.texcoord_index * 2 + 0];
                        vertex.uv.y = 1.0f - attrib.texcoords[index.texcoord_index * 2 + 1];
                        if (uniqueVertices.count(vertex) == 0) {
                            uniqueVertices[vertex] = static_cast<uint32_t>(vertices.size());
                            vertices.push_back(vertex);
                        }
                        indices.push_back(uniqueVertices[vertex]);
                        minpoint.x = vertex.position.x < minpoint.x ? vertex.position.x : minpoint.x;
                        minpoint.y = vertex.position.y < minpoint.y ? vertex.position.y : minpoint.y;
                        minpoint.z = vertex.position.z < minpoint.z ? vertex.position.z : minpoint.z;
                        maxpoint.x = vertex.position.x > maxpoint.x ? vertex.position.x : maxpoint.x;
                        maxpoint.y = vertex.position.y > maxpoint.y ? vertex.position.y : maxpoint.y;
                        maxpoint.z = vertex.position.z > maxpoint.z ? vertex.position.z : maxpoint.z;
                    }
                }
                vec3 midpoint = 0.5f * (minpoint + maxpoint);
                float radius = 0.5f * glm::length(maxpoint - minpoint);
                in_mesh.m_sphere.SetRadius(radius);
                in_mesh.m_sphere.SetCenter(midpoint);
                in_mesh.m_indiceSize = indices.size();
                int facecount = indices.size() / 3;
                for (int j = 0;j < facecount;++j) {
                    GetTangent(vertices[indices[3 * j]], vertices[indices[3 * j + 1]],
                               vertices[indices[3 * j + 2]]);
                    GetTangent(vertices[indices[3 * j + 1]],
                               vertices[indices[3 * j + 2]], vertices[indices[3 * j]]);
                    GetTangent(vertices[indices[3 * j + 2]], vertices[indices[3 * j]],
                               vertices[indices[3 * j + 1]]);
                }
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
    void MeshCreate::LoadObjToMeshes(vector<Mesh *> &in_meshes, const string &in_url, VertexLayout in_vertexLayout) {
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
                using namespace tinyobj;
                string basurl = GetPathFromUrl(in_url);
                spdlog::info("文件夹：{}", basurl);
                attrib_t attrib;
                std::vector<shape_t> shapes;
                std::vector<material_t> materials;
                std::string warn, err;
                bool b_read = LoadObj(&attrib, &shapes, &materials, &warn, &err, in_url.c_str(), basurl.c_str());
                if (!b_read) {
                    std::cout << "读取obj文件失败" << in_url << std::endl;
                }
                spdlog::info("顶点数：{}", attrib.vertices.size() / 3);
                spdlog::info("法线数：{}", attrib.normals.size() / 3);
                spdlog::info("UV数：{}", attrib.texcoords.size() / 2);
                spdlog::info("子模型数：{}", shapes.size());
                spdlog::info("材质数：{}", materials.size());
                vector<uint32_t> splitindex;
                splitindex.push_back(-1);
                for (int i = 0;i < shapes[0].mesh.material_ids.size() - 1;++i) {
                    if (shapes[0].mesh.material_ids[i] != shapes[0].mesh.material_ids[i + 1]) {
                        splitindex.push_back(i);
                    }
                }
                splitindex.push_back(shapes[0].mesh.material_ids.size() - 1);
                vector<SubMeshMtrIdInfo> subMeshMtrIdInfo;
                subMeshMtrIdInfo.resize(splitindex.size() - 1);
                for (int i = 0;i < subMeshMtrIdInfo.size();++i) {
                    subMeshMtrIdInfo[i].startindex = splitindex[i] + 1;
                    subMeshMtrIdInfo[i].count = splitindex[i + 1] - splitindex[i];
                }
                std::unordered_map<VertexPosNorTanUv, uint32_t> uniqueVertices{};
                vector<VertexPosNorTanUv> vertices;
                vector<uint32_t> indices;
                vector<vec3> minpoints;
                vector<vec3> maxpoints;
                for (int i = 0;i < subMeshMtrIdInfo.size();++i) {
                    vec3 minpoint(99999);
                    vec3 maxpoint(-99999);
                    for (int j = subMeshMtrIdInfo[i].startindex*3;j < (subMeshMtrIdInfo[i].count+subMeshMtrIdInfo[i].startindex)*3;++j) {
                        index_t index = shapes[0].mesh.indices[j];
                        VertexPosNorTanUv vertex;
                        vertex.position.x = attrib.vertices[index.vertex_index * 3 + 0];
                        vertex.position.y = attrib.vertices[index.vertex_index * 3 + 1];
                        vertex.position.z = attrib.vertices[index.vertex_index * 3 + 2];
                        vertex.normal.x = attrib.normals[index.normal_index * 3 + 0];
                        vertex.normal.y = attrib.normals[index.normal_index * 3 + 1];
                        vertex.normal.z = attrib.normals[index.normal_index * 3 + 2];
                        vertex.tangent = vec3(1, 0, 0);
                        vertex.uv.x = attrib.texcoords[index.texcoord_index * 2 + 0];
                        vertex.uv.y = 1.0f - attrib.texcoords[index.texcoord_index * 2 + 1];
                        if (uniqueVertices.count(vertex) == 0) {
                            uniqueVertices[vertex] = static_cast<uint32_t>(vertices.size());
                            vertices.push_back(vertex);
                        }
                        indices.push_back(uniqueVertices[vertex]);

                        minpoint.x = vertex.position.x < minpoint.x ? vertex.position.x : minpoint.x;
                        minpoint.y = vertex.position.y < minpoint.y ? vertex.position.y : minpoint.y;
                        minpoint.z = vertex.position.z < minpoint.z ? vertex.position.z : minpoint.z;
                        maxpoint.x = vertex.position.x > maxpoint.x ? vertex.position.x : maxpoint.x;
                        maxpoint.y = vertex.position.y > maxpoint.y ? vertex.position.y : maxpoint.y;
                        maxpoint.z = vertex.position.z > maxpoint.z ? vertex.position.z : maxpoint.z;
                    }
                    minpoints.push_back(minpoint);
                    maxpoints.push_back(maxpoint);
                }
                int facecount = indices.size() / 3;
                for (int j = 0;j < facecount;++j) {
                    GetTangent(vertices[indices[3 * j]], vertices[indices[3 * j + 1]],
                               vertices[indices[3 * j + 2]]);
                    GetTangent(vertices[indices[3 * j + 1]],
                               vertices[indices[3 * j + 2]], vertices[indices[3 * j]]);
                    GetTangent(vertices[indices[3 * j + 2]], vertices[indices[3 * j]],
                               vertices[indices[3 * j + 1]]);
                }
                GLuint VBO;
                GLuint EBO;
                //定义顶点缓冲对象，将顶点数据复制到缓冲中
                glCreateBuffers(1, &VBO);
                glCreateBuffers(1, &EBO);
                glNamedBufferStorage(VBO, (long long) (vertices.size() * sizeof(VertexPosNorTanUv)),
                                     vertices.data(), 0);//创建缓冲并向其中写入数据
                glNamedBufferStorage(EBO, (long long) (indices.size() * sizeof(uint32_t)), indices.data(),
                                     0);
                for (int i = 0;i < subMeshMtrIdInfo.size();++i) {
                    Mesh* submesh = new Mesh();

                    vec3 midpoint = 0.5f * (minpoints[i] + maxpoints[i]);
                    float radius = 0.5f * glm::length(maxpoints[i] - minpoints[i]);
                    submesh->m_sphere.SetRadius(radius);
                    submesh->m_sphere.SetCenter(midpoint);
                    submesh->m_indiceSize = subMeshMtrIdInfo[i].count * 3;
                    submesh->m_indeceStart = subMeshMtrIdInfo[i].startindex * 3;

                    submesh->m_vertexLayout = VERT_POS_NOR_TAN_UV;
                    submesh->VBO = VBO;
                    submesh->EBO = EBO;
                    glCreateVertexArrays(1, &submesh->VAO);
                    glBindVertexArray(submesh->VAO);//开始记录顶点信息
                    glBindBuffer(GL_ARRAY_BUFFER, submesh->VBO);
                    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, submesh->EBO);
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
                    in_meshes.push_back(submesh);
                }
                break;
            }
            default:
                break;
        }
    }
}