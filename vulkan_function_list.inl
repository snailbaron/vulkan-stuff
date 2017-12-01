//
// VULKAN_FUNCTION_EXPORT -- export function via OS API
// VULKAN_FUNCTION_GLOBAL -- export a global level vulkan function, via a call
//                           to vkGetInstanceProcAddr
//

#define VULKAN_FUNCTION_LEVEL EXPORT
VULKAN_FUNCTION(vkGetInstanceProcAddr)
#undef VULKAN_FUNCTION_LEVEL

#define VULKAN_FUNCTION_LEVEL GLOBAL
VULKAN_FUNCTION(vkEnumerateInstanceExtensionProperties)
VULKAN_FUNCTION(vkEnumerateInstanceLayerProperties)
VULKAN_FUNCTION(vkCreateInstance)
#undef VULKAN_FUNCTION_LEVEL

#define VULKAN_FUNCTION_LEVEL INSTANCE
VULKAN_FUNCTION(vkEnumeratePhysicalDevices)
VULKAN_FUNCTION(vkGetPhysicalDeviceProperties)
VULKAN_FUNCTION(vkGetPhysicalDeviceFeatures)
VULKAN_FUNCTION(vkCreateDevice)
VULKAN_FUNCTION(vkGetDeviceProcAddr)

VULKAN_FUNCTION_EXT(vkGetPhysicalDeviceSurfaceSupportKHR, VK_KHR_SURFACE_EXTENSION_NAME)
VULKAN_FUNCTION_EXT(vkGetPhysicalDeviceSurfaceCapabilitiesKHR, VK_KHR_SURFACE_EXTENSION_NAME)
VULKAN_FUNCTION_EXT(vkGetPhysicalDeviceSurfaceFormatsKHR, VK_KHR_SURFACE_EXTENSION_NAME)
#ifdef VK_USE_PLATFORM_WIN32_KHR
    VULKAN_FUNCTION_EXT(vkCreateWin32SurfaceKHR, VK_KHR_WIN32_SURFACE_EXTENSION_NAME)
#elif defined VK_USE_PLATFORM_XCB_KHR
    VULKAN_FUNCTION_EXT(vkCreateXcbSurfaceKHR, VK_KHR_XCB_SURFACE_EXTENSION_NAME)
#elif defined VK_USE_PLATFORM_XLIB_KHR
    VULKAN_FUNCTION_EXT(vkCreateXlibSurfaceKHR, VK_KHR_XLIB_SURFACE_EXTENSION_NAME)
#endif
#undef VULKAN_FUNCTION_LEVEL
