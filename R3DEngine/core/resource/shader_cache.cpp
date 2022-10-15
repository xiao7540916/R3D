//
// Created by Administrator on 2022/9/20.
//

#include "shader_cache.h"
extern std::string SHADER_DIR;
namespace R3D {
    void ShaderCache::Init() {
        Shader phone;
        phone.loadShader(SHADER_DIR + "phone/vert.glsl", SHADER_DIR + "phone/frag.glsl");
        m_shaders.insert({"phone", phone});
        Shader metalpbr;
        metalpbr.loadShader(SHADER_DIR + "metalpbr/vert.glsl", SHADER_DIR + "metalpbr/frag.glsl");
        m_shaders.insert({"metalpbr", metalpbr});
        Shader green;
        green.loadShader(SHADER_DIR + "green/vert.glsl", SHADER_DIR + "green/frag.glsl");
        m_shaders.insert({"green", green});
        Shader depth;
        depth.loadShader(SHADER_DIR + "depth/vert.glsl", SHADER_DIR + "depth/frag.glsl");
        m_shaders.insert({"depth", depth});
        Shader lightshow;
        lightshow.loadShader(SHADER_DIR + "lightshow/vert.glsl", SHADER_DIR + "lightshow/frag.glsl");
        m_shaders.insert({"lightshow", lightshow});
        Shader lightradius;
        lightradius.loadShader(SHADER_DIR + "lightradius/vert.glsl", SHADER_DIR + "lightradius/frag.glsl");
        m_shaders.insert({"lightradius", lightradius});
        Shader lightcull;
        vector<ShaderCreateDesc> lightculldesc = {{SHADER_DIR + "lightcull/lightcull.comp", GL_COMPUTE_SHADER}};
        lightcull.loadShader(lightculldesc);
        m_shaders.insert({"lightcull", lightcull});
        Shader gtao;
        gtao.loadShader(SHADER_DIR + "gtao/vert.glsl", SHADER_DIR + "gtao/frag.glsl");
        m_shaders.insert({"gtao", gtao});
        Shader blur;
        vector<ShaderCreateDesc> blurdesc = {{SHADER_DIR + "blur/blur.comp", GL_COMPUTE_SHADER}};
        blur.loadShader(blurdesc);
        m_shaders.insert({"blur", blur});
        Shader shadowmap;
        shadowmap.loadShader(SHADER_DIR + "shadowmap/vert.glsl", SHADER_DIR + "shadowmap/frag.glsl");
        m_shaders.insert({"shadowmap", shadowmap});
        Shader oitcolor;
        oitcolor.loadShader(SHADER_DIR + "oitcolor/vert.glsl", SHADER_DIR + "oitcolor/frag.glsl");
        m_shaders.insert({"oitcolor", oitcolor});
        Shader oitresolve;
        oitresolve.loadShader(SHADER_DIR + "oitresolve/vert.glsl", SHADER_DIR + "oitresolve/frag.glsl");
        m_shaders.insert({"oitresolve", oitresolve});
        Shader fxaa;
        fxaa.loadShader(SHADER_DIR + "fxaa/vert.glsl", SHADER_DIR + "fxaa/frag.glsl");
        m_shaders.insert({"fxaa", fxaa});
        Shader hdrtolow;
        hdrtolow.loadShader(SHADER_DIR + "hdrtolow/vert.glsl", SHADER_DIR + "hdrtolow/frag.glsl");
        m_shaders.insert({"hdrtolow", hdrtolow});
        Shader downsample;
        vector<ShaderCreateDesc> downsampledesc = {{SHADER_DIR + "bloom/downsample.comp", GL_COMPUTE_SHADER}};
        downsample.loadShader(downsampledesc);
        m_shaders.insert({"downsample", downsample});
        Shader upsample;
        vector<ShaderCreateDesc> upsampledesc = {{SHADER_DIR + "bloom/upsample.comp", GL_COMPUTE_SHADER}};
        upsample.loadShader(upsampledesc);
        m_shaders.insert({"upsample", upsample});
        Shader mergebloom;
        mergebloom.loadShader(SHADER_DIR + "bloom/mergebloomvert.glsl", SHADER_DIR + "bloom/mergebloomfrag.glsl");
        m_shaders.insert({"mergebloom", mergebloom});
    }
    Shader ShaderCache::GetShader(string in_shadertype) {
        if (m_shaders.find(in_shadertype) == m_shaders.end()) {
            std::cout << in_shadertype << " shader is not find" << std::endl;
            return m_shaders["phone"];
        } else {
            return m_shaders[in_shadertype];
        }
    }
    void ShaderCache::Release() {
        m_shaders.clear();
    }
}