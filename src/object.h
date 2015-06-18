#ifndef OBJECT_H
#define OBJECT_H
#include <glm.hpp>

namespace Intersections
{
    struct Record;
}

class Ray;

class Object
{
public:
    virtual bool Intersects(const Ray &ray, Intersections::Record &info) = 0;
};
#endif
