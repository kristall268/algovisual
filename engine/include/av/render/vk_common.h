#pragma once
#include <av/core/logger.h>

#include <vulkan/vulkan.h>

#include <cstdlib>

#define VK_CHECK(expr)                                                                             \
    do {                                                                                           \
        VkResult _r = (expr);                                                                      \
        if (_r != VK_SUCCESS) {                                                                    \
            AV_FATAL("Vulkan error {} at {}:{}", (int)_r, __FILE__, __LINE__);                     \
            std::abort();                                                                          \
        }                                                                                          \
    } while (0)
