#pragma once

#include <vulkan.h>

#include <Windows.h>

#include <vector>

#define VULKAN_FUNCTION(NAME) extern PFN_ ## NAME NAME;
#define VULKAN_FUNCTION_EXT(NAME, EXT) extern PFN_ ## NAME NAME;
#include "vulkan_function_list.inl"
#undef VULKAN_FUNCTION_EXT
#undef VULKAN_FUNCTION

void loadVulkanGlobalFunctions(HMODULE vulkanLibrary);

void loadVulkanInstanceFunctions(
    VkInstance instance,
    const std::vector<VkExtensionProperties>& enabledExtensions);

void loadVulkanDeviceFunctions(
    VkDevice device,
    const std::vector<VkExtensionProperties>& enabledExtensions);
