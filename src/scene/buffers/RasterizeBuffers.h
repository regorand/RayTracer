#pragma once

#include "glWrappers/rasterization/ArrayBuffer.h"
#include "glWrappers/rasterization/VertexArray.h"
#include "glWrappers/rasterization/IndexBuffer.h"

typedef struct raster_buffers_s {
    VertexArray mVa;
    ArrayBuffer mAb;
    IndexBuffer mIb;
} RasterBuffers;