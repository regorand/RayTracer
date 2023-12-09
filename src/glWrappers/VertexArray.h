#pragma once

#include "utils/glewIncludeUtils.h"

class VertexArray
{
private:
    unsigned int m_GlId;

    bool mValid;

public:
    VertexArray();
    ~VertexArray();

    void bind();
    void unbind();

    void setValid(bool isValid);
    bool isValid();
};

