cmake_minimum_required(VERSION 3.21)

project(cpp_template VERSION 0.0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(SRC_FILES
  src/main.cpp
)

add_executable(${PROJECT_NAME} ${SRC_FILES})

target_compile_options(${PROJECT_NAME} PRIVATE
  -Wall
  -Wextra
  -Wpedantic
  -Werror
)

target_compile_options(${PROJECT_NAME} PRIVATE
  $<$<CONFIG:Debug>:-fsanitize=address,undefined -fno-omit-frame-pointer -g>
)

target_link_options(${PROJECT_NAME} PRIVATE
  $<$<CONFIG:Debug>:-fsanitize=address,undefined>
)
