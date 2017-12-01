#include "vulkan_functions.hpp"

#include <Windows.h>

#include <cstdint>
#include <vector>
#include <iostream>

int main()
{
    HMODULE vulkanLibrary = LoadLibrary(L"vulkan-1.dll");
    if (!vulkanLibrary) {
        return 1;
    }

    loadVulkanGlobalFunctions(vulkanLibrary);

    uint32_t extensionCount;
    if (vkEnumerateInstanceExtensionProperties(
            nullptr, &extensionCount, nullptr) != VK_SUCCESS) {
        return 1;
    }

    std::vector<VkExtensionProperties> extensions(extensionCount);
    if (vkEnumerateInstanceExtensionProperties(
            nullptr, &extensionCount, &extensions[0]) != VK_SUCCESS) {
        return 1;
    }

    for (const auto& extension : extensions) {
        std::cout << extension.extensionName << " - " <<
            extension.specVersion << std::endl;
    }
    std::cin.get();

    std::vector<const char*> desiredExtensions {};

    VkApplicationInfo applicationInfo;
    applicationInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    applicationInfo.pNext = nullptr;
    applicationInfo.pApplicationName = "Vulkan Stuff";
    applicationInfo.applicationVersion = VK_MAKE_VERSION(0, 1, 0);
    applicationInfo.pEngineName = "no engine";
    applicationInfo.engineVersion = VK_MAKE_VERSION(0, 1, 0);
    applicationInfo.apiVersion = VK_MAKE_VERSION(1, 0, 0);

    VkInstanceCreateInfo instanceCreateInfo;
    instanceCreateInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    instanceCreateInfo.pNext = nullptr;
    instanceCreateInfo.flags = 0;
    instanceCreateInfo.pApplicationInfo = &applicationInfo;
    instanceCreateInfo.enabledLayerCount = 0;
    instanceCreateInfo.ppEnabledLayerNames = nullptr;
    instanceCreateInfo.enabledExtensionCount = 0;
    instanceCreateInfo.ppEnabledExtensionNames = nullptr;

    VkInstance instance;
    if (vkCreateInstance(&instanceCreateInfo, nullptr, &instance) !=
            VK_SUCCESS) {
        return 1;
    }

    loadVulkanInstanceFunctions(instance, extensions);
}