#pragma once
struct SDL_Window;
union SDL_Event;

namespace av {
class VkContext;
class VkSwapchain;

class ImGuiLayer {
public:
    ImGuiLayer(SDL_Window* w, VkContext& ctx, VkSwapchain& sc);
    ~ImGuiLayer();

    void process_event(const SDL_Event& e);
    void begin_frame(); // NewFrame
    void end_frame_and_render(VkSwapchain& sc, VkContext& ctx);
};
} // namespace av
