cmake_minimum_required(VERSION 3.20)

project(MyProjectName LANGUAGES C)

find_package(glfw3 3.3 REQUIRED)



add_executable(my_program main.c
        window.c
        window.h
        constant.h)


target_link_libraries(my_program PRIVATE glfw)
