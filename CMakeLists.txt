cmake_minimum_required (VERSION 3.0)

project (vulkan_stuff)

add_executable (vulkan_simple
    main.cpp
    vulkan_functions.cpp
)

if (MSVC)
    if (CMAKE_CXX_FLAGS MATCHES "/W[0-4]")
        string (REGEX REPLACE "/W[0-4]" "/W4" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
    else ()
        set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4")
    endif()
else ()
    set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Werror")
endif()

# Use "unicode" versions of API functions on Windows
add_definitions (-DUNICODE)

add_definitions (-DVK_NO_PROTOTYPES)

include_directories (include/vulkan)
