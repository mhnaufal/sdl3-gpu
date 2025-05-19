cmake_minimum_required(VERSION 3.30.3)

set(PARENT_PROJECT_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../../..)

if(NOT TARGET SDL3)
    set(SDL_DIR ${CMAKE_CURRENT_SOURCE_DIR}/SDL3)
    add_subdirectory(${SDL_DIR})
endif()

if(NOT TARGET ${PROJECT_NAME})
    add_subdirectory(${PARENT_PROJECT_DIR} ${CMAKE_CURRENT_BINARY_DIR}/PikumaTryRaster)
endif()

# SDL sources are in a subfolder named "SDL"
# add_subdirectory(SDL3)

# Compilation of companion libraries
#add_subdirectory(SDL_image)
#add_subdirectory(SDL_mixer)
#add_subdirectory(SDL_ttf)

# Your game and its CMakeLists.txt are in a subfolder named "src"
# add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../../.. .)
