cmake_minimum_required(VERSION 3.21)
project(algovisual LANGUAGES CXX)

if(APPLE)
  message(FATAL_ERROR "macOS is not yet supported (planned via MoltenVK).")
endif()


# ==================================================================================================
#                     GLOBAL SETTINGS
# ==================================================================================================
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "" FORCE)
endif()

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)



# ==================================================================================================
#                     PROJECT OPTIONS
# ==================================================================================================
#option(AV_BUILD_EXAMPLES "Build example mini-applications" ON)
#option(AV_BUILD_TESTS "Build unit tests" ON)

# ==================================================================================================
#                     Dependencies (System / vcpkg)
# ==================================================================================================
find_package(Vulkan REQUIRED)

find_package(SDL3 CONFIG QUIET)
if(NOT SDL3_FOUND AND UNIX)
    find_package(PkgConfig REQUIRED)
    pkg_check_modules(SDL3 REQUIRED IMPORTED_TARGET sdl3)
endif()

find_package(spdlog CONFIG REQUIRED)

# ==================================================================================================
#                     TO COMPILE SHADERS, LOOK FOR glslangValidator
# ==================================================================================================
find_program(GLSLANG_VALIDATOR glslangValidator
    HINTS ENV VULKAN_SDK PATH_SUFFIXES bin)
if(NOT GLSLANG_VALIDATOR)
    message(WARNING "glslangValidator not found — shaders will not compile.")
endif()
