
#include "FrameBufferObject.h"
#include "GL/glew.h"

FrameBufferObject::FrameBufferObject() {
    glGenFramebuffers(1, &mGlId);
}

void FrameBufferObject::addTextureAttachment(Texture &texture, GLenum attachment) {
    bind();
    glFramebufferTexture2D(GL_FRAMEBUFFER, attachment, GL_TEXTURE_2D, texture.getGlID(), 0);
    unbind();
}

void FrameBufferObject::bind() {
    glBindFramebuffer(GL_FRAMEBUFFER, mGlId);
}

void FrameBufferObject::unbind() {
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
