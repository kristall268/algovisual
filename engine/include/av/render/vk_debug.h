#pragma once
#include <vulkan/vulkan.h>

namespace av {
void setup_debug_messenger(VkInstance instance, VkDebugUtilsMessengerEXT* out);
void destroy_debug_messenger(VkInstance instance, VkDebugUtilsMessengerEXT messenger);
} // namespace av
