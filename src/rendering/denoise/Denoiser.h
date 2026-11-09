
#ifndef RAYTRACER_DENOISER_H
#define RAYTRACER_DENOISER_H


#include <string>
#include "rendering/TmpBuffers.h"
#include "dataStructures/DataStore.h"
#include "glWrappers/shaders/ShaderProgram.h"
#include "glWrappers/shaders/ShaderUtils.hpp"


class Denoiser {
protected:
    ShaderProgram mShader;

public:
    Denoiser(const std::string& shaderName) : mShader(makeComputeProgram(shaderName)) {};
    virtual ~Denoiser() = default;

    virtual std::string getName() = 0;
    // virtual void performDenoise(TmpBuffers &buffers, DataStore &settings) = 0;
    virtual void performDenoise(TmpBuffers &buffers, std::map<int, Texture> &textureStore, DataStore &settings);

    virtual void buildImgui(DataStore &settings) = 0;

    void reloadShader();

protected:

    void performDenoiseIteration(TmpBuffers &buffers, std::map<int, Texture> &textureStore, DataStore &settings);

    virtual void bindResources(TmpBuffers &buffers, std::map<int, Texture> &textureStore, DataStore &settings) = 0;
    virtual void unBindResources(TmpBuffers &buffers, std::map<int, Texture> &textureStore, DataStore &settings) = 0;
};


#endif //RAYTRACER_DENOISER_H
