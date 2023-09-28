cmake_minimum_required(VERSION 3.25)
project(RayTracer)

set(CMAKE_CXX_STANDARD 17)
add_executable(RayTracer src/main.cpp src/Application.cpp src/Application.h src/RayTracer.cpp src/RayTracer.h src/HostBuffer.cpp src/HostBuffer.h src/Scene.cpp src/Scene.h src/Camera.cpp src/Camera.h src/utils/fileUtils.hpp src/renderers/BufferRenderer.cpp src/renderers/BufferRenderer.h src/glWrappers/Shader.h src/glWrappers/Shader.cpp src/glWrappers/VertexArray.cpp src/glWrappers/VertexArray.h src/glWrappers/ArrayBuffer.cpp src/glWrappers/ArrayBuffer.h src/glWrappers/TextureBuffer.cpp src/glWrappers/TextureBuffer.h src/utils/glewIncludeUtils.h src/utils/asdf.h src/glWrappers/Texture.cpp src/glWrappers/Texture.h src/temp/PerlinNoise.hpp src/geometry/Intersections.hpp src/geometry/Objects.h src/graphics/Material.h)

set(PROJECT_ROOT_DIR ${PROJECT_SOURCE_DIR})

# Constants for lib includes
set(GLFW_DIR libs/glfw/)
set(IMGUI_DIR libs/imgui/)
set(GLM_DIR libs/glm/)
set(GLEW_DIR libs/glew)

# OpenGL
find_package(OpenGL REQUIRED)
if(NOT OPENGL_FOUND)
    message(ERROR " OPENGL not found!")
endif()


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

include_directories( ${OPENGL_INCLUDE_DIRS}  ${GLUT_INCLUDE_DIRS} )

add_subdirectory(${GLM_DIR})
target_link_libraries(RayTracer glm)

target_include_directories(RayTracer PUBLIC
        ${PROJECT_ROOT_DIR}/src # include so local includes are relative to sources root
        ${IMGUI_DIR}
        ${IMGUI_DIR}/backends
        ${OPENGL_INCLUDE_DIR}
        ${GLEW_DIR}/include
        )
