#include <av/render/vk_common.h>
#include <av/render/vk_context.h>
#include <av/render/vk_swapchain.h>

namespace av {

VkSwapchain::VkSwapchain(
    VkContext& ctx, VkSurfaceKHR surface, int w, int h, uint32_t min_image_count)
    : ctx_(ctx), min_image_count_(min_image_count) {
    wd_.Surface = surface;

    VkBool32 res = VK_FALSE;
    vkGetPhysicalDeviceSurfaceSupportKHR(ctx.physical_device(), ctx.queue_family(), surface, &res);
    if (!res)
        throw std::runtime_error("Queue family doesn't support present");

    const VkFormat req_fmt[] = {VK_FORMAT_B8G8R8A8_UNORM,
                                VK_FORMAT_R8G8B8A8_UNORM,
                                VK_FORMAT_B8G8R8_UNORM,
                                VK_FORMAT_R8G8B8_UNORM};
    const VkColorSpaceKHR req_cs = VK_COLORSPACE_SRGB_NONLINEAR_KHR;
    wd_.SurfaceFormat = ImGui_ImplVulkanH_SelectSurfaceFormat(
        ctx.physical_device(), surface, req_fmt, (size_t)IM_ARRAYSIZE(req_fmt), req_cs);

    VkPresentModeKHR modes[] = {
        VK_PRESENT_MODE_MAILBOX_KHR, VK_PRESENT_MODE_IMMEDIATE_KHR, VK_PRESENT_MODE_FIFO_KHR};
    wd_.PresentMode = ImGui_ImplVulkanH_SelectPresentMode(
        ctx.physical_device(), surface, modes, IM_ARRAYSIZE(modes));

    ImGui_ImplVulkanH_CreateOrResizeWindow(ctx.instance(),
                                           ctx.physical_device(),
                                           ctx.device(),
                                           &wd_,
                                           ctx.queue_family(),
                                           nullptr,
                                           w,
                                           h,
                                           min_image_count_);
}

VkSwapchain::~VkSwapchain() {
    ImGui_ImplVulkanH_DestroyWindow(ctx_.instance(), ctx_.device(), &wd_, nullptr);
}

void VkSwapchain::resize(int w, int h) {
    if (w <= 0 || h <= 0)
        return;
    ImGui_ImplVulkan_SetMinImageCount(min_image_count_);
    ImGui_ImplVulkanH_CreateOrResizeWindow(ctx_.instance(),
                                           ctx_.physical_device(),
                                           ctx_.device(),
                                           &wd_,
                                           ctx_.queue_family(),
                                           nullptr,
                                           w,
                                           h,
                                           min_image_count_);
    wd_.FrameIndex = 0;
    rebuild_ = false;
}

} // namespace av
