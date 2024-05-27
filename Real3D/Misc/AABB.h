#ifndef FRUSTUMAABB_H
#define FRUSTUMAABB_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <array>

namespace real
{
    struct AABB
    {
        glm::vec3 min;
        glm::vec3 max;
    };

    struct FrustumPlane
    {
        glm::vec3 normal;
        float distance;

        FrustumPlane() : normal(0.0f), distance(0.0f) {}
        FrustumPlane(const glm::vec3& n, float d) : normal(n), distance(d) {}

        float DistanceToPoint(const glm::vec3& point) const
        {
            return glm::dot(normal, point) + distance;
        }
    };

    class FrustumAABB
    {
    public:
        static bool IsBoxInFrustum(const glm::mat4& viewProjMatrix, const AABB& box);
    private:
    	static std::array<FrustumPlane, 6> ExtractFrustumPlanes(const glm::mat4& viewProjMatrix);
    };
}

#endif // FRUSTUMAABB_H