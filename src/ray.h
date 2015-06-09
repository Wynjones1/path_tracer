#ifndef RAY_H
#define RAY_H
#include "glm.hpp"
#include <iostream>

class Ray
{
public:
    Ray();
    Ray(const glm::vec3 &origin, const glm::vec3 &direction);

    glm::vec3 origin;
    glm::vec3 direction;
};

std::ostream &operator<<(std::ostream &stream, const Ray &ray);

#endif
