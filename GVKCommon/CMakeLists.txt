add_library(GVKCommon INTERFACE)

target_include_directories(GVKCommon
    INTERFACE
        include
)

target_compile_features(GVKCommon
    INTERFACE
        cxx_std_20
)
