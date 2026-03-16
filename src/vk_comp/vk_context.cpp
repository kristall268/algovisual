#include "vk_comp/vk_context.h"
// #include "imgui.h"

// #include "SDL3/SDL.h"
// #include "imgui_impl_sdl3.h"

#include "stdio.h"
#include <cstdlib>
#include <cstring>

static void check_vk_result(VkResult err) {
  if (err == VK_SUCCESS)
    return;
  fprintf(stderr, "[vulkan] Error: VkResult= %d\n", err);
  if (err < 0)
    abort();
}

static bool
IsExtensionAvailable(const ImVector<VkExtensionProperties> &properties,
                     const char *extension) {
  for (const VkExtensionProperties &p : properties)
    if (strcmp(p.extensionName, extension) == 0)
      return true;
  return false;
}
