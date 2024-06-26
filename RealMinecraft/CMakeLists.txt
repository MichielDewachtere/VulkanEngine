# Source files
set(SHADER_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/Resources/Shaders")
set(SHADER_BINARY_DIR "${CMAKE_CURRENT_BINARY_DIR}/Resources/Shaders")

file(GLOB_RECURSE GLSL_SOURCE_FILES
    "${SHADER_SOURCE_DIR}/*.frag"
    "${SHADER_SOURCE_DIR}/*.vert"
)

foreach(GLSL ${GLSL_SOURCE_FILES})
    get_filename_component(FILE_NAME ${GLSL} NAME)
    set(SPIRV "${SHADER_BINARY_DIR}/${FILE_NAME}.spv")
    add_custom_command(
        OUTPUT ${SPIRV}
        COMMAND ${Vulkan_GLSLC_EXECUTABLE} ${GLSL} -o ${SPIRV}
        DEPENDS ${GLSL}
    )
    list(APPEND SPIRV_BINARY_FILES ${SPIRV})
endforeach(GLSL)

add_custom_target(
    Shaders 
    DEPENDS ${SPIRV_BINARY_FILES}
)

# Copy textures to build directory
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/resources/textures DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/resources)
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/resources/models DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/resources)
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/resources/gui DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/resources)

set(SOURCES
    "main.cpp"
    
    "Scenes/TestScene.cpp" 
    "Scenes/TestScene.h" 
 
    "Components/Chunk.cpp"
    "Components/Chunk.h"
    "Components/World.cpp" 
    "Components/Player.cpp" 
    "Components/TransparentModel.cpp" 
    
    "Commands/MoveCommand.cpp"
    "Commands/MoveCommand.h"
    "Commands/RotateCommand.h" 
    "Commands/RotateCommand.cpp" 
    "Commands/InteractCommand.cpp" 

    "Util/BlockParser.h" 
    "Util/BlockParser.cpp" 
    "Util/BlockModel.h" 
    
    "Util/Enumerations.h" 
    "Util/GameUtils.h"
    "Util/GameInfo.h" 
    "Util/Macros.h"
    
    "Util/FluidParser.cpp" 

    "Util/NoiseManager.cpp"
    "Util/NoiseManager.h"
    "Util/SimplexNoise.cpp"
    "Util/SimplexNoise.h"
    
    "Util/bimap.hpp"

    "Materials/DiffuseMaterial.cpp"
    "Materials/DiffuseMaterial.h"
    "Materials/WaterMaterial.cpp" 
    "Materials/TransparentMaterial.cpp" 
    "Materials/OutlineMaterial.cpp"
    "Materials/TranspriteMaterial.cpp" 

 "Components/OutlineBlock.cpp" "Components/Gui.cpp" "Materials/GuiMaterial.cpp" "Materials/GuiMaterial.h" "Util/GuiParser.cpp"  "Util/ChunkParser.cpp" "Util/ChunkParser.h")

add_executable(${PROJECT_NAME} ${SOURCES} ${GLSL_SOURCE_FILES})

if (CMAKE_SYSTEM_NAME STREQUAL "Windows")
    add_custom_command(
        TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND "${CMAKE_COMMAND}" -E copy_if_different "$<TARGET_FILE:SDL2::SDL2>" "$<TARGET_FILE_DIR:VulkanEngine>"
        VERBATIM
    )
add_custom_command(
        TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND "${CMAKE_COMMAND}" -E copy_if_different "$<TARGET_FILE:SDL2_image>" "$<TARGET_FILE_DIR:VulkanEngine>"
        VERBATIM
    )
endif()

add_dependencies(${PROJECT_NAME} Shaders)
# Link libraries
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_include_directories(${PROJECT_NAME} PRIVATE SDL2::SDL2 SDL2::SDL2main SDL2_image RealCore Real3D)
target_link_libraries(${PROJECT_NAME} PRIVATE ${Vulkan_LIBRARIES} SDL2::SDL2 SDL2::SDL2main SDL2_image RealCore Real3D nlohmann_json::nlohmann_json)