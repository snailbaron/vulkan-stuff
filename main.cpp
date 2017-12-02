#include "vulkan_functions.hpp"

#include <Windows.h>

#include <cstdint>
#include <vector>
#include <iostream>
#include <stdexcept>
#include <string>
#include <sstream>
#include <algorithm>
#include <cassert>

void check(VkResult result)
{
    if (result != VK_SUCCESS) {
        throw std::runtime_error("Vulkan error");
    }
}

std::ostream& operator<<(
    std::ostream& stream, const std::vector<VkExtensionProperties>& properties)
{
    for (const auto& props : properties) {
        stream << "* " << props.extensionName << " : " << props.specVersion <<
            std::endl;
    }
    return stream;
}

template <class T>
void print(
    const std::string& tag, const T& value, std::ostream& stream = std::cout)
{
    stream << tag << "\n" << std::string(tag.length(), '-') << "\n" << value <<
        std::endl;
}

std::string versionString(uint32_t version)
{
    std::ostringstream stream;
    stream << VK_VERSION_MAJOR(version) << "." << VK_VERSION_MINOR(version) << "." <<
        VK_VERSION_PATCH(version);
    return stream.str();
}

int main()
{
    HMODULE vulkanLibrary = LoadLibrary(L"vulkan-1.dll");
    if (!vulkanLibrary) {
        return 1;
    }

    loadVulkanGlobalFunctions(vulkanLibrary);

    uint32_t extensionCount;
    check(vkEnumerateInstanceExtensionProperties(
        nullptr, &extensionCount, nullptr));

    std::vector<VkExtensionProperties> extensions(extensionCount);
    check(vkEnumerateInstanceExtensionProperties(
        nullptr, &extensionCount, &extensions[0]));

    print("Instance extensions", extensions);

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
    check(vkCreateInstance(&instanceCreateInfo, nullptr, &instance));

    loadVulkanInstanceFunctions(instance, extensions);

    uint32_t deviceCount;
    check(vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr));

    std::vector<VkPhysicalDevice> devices(deviceCount);
    check(vkEnumeratePhysicalDevices(instance, &deviceCount, &devices[0]));

    assert(devices.size() == 1);
    VkPhysicalDevice device = devices.front();

    uint32_t deviceExtensionCount;
    check(vkEnumerateDeviceExtensionProperties(
        device, nullptr, &deviceExtensionCount, nullptr));

    std::vector<VkExtensionProperties> deviceExtensions(deviceExtensionCount);
    check(vkEnumerateDeviceExtensionProperties(
        device, nullptr, &deviceExtensionCount, &deviceExtensions[0]));

    VkPhysicalDeviceProperties properties;
    vkGetPhysicalDeviceProperties(device, &properties);

    std::cout << "Device properties:" <<
        "\nAPI version: " << versionString(properties.apiVersion) <<
        "\nDriver version: " << versionString(properties.driverVersion) <<
        "\nVendor ID: " << properties.vendorID <<
        "\nDevice ID: " << properties.deviceID <<
        "\nDevice name: " << properties.deviceName <<
        std::endl;

    VkPhysicalDeviceFeatures features;
    vkGetPhysicalDeviceFeatures(device, &features);

    print("Device extensions", deviceExtensions);

    uint32_t queueFamilyCount;
    vkGetPhysicalDeviceQueueFamilyProperties(
        device, &queueFamilyCount, nullptr);

    std::vector<VkQueueFamilyProperties> queueFamilyProps(queueFamilyCount);
    vkGetPhysicalDeviceQueueFamilyProperties(
        device, &queueFamilyCount, &queueFamilyProps[0]);

    for (const auto& props : queueFamilyProps) {
        std::cout << "Device queue family:" << "\nFlags:";
        if (props.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
            std::cout << " GRAPHICS";
        }
        if (props.queueFlags & VK_QUEUE_COMPUTE_BIT) {
            std::cout << " COMPUTE";
        }
        if (props.queueFlags & VK_QUEUE_TRANSFER_BIT) {
            std::cout << " TRANSFER";
        }
        if (props.queueFlags & VK_QUEUE_SPARSE_BINDING_BIT) {
            std::cout << " SPARSE";
        }
        std::cout << "\nQueue count: " << props.queueCount << std::endl;
    }

    VkQueueFlags desiredQueueFlags = VK_QUEUE_GRAPHICS_BIT;

    auto goodQueueFamilyIt = std::find_if(
        queueFamilyProps.begin(), queueFamilyProps.end(),
        [desiredQueueFlags] (const auto& props) {
            return props.queueCount > 0 &&
                (props.queueFlags & desiredQueueFlags) == desiredQueueFlags;
        });
    uint32_t queueFamilyIndex =
        goodQueueFamilyIt - queueFamilyProps.begin();

    std::vector<float> queuePriorities { 1.0f };
    VkDeviceQueueCreateInfo queueCreateInfo;
    queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    queueCreateInfo.pNext = nullptr;
    queueCreateInfo.flags = 0;
    queueCreateInfo.queueFamilyIndex = queueFamilyIndex;
    queueCreateInfo.queueCount = 1;
    queueCreateInfo.pQueuePriorities = &queuePriorities[0];

    VkDeviceCreateInfo deviceCreateInfo;
    deviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    deviceCreateInfo.pNext = nullptr;
    deviceCreateInfo.flags = 0;
    deviceCreateInfo.queueCreateInfoCount = 1;
    deviceCreateInfo.pQueueCreateInfos = &queueCreateInfo;
    deviceCreateInfo.enabledLayerCount = 0;
    deviceCreateInfo.ppEnabledLayerNames = nullptr;
    deviceCreateInfo.enabledExtensionCount = 0;
    deviceCreateInfo.ppEnabledExtensionNames = nullptr;
    deviceCreateInfo.pEnabledFeatures = nullptr;

    VkDevice logicalDevice;
    check(vkCreateDevice(device, &deviceCreateInfo, nullptr, &logicalDevice));

    loadVulkanDeviceFunctions(logicalDevice, deviceExtensions);

    VkQueue queue;
    vkGetDeviceQueue(logicalDevice, queueFamilyIndex, 0, &queue);

    vkDestroyDevice(logicalDevice, nullptr);
    logicalDevice = VK_NULL_HANDLE;

    vkDestroyInstance(instance, nullptr);
    instance = VK_NULL_HANDLE;

    if (!FreeLibrary(vulkanLibrary)) {
        return 1;
    }

    std::cin.get();
}