
#ifndef RAYTRACER_BOXDENOISER_H
#define RAYTRACER_BOXDENOISER_H


#include "Denoiser.h"
#include "glWrappers/shaders/ComputeShader.h"

class BoxDenoiser : public Denoiser {
    const std::string DENOISER_NAME = "Box";

    // ComputeShader mShader;

public:
    BoxDenoiser();

    void buildImgui(DataStore &settings) override;

    std::string getName() override;

private:
    void bindResources(TmpBuffers &buffers, std::map<int, Texture> &textureStore, DataStore &settings) override;
    void unBindResources(TmpBuffers &buffers, std::map<int, Texture> &textureStore, DataStore &settings) override;
};


#endif //RAYTRACER_BOXDENOISER_H
