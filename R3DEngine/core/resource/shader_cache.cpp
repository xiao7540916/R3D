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
        lightcull.loadCompShader(SHADER_DIR + "lightcull/lightcull.comp");
        m_shaders.insert({"lightcull", lightcull});
        Shader gtao;
        gtao.loadShader(SHADER_DIR + "gtao/vert.glsl", SHADER_DIR + "gtao/frag.glsl");
        m_shaders.insert({"gtao", gtao});
        Shader blur;
        blur.loadCompShader(SHADER_DIR + "blur/blur.comp");
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
        Shader hdrtogama;
        hdrtogama.loadShader(SHADER_DIR + "hdrtogama/vert.glsl", SHADER_DIR + "hdrtogama/frag.glsl");
        m_shaders.insert({"hdrtogama", hdrtogama});
        Shader downsample;
        downsample.loadCompShader(SHADER_DIR + "bloom/downsample.comp");
        m_shaders.insert({"downsample", downsample});
        Shader upsample;
        upsample.loadCompShader(SHADER_DIR + "bloom/upsample.comp");
        m_shaders.insert({"upsample", upsample});
        Shader mergebloom;
        mergebloom.loadShader(SHADER_DIR + "bloom/mergebloomvert.glsl", SHADER_DIR + "bloom/mergebloomfrag.glsl");
        m_shaders.insert({"mergebloom", mergebloom});
        Shader depthtococ;
        depthtococ.loadCompShader(SHADER_DIR + "depthoffield/depthtococ.comp");
        m_shaders.insert({"depthtococ", depthtococ});
        Shader circlesample;
        circlesample.loadCompShader(SHADER_DIR + "depthoffield/circlesample.comp");
        m_shaders.insert({"circlesample", circlesample});
        Shader cocdownsample;
        cocdownsample.loadCompShader(SHADER_DIR + "depthoffield/cocdownsample.comp");
        m_shaders.insert({"cocdownsample", cocdownsample});
        Shader mergedof;
        mergedof.loadShader(SHADER_DIR + "depthoffield/mergedofvert.glsl",
                            SHADER_DIR + "depthoffield/mergedoffrag.glsl");
        m_shaders.insert({"mergedof", mergedof});
        Shader depthmin;
        depthmin.loadCompShader(SHADER_DIR + "sssr/depthmin.comp");
        m_shaders.insert({"depthmin", depthmin});
        Shader tracehitpixel;
        tracehitpixel.loadCompShader(SHADER_DIR + "sssr/tracehitpixel.comp");
        m_shaders.insert({"tracehitpixel", tracehitpixel});
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