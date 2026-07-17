file(GLOB_RECURSE DEMO_SOURCES
    CONFIGURE_DEPENDS
    src/*.cpp
)

add_executable(demo
    ${DEMO_SOURCES}
)

target_link_libraries(demo
    PRIVATE
        GVKRender
        GVKAsset
)

target_include_directories(demo
    PRIVATE
        include
)

target_compile_features(demo
    PRIVATE
        cxx_std_20
)

set(DEMO_OUTPUT_DIR
    ${CMAKE_BINARY_DIR}/demo
)

set_target_properties(demo PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${DEMO_OUTPUT_DIR}
)

#
# Slang shaders
#

set(SHADER_TARGET DemoShaders)

set(SHADERS_DIR
    ${DEMO_OUTPUT_DIR}/shaders
)

set(SHADER_SOURCE
    ${CMAKE_CURRENT_SOURCE_DIR}/shaders/shader.slang
)

set(ENTRY_POINTS
    -entry vertMain
    -entry fragMain
)

add_custom_command(
    OUTPUT
        ${SHADERS_DIR}

    COMMAND
        ${CMAKE_COMMAND} -E make_directory ${SHADERS_DIR}

    COMMENT
        "Creating shader output directory"
)

add_custom_command(
    OUTPUT
        ${SHADERS_DIR}/slang.spv

    COMMAND
        ${SLANGC_EXECUTABLE}
            ${SHADER_SOURCE}
            -target spirv
            -profile spirv_1_4
            -emit-spirv-directly
            -fvk-use-entrypoint-name
            ${ENTRY_POINTS}
            -o
            ${SHADERS_DIR}/slang.spv

    DEPENDS
        ${SHADER_SOURCE}

    COMMENT
        "Compiling Slang shaders"

    VERBATIM
)

add_custom_target(
    ${SHADER_TARGET}
    DEPENDS
        ${SHADERS_DIR}/slang.spv
)

add_dependencies(
    demo
    ${SHADER_TARGET}
)

#
# Assets
#

set(ASSET_DIR
    ${CMAKE_CURRENT_SOURCE_DIR}/assets
)

add_custom_command(
    TARGET demo
    POST_BUILD

    COMMAND
        ${CMAKE_COMMAND} -E copy_directory
        ${ASSET_DIR}
        ${DEMO_OUTPUT_DIR}/assets

    COMMENT
        "Copying assets"
)
