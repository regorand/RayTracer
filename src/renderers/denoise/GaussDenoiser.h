
#ifndef RAYTRACER_GAUSSDENOISER_H
#define RAYTRACER_GAUSSDENOISER_H


#include "Denoiser.h"
#include "glWrappers/shaders/ComputeShader.h"

class GaussDenoiser : public Denoiser {

private:
    const std::string DENOISER_NAME = "Gauss";

public:
    GaussDenoiser();

    void buildImgui(DataStore &settings) override;

    std::string getName() override;
};


#endif //RAYTRACER_GAUSSDENOISER_H
