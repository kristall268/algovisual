#include <av/render/vk_common.h>
#include <av/render/vk_context.h>
#include <av/render/vk_debug.h>

#include <algorithm>
#include <cstring>
#include <stdexcept>

namespace av {

static bool has_ext(const std::vector<VkExtensionProperties>& a, const char* name) {
    return std::any_of(a.begin(), a.end(), [&](const VkExtensionProperties& p) {
        return std::strcmp(p.extensionName, name) == 0;
    });
}

VkContext::VkContext(const VkContextDesc& desc) : validation_(desc.enable_validation) {
    create_instance(desc);
}

VkContext::~VkContext() {
    if (desc_pool_)
        vkDestroyDescriptorPool(device_, desc_pool_, nullptr);
    if (device_)
        vkDestroyDevice(device_, nullptr);
    if (debug_msgr_)
        destroy_debug_messenger(instance_, debug_msgr_);
    if (instance_)
        vkDestroyInstance(instance_, nullptr);
}

void VkContext::create_instance(const VkContextDesc& desc) {
    VkApplicationInfo ai{VK_STRUCTURE_TYPE_APPLICATION_INFO};
    ai.pApplicationName = "algovisual";
    ai.apiVersion = VK_API_VERSION_1_3;

    std::vector<const char*> exts = desc.instance_extensions;

    uint32_t prop_count = 0;
    vkEnumerateInstanceExtensionProperties(nullptr, &prop_count, nullptr);
    std::vector<VkExtensionProperties> props(prop_count);
    vkEnumerateInstanceExtensionProperties(nullptr, &prop_count, props.data());

    if (has_ext(props, VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME))
        exts.push_back(VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME);

    std::vector<const char*> layers;
    if (validation_ && has_ext(props, VK_EXT_DEBUG_UTILS_EXTENSION_NAME)) {
        exts.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
        layers.push_back("VK_LAYER_KHRONOS_validation");
    } else {
        validation_ = false;
    }

    VkInstanceCreateInfo ci{VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO};
    ci.pApplicationInfo = &ai;
    ci.enabledExtensionCount = (uint32_t)exts.size();
    ci.ppEnabledExtensionNames = exts.data();
    ci.enabledLayerCount = (uint32_t)layers.size();
    ci.ppEnabledLayerNames = layers.data();

    VK_CHECK(vkCreateInstance(&ci, nullptr, &instance_));

    if (validation_)
        setup_debug_messenger(instance_, &debug_msgr_);
}

void VkContext::select_physical_device(VkSurfaceKHR surface) {
    uint32_t n = 0;
    vkEnumeratePhysicalDevices(instance_, &n, nullptr);
    if (!n)
        throw std::runtime_error("No Vulkan-capable GPU");
    std::vector<VkPhysicalDevice> devs(n);
    vkEnumeratePhysicalDevices(instance_, &n, devs.data());

    // Предпочитаем discrete GPU с present+graphics на одной очереди
    VkPhysicalDevice fallback = VK_NULL_HANDLE;
    uint32_t fallback_q = UINT32_MAX;

    for (auto d : devs) {
        VkPhysicalDeviceProperties p;
        vkGetPhysicalDeviceProperties(d, &p);

        uint32_t qn = 0;
        vkGetPhysicalDeviceQueueFamilyProperties(d, &qn, nullptr);
        std::vector<VkQueueFamilyProperties> qs(qn);
        vkGetPhysicalDeviceQueueFamilyProperties(d, &qn, qs.data());

        for (uint32_t i = 0; i < qn; ++i) {
            VkBool32 present = VK_FALSE;
            vkGetPhysicalDeviceSurfaceSupportKHR(d, i, surface, &present);
            if ((qs[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) && present) {
                if (p.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU) {
                    physical_ = d;
                    queue_family_ = i;
                    break;
                }
                if (fallback == VK_NULL_HANDLE) {
                    fallback = d;
                    fallback_q = i;
                }
            }
        }
        if (physical_)
            break;
    }
    if (!physical_) {
        physical_ = fallback;
        queue_family_ = fallback_q;
    }
    if (!physical_)
        throw std::runtime_error("No suitable GPU with graphics+present");

    VkPhysicalDeviceProperties p;
    vkGetPhysicalDeviceProperties(physical_, &p);
    AV_INFO("GPU: {} (queue family {})", p.deviceName, queue_family_);
}

void VkContext::create_logical_device() {
    float prio = 1.0f;
    VkDeviceQueueCreateInfo qci{VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO};
    qci.queueFamilyIndex = queue_family_;
    qci.queueCount = 1;
    qci.pQueuePriorities = &prio;

    std::vector<const char*> dev_exts = {VK_KHR_SWAPCHAIN_EXTENSION_NAME};

    VkDeviceCreateInfo ci{VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO};
    ci.queueCreateInfoCount = 1;
    ci.pQueueCreateInfos = &qci;
    ci.enabledExtensionCount = (uint32_t)dev_exts.size();
    ci.ppEnabledExtensionNames = dev_exts.data();

    VK_CHECK(vkCreateDevice(physical_, &ci, nullptr, &device_));
    vkGetDeviceQueue(device_, queue_family_, 0, &queue_);

    create_descriptor_pool();
}

void VkContext::create_descriptor_pool() {
    // Щедрый пул для ImGui + будущих пайплайнов
    VkDescriptorPoolSize sizes[] = {
        {VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1000},
        {VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1000},
        {VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1000},
    };
    VkDescriptorPoolCreateInfo ci{VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO};
    ci.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
    ci.maxSets = 1000;
    ci.poolSizeCount = 3;
    ci.pPoolSizes = sizes;
    VK_CHECK(vkCreateDescriptorPool(device_, &ci, nullptr, &desc_pool_));
}

} // namespace av
