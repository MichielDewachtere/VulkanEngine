cmake_minimum_required(VERSION 3.25)

# Project Name
project(VulkanEngine)

# Use C++20
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)

# Use FetchContent to download SDL2
FetchContent_Declare(
    SDL2
    GIT_REPOSITORY https://github.com/libsdl-org/SDL.git
    GIT_TAG        release-2.30.1 # You can change this to the specific version you need
)

FetchContent_MakeAvailable(SDL2)

FetchContent_Declare(
    SDL2_image
    GIT_REPOSITORY https://github.com/libsdl-org/SDL_image.git
    GIT_TAG        release-2.8.2
)

FetchContent_MakeAvailable(SDL2_image)

set(REALCORE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/RealCore")
message(STATUS "About to fetch real core from ${REALCORE_SOURCE_DIR}")
FetchContent_Declare(
    RealCore
    SOURCE_DIR ${REALCORE_SOURCE_DIR}
)

FetchContent_MakeAvailable(RealCore)

# Test if the library target exists
if(TARGET RealCore)
    message(STATUS "RealCore is included")
else()
    message(STATUS "RealCore is not included")
endif()

set(REAL3D_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/Real3D")
message(STATUS "About to fetch real 3D from ${REAL3D_SOURCE_DIR}")
FetchContent_Declare(
    Real3D
    SOURCE_DIR ${REAL3D_SOURCE_DIR}
)

FetchContent_MakeAvailable(Real3D)

# Test if the library target exists
if(TARGET Real3D)
    message(STATUS "Real3D is included")
else()
    message(STATUS "Real3D is not included")
endif()

# Find the required packages
find_package(Vulkan REQUIRED)

# Include Directories
include_directories(${Vulkan_INCLUDE_DIRS})

add_subdirectory(RealMinecraft)

# If using validation layers, copy the required JSON files (optional)
# add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
#    COMMAND ${CMAKE_COMMAND} -E copy_if_different
#        "${CMAKE_SOURCE_DIR}/path_to_json/VK_LAYER_KHRONOS_validation.json"
#        $<TARGET_FILE_DIR:${PROJECT_NAME}>)
