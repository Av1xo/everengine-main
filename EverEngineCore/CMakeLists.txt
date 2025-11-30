cmake_minimum_required(VERSION 3.12)

set(ENGINE_PROJECT_NAME EverEngineCore)

add_library(${ENGINE_PROJECT_NAME} STATIC
    src/EverEngineCore/Utils/test.cpp
)

target_include_directories(${ENGINE_PROJECT_NAME} PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/includes
)

target_compile_features(${ENGINE_PROJECT_NAME} PUBLIC cxx_std_20)

set_target_properties(${ENGINE_PROJECT_NAME} PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin/
)
