cmake_minimum_required(VERSION 3.20)
project(algovisual CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)   # for clangd / .vscode

# ---- Output Directories --------------------------------------
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)

# ── Dependency ───────────────────────────────────────────────
find_package(Vulkan REQUIRED)

# SDL3: сначала пробуем CMake-конфиг, иначе pkg-config
find_package(SDL3 QUIET CONFIG)
if(NOT SDL3_FOUND)
    find_package(PkgConfig REQUIRED)
    pkg_check_modules(SDL3 REQUIRED sdl3)
endif()

# ── Dear ImGui ────────────────────────────────────────────────
# Изменен путь: теперь ImGui ожидается в папке external/imgui
if(NOT DEFINED IMGUI_DIR)
    set(IMGUI_DIR "${CMAKE_SOURCE_DIR}/external/imgui")
endif()

set(IMGUI_SOURCES
    ${IMGUI_DIR}/imgui.cpp
    ${IMGUI_DIR}/imgui_demo.cpp
    ${IMGUI_DIR}/imgui_draw.cpp
    ${IMGUI_DIR}/imgui_tables.cpp
    ${IMGUI_DIR}/imgui_widgets.cpp
    # SDL3 + Vulkan бэкенды
    ${IMGUI_DIR}/backends/imgui_impl_sdl3.cpp
    ${IMGUI_DIR}/backends/imgui_impl_vulkan.cpp
)
 
# ── Исходники проекта ─────────────────────────────────────────
# Добавлен CONFIGURE_DEPENDS для автоматического подхвата новых файлов
# Убрана папка main, теперь все исходники ищутся в src
file(GLOB_RECURSE PROJECT_SOURCES CONFIGURE_DEPENDS
    "${CMAKE_SOURCE_DIR}/src/*.cpp"
)
 
# ── Исполняемый файл ──────────────────────────────────────────
add_executable(${PROJECT_NAME}
    ${PROJECT_SOURCES}
    ${IMGUI_SOURCES}
)

# Убрана папка main из include_directoriesw
target_include_directories(${PROJECT_NAME} PRIVATE
    ${IMGUI_DIR}
    ${IMGUI_DIR}/backends
    ${Vulkan_INCLUDE_DIRS}
    ${CMAKE_SOURCE_DIR}/include
)

# Линкуем SDL3 (поддержка обоих вариантов find_package)
if(TARGET SDL3::SDL3)
    target_link_libraries(${PROJECT_NAME} PRIVATE SDL3::SDL3 Vulkan::Vulkan)
else()
    target_include_directories(${PROJECT_NAME} PRIVATE ${SDL3_INCLUDE_DIRS})
    target_link_libraries(${PROJECT_NAME} PRIVATE ${SDL3_LIBRARIES} Vulkan::Vulkan)
endif()

# ── Опции компиляции ──────────────────────────────────────────
target_compile_definitions(${PROJECT_NAME} PRIVATE
    # Validation layers включаются только в Debug
    $<$<CONFIG:Debug>:IMGUI_VULKAN_DEBUG_REPORT>
)

if(MSVC)
    target_compile_options(${PROJECT_NAME} PRIVATE /W3 /MP)
else()
    target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra -Wpedantic)
endif()