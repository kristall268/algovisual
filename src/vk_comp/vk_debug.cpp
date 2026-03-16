#include "vk_comp/vk_debug.h"
#include <cstdio>

#ifdef APP_USE_VULKAN_DEBUG_REPORT

// Единственное определение хендла
VkDebugReportCallbackEXT g_DebugReport = VK_NULL_HANDLE;

VKAPI_ATTR VkBool32 VKAPI_CALL
debug_report(VkDebugReportFlagsEXT flags, VkDebugReportObjectTypeEXT objectType,
             uint64_t object, size_t location, int32_t messageCode,
             const char *pLayerPrefix, const char *pMessage, void *pUserData) {
  // Подавляем warning'и о неиспользуемых параметрах
  (void)flags;
  (void)object;
  (void)location;
  (void)messageCode;
  (void)pUserData;
  (void)pLayerPrefix;

  fprintf(stderr, "[vulkan] Debug report from ObjectType: %i\nMessage: %s\n\n",
          objectType, pMessage);

  // VK_FALSE = не прерывать вызов, только залогировать
  return VK_FALSE;
}

void SetupDebugReport(VkInstance instance,
                      const VkAllocationCallbacks *allocator) {
  auto f_vkCreateDebugReportCallbackEXT =
      (PFN_vkCreateDebugReportCallbackEXT)vkGetInstanceProcAddr(
          instance, "vkCreateDebugReportCallbackEXT");

  IM_ASSERT(f_vkCreateDebugReportCallbackEXT != nullptr);

  VkDebugReportCallbackCreateInfoEXT ci = {};
  ci.sType = VK_STRUCTURE_TYPE_DEBUG_REPORT_CALLBACK_CREATE_INFO_EXT;
  ci.flags = VK_DEBUG_REPORT_ERROR_BIT_EXT | VK_DEBUG_REPORT_WARNING_BIT_EXT |
             VK_DEBUG_REPORT_PERFORMANCE_WARNING_BIT_EXT;
  ci.pfnCallback = debug_report;
  ci.pUserData = nullptr;

  VkResult err = f_vkCreateDebugReportCallbackEXT(instance, &ci, allocator,
                                                  &g_DebugReport);

  if (err != VK_SUCCESS)
    fprintf(stderr, "[vulkan] Failed to create debug report callback: %d\n",
            err);
}

void CleanupDebugReport(VkInstance instance,
                        const VkAllocationCallbacks *allocator) {
  auto f_vkDestroyDebugReportCallbackEXT =
      (PFN_vkDestroyDebugReportCallbackEXT)vkGetInstanceProcAddr(
          instance, "vkDestroyDebugReportCallbackEXT");

  if (f_vkDestroyDebugReportCallbackEXT)
    f_vkDestroyDebugReportCallbackEXT(instance, g_DebugReport, allocator);
}

#endif // APP_USE_VULKAN_DEBUG_REPORT