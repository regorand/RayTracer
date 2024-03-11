
#ifndef RAYTRACER_DENOISER_H
#define RAYTRACER_DENOISER_H


#include <string>
#include "renderers/TmpBuffers.h"
#include "dataStructures/DataStore.h"
#include "glWrappers/shaders/ComputeShader.h"


class Denoiser {
protected:
    ComputeShader mShader;

public:
    Denoiser(std::string shaderName) : mShader(shaderName) {};
    virtual ~Denoiser() = default;

    virtual std::string getName() = 0;
    // virtual void performDenoise(TmpBuffers &buffers, DataStore &settings) = 0;
    virtual void performDenoise(TmpBuffers &buffers, DataStore &settings);

    virtual void buildImgui(DataStore &settings) = 0;

    void reloadShader();

protected:

    void performDenoiseIteration(TmpBuffers &buffers, DataStore &settings);

};


#endif //RAYTRACER_DENOISER_H
