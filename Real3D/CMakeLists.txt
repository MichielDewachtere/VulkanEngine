cmake_minimum_required(VERSION 3.25)

# set the project name and version
project(Real3D VERSION 1.0)

# specify the C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

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

    "Content/ContentManager.h" 
    "Content/ContentManager.cpp" 
    "Content/Texture2D.h" 
    "Content/Texture2D.cpp"
    "Content/Model.cpp" 
    "Content/Model.h" 

    #ImGui
    "ImGui/imgui_impl_vulkan.cpp"
    "ImGui/imgui_impl_vulkan.h"
 "Util/vk_mem_alloc.h")

add_library(${PROJECT_NAME} STATIC ${SOURCES})

# Find the required packages
find_package(Vulkan REQUIRED)

# Include Directories
include_directories(${Vulkan_INCLUDE_DIRS})
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
# Link libraries
target_link_libraries(${PROJECT_NAME} PRIVATE ${Vulkan_LIBRARIES} SDL2::SDL2 SDL2::SDL2main SDL2_image RealCore)