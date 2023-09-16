cmake_minimum_required(VERSION 3.18)

project(dagger VERSION 0.1)

configure_file(dagger_config.hpp.in, dagger_config.hpp)

# target_include_directories(dagger PUBLIC "${PROJECT_BINARY_DIR}")
add_executable(dagger main.cpp)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# add_library(Dagger_Core executor.cpp)

# set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")


# # Turn on the verbose
# set(CMAKE_VERBOSE_MAKEFILE ON)

# # Include additional language check
# include(CheckLanguage)

# # Adhere to GNU conventions
# include(GNUInstallDirs)