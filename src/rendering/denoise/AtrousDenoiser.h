
#ifndef RAYTRACER_ATROUSDENOISER_H
#define RAYTRACER_ATROUSDENOISER_H


#include "Denoiser.h"

class AtrousDenoiser : public Denoiser {
    const std::string DENOISER_NAME = "Atrous";

    int currentIteration;

public:
    AtrousDenoiser();

    void buildImgui(DataStore &settings) override;

    void performDenoise(TmpBuffers &buffers, std::map<int, Texture> &textureStore, DataStore &settings) override;

    std::string getName() override;

private:
    void bindResources(TmpBuffers &buffers, std::map<int, Texture> &textureStore, DataStore &settings) override;
    void unBindResources(TmpBuffers &buffers, std::map<int, Texture> &textureStore, DataStore &settings) override;
};


#endif //RAYTRACER_ATROUSDENOISER_H
