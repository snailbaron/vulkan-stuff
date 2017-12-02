#include "vulkan_functions.hpp"

#include <stdexcept>
#include <iostream>
#include <algorithm>

#define CONCAT_HELPER(ONE, TWO) ONE ## TWO
#define CONCAT(ONE, TWO) CONCAT_HELPER(ONE, TWO)

#define VULKAN_FUNCTION(NAME) PFN_ ## NAME NAME;
#define VULKAN_FUNCTION_EXT(NAME, EXT) PFN_ ## NAME NAME;
#include "vulkan_function_list.inl"
#undef VULKAN_FUNCTION_EXT
#undef VULKAN_FUNCTION

namespace {

template <class Ptr>
void checkPointer(Ptr ptr, const std::string& tag, const std::string& name)
{
    if (!ptr) {
        throw std::runtime_error(
            "Failed to load vulkan function [" + tag + "]: " + name);
    }
}

template <class Ptr>
void loadExportedFunction(HMODULE vulkanLibrary, Ptr* ptr, const char* name)
{
    *ptr = reinterpret_cast<Ptr>(GetProcAddress(vulkanLibrary, name));
    checkPointer(ptr, "exported", name);
}

template <class Ptr>
void loadGlobalFunction(Ptr* ptr, const char* name)
{
    *ptr = reinterpret_cast<Ptr>(vkGetInstanceProcAddr(nullptr, name));
    checkPointer(ptr, "global", name);
}

template <class Ptr>
void loadInstanceFunction(VkInstance instance, Ptr* ptr, const char* name)
{
    *ptr = reinterpret_cast<Ptr>(vkGetInstanceProcAddr(instance, name));
    checkPointer(ptr, "instance", name);
}

template <class Container, class Predicate>
bool contains(const Container& container, Predicate p)
{
    auto it = std::find_if(container.begin(), container.end(), p);
    return it != container.end();
}

template <class Ptr>
void loadExtensionInstanceFunction(
    VkInstance instance,
    const std::vector<VkExtensionProperties>& enabledExtensions,
    const char* extension,
    Ptr* ptr,
    const char* name)
{
    bool extensionEnabled = contains(
        enabledExtensions,
        [extension] (const VkExtensionProperties& props) {
            return std::string(props.extensionName) == std::string(extension);
        });

    if (!extensionEnabled) {
        std::cerr << "WARNING: Cannot load function '" << name <<
            "': extension '" << extension << "'";
        return;
    }

    *ptr = reinterpret_cast<Ptr>(vkGetInstanceProcAddr(instance, name));
    checkPointer(ptr, "instance | " + std::string(extension), name);
}

template <class Ptr>
void loadDeviceFunction(VkDevice device, Ptr* ptr, const char* name)
{
    *ptr = reinterpret_cast<Ptr>(vkGetDeviceProcAddr(device, name));
    checkPointer(ptr, "device", name);
}

template <class Ptr>
void loadExtensionDeviceFunction(
    VkDevice device,
    const std::vector<VkExtensionProperties>& enabledExtensions,
    const char* extension,
    Ptr* ptr,
    const char* name)
{
    bool extensionEnabled = contains(enabledExtensions,
        [extension] (const VkExtensionProperties& props) {
            return std::string(props.extensionName) == std::string(extension);
        });

    if (!extensionEnabled) {
        std::cerr << "WARNING: Cannot load device function '" << name <<
            "': extension '" << extension << "'";
    }

    *ptr = reinterpret_cast<Ptr>(vkGetDeviceProcAddr(device, name));
    checkPointer(ptr, "device | " + std::string(extension), name);
}

} // namespace

void loadVulkanGlobalFunctions(HMODULE vulkanLibrary)
{
#define VULKAN_FUNCTION_EXPORT(NAME) \
    loadExportedFunction(vulkanLibrary, &NAME, #NAME);

#define VULKAN_FUNCTION_GLOBAL(NAME) \
    loadGlobalFunction(&NAME, #NAME);

#define VULKAN_FUNCTION_INSTANCE(NAME)
#define VULKAN_FUNCTION_EXT_INSTANCE(NAME, EXT)
#define VULKAN_FUNCTION_DEVICE(NAME)
#define VULKAN_FUNCTION_EXT_DEVICE(NAME, EXT)

#define VULKAN_FUNCTION(NAME) CONCAT(VULKAN_FUNCTION_, VULKAN_FUNCTION_LEVEL)(NAME)
#define VULKAN_FUNCTION_EXT(NAME, EXT)

#include "vulkan_function_list.inl"

#undef VULKAN_FUNCTION_EXT
#undef VULKAN_FUNCTION
#undef VULKAN_FUNCTION_DEVICE
#undef VULKAN_FUNCTION_EXT_DEVICE
#undef VULKAN_FUNCTION_INSTANCE
#undef VULKAN_FUNCTION_EXT_INSTANCE
#undef VULKAN_FUNCTION_EXPORT
#undef VULKAN_FUNCTION_GLOBAL
}

void loadVulkanInstanceFunctions(
    VkInstance instance,
    const std::vector<VkExtensionProperties>& enabledExtensions)
{
#define VULKAN_FUNCTION_EXPORT(NAME)
#define VULKAN_FUNCTION_GLOBAL(NAME)
#define VULKAN_FUNCTION_DEVICE(NAME)
#define VULKAN_FUNCTION_EXT_DEVICE(NAME, EXT)

#define VULKAN_FUNCTION_INSTANCE(NAME) \
    loadInstanceFunction(instance, &NAME, #NAME);

#define VULKAN_FUNCTION_EXT_INSTANCE(NAME, EXT) \
    loadExtensionInstanceFunction(instance, enabledExtensions, EXT, &NAME, #NAME);

#define VULKAN_FUNCTION(NAME) CONCAT(VULKAN_FUNCTION_, VULKAN_FUNCTION_LEVEL)(NAME)
#define VULKAN_FUNCTION_EXT(NAME, EXT) CONCAT(VULKAN_FUNCTION_EXT_, VULKAN_FUNCTION_LEVEL)(NAME, EXT)

#include "vulkan_function_list.inl"

#undef VULKAN_FUNCTION_EXT_DEVICE
#undef VULKAN_FUNCTION_EXT
#undef VULKAN_FUNCTION
#undef VULKAN_FUNCTION_INSTANCE
#undef VULKAN_FUNCTION_EXT_INSTANCE
#undef VULKAN_FUNCTION_EXPORT
#undef VULKAN_FUNCTION_GLOBAL
#undef VULKAN_FUNCTION_DEVICE
}

void loadVulkanDeviceFunctions(
    VkDevice device,
    const std::vector<VkExtensionProperties>& enabledExtensions)
{
#define VULKAN_FUNCTION_EXPORT(NAME)
#define VULKAN_FUNCTION_GLOBAL(NAME)
#define VULKAN_FUNCTION_INSTANCE(NAME)
#define VULKAN_FUNCTION_EXT_INSTANCE(NAME, EXT)

#define VULKAN_FUNCTION_DEVICE(NAME)    \
    loadDeviceFunction(device, &NAME, #NAME);

#define VULKAN_FUNCTION_EXT_DEVICE(NAME, EXT)   \
    loadExtensionDeviceFunction(device, enabledExtensions, EXT, &NAME, #NAME);

#define VULKAN_FUNCTION(NAME) CONCAT(VULKAN_FUNCTION_, VULKAN_FUNCTION_LEVEL)(NAME)
#define VULKAN_FUNCTION_EXT(NAME, EXT) CONCAT(VULKAN_FUNCTION_EXT_, VULKAN_FUNCTION_LEVEL)(NAME, EXT)

#include "vulkan_function_list.inl"

#undef VULKAN_FUNCTION_EXT
#undef VULKAN_FUNCTION
#undef VULKAN_FUNCTION_EXT_DEVICE
#undef VULKAN_FUNCTION_DEVICE
#undef VULKAN_FUNCTION_INSTANCE
#undef VULKAN_FUNCTION_EXT_INSTANCE
#undef VULKAN_FUNCTION_GLOBAL
#undef VULKAN_FUNCTION_EXPORT
}

#undef CONCAT
#undef CONCAT_HELPER
