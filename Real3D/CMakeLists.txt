## Source files
#set(SHADER_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/Resources/Shaders")
#set(SHADER_BINARY_DIR "${CMAKE_CURRENT_BINARY_DIR}/Resources/Shaders")
#
#file(GLOB_RECURSE GLSL_SOURCE_FILES
#    "${SHADER_SOURCE_DIR}/*.frag"
#    "${SHADER_SOURCE_DIR}/*.vert"
#)
#
#foreach(GLSL ${GLSL_SOURCE_FILES})
#    get_filename_component(FILE_NAME ${GLSL} NAME)
#    set(SPIRV "${SHADER_BINARY_DIR}/${FILE_NAME}.spv")
#    add_custom_command(
#        OUTPUT ${SPIRV}
#        COMMAND ${Vulkan_GLSLC_EXECUTABLE} ${GLSL} -o ${SPIRV}
#        DEPENDS ${GLSL}
#    )
#    list(APPEND SPIRV_BINARY_FILES ${SPIRV})
#endforeach(GLSL)
#
#add_custom_target(
#    Shaders 
#    DEPENDS ${SPIRV_BINARY_FILES}
#)
#
## Copy textures to build directory
#file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/resources/textures DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/resources)
#file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/resources/models DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/resources)

set(SOURCES
    #"main.cpp"
    "RealEngine.h" 
    "RealEngine.cpp" 

    "Util/VulkanUtil.h"
    "Util/VulkanUtil.cpp"  
    "Util/Structs.h"
    "Util/Concepts.h" 
    "Util/tiny_obj_loader.h" 

    "Core/SwapChain.cpp"
    "Core/SwapChain.h" 
    "Core/CommandPool.h" 
    "Core/CommandPool.cpp" 
    "Core/CommandBuffers/CommandBuffer.cpp" 
    "Core/CommandBuffers/CommandBuffer.h" 
    "Core/DepthBuffer/DepthBuffer.cpp" 
    "Core/DepthBuffer/DepthBuffer.h"  
    "Core/DepthBuffer/DepthBufferManager.cpp" 
    "Core/DepthBuffer/DepthBufferManager.h" 

    # Renderer
    "Graphics/Renderer.cpp" 
    "Graphics/Renderer.h" 
    
    # Render Pass
    "Graphics/RenderPass.h" 
    "Graphics/RenderPass.cpp" 
    
    # ShaderManager
    "Graphics/ShaderManager.cpp" 
    "Graphics/ShaderManager.h" 
    
    # Mesh
    "Mesh/Mesh.h" 
    "Mesh/Mesh.cpp" 
    "Mesh/MeshIndexed.h" 
    "Mesh/MeshIndexed.cpp" 
        # Mesh Factories
    "Mesh/PrimitiveFactory.h" 
    "Mesh/PrimitiveFactory.cpp" 
    "Mesh/MeshFactory.h"
    "Mesh/MeshFactory.cpp"

    # Material
    "Material/Material.cpp"
    "Material/Material.h"
    "Material/MaterialManager.h" 
    "Material/MaterialManager.cpp"
        # Pipeline
    "Material/Pipelines/Pipeline.cpp" 
    "Material/Pipelines/Pipeline.h" 
    "Material/Pipelines/PosCol2DPipeline.cpp" 
    "Material/Pipelines/PosCol2DPipeline.h" 
    "Material/Pipelines/PosNormBase.cpp" 
    "Material/Pipelines/PosNormBase.h"
    "Material/Pipelines/PosColNormPipeline.cpp" 
    "Material/Pipelines/PosColNormPipeline.h" 
    "Material/Pipelines/PosTexNormPipeline.cpp" 
    "Material/Pipelines/PosTexNormPipeline.h"  
    
    # Camera
    "Misc/Camera.cpp" 
    "Misc/Camera.h"
    "Misc/CameraManager.cpp"
    "Misc/CameraManager.h"
    
    # Input
    "Misc/CameraInputManager.cpp" 
    "Misc/CameraInputManager.h"
    
    "Content/ContentManager.h" 
    "Content/ContentManager.cpp" 
    "Content/Texture2D.h" 
    "Content/Texture2D.cpp"
    "Content/Model.cpp" 
    "Content/Model.h" 

    #ImGui
    "ImGui/imgui_impl_vulkan.cpp"
    "ImGui/imgui_impl_vulkan.h"
)

set (LIBRARY_NAME Real3D)

add_library(${LIBRARY_NAME} STATIC ${SOURCES})# ${GLSL_SOURCE_FILES})

if (CMAKE_SYSTEM_NAME STREQUAL "Windows")
    add_custom_command(
        TARGET ${LIBRARY_NAME} POST_BUILD
        COMMAND "${CMAKE_COMMAND}" -E copy_if_different "$<TARGET_FILE:SDL2::SDL2>" "$<TARGET_FILE_DIR:Real3D>"
        VERBATIM
    )
add_custom_command(
        TARGET ${LIBRARY_NAME} POST_BUILD
        COMMAND "${CMAKE_COMMAND}" -E copy_if_different "$<TARGET_FILE:SDL2_image>" "$<TARGET_FILE_DIR:Real3D>"
        VERBATIM
    )
endif()

# Find the required packages
find_package(Vulkan REQUIRED)

# Include Directories
include_directories(${Vulkan_INCLUDE_DIRS})

#add_dependencies(${PROJECT_NAME} Shaders)
# Link libraries
target_include_directories(${LIBRARY_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_include_directories(${LIBRARY_NAME} PRIVATE SDL2::SDL2 SDL2::SDL2main SDL2_image RealCore)
target_link_libraries(${LIBRARY_NAME} PRIVATE ${Vulkan_LIBRARIES} SDL2::SDL2 SDL2::SDL2main SDL2_image RealCore)