#ifndef LIGHT_H
#define LIGHT_H
#include "glm.hpp"

class Light
{
};


class PointLight : public Light
{
public:
    PointLight(glm::vec3 origin, glm::vec3 colour)
    : origin(origin)
    , colour(colour)
    {}

    glm::vec3 origin;
    glm::vec3 colour;
};

#endif
