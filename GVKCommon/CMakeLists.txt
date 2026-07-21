add_library(GVKCommon INTERFACE)
add_subdirectory(external/glm)

target_include_directories(GVKCommon INTERFACE
        include
        external
)

target_link_libraries(GVKCommon INTERFACE glm)

target_compile_features(GVKCommon
    INTERFACE
        cxx_std_20
)
