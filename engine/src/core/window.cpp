#include <av/core/logger.h>
#include <av/core/window.h>

#include <SDL3/SDL.h>
#include <SDL3/SDL_vulkan.h>

#include <stdexcept>

namespace av {
Window::Window(int w, int h, std::string title) {
    if (!SDL_InitSubSystem(SDL_INIT_VIDEO)) {
        throw std::runtime_error(std::string("SDL_Init: ") + SDL_GetError());
    }
    window_ =
        SDL_CreateWindow(title.c_str(),
                         w,
                         h,
                         SDL_WINDOW_VULKAN | SDL_WINDOW_RESIZABLE | SDL_WINDOW_HIGH_PIXEL_DENSITY);
    if (!window_) {
        throw std::runtime_error(std::string("SDL_CreateWindow: ") + SDL_GetError());
    }
    AV_INFO("Window created: {}x{}", w, h);
}

Window::~Window() {
    if (window_)
        SDL_DestroyWindow(window_);
    SDL_QuitSubSystem(SDL_INIT_VIDEO);
}

void Window::framebuffer_size(int& w, int& h) const {
    SDL_GetWindowSizeInPixels(window_, &w, &h);
}

std::vector<const char*> Window::required_instance_extensions() {
    Uint32 count = 0;
    const char* const* exts = SDL_Vulkan_GetInstanceExtensions(&count);
    return std::vector<const char*>(exts, exts + count);
}

VkSurfaceKHR Window::create_surface(VkInstance instance) const {
    VkSurfaceKHR surface = VK_NULL_HANDLE;
    if (!SDL_Vulkan_CreateSurface(window_, instance, nullptr, &surface)) {
        throw std::runtime_error(std::string("SDL_Vulkan_CreateSurface: ") + SDL_GetError());
    }
    return surface;
}
} // namespace av
