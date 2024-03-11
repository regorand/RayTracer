#pragma once

#include "GL/glew.h"
#include "glWrappers/Texture.h"

typedef struct tmpBuffers_s {
    Texture tmp1;
    Texture tmp2;

    bool tex1Source = true;

    Texture &getSourceTex() {
        return tex1Source ? tmp1 : tmp2;
    }

    Texture &getTargetTex() {
        return !tex1Source ? tmp1 : tmp2;
    }

    void swap() {
        tex1Source = !tex1Source;
    }

    tmpBuffers_s(int width, int height):
            tmp1(Texture(width, height)),
            tmp2(Texture(width, height))
    {
        tmp1.setAccessType(GL_READ_WRITE);
        tmp2.setAccessType(GL_READ_WRITE);
    }

} TmpBuffers;