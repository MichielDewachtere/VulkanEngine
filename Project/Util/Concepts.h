#ifndef CONCEPTS_H
#define CONCEPTS_H

#include <concepts>
#include <vulkan/vulkan.h>
#include <array>

// Define a concept for vertex types
template<typename T>
concept vertex_type =
    requires(T) {
		// The type must have a static member function GetBindingDescription()
        { T::GetBindingDescription() } -> std::same_as<std::array<VkVertexInputBindingDescription, T::binding_count>>;
        // The type must have a static member function GetAttributeDescriptions()
        { T::GetAttributeDescriptions() } -> std::same_as<std::array<VkVertexInputAttributeDescription, T::attribute_count>>;
};

class Pipeline;
template <typename P>
concept pipeline_type = std::is_base_of_v<Pipeline, P>;

#endif // CONCEPTS_H