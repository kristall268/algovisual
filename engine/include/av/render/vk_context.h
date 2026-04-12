#pragma once
#include <vulkan/vulkan.h>

#include <vector>

namespace av {

struct VkContextDesc {
    std::vector<const char*> instance_extensions;
    bool enable_validation = false;
};

class VkContext {
public:
    explicit VkContext(const VkContextDesc& desc);
    ~VkContext();
    VkContext(const VkContext&) = delete;
    VkContext& operator=(const VkContext&) = delete;

    // Вызывается после создания surface окна — чтобы выбрать device с present support
    void select_physical_device(VkSurfaceKHR surface);
    void create_logical_device();

    VkInstance instance() const { return instance_; }
    VkPhysicalDevice physical_device() const { return physical_; }
    VkDevice device() const { return device_; }
    uint32_t queue_family() const { return queue_family_; }
    VkQueue queue() const { return queue_; }
    VkDescriptorPool descriptor_pool() const { return desc_pool_; }

private:
    void create_instance(const VkContextDesc& desc);
    void create_descriptor_pool();

    VkInstance instance_ = VK_NULL_HANDLE;
    VkDebugUtilsMessengerEXT debug_msgr_ = VK_NULL_HANDLE;
    VkPhysicalDevice physical_ = VK_NULL_HANDLE;
    VkDevice device_ = VK_NULL_HANDLE;
    uint32_t queue_family_ = UINT32_MAX;
    VkQueue queue_ = VK_NULL_HANDLE;
    VkDescriptorPool desc_pool_ = VK_NULL_HANDLE;
    bool validation_ = false;
};

} // namespace av
