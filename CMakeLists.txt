cmake_minimum_required(VERSION 3.26)
project(Bezier C)
set(CMAKE_C_STANDARD 11)

# Specify the path to SDL2
set(SDL2_DIR "SDL2-2.28.3") # Replace with the path to your SDL2 folder

# Add SDL2 include and library directories
include_directories(${SDL2_DIR}/include)
link_directories(${SDL2_DIR}/lib/x64) # Use x64 or x86 as appropriate

# Add the executable
add_executable(${PROJECT_NAME} main.c)

# Link SDL2 libraries
target_link_libraries(${PROJECT_NAME} SDL2 SDL2main)

# Copy SDL2.dll to the build directory
#add_custom_command(
#        TARGET ${PROJECT_NAME} POST_BUILD
#        COMMAND ${CMAKE_COMMAND} -E copy
#        ${SDL2_DIR}/lib/x64/SDL2.dll $<TARGET_FILE_DIR:Bezier>
#)

# --- SDL2 SETUP ---
#set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake/modules)
#set(SDL2_PATH "SDL2-2.28.3-ming/x86_64-w64-mingw32")

#find_package(SDL2 REQUIRED)
#include_directories(${SDL2_INCLUDE_DIR})
#target_link_libraries(Bezier ${SDL2_LIBRARY})