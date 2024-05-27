#include "AABB.h"

bool real::FrustumAABB::IsBoxInFrustum(const glm::mat4& viewProjMatrix, const AABB& box)
{
    for (const auto& plane : ExtractFrustumPlanes(viewProjMatrix))
    {
        glm::vec3 positiveVertex = box.min;

        if (plane.normal.x >= 0) positiveVertex.x = box.max.x;
        if (plane.normal.y >= 0) positiveVertex.y = box.max.y;
        if (plane.normal.z >= 0) positiveVertex.z = box.max.z;

        if (plane.DistanceToPoint(positiveVertex) < 0)
            return false;
    }

    return true;
}

std::array<real::FrustumPlane, 6> real::FrustumAABB::ExtractFrustumPlanes(const glm::mat4& viewProjMatrix)
{
    std::array<FrustumPlane, 6> planes;

    // Left plane
    planes[0].normal.x = viewProjMatrix[0][3] + viewProjMatrix[0][0];
    planes[0].normal.y = viewProjMatrix[1][3] + viewProjMatrix[1][0];
    planes[0].normal.z = viewProjMatrix[2][3] + viewProjMatrix[2][0];
    planes[0].distance = viewProjMatrix[3][3] + viewProjMatrix[3][0];

    // Right plane
    planes[1].normal.x = viewProjMatrix[0][3] - viewProjMatrix[0][0];
    planes[1].normal.y = viewProjMatrix[1][3] - viewProjMatrix[1][0];
    planes[1].normal.z = viewProjMatrix[2][3] - viewProjMatrix[2][0];
    planes[1].distance = viewProjMatrix[3][3] - viewProjMatrix[3][0];

    // Bottom plane
    planes[2].normal.x = viewProjMatrix[0][3] + viewProjMatrix[0][1];
    planes[2].normal.y = viewProjMatrix[1][3] + viewProjMatrix[1][1];
    planes[2].normal.z = viewProjMatrix[2][3] + viewProjMatrix[2][1];
    planes[2].distance = viewProjMatrix[3][3] + viewProjMatrix[3][1];

    // Top plane
    planes[3].normal.x = viewProjMatrix[0][3] - viewProjMatrix[0][1];
    planes[3].normal.y = viewProjMatrix[1][3] - viewProjMatrix[1][1];
    planes[3].normal.z = viewProjMatrix[2][3] - viewProjMatrix[2][1];
    planes[3].distance = viewProjMatrix[3][3] - viewProjMatrix[3][1];

    // Near plane
    planes[4].normal.x = viewProjMatrix[0][3] + viewProjMatrix[0][2];
    planes[4].normal.y = viewProjMatrix[1][3] + viewProjMatrix[1][2];
    planes[4].normal.z = viewProjMatrix[2][3] + viewProjMatrix[2][2];
    planes[4].distance = viewProjMatrix[3][3] + viewProjMatrix[3][2];

    // Far plane
    planes[5].normal.x = viewProjMatrix[0][3] - viewProjMatrix[0][2];
    planes[5].normal.y = viewProjMatrix[1][3] - viewProjMatrix[1][2];
    planes[5].normal.z = viewProjMatrix[2][3] - viewProjMatrix[2][2];
    planes[5].distance = viewProjMatrix[3][3] - viewProjMatrix[3][2];

    // Normalize planes
    for (auto& plane : planes) 
    {
        const float length = glm::length(plane.normal);
        plane.normal /= length;
        plane.distance /= length;
    }

    return planes;
}
