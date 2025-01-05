cmake_minimum_required(VERSION 3.29)

project(temperature_logger)

set(MAIN_FILE src/core/main.c)

set(COMMON_FILES src/common/utils.c)

if (WIN32)
    set(PLATFORM_FILES
            src/platform/windows/serialport_reader_windows.c
    )
elseif (UNIX)
    set(PLATFORM_FILES
            src/platform/posix/serialport_reader_posix.c
    )
endif()

include_directories(include)

add_executable(${PROJECT_NAME} ${MAIN_FILE} ${COMMON_FILES} ${PLATFORM_FILES})