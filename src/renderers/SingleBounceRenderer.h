
#ifndef RAYTRACER_SINGLEBOUNCERENDERER_H
#define RAYTRACER_SINGLEBOUNCERENDERER_H


#include "glWrappers/Texture.h"
#include "glWrappers/shaders/ComputeShader.h"
#include "glWrappers/TextureBuffer.h"
#include "ImguiUtils.h"
#include "utils/UniformDistribution.h"

typedef struct RenderSettings_s {
    // Path length Params
    bool russianRoulette;
    int constantPathLength;
    float alphaRR; // survival rate for russian roulette path length

    RenderSettings_s() {
        russianRoulette = false;
        constantPathLength = 1;
        alphaRR = 0.9;
    }
} RenderSettings;

class SingleBounceRenderer {

private:
    const unsigned int TARGET_BUFFER_TARGET = 0;

    const unsigned int POS_BUFFER_TARGET = 1;
    const unsigned int DIR_BUFFER_TARGET = 2;
    const unsigned int MULT_BUFFER_TARGET = 3;

    const GLenum POS_BUFFER_FORMAT = GL_RGBA32F;
    const GLenum DIR_BUFFER_FORMAT = GL_RGBA32F;
    const GLenum MULT_BUFFER_FORMAT = GL_RGBA32F;

    int mWidth;
    int mHeight;

    // TODO get TextureBuffers working ? -> More efficient creation
    Texture mPosBuffer;
    Texture mDirBuffer;
    Texture mMultiplierBuffer;

    ComputeShader mShader;
    ComputeShader mNewPathShader;
    ComputeShader mClearTargetShader;

    UniformDistribution dist;

public:
    SingleBounceRenderer(int width, int height);

    void render(Texture *target, RenderSettings renderSettings);

    void reloadShader();

    void clearTarget(Texture *target);

private:

    void renderSinglePass();

    void newPath();

};


#endif //RAYTRACER_SINGLEBOUNCERENDERER_H
