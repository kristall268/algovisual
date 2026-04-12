#include <av/core/logger.h>
#include <av/render/vk_debug.h>

namespace av {
static VKAPI_ATTR VkBool32 VKAPI_CALL debug_cb(VkDebugUtilsMessageSeverityFlagBitsEXT sev,
                                               VkDebugUtilsMessageTypeFlagsEXT,
                                               const VkDebugUtilsMessengerCallbackDataEXT* data,
                                               void*) {
    if (sev & VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT)
        AV_ERROR("[VK] {}", data->pMessage);
    else if (sev & VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT)
        AV_WARN("[VK] {}", data->pMessage);
    else
        AV_INFO("[VK] {}", data->pMessage);
    return VK_FALSE;
}

void setup_debug_messenger(VkInstance inst, VkDebugUtilsMessengerEXT* out) {
    auto f = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(
        inst, "vkCreateDebugUtilsMessengerEXT");
    if (!f)
        return;

    VkDebugUtilsMessengerCreateInfoEXT ci{};
    ci.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
    ci.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
                         VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
    ci.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
                     VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
                     VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
    ci.pfnUserCallback = debug_cb;
    f(inst, &ci, nullptr, out);
}

void destroy_debug_messenger(VkInstance inst, VkDebugUtilsMessengerEXT m) {
    if (!m)
        return;
    auto f = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(
        inst, "vkDestroyDebugUtilsMessengerEXT");
    if (f)
        f(inst, m, nullptr);
}
} // namespace av
