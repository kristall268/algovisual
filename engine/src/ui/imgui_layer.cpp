#include <av/render/vk_common.h>
#include <av/render/vk_context.h>
#include <av/render/vk_swapchain.h>
#include <av/ui/imgui_layer.h>

#include <SDL3/SDL.h>
#include <imgui.h>

#include <backends/imgui_impl_sdl3.h>
#include <backends/imgui_impl_vulkan.h>

namespace av {

ImGuiLayer::ImGuiLayer(SDL_Window* w, VkContext& ctx, VkSwapchain& sc) {
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui::StyleColorsDark();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

    ImGui_ImplSDL3_InitForVulkan(w);

    ImGui_ImplVulkan_InitInfo info{};
    info.Instance = ctx.instance();
    info.PhysicalDevice = ctx.physical_device();
    info.Device = ctx.device();
    info.QueueFamily = ctx.queue_family();
    info.Queue = ctx.queue();
    info.DescriptorPool = ctx.descriptor_pool();
    info.RenderPass = sc.raw()->RenderPass;
    info.MinImageCount = sc.min_image_count();
    info.ImageCount = sc.raw()->ImageCount;
    info.MSAASamples = VK_SAMPLE_COUNT_1_BIT;
    info.CheckVkResultFn = [](VkResult r) {
        if (r != VK_SUCCESS)
            AV_ERROR("[VK] ImGui: {}", (int)r);
    };
    ImGui_ImplVulkan_Init(&info);
}

ImGuiLayer::~ImGuiLayer() {
    ImGui_ImplVulkan_Shutdown();
    ImGui_ImplSDL3_Shutdown();
    ImGui::DestroyContext();
}

void ImGuiLayer::process_event(const SDL_Event& e) {
    ImGui_ImplSDL3_ProcessEvent(&e);
}

void ImGuiLayer::begin_frame() {
    ImGui_ImplVulkan_NewFrame();
    ImGui_ImplSDL3_NewFrame();
    ImGui::NewFrame();
}

void ImGuiLayer::end_frame_and_render(VkSwapchain& sc, VkContext& ctx) {
    ImGui::Render();
    ImDrawData* dd = ImGui::GetDrawData();
    const bool minimized = dd->DisplaySize.x <= 0.0f || dd->DisplaySize.y <= 0.0f;

    auto* wd = sc.raw();
    wd->ClearValue.color.float32[0] = 0.06f;
    wd->ClearValue.color.float32[1] = 0.06f;
    wd->ClearValue.color.float32[2] = 0.08f;
    wd->ClearValue.color.float32[3] = 1.0f;

    if (minimized)
        return;

    VkDevice dev = ctx.device();
    VkSemaphore img_ok = wd->FrameSemaphores[wd->SemaphoreIndex].ImageAcquiredSemaphore;
    VkSemaphore rend_ok = wd->FrameSemaphores[wd->SemaphoreIndex].RenderCompleteSemaphore;

    VkResult r = vkAcquireNextImageKHR(
        dev, wd->Swapchain, UINT64_MAX, img_ok, VK_NULL_HANDLE, &wd->FrameIndex);
    if (r == VK_ERROR_OUT_OF_DATE_KHR || r == VK_SUBOPTIMAL_KHR) {
        sc.mark_rebuild();
        return;
    }
    VK_CHECK(r);

    ImGui_ImplVulkanH_Frame* fd = &wd->Frames[wd->FrameIndex];
    VK_CHECK(vkWaitForFences(dev, 1, &fd->Fence, VK_TRUE, UINT64_MAX));
    VK_CHECK(vkResetFences(dev, 1, &fd->Fence));
    VK_CHECK(vkResetCommandPool(dev, fd->CommandPool, 0));

    VkCommandBufferBeginInfo cbbi{VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO};
    cbbi.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
    VK_CHECK(vkBeginCommandBuffer(fd->CommandBuffer, &cbbi));

    VkRenderPassBeginInfo rpbi{VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO};
    rpbi.renderPass = wd->RenderPass;
    rpbi.framebuffer = fd->Framebuffer;
    rpbi.renderArea.extent = {(uint32_t)wd->Width, (uint32_t)wd->Height};
    rpbi.clearValueCount = 1;
    rpbi.pClearValues = &wd->ClearValue;
    vkCmdBeginRenderPass(fd->CommandBuffer, &rpbi, VK_SUBPASS_CONTENTS_INLINE);

    ImGui_ImplVulkan_RenderDrawData(dd, fd->CommandBuffer);

    vkCmdEndRenderPass(fd->CommandBuffer);
    VK_CHECK(vkEndCommandBuffer(fd->CommandBuffer));

    VkPipelineStageFlags wait = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    VkSubmitInfo si{VK_STRUCTURE_TYPE_SUBMIT_INFO};
    si.waitSemaphoreCount = 1;
    si.pWaitSemaphores = &img_ok;
    si.pWaitDstStageMask = &wait;
    si.commandBufferCount = 1;
    si.pCommandBuffers = &fd->CommandBuffer;
    si.signalSemaphoreCount = 1;
    si.pSignalSemaphores = &rend_ok;
    VK_CHECK(vkQueueSubmit(ctx.queue(), 1, &si, fd->Fence));

    VkPresentInfoKHR pi{VK_STRUCTURE_TYPE_PRESENT_INFO_KHR};
    pi.waitSemaphoreCount = 1;
    pi.pWaitSemaphores = &rend_ok;
    pi.swapchainCount = 1;
    pi.pSwapchains = &wd->Swapchain;
    pi.pImageIndices = &wd->FrameIndex;
    r = vkQueuePresentKHR(ctx.queue(), &pi);
    if (r == VK_ERROR_OUT_OF_DATE_KHR || r == VK_SUBOPTIMAL_KHR) {
        sc.mark_rebuild();
        return;
    }
    VK_CHECK(r);

    wd->SemaphoreIndex = (wd->SemaphoreIndex + 1) % wd->SemaphoreCount;
}

} // namespace av
