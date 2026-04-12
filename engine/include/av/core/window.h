#pragma once
#include <vulkan/vulkan.h>

#include <string>
#include <vector>

struct SDL_Window;

namespace av {
class Window {
public:
    Window(int w, int h, std::string title);
    ~Window();
    Window(const Window&) = delete;
    Window& operator=(const Window&) = delete;

    SDL_Window* handle() const { return window_; }
    void framebuffer_size(int& w, int& h) const;
    bool should_close() const { return should_close_; }
    void request_close() { should_close_ = true; }

    static std::vector<const char*> required_instance_extensions();
    VkSurfaceKHR create_surface(VkInstance instance) const;

private:
    SDL_Window* window_ = nullptr;
    bool should_close_ = false;
};
} // namespace av
