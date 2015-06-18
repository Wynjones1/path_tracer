#ifndef CAMERA_H
#define CAMERA_H
#include "ray.h"

class Camera
{
public:
    virtual Ray Emit(float x, float y) = 0;
};

class OrthoCamera : public Camera
{
public:
    OrthoCamera(const glm::vec3 &origin, const glm::vec3 &forward, const glm::vec3 &right, float width, float height)
    : origin(origin)
    , forward(forward)
    , right(right)
    , width(width)
    , height(height)
    {}

    virtual Ray Emit(float x, float y)
    {
        /*
         (-0.5,0.5)      (0.5, 0.5)
               +------|------+
               |      |      |
               |      |      |
               +------+------|
               |      |      |
               |      |      |
               +------+------|
         (-0.5, -0.5)      (0.5, -1.5)
        */
        Ray ray;
        ray.origin.x  = origin.x + (-width  / 2.0f) + x * width;
        ray.origin.y  = origin.y + (-height / 2.0f) + y * height;
        ray.origin.z  = origin.z;

        ray.direction.x  = 0.0f;
        ray.direction.y  = 0.0f;
        ray.direction.z  = -1.0;
        return ray;
    }
private:
    glm::vec3 origin;
    glm::vec3 forward;
    glm::vec3 right;
    float     width;
    float     height;
};

class PerspectiveCamera : public Camera
{
public:
	PerspectiveCamera()
	: origin(0, 0, 0)
	, forward(0, 0, -1)
	, right(1, 0, 0)
	, fov(1.0)
	{

	}
    PerspectiveCamera(const glm::vec3 &origin, const glm::vec3 &forward, const glm::vec3 &right, float fov)
    : origin(origin)
    , forward(forward)
    , right(right)
    , fov(fov)
    {}

    virtual Ray Emit(float x, float y)
    {
        Ray ray;
        ray.origin = origin;
        ray.direction = glm::vec3(-1.0f + 2 * x, -1.0f + 2 * y, -1);
        ray.direction = glm::normalize(ray.direction);
        return ray;
    }
private:
    glm::vec3 origin;
    glm::vec3 forward;
    glm::vec3 right;
    float     fov;
};

#endif
