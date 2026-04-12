file(GLOB_RECURSE APP_SOURCES CONFIGURE_DEPENDS src/*.cpp src/*.h)

add_executable(algovisual ${APP_SOURCES})
target_link_libraries(algovisual PRIVATE av::engine)

# Shaders and assets — copy them next to the exe file
av_compile_shaders(algovisual
    SOURCES ${CMAKE_SOURCE_DIR}/assets/shaders/ui.vert
            ${CMAKE_SOURCE_DIR}/assets/shaders/ui.frag
    OUTPUT_DIR $<TARGET_FILE_DIR:algovisual>/assets/shaders
)
av_copy_assets(algovisual
    FROM ${CMAKE_SOURCE_DIR}/assets/fonts
         ${CMAKE_SOURCE_DIR}/assets/icons
    TO   $<TARGET_FILE_DIR:algovisual>/assets
)
