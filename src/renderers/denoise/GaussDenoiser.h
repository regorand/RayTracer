
#ifndef RAYTRACER_GAUSSDENOISER_H
#define RAYTRACER_GAUSSDENOISER_H


#include "Denoiser.h"

class GaussDenoiser : public Denoiser {

private:
    const std::string DENOISER_NAME = "Gauss";

public:
    GaussDenoiser();

    void buildImgui(DataStore &settings) override;

    std::string getName() override;

private:
    void bindResources(TmpBuffers &buffers, std::map<int, Texture> &textureStore, DataStore &settings) override;
    void unBindResources(TmpBuffers &buffers, std::map<int, Texture> &textureStore, DataStore &settings) override;
};


#endif //RAYTRACER_GAUSSDENOISER_H
