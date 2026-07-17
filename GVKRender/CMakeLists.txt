file(GLOB_RECURSE RENDER_SOURCES
    CONFIGURE_DEPENDS
    src/*.cpp
)

add_subdirectory(external/glfw)

add_library(GVKRender
    ${RENDER_SOURCES}
)

target_include_directories(GVKRender
    PUBLIC
        include
)

target_link_libraries(GVKRender
    PUBLIC
        GVKCommon
        Vulkan::Vulkan
        glfw
)

target_compile_features(GVKRender
    PUBLIC
        cxx_std_20
)

target_compile_definitions(GVKRender
    PUBLIC
        VULKAN_HPP_DISPATCH_LOADER_DYNAMIC=1
        VULKAN_HPP_NO_STRUCT_CONSTRUCTORS=1
        VULKAN_HPP_HANDLE_ERROR_OUT_OF_DATE_AS_SUCCESS
)
