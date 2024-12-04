cmake_minimum_required(VERSION 3.29)

project(cross_platform_logger)

set(MAIN_FILE src/core/main.c)

set(COMMON_FILES
    src/common/threads.c
    src/common/write_log.c
    src/common/shared_memory.c
)

if (WIN32)
    set(PLATFORM_FILES
        src/platform/windows/threads/threads_windows.c
        src/platform/windows/threads/increment_windows.c
        src/platform/windows/threads/input_windows.c
        src/platform/windows/threads/log_status_windows.c
        src/platform/windows/threads/run_copies_windows.c
        src/platform/windows/utils/write_log_windows.c
        src/platform/windows/shared_memory/shared_memory_windows.c
    )
elseif (UNIX)
    set(PLATFORM_FILES
        src/platform/posix/threads/threads_posix.c
        src/platform/posix/threads/increment_posix.c
        src/platform/posix/threads/input_posix.c
        src/platform/posix/threads/log_status_posix.c
        src/platform/posix/threads/run_copies_posix.c
        src/platform/posix/utils/write_log_posix.c
        src/platform/posix/shared_memory/shared_memory_posix.c
    )
endif()

include_directories(include)

add_executable(${PROJECT_NAME} ${MAIN_FILE} ${COMMON_FILES} ${PLATFORM_FILES})