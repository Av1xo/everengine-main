cmake_minimum_required(VERSION 3.12)

set(ENGINE_PROJECT_NAME EverEngineCore)

# ---------------------
# PUBLIC HEADERS
# ---------------------
set(ENGINE_PUBLIC_INCLUDES 
    includes/EverEngineCore/Application.hpp
    includes/EverEngineCore/Log.hpp
    includes/EverEngineCore/Event.hpp
)

# ---------------------
# PRIVATE HEADERS
# ---------------------
set(ENGINE_PRIVATE_INCLUDES
    # Window
    src/EverEngineCore/Window.hpp

    # Platform
    src/EverEngineCore/Platform/Platform.hpp
    src/EverEngineCore/Platform/Generic/FileSystem.hpp

    # Rendering
    src/EverEngineCore/Rendering/OpenGL/Shader.hpp
    src/EverEngineCore/Rendering/OpenGL/VertexBuffer.hpp
    src/EverEngineCore/Rendering/OpenGL/VertexLayout.hpp

    # Runtime/HAL
    src/EverEngineCore/Runtime/HAL/CPUinfo.hpp
    src/EverEngineCore/Runtime/HAL/MemoryInfo.hpp
    src/EverEngineCore/Runtime/HAL/StorageInfo.hpp
)

# ---------------------
# SOURCE FILES
# ---------------------
set(ENGINE_PRIVATE_SOURCES
    src/EverEngineCore/Application.cpp
    src/EverEngineCore/Window.cpp

    # Platform
    src/EverEngineCore/Platform/Platform.cpp
    src/EverEngineCore/Platform/Generic/FileSystem.cpp

    # Rendering
    src/EverEngineCore/Rendering/OpenGL/Shader.cpp
    src/EverEngineCore/Rendering/OpenGL/VertexBuffer.cpp

    # Runtime/HAL
    src/EverEngineCore/Runtime/HAL/CPUinfo.cpp
    src/EverEngineCore/Runtime/HAL/MemoryInfo.cpp
    src/EverEngineCore/Runtime/HAL/StorageInfo.cpp
)

add_library(${ENGINE_PROJECT_NAME} STATIC
    ${ENGINE_PUBLIC_INCLUDES}
    ${ENGINE_PRIVATE_INCLUDES}
    ${ENGINE_PRIVATE_SOURCES}
)

target_include_directories(${ENGINE_PROJECT_NAME} PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/includes
)

target_include_directories(${ENGINE_PROJECT_NAME} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/includes
)

target_compile_features(${ENGINE_PROJECT_NAME} PUBLIC cxx_std_20)

set_target_properties(${ENGINE_PROJECT_NAME} PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin/
)

# ---------------------
# External libs
# ---------------------
add_subdirectory(../external/glfw ${CMAKE_CURRENT_BINARY_DIR}/glfw)
target_link_libraries(${ENGINE_PROJECT_NAME} PRIVATE glfw)

add_subdirectory(../external/spdlog ${CMAKE_CURRENT_BINARY_DIR}/spdlog)
target_link_libraries(${ENGINE_PROJECT_NAME} PRIVATE spdlog)

add_subdirectory(../external/glad ${CMAKE_CURRENT_BINARY_DIR}/glad)
target_link_libraries(${ENGINE_PROJECT_NAME} PRIVATE glad)

# ---------------------
# ImGui
# ---------------------
set(IMGUI_INCLUDES
    ../external/imgui/imgui.h
    ../external/imgui/backends/imgui_impl_glfw.h
    ../external/imgui/backends/imgui_impl_opengl3.h
)

set(IMGUI_SOURCES
    ../external/imgui/imgui.cpp
    ../external/imgui/backends/imgui_impl_glfw.cpp
    ../external/imgui/backends/imgui_impl_opengl3.cpp
    ../external/imgui/imgui_demo.cpp
    ../external/imgui/imgui_draw.cpp
    ../external/imgui/imgui_tables.cpp
    ../external/imgui/imgui_widgets.cpp
)

add_library(ImGui STATIC
    ${IMGUI_INCLUDES}
    ${IMGUI_SOURCES}
)

target_include_directories(ImGui PRIVATE ../external)
target_include_directories(ImGui PRIVATE ../external/imgui)
target_link_libraries(ImGui PRIVATE glad glfw)

target_include_directories(${ENGINE_PROJECT_NAME} PRIVATE ../external)
target_include_directories(${ENGINE_PROJECT_NAME} PRIVATE ../external/imgui)
target_link_libraries(${ENGINE_PROJECT_NAME} PRIVATE ImGui)
