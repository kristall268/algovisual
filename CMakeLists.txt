cmake_minimum_required(VERSION 3.21)
project(algovisual LANGUAGES CXX)

if(APPLE)
    message(FATAL_ERROR "macOS пока не поддерживается (планируется через MoltenVK).")
endif()

# ---- Global settings ----------------------------------------
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "" FORCE)
endif()

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# ---- Project options ----------------------------------------
option(AV_BUILD_EXAMPLES "Build example mini-apps" ON)
option(AV_BUILD_TESTS    "Build unit tests"        OFF)

# ---- Dependencies (системные / vcpkg) -----------------------
find_package(Vulkan REQUIRED)

find_package(SDL3 CONFIG QUIET)
if(NOT SDL3_FOUND AND UNIX)
    find_package(PkgConfig REQUIRED)
    pkg_check_modules(SDL3 REQUIRED IMPORTED_TARGET sdl3)
endif()

find_package(spdlog CONFIG REQUIRED)

# Для компиляции шейдеров — ищем glslangValidator
find_program(GLSLANG_VALIDATOR glslangValidator
    HINTS ENV VULKAN_SDK PATH_SUFFIXES bin)
if(NOT GLSLANG_VALIDATOR)
    message(WARNING "glslangValidator не найден — шейдеры не будут компилироваться.")
endif()

include(cmake/CompileShaders.cmake)
include(cmake/CopyAssets.cmake)

# ---- ImGui (vendored static lib) ----------------------------
set(IMGUI_DIR "${CMAKE_SOURCE_DIR}/external/imgui" CACHE PATH "")
if(NOT EXISTS "${IMGUI_DIR}/imgui.cpp")
    message(FATAL_ERROR "ImGui не найден в ${IMGUI_DIR}. git clone https://github.com/ocornut/imgui external/imgui")
endif()

add_library(imgui STATIC
    ${IMGUI_DIR}/imgui.cpp
    ${IMGUI_DIR}/imgui_demo.cpp
    ${IMGUI_DIR}/imgui_draw.cpp
    ${IMGUI_DIR}/imgui_tables.cpp
    ${IMGUI_DIR}/imgui_widgets.cpp
    ${IMGUI_DIR}/backends/imgui_impl_sdl3.cpp
    ${IMGUI_DIR}/backends/imgui_impl_vulkan.cpp
)
target_include_directories(imgui PUBLIC ${IMGUI_DIR} ${IMGUI_DIR}/backends)
target_link_libraries(imgui PUBLIC Vulkan::Vulkan
    $<IF:$<TARGET_EXISTS:SDL3::SDL3>,SDL3::SDL3,PkgConfig::SDL3>)
add_library(imgui::imgui ALIAS imgui)

# ---- Subdirectories -----------------------------------------

add_subdirectory(engine)
add_subdirectory(app)

if(AV_BUILD_EXAMPLES)
    add_subdirectory(examples)
endif()

if(AV_BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()
