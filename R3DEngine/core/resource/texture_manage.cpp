//
// Created by Administrator on 2022/9/10.
//
#define STB_IMAGE_IMPLEMENTATION
#include "texture_manage.h"
#include <iostream>
#include <device/device.h>
#include <stb_image.h>
#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <random>
extern string CURRENT_SOURCE_DIR;
namespace R3D {
    TextureManage::~TextureManage() {
    }
    TextureManage *TextureManage::m_textureManage = nullptr;
    TextureManage *TextureManage::GetInstance() {
        if (m_textureManage == nullptr) {
            m_textureManage = NEW TextureManage();
        }
        return m_textureManage;
    }
    TextureManage::TextureManage() {
    }
    void TextureManage::LoadTexture(const string &in_url, GLint in_repeatinfo, GLint in_mipmapinfo) {
        if (m_urlToTexture.find(in_url) == m_urlToTexture.end()) {
            std::cout << in_url << std::endl;
            //纹理
            GLuint texture;
            glGenTextures(1, &texture);
            glBindTexture(GL_TEXTURE_2D, texture);
            //设置纹理包装和过滤的方式
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, in_repeatinfo);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, in_repeatinfo);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, in_mipmapinfo);//设置mipmap采样方式
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, in_mipmapinfo);
            int width, height, nrChannels;
//            stbi_set_flip_vertically_on_load(true);//翻转y轴
            unsigned char *data = stbi_load(in_url.c_str(), &width,
                                            &height, &nrChannels, 0);
            GLint format;
            //根据图片格式自动选择加载方式
            switch (nrChannels) {
                case 1:
                    format = GL_RED;
                    break;
                case 2:
                    format = GL_RG;
                    break;
                case 3:
                    format = GL_RGB;
                    break;
                case 4:
                    format = GL_RGBA;
                    break;
                default:
                    std::cout << "set format false" << std::endl;
            }
            if (!data) {
                std::cout << "加载" << in_url << "失败" << std::endl;
                abort();
            } else {
                glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
                glGenerateMipmap(GL_TEXTURE_2D);
                //查询是否支持各向异性
                if (glfwExtensionSupported("GL_EXT_texture_filter_anisotropic")) {
                    GLfloat anisoSetting = 0.0f;
                    glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY, &anisoSetting);
                    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY, anisoSetting);
                }
            }
            stbi_image_free(data);
            m_urlToTexture.insert({in_url, texture});
        }
    }
    void
    TextureManage::LoadTextureEx(const string &in_url, const TextureLayout &in_textureLayout) {
    }
    void TextureManage::Release() {
        for (auto item = m_urlToTexture.begin();item != m_urlToTexture.end();item++) {
            glDeleteTextures(1, &item->second);
        }
        m_urlToTexture.clear();
        //m_urlToTexture自身占用空间仍需要清理
    }
    void TextureManage::Init() {
        LoadTexture(CURRENT_SOURCE_DIR + "Data/image/black.png");
        LoadTexture(CURRENT_SOURCE_DIR + "Data/image/white.png");
        LoadTexture(CURRENT_SOURCE_DIR + "Data/image/normal.png");
        LoadTexture(CURRENT_SOURCE_DIR + "Data/image/bluenoise.png");
        // Noise texture
        std::vector<glm::vec3> ssaoNoise;
        int noiseSize = 256;
        ssaoNoise.resize(noiseSize*noiseSize);
        std::uniform_real_distribution<GLfloat> randomFloats(0.0, 1.0);
        std::default_random_engine generator;
        for (GLuint i = 0;i < ssaoNoise.size();i++) {
            glm::vec3 noise(randomFloats(generator) * 2.0 - 1.0, randomFloats(generator) * 2.0 - 1.0,
                            randomFloats(generator)); // rotate around z-axis (in tangent space)
            ssaoNoise[i] = noise;
        }
        GLuint noiseTexture;
        glGenTextures(1, &noiseTexture);
        glBindTexture(GL_TEXTURE_2D, noiseTexture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, noiseSize, noiseSize, 0, GL_RGB, GL_FLOAT, ssaoNoise.data());
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        m_urlToTexture.insert({"noiseTexture", noiseTexture});
    }
    GLuint TextureManage::GetTextureByUrl(const string &in_url) {
        return m_urlToTexture[in_url];
    }
}