cmake_minimum_required(VERSION 3.25)
project(RayTracer)

set(CMAKE_CXX_STANDARD 17)
add_executable(RayTracer
        src/main.cpp src/Application.cpp src/Application.h src/HostBuffer.cpp src/HostBuffer.h src/scene/Scene.cpp src/scene/Scene.h src/scene/Camera.cpp src/scene/Camera.h src/utils/fileUtils.hpp src/renderers/BufferRenderer.cpp src/renderers/BufferRenderer.h src/glWrappers/shaders/Shader.h src/glWrappers/shaders/Shader.cpp src/glWrappers/VertexArray.cpp src/glWrappers/VertexArray.h src/glWrappers/ArrayBuffer.cpp src/glWrappers/ArrayBuffer.h src/glWrappers/TextureBuffer.cpp src/glWrappers/TextureBuffer.h src/utils/glewIncludeUtils.h src/glWrappers/Texture.cpp src/glWrappers/Texture.h src/glWrappers/shaders/ComputeShader.cpp src/glWrappers/shaders/ComputeShader.h src/glWrappers/shaders/ShaderUtils.hpp src/glWrappers/shaders/ShaderLoad.cpp src/glWrappers/shaders/ShaderLoad.h src/ImguiUtils.cpp src/ImguiUtils.h src/utils/UniformDistribution.cpp src/utils/UniformDistribution.h src/renderers/PathTracer.cpp src/renderers/PathTracer.h src/glWrappers/shaders/SSBO.cpp src/glWrappers/shaders/SSBO.h src/renderers/Types.h src/deviceShared/BufferBindings.h src/scene/SceneBuffers.cpp src/scene/SceneBuffers.h src/scene/MeshObject.cpp src/scene/MeshObject.h src/utils/SceneCreateUtils.cpp src/utils/SceneCreateUtils.h src/deviceShared/PostProcessFlags.h src/renderers/PostProcessor.cpp src/renderers/PostProcessor.h src/deviceShared/MaterialFlags.h)

set(PROJECT_ROOT_DIR ${PROJECT_SOURCE_DIR})

# Constants for lib includes
set(GLFW_DIR libs/glfw/)
set(IMGUI_DIR libs/imgui/)
set(GLM_DIR libs/glm/)
set(GLEW_DIR libs/glew)

# OpenGL
find_package(OpenGL REQUIRED)
if (NOT OPENGL_FOUND)
    message(ERROR " OPENGL not found!")
endif ()


add_subdirectory(${GLEW_DIR}/build/cmake)
target_link_libraries(RayTracer glew_s)

# TODO
target_sources(RayTracer PUBLIC
        "${IMGUI_DIR}/imgui.h"
        "${IMGUI_DIR}/imgui.cpp"
        "${IMGUI_DIR}/imgui_draw.cpp"
        "${IMGUI_DIR}/imgui_tables.cpp"
        "${IMGUI_DIR}/imgui_widgets.cpp"
        "${IMGUI_DIR}/backends/imgui_impl_glfw.cpp"
        "${IMGUI_DIR}/backends/imgui_impl_opengl3.cpp"
        )

# We only use glfw, no need to test it or build the docs for us
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)

# Add and link libs
add_subdirectory(${GLFW_DIR})
target_link_libraries(RayTracer glfw ${GLFW_LIBRARIES})

target_link_libraries(RayTracer ${OPENGL_LIBRARIES})

include_directories(${OPENGL_INCLUDE_DIRS} ${GLUT_INCLUDE_DIRS})

add_subdirectory(${GLM_DIR})
target_link_libraries(RayTracer glm)

target_include_directories(RayTracer PUBLIC
        ${PROJECT_ROOT_DIR}/src # include so local includes are relative to sources root
        ${IMGUI_DIR}
        ${IMGUI_DIR}/backends
        ${OPENGL_INCLUDE_DIR}
        ${GLEW_DIR}/include
        )
