
#include <stdexcept>
#include "HostBuffer.h"

HostBuffer::HostBuffer(int width, int height):
    mWidth(width), mHeight(height) {

    buffer = new glm::vec3[mWidth * mHeight];
}

HostBuffer::~HostBuffer() {
    delete[] buffer;
}

glm::vec3 HostBuffer::getPixel(int width, int height) {
    if (!validCoords(width, height)) throw std::runtime_error("Outside buffer bounds");
    if (buffer == nullptr) throw std::runtime_error("Buffer not initialized");

    return buffer[height * mWidth + width];
}

void HostBuffer::setPixel(int width, int height, glm::vec3 newVal) {
    if (!validCoords(width, height)) throw std::runtime_error("Outside buffer bounds");
    if (buffer == nullptr) throw std::runtime_error("Buffer not initialized");

    buffer[height * mWidth + width] = newVal;
}

bool HostBuffer::validCoords(int width, int height) {
    return 0 <= width && width < mWidth && 0 <= height && height < mHeight;
}

size_t HostBuffer::getByteSize() {
    return mWidth * mHeight * sizeof (float) * 3;
}

void *HostBuffer::getData() {
    return buffer;
}


