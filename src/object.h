#ifndef OBJECT_H
#define OBJECT_H
#include <glm.hpp>
#include "colour.h"

class Scene;

namespace Intersections
{
    struct Record;
}

class Ray;

class Object
{
public:
    virtual bool Intersects(const Ray &ray, Intersections::Record &info) = 0;
	virtual Colour Shade(const Ray &ray, const Intersections::Record &info, Scene &scene) = 0;
};
#endif
