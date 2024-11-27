cmake_minimum_required(VERSION 3.29)

project(bg_process_manager C)

add_library(run_bg run_bg.c)

add_executable(test_runner test_runner.c)
target_link_libraries(test_runner run_bg)