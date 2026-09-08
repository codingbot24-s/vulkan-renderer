cmake_minimum_required(VERSION 3.20)

project(vulkan-renderer LANGUAGES C)

find_package(glfw3 3.3 REQUIRED)



add_executable(vulkan-renderer main.c
        window.c
        window.h
        constant.h
        init.c
        init.h)



target_link_libraries(vulkan-renderer PRIVATE glfw vulkan)
target_compile_options(vulkan-renderer PRIVATE
    -fsanitize=address
    -fno-omit-frame-pointer
)

target_link_options(vulkan-renderer PRIVATE
    -fsanitize=address
)
