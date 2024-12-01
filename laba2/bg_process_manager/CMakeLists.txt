cmake_minimum_required(VERSION 3.29)

project(bg_process_manager C)

if(WIN32)
    set(PLATFORM_SRC src/platform/windows/run_bg_windows.c)
elseif(UNIX)
    set(PLATFORM_SRC src/platform/posix/run_bg_posix.c)
else()
    message(FATAL_ERROR "Unsupported platform")
endif()

set(COMMON_SRC
    src/common/run_bg.c
)

add_library(run_bg ${COMMON_SRC} ${PLATFORM_SRC})

add_executable(test_runner tests/test_runner.c)

target_link_libraries(test_runner run_bg)

