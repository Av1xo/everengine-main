cmake_minimum_required(VERSION 3.12)

set(ENGINE_PROJECT_NAME EverEngineCore)

set(ENGINE_PUBLIC_INCLUDES 
    includes/EverEngineCore/Application.hpp
)

set(ENGINE_PUBLIC_SOURCES
    src/EverEngineCore/Application.cpp
)

add_library(${ENGINE_PROJECT_NAME} STATIC
    ${ENGINE_PUBLIC_INCLUDES}
    ${ENGINE_PUBLIC_SOURCES}
)

target_include_directories(${ENGINE_PROJECT_NAME} PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/includes
)

target_compile_features(${ENGINE_PROJECT_NAME} PUBLIC cxx_std_20)

set_target_properties(${ENGINE_PROJECT_NAME} PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin/
)

add_subdirectory(../external/glfw ${CMAKE_CURRENT_BINARY_DIR}/glfw)
target_link_libraries(${ENGINE_PROJECT_NAME} PRIVATE glfw)