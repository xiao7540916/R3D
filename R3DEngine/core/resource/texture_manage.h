//
// Created by Administrator on 2022/9/10.
//

#pragma once
#include <structdefine.h>
#include <unordered_map>
#include <string>
using std::unordered_map;
using std::string;
namespace R3D {
    struct TextureLayout {
    };
    class TextureManage {
    public:
        unordered_map<string, GLuint> m_urlToTexture;
        int m_textureBindCache[16] = {0};
        ~TextureManage();
        TextureManage(const TextureManage &) = delete;
        TextureManage &operator=(const TextureManage &) = delete;
        static TextureManage *GetInstance();
        void Init();
        void Release();
        void LoadTexture(const string &in_url,GLint in_param = GL_REPEAT,GLint in_mipmapinfo = GL_LINEAR_MIPMAP_LINEAR);
        void LoadTextureEx(const string &in_url, const TextureLayout &in_textureLayout);
        GLuint GetTextureByUrl(const string &in_url);
    private:
        TextureManage();
        static TextureManage *m_textureManage;
    };
}



