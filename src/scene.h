#ifndef SCENE_H
#define SCENE_H
#include "object.h"
#include "camera.h"
#include "light.h"
#include <vector>

class Scene : public Object
{
public:
	Scene(const char *filename);

    virtual bool Intersects(const Ray &ray, Intersections::Record &info);
	virtual Colour Shade(const Ray &ray, const Intersections::Record &info, Scene &scene);

	std::vector<Object*> objects;
	std::vector<Light*>  lights;
	Camera              *camera;
};

#endif