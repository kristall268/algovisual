#pragma once
#include <string.h>
#include <cstdint>
#include <vector>
#include <vulkan/vulkan.h>

struct SDL_Window;

namespace av{
    class Window{
        public:
            Window(const char* title, uin32_t width, uint32_t height);
            ~Window();
            Window(const Window&) = delete;
            Window& operator=(const Window&) = delete;

            SDL_Window* handle() const { return window_;}
            void framebuffer_size(int* width, int* height) const;
            bool should_close() const;
            void request_close(); { should_close_ = true; }

            static std::vector<const char*> get_required_instance_extensions();
            VkSurfaceKHR create_surface(VkInstance instance) const;

        private:
            SDL_Window* window_;
            bool should_close_ = false;
    };
} // namespace av
    