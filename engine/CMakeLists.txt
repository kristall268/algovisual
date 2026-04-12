file(GLOB_RECURSE AV_ENGINE_SOURCES CONFIGURE_DEPENDS src/*.cpp)
file(GLOB_RECURSE AV_ENGINE_HEADERS CONFIGURE_DEPENDS include/*.h src/*.h)

add_library(av_engine STATIC ${AV_ENGINE_SOURCES} ${AV_ENGINE_HEADERS})
add_library(av::engine ALIAS av_engine)

target_include_directories(av_engine
    PUBLIC  ${CMAKE_CURRENT_SOURCE_DIR}/include
    PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src
)

target_link_libraries(av_engine
    PUBLIC
        Vulkan::Vulkan
        imgui::imgui
        spdlog::spdlog
        $<IF:$<TARGET_EXISTS:SDL3::SDL3>,SDL3::SDL3,PkgConfig::SDL3>
)

target_compile_definitions(av_engine PUBLIC
    $<$<CONFIG:Debug>:AV_DEBUG APP_USE_VULKAN_DEBUG_REPORT>
)

if(MSVC)
    target_compile_options(av_engine PRIVATE /W4 /MP /permissive-)
    target_compile_definitions(av_engine PUBLIC NOMINMAX WIN32_LEAN_AND_MEAN)
else()
    target_compile_options(av_engine PRIVATE -Wall -Wextra -Wpedantic)
endif()
