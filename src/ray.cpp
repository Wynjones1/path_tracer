#include "ray.h"

Ray::Ray()
: origin(0.0f)
, direction(0.0f)
{}

Ray::Ray(const glm::vec3 &origin, const glm::vec3 &direction)
: origin(origin)
, direction(direction)
{}

std::ostream &operator<<(std::ostream &stream, const Ray &ray)
{
    stream << "Ray("
           << "(" << ray.origin.x    << "," << ray.origin.y    << "," << ray.origin.z << "),"
           << "(" << ray.direction.x << "," << ray.direction.y << "," << ray.direction.z << ")"
           << ")";
    return stream;
}
