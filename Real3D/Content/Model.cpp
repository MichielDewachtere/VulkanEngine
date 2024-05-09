#include "Model.h"

#include <iostream>
#include <unordered_map>

#define TINYOBJLOADER_IMPLEMENTATION
#include "Util/tiny_obj_loader.h"

real::Model::Model(const std::string & path, const glm::vec3 & pos)
{
    Load(path, pos);
}

void real::Model::Load(const std::string& path, glm::vec3 position)
{
    tinyobj::attrib_t attrib;
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> materials;
    std::string err;

    if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &err, path.c_str()))
        throw std::runtime_error(err);

    std::unordered_map<PosTexNorm, uint32_t> uniqueVertices{};

    for (const auto& [name, mesh] : shapes) 
    {
        for (const auto& [vertex_index, normal_index, texcoord_index] : mesh.indices) 
        {
            PosTexNorm vertex{};

            vertex.pos = {
                attrib.vertices[3 * vertex_index + 0] + position.x,
                attrib.vertices[3 * vertex_index + 1] + position.y,
                attrib.vertices[3 * vertex_index + 2] + position.z
            };

            vertex.texCoord = {
				attrib.texcoords[2 * texcoord_index + 0],
				1.0f - attrib.texcoords[2 * texcoord_index + 1]
            };

            vertex.normal = {
                attrib.normals[3 * normal_index + 0],
                attrib.normals[3 * normal_index + 1],
				attrib.normals[3 * normal_index + 2]
            };

            if (uniqueVertices.contains(vertex) == false)
            {
                uniqueVertices[vertex] = static_cast<uint32_t>(m_Vertices.size());
                m_Vertices.push_back(vertex);
            }

            m_Indices.push_back(uniqueVertices[vertex]);
        }
    }

    std::cout << "amount of vertices: " << m_Vertices.size() << '\n';
    std::cout << "amount of indices: " << m_Indices.size() << '\n';
}
