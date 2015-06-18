#include "aabb.h"
#include "intersection.h"

AABB::AABB(const glm::vec3 &min, const glm::vec3 &max)
: min(min)
, max(max)
{}


AABB::AABB()
{}

bool AABB::Intersects(const Ray &ray, Intersections::Record &info)
{
    return Intersections::RayAABB(ray, *this, info);
}

AABB::AABB(const std::vector<glm::vec3> &vertices)
{
	min =  glm::vec3(std::numeric_limits<float>().max());
    max = -glm::vec3(std::numeric_limits<float>().max());
    for(auto &v : vertices)
    {
        for(auto i = 0; i < 3; i++)
        {
            if(v[i] < min[i])
            {
                min[i] = v[i];
            }
            if(v[i] > max[i])
            {
                max[i] = v[i];
            }
        }
    }
}

std::ostream &operator <<(std::ostream &stream, const AABB &aabb)
{
    stream << "AABB("
        "(" << aabb.min[0] << "," << aabb.min[1] << "," << aabb.min[2] << ")," <<
        "(" << aabb.max[0] << "," << aabb.max[1] << "," << aabb.max[2] << "))";
    return stream;
}
