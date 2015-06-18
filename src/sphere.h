#ifndef SPHERE_H
#define SPHERE_H
#include "object.h"

class Sphere : public Object
{
public:
    Sphere(glm::vec3 origin, float radius);
    virtual bool Intersects(const Ray &ray, Intersections::Record &info);

    glm::vec3 origin;
    float     radius;
};

#endif
