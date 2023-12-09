#include "VertexArray.h"

VertexArray::VertexArray()
    :m_GlId(0), mValid(true)
{
    unbind();
    glGenVertexArrays(1, &m_GlId);
    glBindVertexArray(m_GlId);
}

VertexArray::~VertexArray()
{
    // glDeleteVertexArrays(1, &m_GlId);
}

void VertexArray::bind()
{
    glBindVertexArray(m_GlId);
}

void VertexArray::unbind()
{
    glBindVertexArray(0);
}

void VertexArray::setValid(bool isValid) {
    mValid = isValid;
}

bool VertexArray::isValid() {
    return mValid;
}
