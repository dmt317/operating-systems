cmake_minimum_required(VERSION 3.29)
project(laba5)

set(CMAKE_C_STANDARD 11)

find_package(SQLite3 REQUIRED)

include_directories(
        ${CMAKE_SOURCE_DIR}/temperature_logger/include
        ${CMAKE_SOURCE_DIR}/database/include
        ${CMAKE_SOURCE_DIR}/lib
)

if (WIN32)
    set(PLATFORM_FILES
            ${CMAKE_SOURCE_DIR}/temperature_logger/src/platform/windows/serialport_reader_windows.c
    )
elseif (UNIX)
    set(PLATFORM_FILES
            ${CMAKE_SOURCE_DIR}/temperature_logger/src/platform/posix/serialport_reader_posix.c
    )
endif()

add_library(cjson STATIC
        ${CMAKE_SOURCE_DIR}/lib/cJSON.c
)

add_library(database
        ${CMAKE_SOURCE_DIR}/database/src/database.c
)
target_include_directories(database PUBLIC include)

add_executable(temperature_logger
        ${CMAKE_SOURCE_DIR}/temperature_logger/src/core/main.c
        ${CMAKE_SOURCE_DIR}/temperature_logger/src/common/utils.c
        ${PLATFORM_FILES}
)
target_link_libraries(temperature_logger PRIVATE database)
target_link_libraries(temperature_logger PRIVATE SQLite::SQLite3)

add_executable(server
        ${CMAKE_SOURCE_DIR}/server/src/main.c
)
target_link_libraries(server PRIVATE database)
target_link_libraries(server PRIVATE SQLite::SQLite3 cjson)
target_include_directories(server PRIVATE ${CMAKE_SOURCE_DIR}/lib)

add_executable(device_simulator
        ${CMAKE_SOURCE_DIR}/device_simulator/src/main.c
)