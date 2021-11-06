cmake_minimum_required(VERSION 3.1)
project(iconpp LANGUAGES CXX VERSION 1.0)

# --------------------------------------------------------------------------------------------------------
# Create library
# --------------------------------------------------------------------------------------------------------

add_library(${PROJECT_NAME} "src/icon.cpp")
target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_17)
target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Werror -Wextra -pedantic)
set_target_properties(${PROJECT_NAME} PROPERTIES CXX_STANDARD 17 CXX_EXTENSIONS OFF CXX_STANDARD_REQUIRED ON)

# --------------------------------------------------------------------------------------------------------
# Include "include" folder
# --------------------------------------------------------------------------------------------------------

target_include_directories(${PROJECT_NAME} PUBLIC "include")

# --------------------------------------------------------------------------------------------------------
# Link required libraries
# --------------------------------------------------------------------------------------------------------

include(FetchContent)

FetchContent_Declare(expected GIT_REPOSITORY "https://github.com/TartanLlama/expected")

option(EXPECTED_BUILD_TESTS "" OFF)
option(EXPECTED_BUILD_PACKAGE "" OFF)
FetchContent_MakeAvailable(expected)

target_link_libraries(${PROJECT_NAME} PRIVATE dl PUBLIC tl::expected)

# --------------------------------------------------------------------------------------------------------
# Tests
# --------------------------------------------------------------------------------------------------------

option(iconpp_tests "Build tests" OFF)

if (iconpp_tests)
    message(STATUS "Building iconpp tests")

    if (NOT TARGET Catch2::Catch2WithMain)
        include(FetchContent)
        FetchContent_Declare(Catch2 GIT_REPOSITORY "https://github.com/catchorg/Catch2" GIT_TAG v2.13.7)
        option(CATCH_BUILD_STATIC_LIBRARY "" ON)
        FetchContent_MakeAvailable(Catch2)
    endif()

    file(GLOB src "tests/*.cpp")
    add_executable(${PROJECT_NAME}_tests ${src})
    target_link_libraries(${PROJECT_NAME}_tests PRIVATE Catch2::Catch2WithMain ${PROJECT_NAME})
    
    if (UNIX)
        target_compile_options(${PROJECT_NAME}_tests INTERFACE -Wall -Werror -Wextra -pedantic)
    endif()
    
    target_compile_features(${PROJECT_NAME}_tests INTERFACE cxx_std_17)
    set_target_properties(${PROJECT_NAME}_tests PROPERTIES CXX_STANDARD 17 CXX_EXTENSIONS OFF CXX_STANDARD_REQUIRED ON)
endif()