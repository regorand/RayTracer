#pragma once

#include "GL/glew.h"
#include "Texture.h"

class FrameBufferObject {
    unsigned int mGlId;


public:
    FrameBufferObject();

    void bind();
    void unbind();

    void addTextureAttachment(Texture &texture, GLenum attachment);
};
