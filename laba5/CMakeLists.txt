cmake_minimum_required(VERSION 3.10)
project(TemperatureMonitoring)

set(CMAKE_C_STANDARD 99)

set(SRC_ROOT ${CMAKE_CURRENT_SOURCE_DIR})

add_library(sqlite3 STATIC ${SRC_ROOT}/lib/sqlite3.c)
add_library(cJSON STATIC ${SRC_ROOT}/lib/cJSON.c)

include_directories(
        ${SRC_ROOT}/database/include
        ${SRC_ROOT}/server/include
        ${SRC_ROOT}/temperature_logger/include
        ${SRC_ROOT}/lib
)

set(DATABASE_SOURCES ${SRC_ROOT}/database/src/database.c)
add_library(database STATIC ${DATABASE_SOURCES})
target_link_libraries(database sqlite3)

set(DEVICE_SIMULATOR_SOURCES ${SRC_ROOT}/device_simulator/src/main.c)
add_executable(device_simulator ${DEVICE_SIMULATOR_SOURCES})

set(SERVER_SOURCES
        ${SRC_ROOT}/server/src/server.c
        ${SRC_ROOT}/server/src/handlers.c
)
add_library(server STATIC ${SERVER_SOURCES})
target_link_libraries(server cJSON sqlite3)

set(TEMP_LOGGER_SOURCES
        ${SRC_ROOT}/temperature_logger/src/core/main.c
        ${SRC_ROOT}/temperature_logger/src/common/utils.c
)

if(WIN32)
    list(APPEND TEMP_LOGGER_SOURCES ${SRC_ROOT}/temperature_logger/src/platform/windows/serialport_reader_windows.c)
else()
    list(APPEND TEMP_LOGGER_SOURCES ${SRC_ROOT}/temperature_logger/src/platform/posix/serialport_reader_posix.c)
endif()

add_executable(temperature_logger ${TEMP_LOGGER_SOURCES})
target_link_libraries(temperature_logger database server sqlite3 cJSON)
if(WIN32)
    target_link_libraries(temperature_logger ws2_32)
endif ()