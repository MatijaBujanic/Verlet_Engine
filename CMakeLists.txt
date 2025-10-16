cmake_minimum_required(VERSION 3.20)
project(MySFMLApp LANGUAGES CXX)

# Use C++17 or later
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Add all source files
set(SOURCES
    src/main.cpp
    src/render.cpp
    src/solver.cpp
)

# Create the executable
add_executable(${PROJECT_NAME} ${SOURCES})

# Find SFML 3.0
find_package(SFML 3 REQUIRED COMPONENTS system window graphics)

# Link SFML
target_link_libraries(${PROJECT_NAME} PRIVATE sfml-system sfml-window sfml-graphics)

# Optional: add warnings and debugging
if(MSVC)
  target_compile_options(${PROJECT_NAME} PRIVATE /W4)
else()
  target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra -pedantic)
endif()

