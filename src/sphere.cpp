#include "sphere.h"
#include "intersection.h"

Sphere::Sphere(glm::vec3 origin, float radius)
: origin(origin)
, radius(radius)
{}

bool Sphere::Intersects(const Ray &ray, Intersections::Record &info)
{
    return Intersections::RaySphere(ray, *this, info);
}
