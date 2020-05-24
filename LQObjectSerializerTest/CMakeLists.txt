cmake_minimum_required(VERSION 3.5)

project(LQObjectSerializerTest LANGUAGES CXX)


find_package(Qt5Test REQUIRED)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
enable_testing()

include_directories(../LQObjectSerializer)

add_executable(LQObjectSerializerTest
    tst_lqobjectserializertest.cpp
    ../LQObjectSerializer/lserializer.cpp
    )
add_test(NAME LQObjectSerializerTest COMMAND LQObjectSerializerTest)

target_link_libraries(LQObjectSerializerTest PRIVATE Qt5::Test)

