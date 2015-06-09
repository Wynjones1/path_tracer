#ifndef OBJECT_H
#define OBJECT_H
#include <glm.hpp>

namespace Intersections
{
    struct Intersection;
}

class Ray;

class Object
{
public:
    virtual bool Intersects(const Ray &ray, Intersections::Intersection &info) = 0;
};
#endif
