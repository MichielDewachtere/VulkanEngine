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
    "Util/VulkanUtil.h"
    "Util/VulkanUtil.cpp"
    # Archived -> Should be destroyed
    "archived/VulkanBase.h" 
    "archived/labwork/Week01.cpp"
    "archived/labwork/Week02.cpp" 
    "archived/labwork/Week03.cpp" 
    "archived/labwork/Week04.cpp" 
    "archived/labwork/Week05.cpp"
    "archived/labwork/Week06.cpp"
    "Util/Singleton.h" 
    "Util/Structs.h"
    # Logger
    "Util/Logger.cpp"
    "Util/Logger.h"
    "Engine.h" 
    "Engine.cpp" 
    "Core/SwapChain.cpp"
    "Core/SwapChain.h" 
    # Renderer
    "Graphics/Renderer.cpp" 
    "Graphics/Renderer.h" 
    # ShaderManager
    "Graphics/ShaderManager.cpp" 
    "Graphics/ShaderManager.h" 
    # Vertex Input
    "Graphics/VertexInput.cpp" 
    "Graphics/VertexInput.h" 
    # Material
    "Graphics/Material/Material.cpp"
    "Graphics/Material/Material.h"
    "Graphics/Material/Pipelines/Pipeline.cpp" 
    "Graphics/Material/Pipelines/Pipeline.h" 
    "Graphics/Material/Pipelines/DefaultPipeline.cpp" 
    "Graphics/Material/Pipelines/DefaultPipeline.h" 
    "Graphics/Material/RenderPasses/RenderPass.h" 
    "Graphics/Material/RenderPasses/RenderPass.cpp" 
    "Graphics/Material/RenderPasses/DefaultRenderPass.cpp" 
    "Graphics/Material/RenderPasses/DefaultRenderPass.h" 
    "Graphics/Material/CommandBuffers/CommandBuffer.cpp" 
    "Graphics/Material/CommandBuffers/CommandBuffer.h" 
    # Pipeline Manager -> TODO: Destroy
    "Graphics/PipelineManager.cpp" 
    "Graphics/PipelineManager.h" 
)

# Create the executable
add_executable(${PROJECT_NAME} ${SOURCES} ${GLSL_SOURCE_FILES} )
add_dependencies(${PROJECT_NAME} Shaders)
# Link libraries
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(${PROJECT_NAME} PRIVATE ${Vulkan_LIBRARIES} glfw)