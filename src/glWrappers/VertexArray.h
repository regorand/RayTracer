#pragma once

#include "utils/glewIncludeUtils.h"

class VertexArray
{
private:
    unsigned int m_GlId;

public:
    VertexArray();
    ~VertexArray();

    void bind();
    void unbind();
};

