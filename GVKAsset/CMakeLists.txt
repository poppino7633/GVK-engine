file(GLOB_RECURSE ASSET_SOURCES
    CONFIGURE_DEPENDS
    src/*.cpp
)


add_library(GVKAsset
    ${ASSET_SOURCES}
)

target_include_directories(GVKAsset
    PUBLIC
        include
    PRIVATE
        external
)

target_link_libraries(GVKAsset
    PUBLIC
        GVKCommon
)

target_compile_features(GVKAsset
    PUBLIC
        cxx_std_20
)
