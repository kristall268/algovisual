#include <av/core/application.h>
#include <av/core/logger.h>
#include <av/core/window.h>
#include <av/render/vk_context.h>
#include <av/render/vk_swapchain.h>
#include <av/scene/scene.h>
#include <av/scene/scene_manager.h>
#include <av/ui/imgui_layer.h>

#include <SDL3/SDL.h>
#include <imgui.h>

#include <chrono>

#include <backends/imgui_impl_sdl3.h>
#include <backends/imgui_impl_vulkan.h>

namespace av {

struct Application::Impl {
    std::unique_ptr<Window> window;
    std::unique_ptr<VkContext> ctx;
    VkSurfaceKHR surface = VK_NULL_HANDLE;
    std::unique_ptr<VkSwapchain> swap;
    std::unique_ptr<ImGuiLayer> ui;
    SceneManager scenes;
};

Application::Application(int w, int h, std::string title) : impl_(std::make_unique<Impl>()) {
    Logger::init();
    AV_INFO("Application starting: {}", title);

    impl_->window = std::make_unique<Window>(w, h, std::move(title));

    VkContextDesc d;
    d.instance_extensions = Window::required_instance_extensions();
#ifdef AV_DEBUG
    d.enable_validation = true;
#endif
    impl_->ctx = std::make_unique<VkContext>(d);

    impl_->surface = impl_->window->create_surface(impl_->ctx->instance());
    impl_->ctx->select_physical_device(impl_->surface);
    impl_->ctx->create_logical_device();

    int fw, fh;
    impl_->window->framebuffer_size(fw, fh);
    impl_->swap = std::make_unique<VkSwapchain>(*impl_->ctx, impl_->surface, fw, fh);
    impl_->ui = std::make_unique<ImGuiLayer>(impl_->window->handle(), *impl_->ctx, *impl_->swap);
}

Application::~Application() {
    if (impl_->ctx)
        vkDeviceWaitIdle(impl_->ctx->device());
    // Tear down scenes while ImGui/Vulkan are still alive — scene on_detach
    // may touch ImGui state or Vulkan resources.
    impl_->scenes.reset(nullptr);
    impl_->scenes.process_pending();
    impl_->ui.reset();
    impl_->swap.reset();
    if (impl_->surface && impl_->ctx)
        vkDestroySurfaceKHR(impl_->ctx->instance(), impl_->surface, nullptr);
    impl_->ctx.reset();
    impl_->window.reset();
    AV_INFO("Application exited cleanly");
}

void Application::push_scene(std::unique_ptr<IScene> scene) {
    impl_->scenes.push(std::move(scene));
}

void Application::run() {
    using clock = std::chrono::high_resolution_clock;
    auto prev = clock::now();

    while (!impl_->window->should_close()) {
        SDL_Event e;
        while (SDL_PollEvent(&e)) {
            impl_->ui->process_event(e);
            if (e.type == SDL_EVENT_QUIT)
                impl_->window->request_close();
            if (e.type == SDL_EVENT_WINDOW_CLOSE_REQUESTED &&
                e.window.windowID == SDL_GetWindowID(impl_->window->handle()))
                impl_->window->request_close();
            if (e.type == SDL_EVENT_WINDOW_PIXEL_SIZE_CHANGED)
                impl_->swap->mark_rebuild();
        }

        if (impl_->swap->needs_rebuild()) {
            int w, h;
            impl_->window->framebuffer_size(w, h);
            if (w > 0 && h > 0) {
                ImGui_ImplVulkan_SetMinImageCount(impl_->swap->min_image_count());
                impl_->swap->resize(w, h);
            }
        }

        auto now = clock::now();
        float dt = std::chrono::duration<float>(now - prev).count();
        prev = now;

        // Apply any push/pop/replace/reset queued during the previous frame
        // before we start drawing the new one.
        impl_->scenes.process_pending();

        impl_->ui->begin_frame();

        if (ImGui::Begin("Stats")) {
            ImGui::Text("FPS: %.1f  (%.2f ms)", 1.0f / dt, dt * 1000.0f);
            if (auto* s = impl_->scenes.active())
                ImGui::Text("Scene: %s  (depth %zu)", s->name(), impl_->scenes.size());
            else
                ImGui::TextDisabled("No active scene");
        }
        ImGui::End();

        impl_->scenes.on_update(dt);
        impl_->scenes.on_ui();

        impl_->ui->end_frame_and_render(*impl_->swap, *impl_->ctx);
    }
}

} // namespace av
