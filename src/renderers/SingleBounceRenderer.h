
#ifndef RAYTRACER_SINGLEBOUNCERENDERER_H
#define RAYTRACER_SINGLEBOUNCERENDERER_H


#include "glWrappers/Texture.h"
#include "glWrappers/shaders/ComputeShader.h"
#include "glWrappers/TextureBuffer.h"

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

public:
    SingleBounceRenderer(int width, int height);

    void render(Texture *target, int pathLength);

    void reloadShader();

    void clearTarget(Texture *target);
private:

    void renderSinglePass();

    void newPath();

};


#endif //RAYTRACER_SINGLEBOUNCERENDERER_H
