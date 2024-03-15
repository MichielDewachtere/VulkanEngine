# Source files
set(SHADER_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/shaders")
set(SHADER_BINARY_DIR "${CMAKE_CURRENT_BINARY_DIR}/shaders")

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


set(SOURCES
    "main.cpp"
    "Engine.h" 
    "Engine.cpp" 

    "Util/VulkanUtil.h"
    "Util/VulkanUtil.cpp"
    "Util/Singleton.h" 
    "Util/Structs.h"
    "Util/GameTime.h"
    "Util/GameTime.cpp"
    # Logger
    "Util/Logger.cpp"
    "Util/Logger.h"

    "Core/SwapChain.cpp"
    "Core/SwapChain.h" 
    "Core/CommandPool.h" 
    "Core/CommandPool.cpp" 

    # Renderer
    "Graphics/Renderer.cpp" 
    "Graphics/Renderer.h" 
    # ShaderManager
    "Graphics/ShaderManager.cpp" 
    "Graphics/ShaderManager.h" 
    # Vertex Input
    "Graphics/VertexInput.cpp" 
    "Graphics/VertexInput.h" 
    # Mesh
    "Mesh/Mesh.h" 
    "Mesh/Mesh.cpp" 
    
    # Material
    "Material/Material.cpp"
    "Material/Material.h"
        # Pipeline
    "Material/Pipelines/Pipeline.cpp" 
    "Material/Pipelines/Pipeline.h" 
    "Material/Pipelines/DefaultPipeline.cpp" 
    "Material/Pipelines/DefaultPipeline.h" 
        # Render Pass
    "Material/RenderPasses/RenderPass.h" 
    "Material/RenderPasses/RenderPass.cpp" 
    "Material/RenderPasses/DefaultRenderPass.cpp" 
    "Material/RenderPasses/DefaultRenderPass.h" 
        # Command Buffer
    "Material/CommandBuffers/CommandBuffer.cpp" 
    "Material/CommandBuffers/CommandBuffer.h" 
)

# Create the executable
add_executable(${PROJECT_NAME} ${SOURCES} ${GLSL_SOURCE_FILES} )
add_dependencies(${PROJECT_NAME} Shaders)
# Link libraries
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(${PROJECT_NAME} PRIVATE ${Vulkan_LIBRARIES} glfw)