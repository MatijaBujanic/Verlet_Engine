cmake_minimum_required(VERSION 3.20)
project(MySFMLApp LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(SOURCES src/main.cpp src/renderer.hpp src/solver.hpp)

add_executable(${PROJECT_NAME} ${SOURCES})

# --- SFML 3.0 ---
find_package(SFML 3 REQUIRED COMPONENTS Graphics Window Audio Network)

target_link_libraries(${PROJECT_NAME} PRIVATE SFML::Graphics SFML::Window
                                              SFML::Audio SFML::Network)

# Optional compiler warnings
if(MSVC)
  target_compile_options(${PROJECT_NAME} PRIVATE /W4)
else()
  target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra -pedantic)
endif()
