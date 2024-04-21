cmake_minimum_required(VERSION 3.25)

# set the project name and version
project(RealCore VERSION 1.0)

# specify the C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

include(FetchContent)

# Fetch SDL2
FetchContent_Declare(
    SDL2
    GIT_REPOSITORY https://github.com/libsdl-org/SDL.git
    GIT_TAG        release-2.30.1
)
FetchContent_MakeAvailable(SDL2)

# Fetch GLM
FetchContent_Declare(
	glm
	GIT_REPOSITORY	https://github.com/g-truc/glm.git
	GIT_TAG 		bf71a834948186f4097caa076cd2663c69a10e1e #refs/tags/1.0.1
)
FetchContent_MakeAvailable(glm)

# Find XInput library (assuming it's part of DirectX)
#find_path(DIRECTX_INCLUDE_DIR NAMES "XInput.h" PATH_SUFFIXES "Include")
#find_library(DIRECTX_LIB NAMES "XInput.lib" PATH_SUFFIXES "Lib")

# Include DirectX headers
#include_directories(${DIRECTX_INCLUDE_DIR})

set(LIBS XInput)

# Add the source files to the library
file(GLOB SOURCES "real_core/*.cpp")
add_library(RealCore STATIC ${SOURCES})

# Include directories for the library
target_include_directories(RealCore PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}")
# Link against SDL2 and GLM
target_link_libraries(RealCore PRIVATE ${LIBS} SDL2::SDL2 SDL2::SDL2main glm::glm)