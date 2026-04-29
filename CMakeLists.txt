cmake_minimum_required(VERSION 3.16)
project(AesEncryptor VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Find Qt6
find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

# Core Library
add_library(AesCore
    src/core/AesBlock.cpp
    src/core/AesBlock.h
    src/core/IEncryptor.h
    src/core/AesSequential.cpp
    src/core/AesSequential.h
    src/core/AesParallel.cpp
    src/core/AesParallel.h
    src/core/EncryptorFactory.cpp
    src/core/EncryptorFactory.h
    src/benchmark/Benchmark.cpp
    src/benchmark/Benchmark.h
)
target_include_directories(AesCore PUBLIC src)

# Main Executable
add_executable(AesEncryptorApp
    src/main.cpp
    src/gui/MainWindow.cpp
    src/gui/MainWindow.h
)
target_link_libraries(AesEncryptorApp PRIVATE AesCore Qt6::Widgets)

# Google Test
include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

enable_testing()

add_executable(AesTests
    tests/AesTests.cpp
)
target_link_libraries(AesTests PRIVATE AesCore gtest_main)

include(GoogleTest)
gtest_discover_tests(AesTests)
