#pragma once
#include <vulkan/vulkan.h>

#include <backends/imgui_impl_vulkan.h>

namespace av {
class VkContext;

// Тонкая обёртка над ImGui_ImplVulkanH_Window — берём её готовое управление
// swapchain/framebuffers/cmd buffers, потому что с ним дружит ImGui backend.
class VkSwapchain {
public:
    VkSwapchain(VkContext& ctx, VkSurfaceKHR surface, int w, int h, uint32_t min_image_count = 2);
    ~VkSwapchain();

    void resize(int w, int h);

    ImGui_ImplVulkanH_Window* raw() { return &wd_; }
    const ImGui_ImplVulkanH_Window* raw() const { return &wd_; }

    uint32_t min_image_count() const { return min_image_count_; }
    bool needs_rebuild() const { return rebuild_; }
    void mark_rebuild() { rebuild_ = true; }
    void clear_rebuild() { rebuild_ = false; }

private:
    VkContext& ctx_;
    ImGui_ImplVulkanH_Window wd_{};
    uint32_t min_image_count_;
    bool rebuild_ = false;
};
} // namespace av
