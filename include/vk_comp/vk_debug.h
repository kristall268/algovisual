#pragma once
#include "imgui.h" // для IM_ASSERT
#include <vulkan/vulkan.h>

#ifdef APP_USE_VULKAN_DEBUG_REPORT

extern VkDebugReportCallbackEXT g_DebugReport;

// Callback — регистрируется в Vulkan, вызывается автоматически при ошибках
VKAPI_ATTR VkBool32 VKAPI_CALL
debug_report(VkDebugReportFlagsEXT flags, VkDebugReportObjectTypeEXT objectType,
             uint64_t object, size_t location, int32_t messageCode,
             const char *pLayerPrefix, const char *pMessage, void *pUserData);

// Регистрация callback'а — вызывать после vkCreateInstance
void SetupDebugReport(VkInstance instance,
                      const VkAllocationCallbacks *allocator);

// Удаление callback'а — вызывать перед vkDestroyInstance
void CleanupDebugReport(VkInstance instance,
                        const VkAllocationCallbacks *allocator);

#endif // APP_USE_VULKAN_DEBUG_REPORT