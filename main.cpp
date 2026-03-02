#include "imgui_impl_vulkan.h"
#include <cstdint>
#include <iostream>
#include <vector>
#include <vulkan/vulkan_core.h>
using namespace std;

#ifdef IMGUI_IMPL_VULKAN_USE_VOLK
#define VOLK_IMPLEMENTATIONS
#include <volk.h>
#endif

static VkAllocationCallbacks *g_Allocator = nullptr;
static VkInstance g_Instance = VK_NULL_HANDLE;
static VkPhysicalDevice g_PhysicalDevice = VK_NULL_HANDLE;
static VkDevice g_Device = VK_NULL_HANDLE;
static uint32_t g_QueueFamily = (uint32_t)-1;
static VkQueue g_Queue = VK_NULL_HANDLE;
static VkPipelineCache g_PipelineCache = VK_NULL_HANDLE;
static VkDescriptorPool g_DescriptorPool = VK_NULL_HANDLE;

static ImGui_ImplVulkanH_Window g_MainWindowData;
static uint32_t g_MinImageCount = 2;
static bool g_SwapChainRebuild = false;

static void checK_vk_result(VkResult err) {
  if (err == 0) {
    return;
  }
  cout << "[vulkan] Error: VkResult = " << err << endl;
  if (err < 0) {
    abort();
  }
}

static void SetupVulkan(vector<const char *> instance_extensions) {
  VkResult err;
  {
    VkInstanceCreateInfo create_info = {};
    create_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;

#ifdef IMGUI_VULKAN_DEBUG_REPORT
    const char *layers = {"VK_LAYER_KHRONOS_validation"};
    create_info.enabledLayerCount = 1;
    create_info.ppEnabledLayersName = layer;
    instance_extensions.push_back("VK_EXT_debug_report");
#endif

    create_info.enabledExtensionCount = (uint32_t)instance_extensions.size();
    create_info.ppEnabledLayerNames = instance_extensions.data();
    err = vkCreateInstance(&create_info, g_Allocator, &g_Instance);
    checK_vk_result(err);
  }
}