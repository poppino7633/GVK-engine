cmake_minimum_required(VERSION 3.29)


set(CMAKE_C_COMPILER clang)
set(CMAKE_CXX_COMPILER clang++)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

project(GVK)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/CMake")

find_package(Vulkan 1.4.335 REQUIRED)


find_program(SLANGC_EXECUTABLE
    slangc
    HINTS $ENV{VULKAN_SDK}/bin
    REQUIRED
)

add_subdirectory(GVKCommon)
add_subdirectory(GVKRender)
add_subdirectory(GVKAsset)
add_subdirectory(demo)
