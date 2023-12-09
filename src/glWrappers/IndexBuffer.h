#pragma once

class IndexBuffer
{
private:
    unsigned int m_GLId;
    unsigned int count;

    bool mValid;

public:
    IndexBuffer();

    IndexBuffer(unsigned int *data, unsigned int count);
    ~IndexBuffer();

    void bind();
    void unbind();

    unsigned int getCount();

    bool isValid();
};

