#include "imgui.h"
#include "imgui_impl_vulkan.h"

#include "SDL3/SDL.h"
#include "SDL3/SDL_vulkan.h"
#include "imgui_impl_sdl3.h"

#include "cstdlib"
#include "stdio.h"
#include <SDL3/SDL_error.h>
#include <SDL3/SDL_events.h>
#include <SDL3/SDL_stdinc.h>
#include <SDL3/SDL_video.h>
#include <cstdlib>

int main() {
  if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_GAMEPAD)) {
    printf("Error: SDL_INIT(): %s\n", SDL_GetError());
    return EXIT_FAILURE;
  }

  Uint32 window_flags =
      SDL_WINDOW_VULKAN | SDL_WINDOW_RESIZABLE | SDL_WINDOW_HIGH_PIXEL_DENSITY;
  SDL_Window *window = SDL_CreateWindow("algovisual", 1280, 720, window_flags);
  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImGuiIO &io = ImGui::GetIO();
  (void)io;
  io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
  io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;
  ImGui::StyleColorsDark();

  ImGui_ImplSDL3_InitForVulkan(window);

  bool done = false;

  while (!done) {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
      ImGui_ImplSDL3_ProcessEvent(&event);
      if (event.type == SDL_EVENT_QUIT) {
        done = true;
      }
      if (event.type == SDL_EVENT_WINDOW_CLOSE_REQUESTED &&
          event.window.windowID == SDL_GetWindowID(window)) {
        done = true;
      }
    }
    ImGui_ImplVulkan_NewFrame();
    ImGui_ImplSDL3_NewFrame();
    ImGui::NewFrame();

    ImGui::ShowDemoWindow();

    ImGui::Render();
    ImDrawData *draw_data = ImGui::GetDrawData();
  }
  ImGui_ImplSDL3_Shutdown();
  ImGui::DestroyContext();

  // Destroy Vulkan objects here...

  SDL_DestroyWindow(window);
  SDL_Quit();

  return EXIT_SUCCESS;
}