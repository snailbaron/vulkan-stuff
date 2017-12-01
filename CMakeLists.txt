cmake_minimum_required (VERSION 3.0)

project (vulkan_stuff)

add_executable (vulkan_simple
    main.cpp
    vulkan_functions.cpp
)

# Use "unicode" versions of API functions on Windows
add_definitions (-DUNICODE)

add_definitions (-DVK_NO_PROTOTYPES)

include_directories (include/vulkan)
