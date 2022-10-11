cmake_minimum_required(VERSION 3.15)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Core)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Core)

add_library(lqobjectserializer STATIC)
target_sources(lqobjectserializer PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}/lserializer.h
    ${CMAKE_CURRENT_LIST_DIR}/lserializer.cpp
)

target_include_directories(lqobjectserializer
    PUBLIC ${CMAKE_CURRENT_LIST_DIR})

target_link_libraries(lqobjectserializer PRIVATE
    Qt${QT_VERSION_MAJOR}::Core
)
