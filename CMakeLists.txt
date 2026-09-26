cmake_minimum_required(VERSION 3.20)

project(vulkan-renderer LANGUAGES C)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(glfw3 3.3 REQUIRED)

add_executable(vulkan-renderer
        main.c
        window.c
        window.h
        constant.h
        init.c
        init.h
        input.h
        input.c

        memory/rmemory.c
        memory/rmemory.h

        log/loger.h
        log/loger.c

        vk_core/include/vk_instance.h
        vk_core/vk_instance.c
        vk_core/include/vk_surface.h
        vk_core/vk_surface.c
        vk_core/include/vk_device.h
        vk_core/vk_device.c
        vk_core/include/vk_swapchain.h
        vk_core/vk_swapchain.c
        vk_core/include/vk_image.h
        vk_core/vk_image.c
        vk_core/include/vk_commandpool.h
        vk_core/vk_commandpool.c
        vk_core/include/vk_command_buffers.h
        vk_core/vk_command_buffer.c
        vk_core/include/vk_graphics.h
        vk_core/vk_graphics.c
        vk_core/include/vk_sync.h
        vk_core/vk_sync.c
)

set(VULKAN_SDK "/opt/vulkan/1.4.357.1/x86_64")
target_include_directories(vulkan-renderer PRIVATE
        "${VULKAN_SDK}/include"
)

target_include_directories(vulkan-renderer PRIVATE
        "$ENV{VULKAN_SDK}/include"
)

target_compile_definitions(vulkan-renderer PRIVATE
        $<$<CONFIG:Release>:NDEBUG>
)

target_link_libraries(vulkan-renderer PRIVATE
        glfw
        /lib/x86_64-linux-gnu/libvulkan.so.1
)

target_compile_options(vulkan-renderer PRIVATE
        -fsanitize=address
        -fno-omit-frame-pointer
)

target_link_options(vulkan-renderer PRIVATE
        -fsanitize=address
)
