cmake_minimum_required(VERSION 3.20)
project(MyVulkanApp)

find_package(Vulkan REQUIRED)
find_package(glfw3 REQUIRED)

set(IMGUI_DIR /home/kanditty/exfat_windows_cachy/Documents/lmgui/imgui)

add_library(imgui STATIC
    ${IMGUI_DIR}/imgui.cpp
    ${IMGUI_DIR}/imgui_draw.cpp
    ${IMGUI_DIR}/imgui_tables.cpp
    ${IMGUI_DIR}/imgui_widgets.cpp
    ${IMGUI_DIR}/backends/imgui_impl_glfw.cpp
    ${IMGUI_DIR}/backends/imgui_impl_vulkan.cpp
)

target_include_directories(imgui PUBLIC
    ${IMGUI_DIR}
    ${IMGUI_DIR}/backends
)

# Vulkan::Vulkan — двойное двоеточие, не одинарное
target_link_libraries(imgui PUBLIC Vulkan::Vulkan glfw)

# add_executable требует исходники, не имя файла
add_executable(app main.cpp)

# имя должно совпадать с add_executable — app, не main.cpp
target_link_libraries(app PRIVATE imgui)