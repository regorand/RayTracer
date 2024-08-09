cmake_minimum_required(VERSION 3.25)
project(RayTracer)

set(CMAKE_CXX_STANDARD 17)
add_executable(RayTracer
        src/main.cpp src/Application.cpp src/Application.h src/scene/camera/Camera.cpp src/scene/camera/Camera.h src/utils/fileUtils.hpp src/renderers/BufferRenderer.cpp src/renderers/BufferRenderer.h src/glWrappers/rasterization/VertexArray.cpp src/glWrappers/rasterization/VertexArray.h src/glWrappers/rasterization/ArrayBuffer.cpp src/glWrappers/rasterization/ArrayBuffer.h src/glWrappers/TextureBuffer.cpp src/glWrappers/TextureBuffer.h src/utils/glewIncludeUtils.h src/glWrappers/Texture.cpp src/glWrappers/Texture.h src/glWrappers/shaders/ShaderUtils.hpp src/glWrappers/shaders/ShaderLoad.cpp src/glWrappers/shaders/ShaderLoad.h src/gui/ImguiUtils.cpp src/gui/ImguiUtils.h src/utils/RandomEngine.cpp src/utils/RandomEngine.h src/renderers/PathTracer.cpp src/renderers/PathTracer.h src/deviceShared/BufferBindings.h src/deviceShared/PostProcessFlags.h src/renderers/PostProcessor.cpp src/renderers/PostProcessor.h src/deviceShared/MaterialFlags.h src/renderers/Rasterizer.cpp src/renderers/Rasterizer.h src/glWrappers/rasterization/IndexBuffer.cpp src/glWrappers/rasterization/IndexBuffer.h src/glWrappers/ShaderStorageBuffer.h src/glWrappers/ShaderStorageBuffer.cpp src/userInput/KeyStateManager.cpp src/userInput/KeyStateManager.h src/userInput/CameraManipulator.cpp src/userInput/CameraManipulator.h src/dataStructures/DataStore.cpp src/dataStructures/DataStore.h src/dataStructures/SettingIndex.h src/gui/ImGuiSidebarTab.h src/dataStructures/Events.h src/scene/camera/CameraController.cpp src/scene/camera/CameraController.h src/renderers/denoise/Denoiser.h src/renderers/TmpBuffers.h src/renderers/denoise/GaussDenoiser.cpp src/renderers/denoise/GaussDenoiser.h src/renderers/denoise/BoxDenoiser.cpp src/renderers/denoise/BoxDenoiser.h src/renderers/denoise/Denoiser.cpp src/glWrappers/FrameBufferObject.cpp src/glWrappers/FrameBufferObject.h src/glWrappers/TextureFactory.cpp src/glWrappers/TextureFactory.h src/renderers/denoise/AtrousDenoiser.cpp src/renderers/denoise/AtrousDenoiser.h src/dataStructures/TextureNames.h src/scene/scene_loading/ModelLoader.cpp src/scene/scene_loading/ModelLoader.h src/scene/Scene.cpp src/scene/Scene.h src/scene/camera/CameraManipulator.cpp src/scene/camera/CameraManipulator.h src/scene/buffers/import_scene_error.h src/scene/Material.h src/scene/buffers/RasterizeBuffers.h
        src/glWrappers/GLBuffer.cpp
        src/glWrappers/GLBuffer.h
        src/glWrappers/shaders/Shader.cpp
        src/glWrappers/shaders/Shader.h
        src/glWrappers/shaders/ShaderProgram.cpp
        src/glWrappers/shaders/ShaderProgram.h
        src/scene/SceneBuffers.h
        src/scene/SceneImporter.cpp
        src/scene/SceneImporter.h
        src/scene/SceneTypes.h
)

set(PROJECT_ROOT_DIR ${PROJECT_SOURCE_DIR})

# Constants for lib includes
set(GLFW_DIR libs/glfw/)
set(IMGUI_DIR libs/imgui/)
set(GLM_DIR libs/glm/)
set(GLEW_DIR libs/glew)
set(TINY_GLTF_DIR libs/tinygltf)


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

add_subdirectory(${TINY_GLTF_DIR})
target_link_libraries(RayTracer tinygltf)

target_include_directories(RayTracer PUBLIC
        ${PROJECT_ROOT_DIR}/src # include so local includes are relative to sources root
        ${IMGUI_DIR}
        ${IMGUI_DIR}/backends
        ${OPENGL_INCLUDE_DIR}
        ${GLEW_DIR}/include
        )
