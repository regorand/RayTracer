
#ifndef RAYTRACER_HOSTBUFFER_H
#define RAYTRACER_HOSTBUFFER_H

#include "glm/vec3.hpp"

class HostBuffer {

private:
    int mWidth, mHeight;

    glm::vec3 *buffer = nullptr;
public:
    HostBuffer(int width, int height);

    ~HostBuffer();

    [[nodiscard]] int getWidth() const {return mWidth;};
    [[nodiscard]] int getHeight() const {return mHeight;};

    glm::vec3 getPixel(int width, int height);
    void setPixel(int width, int height, glm::vec3 newVal);

    void *getData();

    size_t getByteSize();

private:
    bool validCoords(int width, int height);

};


#endif //RAYTRACER_HOSTBUFFER_H
