cmake_minimum_required(VERSION 3.5)

project(LQObjectSerializerTest LANGUAGES CXX)

find_package(Qt6 COMPONENTS Core REQUIRED)
find_package(Qt6Test REQUIRED)
find_package(Qt6Network REQUIRED)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

enable_testing()

include_directories(../LQObjectSerializer)

add_executable(LQObjectSerializerTest
    ../tst_lqobjectserializertest.cpp
    ../res.qrc
    ../../LQObjectSerializer/lserializer.cpp
    )
add_test(NAME LQObjectSerializerTest COMMAND LQObjectSerializerTest)

add_executable(LGithubTestCase
    ../tst_githubtestcase.cpp
    ../res.qrc
    ../../LQObjectSerializer/lserializer.cpp
    )
add_test(NAME LGithubTestCase COMMAND LGithubTestCase)

target_link_libraries(LQObjectSerializerTest PRIVATE Qt6::Core Qt6::Test Qt6::Network)
target_link_libraries(LGithubTestCase PRIVATE Qt6::Core Qt6::Test Qt6::Network)

if(MSVC)
  target_compile_options(LQObjectSerializerTest PRIVATE /W4 /WX)
else()
  target_compile_options(LQObjectSerializerTest PRIVATE -Wall -Wextra -Wpedantic -Werror)
endif()
